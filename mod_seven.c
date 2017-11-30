/*
 mod_seven 1.0 - PMOD GLOBAL
 Copyright (C) 2015 LAR
 Author: Tulio Pascoal
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_connection.h"
#include "http_log.h"
#include "mpm_common.h"
#include "ap_mpm.h"
#include "ap_release.h"
#include "apr_hash.h"
#include "apr_strings.h"
#include "scoreboard.h"
#include "http_protocol.h"
#include "http_request.h"
#include <assert.h>
#include <apr_thread_mutex.h>

#define MODULE_NAME "mod_seven"
#define MODULE_VERSION "1.0"

static int server_limit;
static int ipmod;
static double dpmod;
/* Declaracao do filtro */
static ap_filter_rec_t *_input_filter;

#ifdef APLOG_USE_MODULE
APLOG_USE_MODULE(seven);
#endif

struct ap_sb_handle_t {
    int child_num;
    int thread_num;
};


/* Prototipo das Funcoes */
char* itoa(long long, char[]);
int uniform_distribution(int, int);
double rand_prob();

/* Prototipo dos Metodos do Modulo */
static void register_hooks(apr_pool_t *);
static int create_request_hook(request_rec *);
static void *create_server_config(apr_pool_t *, server_rec *);


char* itoa(long long i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

int uniform_distribution(int a, int b)
  {
    return rand() / (RAND_MAX + 1.0) * (b - a) + a;
}

double rand_prob()
  {
    return (double)rand() / (double)RAND_MAX;
}


/* Inicializacao e configuracao global do modulo */
static int post_config(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_DAEMONS, &server_limit);
    ap_mpm_query(AP_MPMQ_HARD_LIMIT_DAEMONS, &ipmod);
    dpmod = (double)ipmod;

    ap_log_error(APLOG_MARK, APLOG_WARNING, 0, NULL, MODULE_NAME " " MODULE_VERSION " iniciado.");
    return OK;
}



/* Filtro das requisicoes */
static int seven_input_filter(ap_filter_t *f, apr_bucket_brigade *bb,
                           ap_input_mode_t mode, apr_read_type_e block,
                           apr_off_t readbytes)
{

  /* Resgatando informacoes da request (IP:PORTA) */
  char req_id[25];
  /* Resgatando porta da requisicao */
  int porta_aux = f->r->connection->client_addr->port;
  char porta[10];
  /* Transformando em char */
  itoa(porta_aux,porta);
  /* Concatenando o ip e a porta "IP:PORTA" */
  strcpy(req_id,f->r->useragent_ip);
  strcat(req_id,":");
  strcat(req_id,porta);
  /* Resgatando Socket */
  apr_socket_t *csd = ap_get_conn_socket(f->c);

  
  /* Checar se tem requests com flag deleted */
  int k = 0, l = 0;
  int busy2 = 0;

  worker_score *ws_record2;
  global_score *g_record2;

  g_record2 = ap_get_scoreboard_global();

    for (k = 0; k < g_record2->server_limit; ++k) {
      for (l = 0; l < g_record2->thread_limit; ++l) { 
          
            #if AP_SERVER_MAJORVERSION_NUMBER == 2 && AP_SERVER_MINORVERSION_NUMBER > 2
            ws_record2 = ap_get_scoreboard_worker_from_indexes(k, l);
            #else
            ws_record2 = ap_get_scoreboard_worker(k, l);
            #endif
            #ws_record2 = ap_get_scoreboard_worker_from_indexes(k, l);
          
          /* Foi encontrado com flag deleted */
          if((strcmp(req_id, ws_record2->request) == 0)) {
              
              /* Apagando do Worker Score */
              apr_cpystrn(ws_record2->client, "", sizeof(ws_record2->client));
              apr_cpystrn(ws_record2->request, "", sizeof(ws_record2->request));
              ws_record2->pid = 0;

              /* Dropando do servidor */
              apr_table_setn(f->c->notes, "short-lingering-close", "1");
              f->c->aborted = -1;
              f->c->keepalive = AP_CONN_CLOSE;
              apr_socket_close(csd);
              
              return APR_SUCCESS;
          }
    }
  
 }

  /* Lock no Mutex */
  #if APR_HAS_THREADS
  apr_thread_mutex_t *mutex = NULL;
  assert(apr_thread_mutex_create(&mutex, APR_THREAD_MUTEX_DEFAULT, f->r->server->process->pool) == APR_SUCCESS); 
  apr_thread_mutex_lock(mutex);
  #endif
  
  /* Adicionando requisicao no Scoreboard */
  ap_sb_handle_t *sbh = f->c->sbh;
  worker_score *me =  &ap_scoreboard_image->servers[sbh->child_num][sbh->thread_num];
  apr_cpystrn(me->client, req_id, sizeof(me->client));
 

  /* Contagem dos MaxReqWorkers atual */
  int i = 0, j = 0;
  int busy = 0;

  worker_score *ws_record;
  global_score *g_record;

  g_record = ap_get_scoreboard_global();

    for (i = 0; i < g_record->server_limit; ++i) {
      for (j = 0; j < g_record->thread_limit; ++j) { 
      
            #if AP_SERVER_MAJORVERSION_NUMBER == 2 && AP_SERVER_MINORVERSION_NUMBER > 2
            ws_record = ap_get_scoreboard_worker_from_indexes(i, j);
            #else
            ws_record = ap_get_scoreboard_worker(i, j);
            #endif
            #ws_record = ap_get_scoreboard_worker_from_indexes(i, j);
          
          if((strcmp("::1", ws_record->client) != 0) && (strcmp("", ws_record->client) != 0)){
              busy++;
          }
    }
 }

  /* Checagem para Logica do Seven: se quantidade
  de Workers atual ultrapassar SEVEN_BUFFER_LENGTH,
  Estrategia Seven e iniciada */
  if (busy >= server_limit) {

            /* Ganhou na moeda */
            if ( rand_prob() < (server_limit/dpmod) ) {

              dpmod += 1;
              
              /* Colocar flag deleted num Worker do Scoreboard 
              escolhido pela distribuicao uniforme */ 
              int selected = uniform_distribution(0,busy);
              /* Resgatar o Worker */
              worker_score *labeled = &ap_scoreboard_image->servers[selected][j];
              /* Colocando flag deleted */
              apr_cpystrn(labeled->request, labeled->client, sizeof(labeled->request));

              return ap_get_brigade(f->next, bb, mode, block, readbytes);

            /* Perdeu na moeda */
          } else {

              dpmod += 1;

              /* Retirar o Worker do Scoreboard */
              apr_cpystrn(me->client, "", sizeof(me->client));
              apr_cpystrn(me->request, "", sizeof(me->request));
              
              /* Dropando do Servidor */
              apr_table_setn(f->c->notes, "short-lingering-close", "1");
              f->c->aborted = -1;
              f->c->keepalive = AP_CONN_CLOSE;     
              apr_socket_close(csd);
            
              return APR_SUCCESS;

          }
      }

  #if APR_HAS_THREADS
  apr_thread_mutex_unlock(mutex);
  #endif

  return ap_get_brigade(f->next, bb, mode, block, readbytes);

}


/* Gancho de requisicoes */
static int create_request_hook(request_rec *r) {
    
    /* Resgatando IP da requisicao */
    char req_id[25];
    strcpy(req_id,r->connection->client_ip);

    /* Checando Dummy requests do Apache */
    if (strcmp(req_id, "::1") == 0) {

      return DECLINED;
    }

    ap_add_input_filter_handle(_input_filter, NULL, r, r->connection);
    return OK;
}

/* Registro de ganchos do modulo */
static void register_hooks(apr_pool_t *p)
{ 

  ap_hook_post_config(post_config, NULL, NULL, APR_HOOK_MIDDLE);
  ap_hook_create_request(create_request_hook, NULL, NULL, APR_HOOK_MIDDLE);
  _input_filter = ap_register_input_filter("mod_seven_input_filter",
                                           seven_input_filter, NULL,
                                           AP_FTYPE_PROTOCOL);
}


/* Dados do modulo */
module AP_MODULE_DECLARE_DATA seven_module = {
  STANDARD20_MODULE_STUFF,
  NULL,     /* create per-dir config structures */
  NULL,     /* merge  per-dir config structures */
  NULL,     /* create per-server config structures */
  NULL,     /* merge  per-server config structures */
  NULL,   /* table of config file commands */
  register_hooks /* register module's hooks/etc. with the core */
};
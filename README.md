# mod_seven


An Apache module for DDoS attacks mitigation. Tested against Slowloris, HTTP POST and Slowread.
Keeps Apache  aservervailability above XX%.


For more details and understanding of mod_seven and Application Layer DoS attacks, please check [1] and [2]:

[1] Pascoal, T. A., Correa, J. H. G., Nigam, V., Fonseca, I. E., 2017. “An Anti-Application Layer DoS Attack Module: A Quality of Service and User Experience Analysis”. In XXXV Brazilian Symposium on Computer Networks, pp. 1003-1016. SBC, 2017. In portuguese.

[2] Pascoal, T. A., Correa, J. H. G., Nigam, V., Fonseca, I. E., 2016. “A module for defending application layer DDoS attacks using selective strategies”. In XXXIV Brazilian Symposium on Telecommunications and Signal Processing, pp. 969-973. SBC, 2016. In portuguese.

---------------------------------------------------------------------------------------------------------------------------------
Installation:

1. First, install apache dev:

# apt-get install apache2-dev

2. Second, install and activates mod_seven module:

# apxs2 -i -c -a mod_seven.c

3. Finally, restart Apache service:

# service apache2 restart

Files:
---------------------------------------------------------------------------------------------------------------------------------

+ mod_seven - Manual: mod_seven Manual in Portuguese;
+ scenario_experiments: One of the many used scenario for experiments;

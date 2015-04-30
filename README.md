Analysis code for K600 experiments.

Most users will only need to alter certain sections of the code as set-out below.
This branch has been generated from the SetupDev version of the K600analyser code. The file config.cfg should be modified to give the correct configuration for the detectors used. Note that the only ancillary detectors currently enabled by me (PA) in the code are W1s and MMMs. I hope to add HAGAR soon.

In all other cases, new ancillary detectors must be added manually by the user. This will require a separate module to be added to dump the data into the correct data structure. In addition, the Parameters.c file will need to be modified to read in the new configuration parameters for the new ancillary detectors. E-mail me or raise on issue on the repo if you need help with this.

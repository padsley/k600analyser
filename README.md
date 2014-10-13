Analysis code for K600 experiments.

Most users will only need to alter certain sections of the code as set-out below.

PR228 branch.

The code is set out so that the ADC/TDC channels for the various detectors are defined in Parameters.c. This file also contains the calibration parameters for the code. This file is likely to be one of the files that the user will be forced to modify if they want to do things.

The sorter.sh file is also likely to need changing as the path needs to be set correctly for the data to be found and processed. The sorter.sh script is organised such that it should format the file names automatically and sort them correctly. If you change this, beware of the fact that the code will crash if you fail to set the names correctly. Be prepared for this change.

If you have found problems with the code, please sign up for the github repo and tell us about them. Then we can have a go at fixing them.

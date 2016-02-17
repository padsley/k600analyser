# k6600analyser 

We no longer have a file called f-plane.c; it is replaced with main.c

The code is now structured so that most of the subroutines that used to be in f-plane.c are now moved
to FocalPlane.c with definitions to Focalplane.h.

The ANA_MODULE focalplane_module was changed to main_module, and accordingly
we have main_event, main_bor, main_eor and main_init instead of focal_event etc.
These changes are affected in both analyzer.c and main.c
These are just naming conventions that I feel is more logical.

Also, a lot of variable definitions that used to be in f-plane.c is moved to parameters.h

The codes adc.c and qdc.c were cleaned up. A lot of code that was commented out for the 
past few years were deleted.  Similarly a lot of dead weight code in f-plane.c (now main.c)
were deleted. These include provision for histograms for NaI and Si detectors, which is now
handled entirely by 
GammaData.c
HagarSort.c
MMM.c
SiliconData.c
W1.c
etc.

The raytracing code was improved to include fixZ and fixX subroutines.
The chisq calculation was changed to a rsquared calculation (the chi sq value
was never the proper chisq value in any case)

A small 3mm adjustment was made to the Y calculation.

The code is still not as I want it to be.

The main aim is to take out all the VDC content and move it out to a FocalplaneDATA.c/h and VDC.c/h 
structure that will mimic the structure Phil created for e.g. SiliconDATA.c/h and MMM.c/h.
So far all that was done is the first step:  to make f-plane.c into main.c, and to take out all the 
subroutines in this file (that makes it so difficult to read) out to a temporary Focalplane.c/h structure...

What I would also still like to do is to make sure all angle calcs, line shape corrections etc
use parameters from the config file. Lets do away with the use of the ODB for these!

Most users will only need to alter certain sections of the code as set-out below.
This branch has been generated from the SetupDev version of the K600analyser code. The file config.cfg should be modified to give the correct configuration for the detectors used. Note that the only ancillary detectors currently enabled by me (PA) in the code are W1s and MMMs. I hope to add HAGAR soon.

In all other cases, new ancillary detectors must be added manually by the user. This will require a separate module to be added to dump the data into the correct data structure. In addition, the Parameters.c file will need to be modified to read in the new configuration parameters for the new ancillary detectors. E-mail me or raise on issue on the repo if you need help with this.

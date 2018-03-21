# k600analyser for PR259

March 2018: starting with files from the master branch, this code
should enable data analysis of the coincidence data for PR259.
All the latest capabilities found in the masterbranch, such as 
lineshape corrections based on TOF, should therefore be available.

Some comments for the analysis personal:

- The LUT's are good starting values. But check if they are good enough.
- Ditto for the CableLength file
- The parameters in the config file are only starting values, with correct
  channel mappings for the Si detectors. But dont assume any of the lineshape
  and K600 calibration parameters are good.
- The Si calibration parameters follow from PR254, which immediately
  preceded PR259 in the experimental schedule. Again, I did not specifically
  check that these calibration parameters are good all round, so you should
  make sure it is.
- I aligned the TDC info so that one can hopefully have only 1 CUT for the 
  MMM PID plot (i.e. when you plot SiliconTimeOffset-tof:SiliconEnergy).
  The offsets a listed in file TDCOffsetsPR259.dat may require some tweaking

RN


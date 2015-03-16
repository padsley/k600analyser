#include "actarSort.h"
#include "actarRawData.h"
#include <stdio.h>
#include <vector>

private int recursive = 0;
private int numchan = 64;
public int source[numchan];
public int pulser[numchan];
public int calibration[numchan];

actarSort::actarSort()
{

}

actarSort::~actarSort()
{

}

void actarSort::sortActarData()
{
  //not putting in for loop, rather receive input then send the calibration data back bit by bit
  //this void can call itself recursively and terminate once all of the channels have been spooled through

  if(recursive < 64)
    {
      calibration[recursive] = source[recursive]-pulser[recursive];
      recursive++;
      sortActarData();
    }
  else if (recursive == 64)
    {
      return calibration;
      recursive == 0;
    }
}

void actarSort::fillPulser(int channel, float value)
{
  pulser[channel] = value;
}

void actarSort::fillSource(int channel, float value)
{
  source[channel] = value;
}

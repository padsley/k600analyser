#include "multiTDC.h"

multiTDC::multiTDC(int ntdc, int *TDC_channel_import, float *TDC_value_import)
{
  TDChits = 0;
  int *ChannelCounter = new int[896];
  int *GoodChannelCounter = new int[896];
  
  for(int i=0;i<896;i++)  {ChannelCounter[i]=0;}
  for(int i=0;i<896;i++)  {GoodChannelCounter[i]=0;}

  for(int n=0;n<ntdc;n++){
    if(TDC_channel_import[n]>=0 && TDC_channel_import[n]<7*128){   
    //if(TDC_channel_import[n]>=6*128+48 && TDC_channel_import[n]<7*128){   //Only store events from the last TDC, i.e. the ancillary detector events
	ChannelCounter[TDC_channel_import[n]]+=1;
	//printf("counter= %i for TDC channel %i \n",ChannelCounter[TDC_channel_import[n]],TDC_channel_import[n]);
    }	
    if(TDC_channel_import[n]>=0 && TDC_channel_import[n]<7*128 && TDC_value_import[n]>PulseLimits[0] && TDC_value_import[n]<PulseLimits[1]){
    //if(TDC_channel_import[n]>=6*128+48 && TDC_channel_import[n]<7*128 && TDC_value_import[n]>PulseLimits[0] && TDC_value_import[n]<PulseLimits[1]){
	GoodChannelCounter[TDC_channel_import[n]]+=1;
	//printf("Good channel counter= %i for TDC channel %i \n",GoodChannelCounter[TDC_channel_import[n]],TDC_channel_import[n]);
    }
  }

  for(int n=0;n<ntdc;n++){
    if(TDC_channel_import[n]>=0 && TDC_channel_import[n]<7*128){
    //if(TDC_channel_import[n]>=6*128+48 && TDC_channel_import[n]<7*128){
      if(ChannelCounter[TDC_channel_import[n]]==0){
	// this condition should never be met as we set everthing to non-zero value a few lines earlier....RN
      }
      else if(ChannelCounter[TDC_channel_import[n]]==1){
	//The reason that we do this this way is to look at how many events fall outside the good beampulse:
	//only when we have multiple hits do we need to worry about the multiple hits and this should be quicker 
	//
	SetChannel(TDC_channel_import[n]);
	SetValue(TDC_value_import[n]);
	SetMult(ChannelCounter[TDC_channel_import[n]]);
	TDChits++;
      }
      else if(ChannelCounter[TDC_channel_import[n]]>1 && GoodChannelCounter[TDC_channel_import[n]]==1 && TDC_value_import[n]>PulseLimits[0] && TDC_value_import[n]<PulseLimits[1]){
	SetChannel(TDC_channel_import[n]);
	SetValue(TDC_value_import[n]);
	SetMult(ChannelCounter[TDC_channel_import[n]]);
	TDChits++;
      }
//       else if(ChannelCounter[TDC_channel_import[n]]>1 && GoodChannelCounter[TDC_channel_import[n]]==2
      else if(ChannelCounter[TDC_channel_import[n]]>1 && GoodChannelCounter[TDC_channel_import[n]]>1){
	printf("The number of TDC hits within the user-defined 'good pulse' is greater than 1; the number of hits is %d. The code currently doesn't deal with this.\n",GoodChannelCounter[TDC_channel_import[n]]);
	SetMult(100);  // value of 100 used as a marker in analysis for this type of event
      }
      else if(ChannelCounter[TDC_channel_import[n]]>1 && GoodChannelCounter[TDC_channel_import[n]]==0){
	//Take the first TDC event for a channel where nothing falls within the good beampulse
	SetChannel(TDC_channel_import[n]);
	SetValue(TDC_value_import[n]);
	TDChits++;
	SetMult(ChannelCounter[TDC_channel_import[n]]);
	ChannelCounter[TDC_channel_import[n]]=0;//Now set the ChannelCounter for the channel to zero as we don't want to take any more of these events - this forces the change above in the first part of the if chain. Need to fix at some point.
      }
    }
  }
  
  SetHits(TDChits);
  delete ChannelCounter;
  delete GoodChannelCounter;
}



multiTDC::~multiTDC()
{
}

void multiTDC::PrintEvent()
{
  printf("Size of event: %d\n", SizeOfEvent());
  //TDCchannels.clear();
  //TDCvalues.clear();
}

bool multiTDC::TestEvent()
{
  bool result = true;
  if(TDChits != TDCchannels.size()){printf("TDChits != TDCchannels.size()\n");result = false;}
  if(TDChits != TDCvalues.size()){printf("TDChits != TDCvalues.size()\n");result = false;}
  if(TDCchannels.size() != TDCvalues.size()){printf("TDCchannels.size() != TDCvalues.size()\n");result = false;}
  return result;
}

void multiTDC::ClearEvent()
{
  TDChits=0;
  TDCchannels.clear();
  TDCvalues.clear();
}

unsigned int multiTDC::SizeOfEvent()
{
  return TDChits;
}

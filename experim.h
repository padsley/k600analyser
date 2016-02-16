/********************************************************************\

  Name:         experim.h
  Created by:   ODBedit program

  Contents:     This file contains C structures for the "Experiment"
                tree in the ODB and the "/Analyzer/Parameters" tree.

                Additionally, it contains the "Settings" subtree for
                all items listed under "/Equipment" as well as their
                event definition.

                It can be used by the frontend and analyzer to work
                with these information.

                All C structures are accompanied with a string represen-
                tation which can be used in the db_create_record function
                to setup an ODB structure which matches the C structure.

  Created on:   Fri Jul  3 15:23:14 2015

\********************************************************************/

#define EXP_PARAM_DEFINED

typedef struct {
  char      comment[80];
} EXP_PARAM;

#define EXP_PARAM_STR(_name) char *_name[] = {\
"[.]",\
"Comment = STRING : [80] Analysis",\
"",\
NULL }

#ifndef EXCL_ADC

#define ADC_PARAM_DEFINED

typedef struct {
  INT       software_gain[16];
  short     histogram_threshold;
  INT       adc_threshold;
} ADC_PARAM;

#define ADC_PARAM_STR(_name) char *_name[] = {\
"[.]",\
"Software Gain = INT[16] :",\
"[0] 0",\
"[1] 0",\
"[2] 0",\
"[3] 0",\
"[4] 0",\
"[5] 0",\
"[6] 0",\
"[7] 0",\
"[8] 0",\
"[9] 0",\
"[10] 0",\
"[11] 0",\
"[12] 0",\
"[13] 0",\
"[14] 0",\
"[15] 0",\
"Histogram Threshold = SHORT : 6",\
"adc_threshold = INT : 5",\
"",\
NULL }

#endif

#ifndef EXCL_QDC

#define QDC_PARAM_DEFINED

typedef struct {
  INT       software_gain[16];
  short     histogram_threshold;
  INT       adc_threshold;
} QDC_PARAM;

#define QDC_PARAM_STR(_name) char *_name[] = {\
"[.]",\
"Software Gain = INT[16] :",\
"[0] 0",\
"[1] 0",\
"[2] 0",\
"[3] 0",\
"[4] 0",\
"[5] 0",\
"[6] 0",\
"[7] 0",\
"[8] 0",\
"[9] 0",\
"[10] 0",\
"[11] 0",\
"[12] 0",\
"[13] 0",\
"[14] 0",\
"[15] 0",\
"Histogram Threshold = SHORT : 6",\
"adc_threshold = INT : 5",\
"",\
NULL }

#endif

#ifndef EXCL_FOCALPLANE

#define FOCALPLANE_PARAM_DEFINED

typedef struct {
  INT       x1_driftt_low;
  INT       x1_driftt_hi;
  INT       x2_driftt_low;
  INT       u1_driftt_low;
  INT       u2_driftt_low;
  INT       x2_driftt_hi;
  INT       u2_driftt_hi;
  INT       u1_driftt_hi;
  INT       lowtof;
  INT       hitof;
  INT       lowpad1;
  INT       lowpad2;
  INT       hipad1;
  INT       hipad2;
  float     a0;
  float     a1;
  float     a2;
  float     b0;
  float     b1;
  float     b2;
  float     a0xcorr;
  float     a1xcorr;
  float     c00;
  float     c01;
  float     c02;
  float     c11;
  float     c10;
  float     c12;
  float     c20;
  float     c21;
  float     c22;
  float     c30;
  float     c31;
  float     c32;
  float     d00;
  float     d01;
  float     d02;
  float     d10;
  float     d11;
  float     d12;
  float     d20;
  float     d21;
  float     d22;
  float     d30;
  float     d31;
  float     d32;
  float     a2xcorr;
  float     b0xcorr;
  float     b1xcorr;
  float     b2xcorr;
  float     a3xcorr;
  float     c0;
  float     c1;
  float     c2;
  float     d0;
  float     d1;
  float     d2;
  float     c0xcorr;
  float     c1xcorr;
  float     c2xcorr;
  float     d0xcorr;
  float     d1xcorr;
  float     d2xcorr;
  float     a4xcorr;
  float     yoffset;
  float     f0xcorr;
  float     f1xcorr;
  float     x0;
  float     mtarget;
  float     calpa;
  float     calpb;
  float     calpc;
} FOCALPLANE_PARAM;

#define FOCALPLANE_PARAM_STR(_name) char *_name[] = {\
"[.]",\
"x1_driftt_low = INT : 6100",\
"x1_driftt_hi = INT : 8050",\
"x2_driftt_low = INT : 6100",\
"u1_driftt_low = INT : 6100",\
"u2_driftt_low = INT : 6100",\
"x2_driftt_hi = INT : 8050",\
"u2_driftt_hi = INT : 8050",\
"u1_driftt_hi = INT : 8050",\
"lowtof = INT : 2000",\
"hitof = INT : 6000",\
"lowpad1 = INT : 1",\
"lowpad2 = INT : 0",\
"hipad1 = INT : 2000",\
"hipad2 = INT : 4000",\
"a0 = FLOAT : -1.05242",\
"a1 = FLOAT : 0.00022768",\
"a2 = FLOAT : 0",\
"b0 = FLOAT : 35.0792",\
"b1 = FLOAT : -0.0119133",\
"b2 = FLOAT : 0",\
"a0xcorr = FLOAT : 3.7",\
"a1xcorr = FLOAT : 0.6",\
"c00 = FLOAT : -0.149905",\
"c01 = FLOAT : 0.000190706",\
"c02 = FLOAT : -1.00038e-07",\
"c11 = FLOAT : -1.01008e-05",\
"c10 = FLOAT : 0.0120318",\
"c12 = FLOAT : -6.39233e-09",\
"c20 = FLOAT : 0.0141237",\
"c21 = FLOAT : -4.05089e-05",\
"c22 = FLOAT : 3.25848e-08",\
"c30 = FLOAT : -0.00243223",\
"c31 = FLOAT : -8.68271e-06",\
"c32 = FLOAT : 1.58795e-08",\
"d00 = FLOAT : -0.686069",\
"d01 = FLOAT : -0.000487973",\
"d02 = FLOAT : 9.36511e-07",\
"d10 = FLOAT : 0.13385",\
"d11 = FLOAT : -0.000277455",\
"d12 = FLOAT : 1.41865e-07",\
"d20 = FLOAT : -0.0221455",\
"d21 = FLOAT : 0.000187413",\
"d22 = FLOAT : -2.33532e-07",\
"d30 = FLOAT : -0.0231652",\
"d31 = FLOAT : -5.69357e-05",\
"d32 = FLOAT : 1.16068e-07",\
"a2xcorr = FLOAT : 0",\
"b0xcorr = FLOAT : 0.08",\
"b1xcorr = FLOAT : -0.01",\
"b2xcorr = FLOAT : 0",\
"a3xcorr = FLOAT : 0",\
"c0 = FLOAT : -0.0388196",\
"c1 = FLOAT : -2.31421e-05",\
"c2 = FLOAT : 0",\
"d0 = FLOAT : 0.269372",\
"d1 = FLOAT : 0.000451383",\
"d2 = FLOAT : 0",\
"c0xcorr = FLOAT : 0",\
"c1xcorr = FLOAT : 0",\
"c2xcorr = FLOAT : 0",\
"d0xcorr = FLOAT : 0",\
"d1xcorr = FLOAT : 0",\
"d2xcorr = FLOAT : 0",\
"a4xcorr = FLOAT : 0",\
"yoffset = FLOAT : 5",\
"f0xcorr = FLOAT : 0",\
"f1xcorr = FLOAT : 0",\
"x0 = FLOAT : 647.505",\
"mtarget = FLOAT : 15.99492",\
"calpa = FLOAT : -1.5768e-05",\
"calpb = FLOAT : 0.126985",\
"calpc = FLOAT : 1119.45",\
"",\
NULL }

#endif

#ifndef EXCL_GLOBAL

#define GLOBAL_PARAM_DEFINED

typedef struct {
  INT       misswires;
  INT       max_tdc_channels;
  float     z_x1x2;
  float     x_x1x2;
  INT       min_x_wires;
  INT       min_u_wires;
  INT       max_x_wires;
  INT       max_u_wires;
  INT       lut_x1_offset;
  INT       lut_u1_offset;
  INT       lut_x2_offset;
  INT       lut_u2_offset;
  INT       x1_1st_wire_chan;
  INT       x1_last_wire_chan;
  INT       x2_1st_wire_chan;
  INT       x2_last_wire_chan;
  INT       u1_1st_wire_chan;
  INT       u1_last_wire_chan;
  INT       u2_1st_wire_chan;
  INT       u2_last_wire_chan;
  float     adc_threshold;
} GLOBAL_PARAM;

#define GLOBAL_PARAM_STR(_name) char *_name[] = {\
"[.]",\
"misswires = INT : 2",\
"max_tdc_channels = INT : 1000",\
"z_x1x2 = FLOAT : 285",\
"x_x1x2 = FLOAT : 515",\
"min_x_wires = INT : 3",\
"min_u_wires = INT : 3",\
"max_x_wires = INT : 9",\
"max_u_wires = INT : 8",\
"lut_x1_offset = INT : 30",\
"lut_u1_offset = INT : 30",\
"lut_x2_offset = INT : 30",\
"lut_u2_offset = INT : 30",\
"x1_1st_wire_chan = INT : 0",\
"x1_last_wire_chan = INT : 200",\
"x2_1st_wire_chan = INT : 500",\
"x2_last_wire_chan = INT : 700",\
"u1_1st_wire_chan = INT : 300",\
"u1_last_wire_chan = INT : 443",\
"u2_1st_wire_chan = INT : 800",\
"u2_last_wire_chan = INT : 943",\
"ADC Threshold = FLOAT : 5",\
"",\
NULL }

#endif

#ifndef EXCL_TRIGGER

#define TRIGGER_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  INT       type;
  INT       log_history;
  char      frontend_name[256];
  WORD      trigger_mask;
  char      format[80];
} TRIGGER_COMMON;

#define TRIGGER_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 0",\
"Type = INT : 2",\
"Log history = INT : 0",\
"Frontend name = STRING : [256] K600 frontend",\
"Trigger mask = WORD : 0",\
"Format = STRING : [80] MIDAS",\
"",\
NULL }

#endif

#ifndef EXCL_SCALER

#define SCALER_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  INT       type;
  WORD      trigger_mask;
  char      buffer[32];
  INT       source;
  char      format[8];
  BOOL      enabled;
  INT       read_on;
  INT       period;
  double    event_limit;
  DWORD     num_subevents;
  INT       log_history;
  char      frontend_host[32];
  char      frontend_name[32];
  char      frontend_file_name[256];
} SCALER_COMMON;

#define SCALER_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 2",\
"Type = INT : 33",\
"Trigger mask = WORD : 0",\
"Buffer = STRING : [32] SYSTEM",\
"Source = INT : 0",\
"Format = STRING : [8] MIDAS",\
"Enabled = BOOL : y",\
"Read on = INT : 377",\
"Period = INT : 1000",\
"Event limit = DOUBLE : 0",\
"Num subevents = DWORD : 0",\
"Log history = INT : 0",\
"Frontend host = STRING : [32] vmevmic2",\
"Frontend name = STRING : [32] K600 frontend",\
"Frontend file name = STRING : [256] k600frontend.c",\
"",\
NULL }

#define SCALER_SETTINGS_DEFINED

typedef struct {
  char      names[8][32];
} SCALER_SETTINGS;

#define SCALER_SETTINGS_STR(_name) char *_name[] = {\
"[.]",\
"Names = STRING[8] :",\
"[32] scaler1",\
"[32] scaler2",\
"[32] scaler3",\
"[32] scaler4",\
"[32] scaler5",\
"[32] scaler6",\
"[32] scaler7",\
"[32] scaler8",\
"",\
NULL }

#endif

#ifndef EXCL_WIRECHAMBER

#define WIRECHAMBER_SETTINGS_DEFINED

typedef struct {
  struct {
    DWORD     baseaddress;
    char      moduletype[7];
    BOOL      triggermatching;
    BOOL      keep_token;
    BOOL      setautoloaduserconfig;
    INT       windowwidth;
    INT       windowoffset;
    INT       extrasearchmargin;
    INT       rejectmargin;
    BOOL      triggertimesubtraction;
    short     edgedetection;
    short     leastsignificantbit;
    short     resolutionleadingwidth;
    short     channeldeadtime;
    BOOL      headertrailer;
    WORD      maximumhitsperevent;
    BOOL      errormark;
    BOOL      errorbypass;
    INT       tdcinteralerror;
    WORD      effectivereadoutfifosize;
    DWORD     channelenableupper;
    DWORD     channelenablelower;
    WORD      globaloffset;
    WORD      channeloffset[128];
    WORD      spare;
    WORD      testmode;
    short     controlregister;
    short     statusregister;
    BOOL      readregisterstoggle;
  } v1190a_0;
  struct {
    DWORD     baseaddress;
    char      moduletype[7];
    BOOL      triggermatching;
    BOOL      keep_token;
    BOOL      setautoloaduserconfig;
    INT       windowwidth;
    INT       windowoffset;
    INT       extrasearchmargin;
    INT       rejectmargin;
    BOOL      triggertimesubtraction;
    short     edgedetection;
    short     leastsignificantbit;
    short     resolutionleadingwidth;
    short     channeldeadtime;
    BOOL      headertrailer;
    WORD      maximumhitsperevent;
    BOOL      errormark;
    BOOL      errorbypass;
    INT       tdcinteralerror;
    WORD      effectivereadoutfifosize;
    DWORD     channelenableupper;
    DWORD     channelenablelower;
    WORD      globaloffset;
    WORD      channeloffset[128];
    WORD      spare;
    WORD      testmode;
    short     controlregister;
    short     statusregister;
    BOOL      readregisterstoggle;
  } v1190a_1;
  struct {
    DWORD     baseaddress;
    char      moduletype[7];
    BOOL      triggermatching;
    BOOL      keep_token;
    BOOL      setautoloaduserconfig;
    INT       windowwidth;
    INT       windowoffset;
    INT       extrasearchmargin;
    INT       rejectmargin;
    BOOL      triggertimesubtraction;
    short     edgedetection;
    short     leastsignificantbit;
    short     resolutionleadingwidth;
    short     channeldeadtime;
    BOOL      headertrailer;
    WORD      maximumhitsperevent;
    BOOL      errormark;
    BOOL      errorbypass;
    INT       tdcinteralerror;
    WORD      effectivereadoutfifosize;
    DWORD     channelenableupper;
    DWORD     channelenablelower;
    WORD      globaloffset;
    WORD      channeloffset[128];
    WORD      spare;
    WORD      testmode;
    short     controlregister;
    short     statusregister;
    BOOL      readregisterstoggle;
  } v1190a_2;
  struct {
    DWORD     baseaddress;
    char      moduletype[7];
    BOOL      triggermatching;
    BOOL      keep_token;
    BOOL      setautoloaduserconfig;
    INT       windowwidth;
    INT       windowoffset;
    INT       extrasearchmargin;
    INT       rejectmargin;
    BOOL      triggertimesubtraction;
    short     edgedetection;
    short     leastsignificantbit;
    short     resolutionleadingwidth;
    short     channeldeadtime;
    BOOL      headertrailer;
    WORD      maximumhitsperevent;
    BOOL      errormark;
    BOOL      errorbypass;
    INT       tdcinteralerror;
    WORD      effectivereadoutfifosize;
    DWORD     channelenableupper;
    DWORD     channelenablelower;
    WORD      globaloffset;
    WORD      channeloffset[128];
    WORD      spare;
    WORD      testmode;
    short     controlregister;
    short     statusregister;
    BOOL      readregisterstoggle;
  } v1190a_3;
  struct {
    DWORD     baseaddress;
    char      moduletype[7];
    BOOL      triggermatching;
    BOOL      keep_token;
    BOOL      setautoloaduserconfig;
    INT       windowwidth;
    INT       windowoffset;
    INT       extrasearchmargin;
    INT       rejectmargin;
    BOOL      triggertimesubtraction;
    short     edgedetection;
    short     leastsignificantbit;
    short     resolutionleadingwidth;
    short     channeldeadtime;
    BOOL      headertrailer;
    WORD      maximumhitsperevent;
    BOOL      errormark;
    BOOL      errorbypass;
    INT       tdcinteralerror;
    WORD      effectivereadoutfifosize;
    DWORD     channelenableupper;
    DWORD     channelenablelower;
    WORD      globaloffset;
    WORD      channeloffset[128];
    WORD      spare;
    WORD      testmode;
    short     controlregister;
    short     statusregister;
    BOOL      readregisterstoggle;
  } v1190a_4;
  struct {
    DWORD     baseaddress;
    char      moduletype[7];
    BOOL      triggermatching;
    BOOL      keep_token;
    BOOL      setautoloaduserconfig;
    INT       windowwidth;
    INT       windowoffset;
    INT       extrasearchmargin;
    INT       rejectmargin;
    BOOL      triggertimesubtraction;
    short     edgedetection;
    short     leastsignificantbit;
    short     resolutionleadingwidth;
    short     channeldeadtime;
    BOOL      headertrailer;
    WORD      maximumhitsperevent;
    BOOL      errormark;
    BOOL      errorbypass;
    INT       tdcinteralerror;
    WORD      effectivereadoutfifosize;
    DWORD     channelenableupper;
    DWORD     channelenablelower;
    WORD      globaloffset;
    WORD      channeloffset[128];
    WORD      spare;
    WORD      testmode;
    short     controlregister;
    short     statusregister;
    BOOL      readregisterstoggle;
  } v1190a_5;
  struct {
    INT       thresholds[16];
  } v792n_0;
  INT       channel2wire[896];
} WIRECHAMBER_SETTINGS;

#define WIRECHAMBER_SETTINGS_STR(_name) char *_name[] = {\
"[V1190A_0]",\
"BaseAddress = DWORD : 0",\
"ModuleType = STRING : [7] ",\
"TriggerMatching = BOOL : n",\
"Keep_Token = BOOL : n",\
"SetAutoLoadUserConfig = BOOL : n",\
"WindowWidth = INT : 0",\
"WindowOffset = INT : 0",\
"ExtraSearchMargin = INT : 0",\
"RejectMargin = INT : 0",\
"TriggerTimeSubtraction = BOOL : n",\
"EdgeDetection = SHORT : 0",\
"LeastSignificantBit = SHORT : 0",\
"ResolutionLeadingWidth = SHORT : 0",\
"ChannelDeadTime = SHORT : 0",\
"HeaderTrailer = BOOL : n",\
"MaximumHitsPerEvent = WORD : 0",\
"ErrorMark = BOOL : n",\
"ErrorBypass = BOOL : n",\
"TDCInteralError = INT : 0",\
"EffectiveReadoutFIFOSize = WORD : 0",\
"ChannelEnableUpper = DWORD : 0",\
"ChannelEnableLower = DWORD : 0",\
"GlobalOffset = WORD : 0",\
"ChannelOffset = WORD[128] :",\
"[0] 0",\
"[1] 0",\
"[2] 0",\
"[3] 0",\
"[4] 0",\
"[5] 0",\
"[6] 0",\
"[7] 0",\
"[8] 0",\
"[9] 0",\
"[10] 0",\
"[11] 0",\
"[12] 0",\
"[13] 0",\
"[14] 0",\
"[15] 0",\
"[16] 0",\
"[17] 0",\
"[18] 0",\
"[19] 0",\
"[20] 0",\
"[21] 0",\
"[22] 0",\
"[23] 0",\
"[24] 0",\
"[25] 0",\
"[26] 0",\
"[27] 0",\
"[28] 0",\
"[29] 0",\
"[30] 0",\
"[31] 0",\
"[32] 0",\
"[33] 0",\
"[34] 0",\
"[35] 0",\
"[36] 0",\
"[37] 0",\
"[38] 0",\
"[39] 0",\
"[40] 0",\
"[41] 0",\
"[42] 0",\
"[43] 0",\
"[44] 0",\
"[45] 0",\
"[46] 0",\
"[47] 0",\
"[48] 0",\
"[49] 0",\
"[50] 0",\
"[51] 0",\
"[52] 0",\
"[53] 0",\
"[54] 0",\
"[55] 0",\
"[56] 0",\
"[57] 0",\
"[58] 0",\
"[59] 0",\
"[60] 0",\
"[61] 0",\
"[62] 0",\
"[63] 0",\
"[64] 0",\
"[65] 0",\
"[66] 0",\
"[67] 0",\
"[68] 0",\
"[69] 0",\
"[70] 0",\
"[71] 0",\
"[72] 0",\
"[73] 0",\
"[74] 0",\
"[75] 0",\
"[76] 0",\
"[77] 0",\
"[78] 0",\
"[79] 0",\
"[80] 0",\
"[81] 0",\
"[82] 0",\
"[83] 0",\
"[84] 0",\
"[85] 0",\
"[86] 0",\
"[87] 0",\
"[88] 0",\
"[89] 0",\
"[90] 0",\
"[91] 0",\
"[92] 0",\
"[93] 0",\
"[94] 0",\
"[95] 0",\
"[96] 0",\
"[97] 0",\
"[98] 0",\
"[99] 0",\
"[100] 0",\
"[101] 0",\
"[102] 0",\
"[103] 0",\
"[104] 0",\
"[105] 0",\
"[106] 0",\
"[107] 0",\
"[108] 0",\
"[109] 0",\
"[110] 0",\
"[111] 0",\
"[112] 0",\
"[113] 0",\
"[114] 0",\
"[115] 0",\
"[116] 0",\
"[117] 0",\
"[118] 0",\
"[119] 0",\
"[120] 0",\
"[121] 0",\
"[122] 0",\
"[123] 0",\
"[124] 0",\
"[125] 0",\
"[126] 0",\
"[127] 0",\
"Spare = WORD : 0",\
"TestMode = WORD : 0",\
"controlregister = SHORT : 0",\
"statusregister = SHORT : 0",\
"readregisterstoggle = BOOL : n",\
"",\
"[V1190A_1]",\
"BaseAddress = DWORD : 0",\
"ModuleType = STRING : [7] ",\
"TriggerMatching = BOOL : n",\
"Keep_Token = BOOL : n",\
"SetAutoLoadUserConfig = BOOL : n",\
"WindowWidth = INT : 0",\
"WindowOffset = INT : 0",\
"ExtraSearchMargin = INT : 0",\
"RejectMargin = INT : 0",\
"TriggerTimeSubtraction = BOOL : n",\
"EdgeDetection = SHORT : 0",\
"LeastSignificantBit = SHORT : 0",\
"ResolutionLeadingWidth = SHORT : 0",\
"ChannelDeadTime = SHORT : 0",\
"HeaderTrailer = BOOL : n",\
"MaximumHitsPerEvent = WORD : 0",\
"ErrorMark = BOOL : n",\
"ErrorBypass = BOOL : n",\
"TDCInteralError = INT : 0",\
"EffectiveReadoutFIFOSize = WORD : 0",\
"ChannelEnableUpper = DWORD : 0",\
"ChannelEnableLower = DWORD : 0",\
"GlobalOffset = WORD : 0",\
"ChannelOffset = WORD[128] :",\
"[0] 0",\
"[1] 0",\
"[2] 0",\
"[3] 0",\
"[4] 0",\
"[5] 0",\
"[6] 0",\
"[7] 0",\
"[8] 0",\
"[9] 0",\
"[10] 0",\
"[11] 0",\
"[12] 0",\
"[13] 0",\
"[14] 0",\
"[15] 0",\
"[16] 0",\
"[17] 0",\
"[18] 0",\
"[19] 0",\
"[20] 0",\
"[21] 0",\
"[22] 0",\
"[23] 0",\
"[24] 0",\
"[25] 0",\
"[26] 0",\
"[27] 0",\
"[28] 0",\
"[29] 0",\
"[30] 0",\
"[31] 0",\
"[32] 0",\
"[33] 0",\
"[34] 0",\
"[35] 0",\
"[36] 0",\
"[37] 0",\
"[38] 0",\
"[39] 0",\
"[40] 0",\
"[41] 0",\
"[42] 0",\
"[43] 0",\
"[44] 0",\
"[45] 0",\
"[46] 0",\
"[47] 0",\
"[48] 0",\
"[49] 0",\
"[50] 0",\
"[51] 0",\
"[52] 0",\
"[53] 0",\
"[54] 0",\
"[55] 0",\
"[56] 0",\
"[57] 0",\
"[58] 0",\
"[59] 0",\
"[60] 0",\
"[61] 0",\
"[62] 0",\
"[63] 0",\
"[64] 0",\
"[65] 0",\
"[66] 0",\
"[67] 0",\
"[68] 0",\
"[69] 0",\
"[70] 0",\
"[71] 0",\
"[72] 0",\
"[73] 0",\
"[74] 0",\
"[75] 0",\
"[76] 0",\
"[77] 0",\
"[78] 0",\
"[79] 0",\
"[80] 0",\
"[81] 0",\
"[82] 0",\
"[83] 0",\
"[84] 0",\
"[85] 0",\
"[86] 0",\
"[87] 0",\
"[88] 0",\
"[89] 0",\
"[90] 0",\
"[91] 0",\
"[92] 0",\
"[93] 0",\
"[94] 0",\
"[95] 0",\
"[96] 0",\
"[97] 0",\
"[98] 0",\
"[99] 0",\
"[100] 0",\
"[101] 0",\
"[102] 0",\
"[103] 0",\
"[104] 0",\
"[105] 0",\
"[106] 0",\
"[107] 0",\
"[108] 0",\
"[109] 0",\
"[110] 0",\
"[111] 0",\
"[112] 0",\
"[113] 0",\
"[114] 0",\
"[115] 0",\
"[116] 0",\
"[117] 0",\
"[118] 0",\
"[119] 0",\
"[120] 0",\
"[121] 0",\
"[122] 0",\
"[123] 0",\
"[124] 0",\
"[125] 0",\
"[126] 0",\
"[127] 0",\
"Spare = WORD : 0",\
"TestMode = WORD : 0",\
"controlregister = SHORT : 0",\
"statusregister = SHORT : 0",\
"readregisterstoggle = BOOL : n",\
"",\
"[V1190A_2]",\
"BaseAddress = DWORD : 0",\
"ModuleType = STRING : [7] ",\
"TriggerMatching = BOOL : n",\
"Keep_Token = BOOL : n",\
"SetAutoLoadUserConfig = BOOL : n",\
"WindowWidth = INT : 0",\
"WindowOffset = INT : 0",\
"ExtraSearchMargin = INT : 0",\
"RejectMargin = INT : 0",\
"TriggerTimeSubtraction = BOOL : n",\
"EdgeDetection = SHORT : 0",\
"LeastSignificantBit = SHORT : 0",\
"ResolutionLeadingWidth = SHORT : 0",\
"ChannelDeadTime = SHORT : 0",\
"HeaderTrailer = BOOL : n",\
"MaximumHitsPerEvent = WORD : 0",\
"ErrorMark = BOOL : n",\
"ErrorBypass = BOOL : n",\
"TDCInteralError = INT : 0",\
"EffectiveReadoutFIFOSize = WORD : 0",\
"ChannelEnableUpper = DWORD : 0",\
"ChannelEnableLower = DWORD : 0",\
"GlobalOffset = WORD : 0",\
"ChannelOffset = WORD[128] :",\
"[0] 0",\
"[1] 0",\
"[2] 0",\
"[3] 0",\
"[4] 0",\
"[5] 0",\
"[6] 0",\
"[7] 0",\
"[8] 0",\
"[9] 0",\
"[10] 0",\
"[11] 0",\
"[12] 0",\
"[13] 0",\
"[14] 0",\
"[15] 0",\
"[16] 0",\
"[17] 0",\
"[18] 0",\
"[19] 0",\
"[20] 0",\
"[21] 0",\
"[22] 0",\
"[23] 0",\
"[24] 0",\
"[25] 0",\
"[26] 0",\
"[27] 0",\
"[28] 0",\
"[29] 0",\
"[30] 0",\
"[31] 0",\
"[32] 0",\
"[33] 0",\
"[34] 0",\
"[35] 0",\
"[36] 0",\
"[37] 0",\
"[38] 0",\
"[39] 0",\
"[40] 0",\
"[41] 0",\
"[42] 0",\
"[43] 0",\
"[44] 0",\
"[45] 0",\
"[46] 0",\
"[47] 0",\
"[48] 0",\
"[49] 0",\
"[50] 0",\
"[51] 0",\
"[52] 0",\
"[53] 0",\
"[54] 0",\
"[55] 0",\
"[56] 0",\
"[57] 0",\
"[58] 0",\
"[59] 0",\
"[60] 0",\
"[61] 0",\
"[62] 0",\
"[63] 0",\
"[64] 0",\
"[65] 0",\
"[66] 0",\
"[67] 0",\
"[68] 0",\
"[69] 0",\
"[70] 0",\
"[71] 0",\
"[72] 0",\
"[73] 0",\
"[74] 0",\
"[75] 0",\
"[76] 0",\
"[77] 0",\
"[78] 0",\
"[79] 0",\
"[80] 0",\
"[81] 0",\
"[82] 0",\
"[83] 0",\
"[84] 0",\
"[85] 0",\
"[86] 0",\
"[87] 0",\
"[88] 0",\
"[89] 0",\
"[90] 0",\
"[91] 0",\
"[92] 0",\
"[93] 0",\
"[94] 0",\
"[95] 0",\
"[96] 0",\
"[97] 0",\
"[98] 0",\
"[99] 0",\
"[100] 0",\
"[101] 0",\
"[102] 0",\
"[103] 0",\
"[104] 0",\
"[105] 0",\
"[106] 0",\
"[107] 0",\
"[108] 0",\
"[109] 0",\
"[110] 0",\
"[111] 0",\
"[112] 0",\
"[113] 0",\
"[114] 0",\
"[115] 0",\
"[116] 0",\
"[117] 0",\
"[118] 0",\
"[119] 0",\
"[120] 0",\
"[121] 0",\
"[122] 0",\
"[123] 0",\
"[124] 0",\
"[125] 0",\
"[126] 0",\
"[127] 0",\
"Spare = WORD : 0",\
"TestMode = WORD : 0",\
"controlregister = SHORT : 0",\
"statusregister = SHORT : 0",\
"readregisterstoggle = BOOL : n",\
"",\
"[V1190A_3]",\
"BaseAddress = DWORD : 0",\
"ModuleType = STRING : [7] ",\
"TriggerMatching = BOOL : n",\
"Keep_Token = BOOL : n",\
"SetAutoLoadUserConfig = BOOL : n",\
"WindowWidth = INT : 0",\
"WindowOffset = INT : 0",\
"ExtraSearchMargin = INT : 0",\
"RejectMargin = INT : 0",\
"TriggerTimeSubtraction = BOOL : n",\
"EdgeDetection = SHORT : 0",\
"LeastSignificantBit = SHORT : 0",\
"ResolutionLeadingWidth = SHORT : 0",\
"ChannelDeadTime = SHORT : 0",\
"HeaderTrailer = BOOL : n",\
"MaximumHitsPerEvent = WORD : 0",\
"ErrorMark = BOOL : n",\
"ErrorBypass = BOOL : n",\
"TDCInteralError = INT : 0",\
"EffectiveReadoutFIFOSize = WORD : 0",\
"ChannelEnableUpper = DWORD : 0",\
"ChannelEnableLower = DWORD : 0",\
"GlobalOffset = WORD : 0",\
"ChannelOffset = WORD[128] :",\
"[0] 0",\
"[1] 0",\
"[2] 0",\
"[3] 0",\
"[4] 0",\
"[5] 0",\
"[6] 0",\
"[7] 0",\
"[8] 0",\
"[9] 0",\
"[10] 0",\
"[11] 0",\
"[12] 0",\
"[13] 0",\
"[14] 0",\
"[15] 0",\
"[16] 0",\
"[17] 0",\
"[18] 0",\
"[19] 0",\
"[20] 0",\
"[21] 0",\
"[22] 0",\
"[23] 0",\
"[24] 0",\
"[25] 0",\
"[26] 0",\
"[27] 0",\
"[28] 0",\
"[29] 0",\
"[30] 0",\
"[31] 0",\
"[32] 0",\
"[33] 0",\
"[34] 0",\
"[35] 0",\
"[36] 0",\
"[37] 0",\
"[38] 0",\
"[39] 0",\
"[40] 0",\
"[41] 0",\
"[42] 0",\
"[43] 0",\
"[44] 0",\
"[45] 0",\
"[46] 0",\
"[47] 0",\
"[48] 0",\
"[49] 0",\
"[50] 0",\
"[51] 0",\
"[52] 0",\
"[53] 0",\
"[54] 0",\
"[55] 0",\
"[56] 0",\
"[57] 0",\
"[58] 0",\
"[59] 0",\
"[60] 0",\
"[61] 0",\
"[62] 0",\
"[63] 0",\
"[64] 0",\
"[65] 0",\
"[66] 0",\
"[67] 0",\
"[68] 0",\
"[69] 0",\
"[70] 0",\
"[71] 0",\
"[72] 0",\
"[73] 0",\
"[74] 0",\
"[75] 0",\
"[76] 0",\
"[77] 0",\
"[78] 0",\
"[79] 0",\
"[80] 0",\
"[81] 0",\
"[82] 0",\
"[83] 0",\
"[84] 0",\
"[85] 0",\
"[86] 0",\
"[87] 0",\
"[88] 0",\
"[89] 0",\
"[90] 0",\
"[91] 0",\
"[92] 0",\
"[93] 0",\
"[94] 0",\
"[95] 0",\
"[96] 0",\
"[97] 0",\
"[98] 0",\
"[99] 0",\
"[100] 0",\
"[101] 0",\
"[102] 0",\
"[103] 0",\
"[104] 0",\
"[105] 0",\
"[106] 0",\
"[107] 0",\
"[108] 0",\
"[109] 0",\
"[110] 0",\
"[111] 0",\
"[112] 0",\
"[113] 0",\
"[114] 0",\
"[115] 0",\
"[116] 0",\
"[117] 0",\
"[118] 0",\
"[119] 0",\
"[120] 0",\
"[121] 0",\
"[122] 0",\
"[123] 0",\
"[124] 0",\
"[125] 0",\
"[126] 0",\
"[127] 0",\
"Spare = WORD : 0",\
"TestMode = WORD : 0",\
"controlregister = SHORT : 0",\
"statusregister = SHORT : 0",\
"readregisterstoggle = BOOL : n",\
"",\
"[V1190A_4]",\
"BaseAddress = DWORD : 0",\
"ModuleType = STRING : [7] ",\
"TriggerMatching = BOOL : n",\
"Keep_Token = BOOL : n",\
"SetAutoLoadUserConfig = BOOL : n",\
"WindowWidth = INT : 0",\
"WindowOffset = INT : 0",\
"ExtraSearchMargin = INT : 0",\
"RejectMargin = INT : 0",\
"TriggerTimeSubtraction = BOOL : n",\
"EdgeDetection = SHORT : 0",\
"LeastSignificantBit = SHORT : 0",\
"ResolutionLeadingWidth = SHORT : 0",\
"ChannelDeadTime = SHORT : 0",\
"HeaderTrailer = BOOL : n",\
"MaximumHitsPerEvent = WORD : 0",\
"ErrorMark = BOOL : n",\
"ErrorBypass = BOOL : n",\
"TDCInteralError = INT : 0",\
"EffectiveReadoutFIFOSize = WORD : 0",\
"ChannelEnableUpper = DWORD : 0",\
"ChannelEnableLower = DWORD : 0",\
"GlobalOffset = WORD : 0",\
"ChannelOffset = WORD[128] :",\
"[0] 0",\
"[1] 0",\
"[2] 0",\
"[3] 0",\
"[4] 0",\
"[5] 0",\
"[6] 0",\
"[7] 0",\
"[8] 0",\
"[9] 0",\
"[10] 0",\
"[11] 0",\
"[12] 0",\
"[13] 0",\
"[14] 0",\
"[15] 0",\
"[16] 0",\
"[17] 0",\
"[18] 0",\
"[19] 0",\
"[20] 0",\
"[21] 0",\
"[22] 0",\
"[23] 0",\
"[24] 0",\
"[25] 0",\
"[26] 0",\
"[27] 0",\
"[28] 0",\
"[29] 0",\
"[30] 0",\
"[31] 0",\
"[32] 0",\
"[33] 0",\
"[34] 0",\
"[35] 0",\
"[36] 0",\
"[37] 0",\
"[38] 0",\
"[39] 0",\
"[40] 0",\
"[41] 0",\
"[42] 0",\
"[43] 0",\
"[44] 0",\
"[45] 0",\
"[46] 0",\
"[47] 0",\
"[48] 0",\
"[49] 0",\
"[50] 0",\
"[51] 0",\
"[52] 0",\
"[53] 0",\
"[54] 0",\
"[55] 0",\
"[56] 0",\
"[57] 0",\
"[58] 0",\
"[59] 0",\
"[60] 0",\
"[61] 0",\
"[62] 0",\
"[63] 0",\
"[64] 0",\
"[65] 0",\
"[66] 0",\
"[67] 0",\
"[68] 0",\
"[69] 0",\
"[70] 0",\
"[71] 0",\
"[72] 0",\
"[73] 0",\
"[74] 0",\
"[75] 0",\
"[76] 0",\
"[77] 0",\
"[78] 0",\
"[79] 0",\
"[80] 0",\
"[81] 0",\
"[82] 0",\
"[83] 0",\
"[84] 0",\
"[85] 0",\
"[86] 0",\
"[87] 0",\
"[88] 0",\
"[89] 0",\
"[90] 0",\
"[91] 0",\
"[92] 0",\
"[93] 0",\
"[94] 0",\
"[95] 0",\
"[96] 0",\
"[97] 0",\
"[98] 0",\
"[99] 0",\
"[100] 0",\
"[101] 0",\
"[102] 0",\
"[103] 0",\
"[104] 0",\
"[105] 0",\
"[106] 0",\
"[107] 0",\
"[108] 0",\
"[109] 0",\
"[110] 0",\
"[111] 0",\
"[112] 0",\
"[113] 0",\
"[114] 0",\
"[115] 0",\
"[116] 0",\
"[117] 0",\
"[118] 0",\
"[119] 0",\
"[120] 0",\
"[121] 0",\
"[122] 0",\
"[123] 0",\
"[124] 0",\
"[125] 0",\
"[126] 0",\
"[127] 0",\
"Spare = WORD : 0",\
"TestMode = WORD : 0",\
"controlregister = SHORT : 0",\
"statusregister = SHORT : 0",\
"readregisterstoggle = BOOL : n",\
"",\
"[V1190A_5]",\
"BaseAddress = DWORD : 0",\
"ModuleType = STRING : [7] ",\
"TriggerMatching = BOOL : n",\
"Keep_Token = BOOL : n",\
"SetAutoLoadUserConfig = BOOL : n",\
"WindowWidth = INT : 0",\
"WindowOffset = INT : 0",\
"ExtraSearchMargin = INT : 0",\
"RejectMargin = INT : 0",\
"TriggerTimeSubtraction = BOOL : n",\
"EdgeDetection = SHORT : 0",\
"LeastSignificantBit = SHORT : 0",\
"ResolutionLeadingWidth = SHORT : 0",\
"ChannelDeadTime = SHORT : 0",\
"HeaderTrailer = BOOL : n",\
"MaximumHitsPerEvent = WORD : 0",\
"ErrorMark = BOOL : n",\
"ErrorBypass = BOOL : n",\
"TDCInteralError = INT : 0",\
"EffectiveReadoutFIFOSize = WORD : 0",\
"ChannelEnableUpper = DWORD : 0",\
"ChannelEnableLower = DWORD : 0",\
"GlobalOffset = WORD : 0",\
"ChannelOffset = WORD[128] :",\
"[0] 0",\
"[1] 0",\
"[2] 0",\
"[3] 0",\
"[4] 0",\
"[5] 0",\
"[6] 0",\
"[7] 0",\
"[8] 0",\
"[9] 0",\
"[10] 0",\
"[11] 0",\
"[12] 0",\
"[13] 0",\
"[14] 0",\
"[15] 0",\
"[16] 0",\
"[17] 0",\
"[18] 0",\
"[19] 0",\
"[20] 0",\
"[21] 0",\
"[22] 0",\
"[23] 0",\
"[24] 0",\
"[25] 0",\
"[26] 0",\
"[27] 0",\
"[28] 0",\
"[29] 0",\
"[30] 0",\
"[31] 0",\
"[32] 0",\
"[33] 0",\
"[34] 0",\
"[35] 0",\
"[36] 0",\
"[37] 0",\
"[38] 0",\
"[39] 0",\
"[40] 0",\
"[41] 0",\
"[42] 0",\
"[43] 0",\
"[44] 0",\
"[45] 0",\
"[46] 0",\
"[47] 0",\
"[48] 0",\
"[49] 0",\
"[50] 0",\
"[51] 0",\
"[52] 0",\
"[53] 0",\
"[54] 0",\
"[55] 0",\
"[56] 0",\
"[57] 0",\
"[58] 0",\
"[59] 0",\
"[60] 0",\
"[61] 0",\
"[62] 0",\
"[63] 0",\
"[64] 0",\
"[65] 0",\
"[66] 0",\
"[67] 0",\
"[68] 0",\
"[69] 0",\
"[70] 0",\
"[71] 0",\
"[72] 0",\
"[73] 0",\
"[74] 0",\
"[75] 0",\
"[76] 0",\
"[77] 0",\
"[78] 0",\
"[79] 0",\
"[80] 0",\
"[81] 0",\
"[82] 0",\
"[83] 0",\
"[84] 0",\
"[85] 0",\
"[86] 0",\
"[87] 0",\
"[88] 0",\
"[89] 0",\
"[90] 0",\
"[91] 0",\
"[92] 0",\
"[93] 0",\
"[94] 0",\
"[95] 0",\
"[96] 0",\
"[97] 0",\
"[98] 0",\
"[99] 0",\
"[100] 0",\
"[101] 0",\
"[102] 0",\
"[103] 0",\
"[104] 0",\
"[105] 0",\
"[106] 0",\
"[107] 0",\
"[108] 0",\
"[109] 0",\
"[110] 0",\
"[111] 0",\
"[112] 0",\
"[113] 0",\
"[114] 0",\
"[115] 0",\
"[116] 0",\
"[117] 0",\
"[118] 0",\
"[119] 0",\
"[120] 0",\
"[121] 0",\
"[122] 0",\
"[123] 0",\
"[124] 0",\
"[125] 0",\
"[126] 0",\
"[127] 0",\
"Spare = WORD : 0",\
"TestMode = WORD : 0",\
"controlregister = SHORT : 0",\
"statusregister = SHORT : 0",\
"readregisterstoggle = BOOL : n",\
"",\
"[V792N_0]",\
"Thresholds = INT[16] :",\
"[0] 7",\
"[1] 7",\
"[2] 7",\
"[3] 7",\
"[4] 511",\
"[5] 511",\
"[6] 511",\
"[7] 7",\
"[8] 511",\
"[9] 511",\
"[10] 511",\
"[11] 511",\
"[12] 511",\
"[13] 511",\
"[14] 511",\
"[15] 511",\
"",\
"[.]",\
"Channel2Wire = INT[896] :",\
"[0] 0",\
"[1] 0",\
"[2] 0",\
"[3] 0",\
"[4] 0",\
"[5] 0",\
"[6] 0",\
"[7] 0",\
"[8] 0",\
"[9] 0",\
"[10] 0",\
"[11] 0",\
"[12] 0",\
"[13] 0",\
"[14] 0",\
"[15] 0",\
"[16] 10",\
"[17] 11",\
"[18] 12",\
"[19] 13",\
"[20] 14",\
"[21] 15",\
"[22] 0",\
"[23] 0",\
"[24] 0",\
"[25] 0",\
"[26] 0",\
"[27] 0",\
"[28] 0",\
"[29] 0",\
"[30] 0",\
"[31] 0",\
"[32] 16",\
"[33] 17",\
"[34] 18",\
"[35] 19",\
"[36] 20",\
"[37] 21",\
"[38] 22",\
"[39] 23",\
"[40] 24",\
"[41] 25",\
"[42] 26",\
"[43] 27",\
"[44] 28",\
"[45] 29",\
"[46] 30",\
"[47] 31",\
"[48] 32",\
"[49] 33",\
"[50] 34",\
"[51] 35",\
"[52] 36",\
"[53] 37",\
"[54] 38",\
"[55] 39",\
"[56] 40",\
"[57] 41",\
"[58] 42",\
"[59] 43",\
"[60] 44",\
"[61] 45",\
"[62] 46",\
"[63] 47",\
"[64] 48",\
"[65] 49",\
"[66] 50",\
"[67] 51",\
"[68] 52",\
"[69] 53",\
"[70] 54",\
"[71] 55",\
"[72] 56",\
"[73] 57",\
"[74] 58",\
"[75] 59",\
"[76] 60",\
"[77] 61",\
"[78] 62",\
"[79] 63",\
"[80] 64",\
"[81] 65",\
"[82] 66",\
"[83] 67",\
"[84] 68",\
"[85] 69",\
"[86] 70",\
"[87] 71",\
"[88] 72",\
"[89] 73",\
"[90] 74",\
"[91] 75",\
"[92] 76",\
"[93] 77",\
"[94] 78",\
"[95] 79",\
"[96] 80",\
"[97] 81",\
"[98] 82",\
"[99] 83",\
"[100] 84",\
"[101] 85",\
"[102] 86",\
"[103] 87",\
"[104] 88",\
"[105] 89",\
"[106] 90",\
"[107] 91",\
"[108] 92",\
"[109] 93",\
"[110] 94",\
"[111] 95",\
"[112] 96",\
"[113] 97",\
"[114] 98",\
"[115] 99",\
"[116] 100",\
"[117] 101",\
"[118] 102",\
"[119] 103",\
"[120] 104",\
"[121] 105",\
"[122] 106",\
"[123] 107",\
"[124] 108",\
"[125] 109",\
"[126] 110",\
"[127] 111",\
"[128] 0",\
"[129] 0",\
"[130] 0",\
"[131] 0",\
"[132] 0",\
"[133] 0",\
"[134] 0",\
"[135] 0",\
"[136] 0",\
"[137] 0",\
"[138] 0",\
"[139] 0",\
"[140] 0",\
"[141] 0",\
"[142] 0",\
"[143] 0",\
"[144] 112",\
"[145] 113",\
"[146] 114",\
"[147] 115",\
"[148] 116",\
"[149] 117",\
"[150] 118",\
"[151] 119",\
"[152] 120",\
"[153] 121",\
"[154] 122",\
"[155] 123",\
"[156] 124",\
"[157] 125",\
"[158] 126",\
"[159] 127",\
"[160] 128",\
"[161] 129",\
"[162] 130",\
"[163] 131",\
"[164] 132",\
"[165] 133",\
"[166] 134",\
"[167] 135",\
"[168] 136",\
"[169] 137",\
"[170] 138",\
"[171] 139",\
"[172] 140",\
"[173] 141",\
"[174] 142",\
"[175] 143",\
"[176] 144",\
"[177] 145",\
"[178] 146",\
"[179] 147",\
"[180] 148",\
"[181] 149",\
"[182] 150",\
"[183] 151",\
"[184] 152",\
"[185] 153",\
"[186] 154",\
"[187] 155",\
"[188] 156",\
"[189] 157",\
"[190] 158",\
"[191] 159",\
"[192] 160",\
"[193] 161",\
"[194] 162",\
"[195] 163",\
"[196] 164",\
"[197] 165",\
"[198] 166",\
"[199] 167",\
"[200] 168",\
"[201] 169",\
"[202] 170",\
"[203] 171",\
"[204] 172",\
"[205] 173",\
"[206] 174",\
"[207] 175",\
"[208] 176",\
"[209] 177",\
"[210] 178",\
"[211] 179",\
"[212] 180",\
"[213] 181",\
"[214] 182",\
"[215] 183",\
"[216] 184",\
"[217] 185",\
"[218] 186",\
"[219] 187",\
"[220] 188",\
"[221] 189",\
"[222] 190",\
"[223] 191",\
"[224] 192",\
"[225] 193",\
"[226] 194",\
"[227] 195",\
"[228] 196",\
"[229] 197",\
"[230] 198",\
"[231] 199",\
"[232] 200",\
"[233] 201",\
"[234] 202",\
"[235] 203",\
"[236] 204",\
"[237] 205",\
"[238] 206",\
"[239] 207",\
"[240] 315",\
"[241] 314",\
"[242] 313",\
"[243] 312",\
"[244] 311",\
"[245] 310",\
"[246] 309",\
"[247] 308",\
"[248] 307",\
"[249] 306",\
"[250] 305",\
"[251] 304",\
"[252] 303",\
"[253] 302",\
"[254] 301",\
"[255] 300",\
"[256] 0",\
"[257] 0",\
"[258] 0",\
"[259] 0",\
"[260] 0",\
"[261] 0",\
"[262] 0",\
"[263] 0",\
"[264] 0",\
"[265] 0",\
"[266] 0",\
"[267] 0",\
"[268] 0",\
"[269] 0",\
"[270] 0",\
"[271] 0",\
"[272] 331",\
"[273] 330",\
"[274] 329",\
"[275] 328",\
"[276] 327",\
"[277] 326",\
"[278] 325",\
"[279] 324",\
"[280] 323",\
"[281] 322",\
"[282] 321",\
"[283] 320",\
"[284] 319",\
"[285] 318",\
"[286] 317",\
"[287] 316",\
"[288] 347",\
"[289] 346",\
"[290] 345",\
"[291] 344",\
"[292] 343",\
"[293] 342",\
"[294] 341",\
"[295] 340",\
"[296] 339",\
"[297] 338",\
"[298] 337",\
"[299] 336",\
"[300] 335",\
"[301] 334",\
"[302] 333",\
"[303] 332",\
"[304] 363",\
"[305] 362",\
"[306] 361",\
"[307] 360",\
"[308] 359",\
"[309] 358",\
"[310] 357",\
"[311] 356",\
"[312] 355",\
"[313] 354",\
"[314] 353",\
"[315] 352",\
"[316] 351",\
"[317] 350",\
"[318] 349",\
"[319] 348",\
"[320] 379",\
"[321] 378",\
"[322] 377",\
"[323] 376",\
"[324] 375",\
"[325] 374",\
"[326] 373",\
"[327] 372",\
"[328] 371",\
"[329] 370",\
"[330] 369",\
"[331] 368",\
"[332] 367",\
"[333] 366",\
"[334] 365",\
"[335] 364",\
"[336] 395",\
"[337] 394",\
"[338] 393",\
"[339] 392",\
"[340] 391",\
"[341] 390",\
"[342] 389",\
"[343] 388",\
"[344] 387",\
"[345] 386",\
"[346] 385",\
"[347] 384",\
"[348] 383",\
"[349] 382",\
"[350] 381",\
"[351] 380",\
"[352] 411",\
"[353] 410",\
"[354] 409",\
"[355] 408",\
"[356] 407",\
"[357] 406",\
"[358] 405",\
"[359] 404",\
"[360] 403",\
"[361] 402",\
"[362] 401",\
"[363] 400",\
"[364] 399",\
"[365] 398",\
"[366] 397",\
"[367] 396",\
"[368] 427",\
"[369] 426",\
"[370] 425",\
"[371] 424",\
"[372] 423",\
"[373] 422",\
"[374] 421",\
"[375] 420",\
"[376] 419",\
"[377] 418",\
"[378] 417",\
"[379] 416",\
"[380] 415",\
"[381] 414",\
"[382] 413",\
"[383] 412",\
"[384] 0",\
"[385] 0",\
"[386] 0",\
"[387] 0",\
"[388] 0",\
"[389] 0",\
"[390] 0",\
"[391] 0",\
"[392] 0",\
"[393] 0",\
"[394] 0",\
"[395] 0",\
"[396] 0",\
"[397] 0",\
"[398] 0",\
"[399] 0",\
"[400] 443",\
"[401] 442",\
"[402] 441",\
"[403] 440",\
"[404] 439",\
"[405] 438",\
"[406] 437",\
"[407] 436",\
"[408] 435",\
"[409] 434",\
"[410] 433",\
"[411] 432",\
"[412] 431",\
"[413] 430",\
"[414] 429",\
"[415] 428",\
"[416] 510",\
"[417] 511",\
"[418] 512",\
"[419] 513",\
"[420] 514",\
"[421] 515",\
"[422] 0",\
"[423] 0",\
"[424] 0",\
"[425] 0",\
"[426] 0",\
"[427] 0",\
"[428] 0",\
"[429] 0",\
"[430] 0",\
"[431] 0",\
"[432] 516",\
"[433] 517",\
"[434] 518",\
"[435] 519",\
"[436] 520",\
"[437] 521",\
"[438] 522",\
"[439] 523",\
"[440] 524",\
"[441] 525",\
"[442] 526",\
"[443] 527",\
"[444] 528",\
"[445] 529",\
"[446] 530",\
"[447] 531",\
"[448] 532",\
"[449] 533",\
"[450] 534",\
"[451] 535",\
"[452] 536",\
"[453] 537",\
"[454] 538",\
"[455] 539",\
"[456] 540",\
"[457] 541",\
"[458] 542",\
"[459] 543",\
"[460] 544",\
"[461] 545",\
"[462] 546",\
"[463] 547",\
"[464] 548",\
"[465] 549",\
"[466] 550",\
"[467] 551",\
"[468] 552",\
"[469] 553",\
"[470] 554",\
"[471] 555",\
"[472] 556",\
"[473] 557",\
"[474] 558",\
"[475] 559",\
"[476] 560",\
"[477] 561",\
"[478] 562",\
"[479] 563",\
"[480] 564",\
"[481] 565",\
"[482] 566",\
"[483] 567",\
"[484] 568",\
"[485] 569",\
"[486] 570",\
"[487] 571",\
"[488] 572",\
"[489] 573",\
"[490] 574",\
"[491] 575",\
"[492] 576",\
"[493] 577",\
"[494] 578",\
"[495] 579",\
"[496] 580",\
"[497] 581",\
"[498] 582",\
"[499] 583",\
"[500] 584",\
"[501] 585",\
"[502] 586",\
"[503] 587",\
"[504] 588",\
"[505] 589",\
"[506] 590",\
"[507] 591",\
"[508] 592",\
"[509] 593",\
"[510] 594",\
"[511] 595",\
"[512] 0",\
"[513] 0",\
"[514] 0",\
"[515] 0",\
"[516] 0",\
"[517] 0",\
"[518] 0",\
"[519] 0",\
"[520] 0",\
"[521] 0",\
"[522] 0",\
"[523] 0",\
"[524] 0",\
"[525] 0",\
"[526] 0",\
"[527] 0",\
"[528] 596",\
"[529] 597",\
"[530] 598",\
"[531] 599",\
"[532] 600",\
"[533] 601",\
"[534] 602",\
"[535] 603",\
"[536] 604",\
"[537] 605",\
"[538] 606",\
"[539] 607",\
"[540] 608",\
"[541] 609",\
"[542] 610",\
"[543] 611",\
"[544] 612",\
"[545] 613",\
"[546] 614",\
"[547] 615",\
"[548] 616",\
"[549] 617",\
"[550] 618",\
"[551] 619",\
"[552] 620",\
"[553] 621",\
"[554] 622",\
"[555] 623",\
"[556] 624",\
"[557] 625",\
"[558] 626",\
"[559] 627",\
"[560] 628",\
"[561] 629",\
"[562] 630",\
"[563] 631",\
"[564] 632",\
"[565] 633",\
"[566] 634",\
"[567] 635",\
"[568] 636",\
"[569] 637",\
"[570] 638",\
"[571] 639",\
"[572] 640",\
"[573] 641",\
"[574] 642",\
"[575] 643",\
"[576] 644",\
"[577] 645",\
"[578] 646",\
"[579] 647",\
"[580] 648",\
"[581] 649",\
"[582] 650",\
"[583] 651",\
"[584] 652",\
"[585] 653",\
"[586] 654",\
"[587] 655",\
"[588] 656",\
"[589] 657",\
"[590] 658",\
"[591] 659",\
"[592] 660",\
"[593] 661",\
"[594] 662",\
"[595] 663",\
"[596] 664",\
"[597] 665",\
"[598] 666",\
"[599] 667",\
"[600] 668",\
"[601] 669",\
"[602] 670",\
"[603] 671",\
"[604] 672",\
"[605] 673",\
"[606] 674",\
"[607] 675",\
"[608] 676",\
"[609] 677",\
"[610] 678",\
"[611] 679",\
"[612] 680",\
"[613] 681",\
"[614] 682",\
"[615] 683",\
"[616] 684",\
"[617] 685",\
"[618] 686",\
"[619] 687",\
"[620] 688",\
"[621] 689",\
"[622] 690",\
"[623] 691",\
"[624] 692",\
"[625] 693",\
"[626] 694",\
"[627] 695",\
"[628] 696",\
"[629] 697",\
"[630] 698",\
"[631] 699",\
"[632] 700",\
"[633] 701",\
"[634] 702",\
"[635] 703",\
"[636] 704",\
"[637] 705",\
"[638] 706",\
"[639] 707",\
"[640] 0",\
"[641] 0",\
"[642] 0",\
"[643] 0",\
"[644] 0",\
"[645] 0",\
"[646] 0",\
"[647] 0",\
"[648] 0",\
"[649] 0",\
"[650] 0",\
"[651] 0",\
"[652] 0",\
"[653] 0",\
"[654] 0",\
"[655] 0",\
"[656] 815",\
"[657] 814",\
"[658] 813",\
"[659] 812",\
"[660] 811",\
"[661] 810",\
"[662] 809",\
"[663] 808",\
"[664] 807",\
"[665] 806",\
"[666] 805",\
"[667] 804",\
"[668] 803",\
"[669] 802",\
"[670] 801",\
"[671] 800",\
"[672] 831",\
"[673] 830",\
"[674] 829",\
"[675] 828",\
"[676] 827",\
"[677] 826",\
"[678] 825",\
"[679] 824",\
"[680] 823",\
"[681] 822",\
"[682] 821",\
"[683] 820",\
"[684] 819",\
"[685] 818",\
"[686] 817",\
"[687] 816",\
"[688] 847",\
"[689] 846",\
"[690] 845",\
"[691] 844",\
"[692] 843",\
"[693] 842",\
"[694] 841",\
"[695] 840",\
"[696] 839",\
"[697] 838",\
"[698] 837",\
"[699] 836",\
"[700] 835",\
"[701] 834",\
"[702] 833",\
"[703] 832",\
"[704] 863",\
"[705] 862",\
"[706] 861",\
"[707] 860",\
"[708] 859",\
"[709] 858",\
"[710] 857",\
"[711] 856",\
"[712] 855",\
"[713] 854",\
"[714] 853",\
"[715] 852",\
"[716] 851",\
"[717] 850",\
"[718] 849",\
"[719] 848",\
"[720] 879",\
"[721] 878",\
"[722] 877",\
"[723] 876",\
"[724] 875",\
"[725] 874",\
"[726] 873",\
"[727] 872",\
"[728] 871",\
"[729] 870",\
"[730] 869",\
"[731] 868",\
"[732] 867",\
"[733] 866",\
"[734] 865",\
"[735] 864",\
"[736] 895",\
"[737] 894",\
"[738] 893",\
"[739] 892",\
"[740] 891",\
"[741] 890",\
"[742] 889",\
"[743] 888",\
"[744] 887",\
"[745] 886",\
"[746] 885",\
"[747] 884",\
"[748] 883",\
"[749] 882",\
"[750] 881",\
"[751] 880",\
"[752] 911",\
"[753] 910",\
"[754] 909",\
"[755] 908",\
"[756] 907",\
"[757] 906",\
"[758] 905",\
"[759] 904",\
"[760] 903",\
"[761] 902",\
"[762] 901",\
"[763] 900",\
"[764] 899",\
"[765] 898",\
"[766] 897",\
"[767] 896",\
"[768] 0",\
"[769] 0",\
"[770] 0",\
"[771] 0",\
"[772] 0",\
"[773] 0",\
"[774] 0",\
"[775] 0",\
"[776] 0",\
"[777] 0",\
"[778] 0",\
"[779] 0",\
"[780] 0",\
"[781] 0",\
"[782] 0",\
"[783] 0",\
"[784] 927",\
"[785] 926",\
"[786] 925",\
"[787] 924",\
"[788] 923",\
"[789] 922",\
"[790] 921",\
"[791] 920",\
"[792] 919",\
"[793] 918",\
"[794] 917",\
"[795] 916",\
"[796] 915",\
"[797] 914",\
"[798] 913",\
"[799] 912",\
"[800] 943",\
"[801] 942",\
"[802] 941",\
"[803] 940",\
"[804] 939",\
"[805] 938",\
"[806] 937",\
"[807] 936",\
"[808] 935",\
"[809] 934",\
"[810] 933",\
"[811] 932",\
"[812] 931",\
"[813] 930",\
"[814] 929",\
"[815] 928",\
"[816] 0",\
"[817] 0",\
"[818] 0",\
"[819] 0",\
"[820] 0",\
"[821] 0",\
"[822] 0",\
"[823] 0",\
"[824] 0",\
"[825] 0",\
"[826] 0",\
"[827] 0",\
"[828] 0",\
"[829] 0",\
"[830] 0",\
"[831] 0",\
"[832] 0",\
"[833] 0",\
"[834] 0",\
"[835] 0",\
"[836] 0",\
"[837] 0",\
"[838] 0",\
"[839] 0",\
"[840] 0",\
"[841] 0",\
"[842] 0",\
"[843] 0",\
"[844] 0",\
"[845] 0",\
"[846] 0",\
"[847] 0",\
"[848] 0",\
"[849] 0",\
"[850] 0",\
"[851] 0",\
"[852] 0",\
"[853] 0",\
"[854] 0",\
"[855] 0",\
"[856] 0",\
"[857] 0",\
"[858] 0",\
"[859] 0",\
"[860] 0",\
"[861] 0",\
"[862] 0",\
"[863] 0",\
"[864] 0",\
"[865] 0",\
"[866] 0",\
"[867] 0",\
"[868] 0",\
"[869] 0",\
"[870] 0",\
"[871] 0",\
"[872] 0",\
"[873] 0",\
"[874] 0",\
"[875] 0",\
"[876] 0",\
"[877] 0",\
"[878] 0",\
"[879] 0",\
"[880] 0",\
"[881] 0",\
"[882] 0",\
"[883] 0",\
"[884] 0",\
"[885] 0",\
"[886] 0",\
"[887] 0",\
"[888] 0",\
"[889] 0",\
"[890] 0",\
"[891] 0",\
"[892] 0",\
"[893] 0",\
"[894] 0",\
"[895] 0",\
"",\
NULL }

#define WIRECHAMBER_COMMON_DEFINED

typedef struct {
  WORD      event_id;
  WORD      trigger_mask;
  char      buffer[32];
  INT       type;
  INT       source;
  char      format[8];
  BOOL      enabled;
  INT       read_on;
  INT       period;
  double    event_limit;
  DWORD     num_subevents;
  INT       log_history;
  char      frontend_host[32];
  char      frontend_name[32];
  char      frontend_file_name[256];
} WIRECHAMBER_COMMON;

#define WIRECHAMBER_COMMON_STR(_name) char *_name[] = {\
"[.]",\
"Event ID = WORD : 1",\
"Trigger mask = WORD : 0",\
"Buffer = STRING : [32] SYSTEM",\
"Type = INT : 2",\
"Source = INT : 16777215",\
"Format = STRING : [8] MIDAS",\
"Enabled = BOOL : y",\
"Read on = INT : 257",\
"Period = INT : 500",\
"Event limit = DOUBLE : 0",\
"Num subevents = DWORD : 0",\
"Log history = INT : 0",\
"Frontend host = STRING : [32] vmevmic2",\
"Frontend name = STRING : [32] K600 frontend",\
"Frontend file name = STRING : [256] k600frontend.c",\
"",\
NULL }

#endif


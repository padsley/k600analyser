int ADCModules = 4;

int TDCModules = 6;

int NumberOfMMM = 4;

int MMMADCChannelLimits[4][4] = {{0, 15, 80, 87},{0+16, 15+16, 80+8, 87+8},{0+32, 15+32, 80+16, 87+16},{0+48, 15+48, 80+24, 87+24}};
//int MMMTDCChannelLimits[4][4] = {{0, 15, 80, 87},{0+16, 15+16, 80+8, 87+8},{0+32, 15+32, 80+16, 87+16},{0+48, 15+48, 80+24, 87+24}};
int MMMTDCChannelLimits[4][2] = {{6*128+48,128*6+48+15},{128*6+48+16,128*6+48+31},{128*6+48+32,128*6+48+47},{128*6+48+48,128*6+48+63}};


int PulseLimits[2] = {1800, 2400};

//double silicon_offset[128] = {0,-274.08,-320.268,-276.639,-307.713,-277.398,-288.216,-309.729,-265.351,-298.041,-321.789,-356.177,-344.563,-323.221,-218.666,-344.563,-297.208,-247.971,-228.059,-290.765,-238.934,-248.182,-216.219,-287.868,-256.936,-321.205,-300.341,-301.549,-341.153,-290.028,-299.167,-288.63,-268.313,0,-190.629,-244.673,-187.919,-288.876,-265.923,-233.731,-242.76,-309.83,-233.08,-232.805,-248.182,-266.349,-229.507,-266.19,-223.184,-4.651,-49.8778,-21.3655,-18.3614,-7.1931,-32.4938,-73.3119,-48.8995,-57.7907,-41.4042,-106.385,-72.4238,-63.9122,-74.1954,-93.4164,2.79357,115.201,44.9621,89.0187,59.7967,92.0453,93.5147,27.3013,53.3434,17.9635,104.755,98.7182,106.749,-8.78735,102.458,40.4155,0,-6.33817,-93.8095,54.0251,6.41224,-51.917,25.9019,-2.49579,-185.57,0,33.1793,-11.794,77.1331,-177.999,9.39762,43.5001,-24.7139,-62.3302,-32.9844,-127.202,-93.9477,-57.7907,-16.6973,-1.49596,-60.8991,-59.872,-16.6973,0.488521,-77.3572,-57.4844,-143.778,-49.2351,-31.9861,115.867,139.143,108.865,146.025,142.463,101.954,113.829,154.345,139.268,161.495,50.6649,140.197,138.651,120.136,113.022};
 
//double silicon_gain[128] = {1,2.89101,2.91233,2.82188,2.99927,2.8337,2.80186,2.84574,2.82188,2.74021,2.82188,2.90344,2.90344,2.92856,2.84574,2.90344,2.89101,2.58439,2.58149,2.59138,2.53858,2.63914,2.66737,2.75137,2.64957,2.62888,2.58149,2.5129,2.60144,2.68869,2.66748,2.74809,2.60144,0,2.72908,2.77871,2.7707,2.72908,2.73695,2.71811,2.79029,2.7707,2.73695,2.7821,2.63914,2.71811,2.92856,2.7821,2.75929,2.74013,2.82536,2.7821,2.69951,2.71032,2.7821,2.69634,2.71811,2.74809,2.8135,2.82536,2.68877,2.8135,2.80525,2.8338,102.532,2.61135,2.6216,2.57156,2.58429,2.58149,2.60134,2.57156,2.46059,2.57448,2.61135,2.50086,2.55493,2.60849,2.51959,2.58149,0,3.01816,3.13637,3.01816,3.00872,3.35704,3.08739,3.05934,2.9953,0,3.2484,3.22198,2.60856,3.24842,3.16139,3.23748,3.06911,2.79846,2.75938,2.82193,2.69951,2.74809,2.8337,2.96352,2.95058,2.70721,2.8337,2.7707,2.91596,2.84221,2.7821,2.93773,2.89973,2.09892,2.16726,2.15533,2.1215,2.18627,2.11675,2.17422,2.08592,2.13013,2.1534,2.11015,2.06223,2.0878,2.16229,2.12346};

double silicon_offset[128] = {-274.08,-320.268,-276.639,-307.713,-277.398,-288.216,-309.729,-265.351,-298.041,-321.789,-356.177,-344.563,-323.221,-218.666,-344.563,-297.208,-247.971,-228.059,-290.765,-238.934,-248.182,-216.219,-287.868,-256.936,-321.205,-300.341,-301.549,-341.153,-290.028,-299.167,-288.63,-268.313,0,-190.629,-244.673,-187.919,-288.876,-265.923,-233.731,-242.76,-309.83,-233.08,-232.805,-248.182,-266.349,-229.507,-266.19,-223.184,-4.651,-49.8778,-21.3655,-18.3614,-7.1931,-32.4938,-73.3119,-48.8995,-57.7907,-41.4042,-106.385,-72.4238,-63.9122,-74.1954,-93.4164,-76.8822,115.201,44.9621,89.0187,59.7967,92.0453,93.5147,27.3013,53.3434,17.9635,104.755,98.7182,106.749,-8.78735,102.458,40.4155,0,-6.33817,-93.8095,54.0251,6.41224,-51.917,25.9019,-2.49579,-185.57,0,33.1793,-11.794,77.1331,-177.999,9.39762,43.5001,-24.7139,-62.3302,-32.9844,-127.202,-93.9477,-57.7907,-16.6973,-1.49596,-60.8991,-59.872,-16.6973,0.488521,-77.3572,-57.4844,-143.778,-49.2351,-31.9861,115.867,139.143,108.865,146.025,142.463,101.954,113.829,154.345,139.268,161.495,50.6649,140.197,138.651,120.136,113.022,102.532};

double silicon_gain[128] = {2.89101,2.91233,2.82188,2.99927,2.8337,2.80186,2.84574,2.82188,2.74021,2.82188,2.90344,2.90344,2.92856,2.84574,2.90344,2.89101,2.58439,2.58149,2.59138,2.53858,2.63914,2.66737,2.75137,2.64957,2.62888,2.58149,2.5129,2.60144,2.68869,2.66748,2.74809,2.60144,0,2.72908,2.77871,2.7707,2.72908,2.73695,2.71811,2.79029,2.7707,2.73695,2.7821,2.63914,2.71811,2.92856,2.7821,2.75929,2.74013,2.82536,2.7821,2.69951,2.71032,2.7821,2.69634,2.71811,2.74809,2.8135,2.82536,2.68877,2.8135,2.80525,2.8338,2.79357,2.61135,2.6216,2.57156,2.58429,2.58149,2.60134,2.57156,2.46059,2.57448,2.61135,2.50086,2.55493,2.60849,2.51959,2.58149,0,3.01816,3.13637,3.01816,3.00872,3.35704,3.08739,3.05934,2.9953,0,3.2484,3.22198,2.60856,3.24842,3.16139,3.23748,3.06911,2.79846,2.75938,2.82193,2.69951,2.74809,2.8337,2.96352,2.95058,2.70721,2.8337,2.7707,2.91596,2.84221,2.7821,2.93773,2.89973,2.09892,2.16726,2.15533,2.1215,2.18627,2.11675,2.17422,2.08592,2.13013,2.1534,2.11015,2.06223,2.0878,2.16229,2.12346,2.16718};


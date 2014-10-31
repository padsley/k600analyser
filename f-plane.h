/*-- init routine -----------------------------------------------------------------------------*/
INT focal_init(void)
//-----Initialise memory space for histograms--------
{
  printf("focal_init : L5\n");
   char name[256];
   char title[256];
   Int_t  i =0;

   //^^^^^^^^^^^^^^^ finish a few lut and channel related issues before variable definitions^^^^^^^^^^^^^^^^^^^^^^^
   GetODBGlobals();                // get globals that can be set in the ODB
   //PrintODBstuff();

   MapChannel2Wire();    // to be used when the ODB works. Othewise use setupchannels2wire()
   //setupchannel2wireODB();    // used when ODB is doing its thing. Only use it once to change the odb as needed
   setupchannel2wire();    // used when ODB is not doing its thing. Then you use it everytime at the start of run.

   //   read_cable(cableOffset,ALL_CABLE_OFFSET,"CableLength.dat");
   read_cable(cableOffset,"CableLength.dat");
   read_lut(lutx1,globals.lut_x1_offset,"lut-newdaq.dat");              
   read_lut(lutx2,globals.lut_x2_offset,"lut-newdaq.dat");   
   read_lut(lutu1,globals.lut_u1_offset,"lut-newdaq.dat");         
   read_lut(lutu2,globals.lut_u2_offset,"lut-newdaq.dat");              
   
   open_subfolder("LUT&Cable");
     hX1_lut = H1_BOOK("LUT_X1","LUT X1",8000,0,8000);
     hX2_lut = H1_BOOK("LUT_X2","LUT X2",8000,0,8000);
     hU1_lut = H1_BOOK("LUT_U1","LUT U1",8000,0,8000);
     hU2_lut = H1_BOOK("LUT_U2","LUT U2",8000,0,8000);
     hcable_off = H1_BOOK("Cable_offset","cable offsets",896,0,896);
   close_subfolder(); 

   // And immediately fill LUT spectra
   for(int j = 0; j < 8000; j++) {
     for(int k = 0; k < lutx1[j]*1000; k++) {
        hX1_lut->Fill(j);
     }
     for(int k = 0; k < lutx2[j]*1000; k++) {
        hX2_lut->Fill(j);
     }
     for(int k = 0; k < lutu1[j]*1000; k++) {
        hU1_lut->Fill(j);
     }
     for(int k = 0; k < lutu2[j]*1000; k++) {
        hU2_lut->Fill(j);
     }
   }

   for(int j = 0; j < 896; j++) {
     //printf("cable offset %f \n",cableOffset[j]);
     //     for(int k = 0; k < (int(cableOffset[j])+ALL_CABLE_OFFSET); k++) {  
     for(int k = 0; k < (int(cableOffset[j])); k++) {  
       hcable_off->Fill(j);                     
     }
   }

   //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  t1->Branch("tof",&b_tof,"b_tof/I");
  t1->Branch("pad1",&b_pad1,"b_pad1/D");
  t1->Branch("pad2",&b_pad2,"b_pad2/D");

  t1->Branch("X1pos",&b_X1pos,"b_X1pos/D");
  t1->Branch("X1th",&b_X1th,"b_X1th/D");
  t1->Branch("X1flag",&b_X1flag,"b_X1flag/I");
  t1->Branch("X1chisq",&b_X1chisq,"b_X1chisq/D");
  t1->Branch("X1res1",&b_X1res1,"b_X1res1/D");
  t1->Branch("X1eff",&b_X1eff,"b_X1eff/D");
  t1->Branch("X1hits",&b_X1hits,"b_X1hits/I");

  t1->Branch("U1pos",&b_U1pos,"b_U1pos/D");
  t1->Branch("U1th",&b_U1th,"b_U1th/D");
  t1->Branch("U1flag",&b_U1flag,"b_U1flag/I");
  t1->Branch("U1chisq",&b_U1chisq,"b_U1chisq/D");
  t1->Branch("U1res1",&b_U1res1,"b_U1res1/D");
  t1->Branch("U1eff",&b_U1eff,"b_U1eff/D");
  t1->Branch("U1hits",&b_U1hits,"b_U1hits/I");

  t1->Branch("ThFP",&b_ThFP,"b_ThFP/D");
  t1->Branch("Y1",&b_Y1,"b_Y1/D");
  t1->Branch("Y2",&b_Y2,"b_Y2/D");


  t2->Branch("X1dtRaw",X1dtRaw,"X1dtRaw[10]/D");



   //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

   hEventID  = H1_BOOK("EventID","each bin represents a type of event",30,0,30);
   hPR       = H1_BOOK("PatternRegister","Pattern register for all events ",20,0,20);

   //   hTOF_Raw  = H1_BOOK("TOF_Raw","Time of Flight, 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);       
   hTOF      = H1_BOOK("TOF","Time of Flight (Reftime corrected)",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   hPad1vsTOF  = H2_BOOK("Pad1vsTOF","Paddle 1 ave VS TOF", 1000,TDC_MIN_TIME,TDC_MAX_TIME, 1000, 0, 4095 );
   hPad1vsTOFg  = H2_BOOK("Pad1vsTOFg","Paddle 1 ave VS TOF (gated)", 1000,TDC_MIN_TIME,TDC_MAX_TIME, 1000, 0, 4095 );

   hTDCperEventRaw = H1_BOOK("TDCperEventRaw","TDC channels/event (All data)",100,0,100);
   hTDCperEvent    = H1_BOOK("TDCperEvent","TDC channels/event (PID selected)",100,0,100);

   for(int counter=0;counter<NR_OF_TDCS;counter++){
	  sprintf(name,"TDCModule%d",counter+1);
	  sprintf(title,"TDCModule %d Raw Specturm",counter);
          TDCrawSpectra[counter]=H1_BOOK(name,title,128,0,128);
   }

   for(int counter=0;counter<NR_OF_TDCS;counter++){
	  sprintf(name,"TDC2DModule%d",counter+1);
	  sprintf(title,"TDC2DModule %d ",counter);
          hTDC2DModule[counter]=H2_BOOK(name,title, 4000, 0, 14999,128,0,128);
   }

   hHitPatternAll   = H1_BOOK("HitPatternALL","Hits/Chan (ALL data)",1000,0,1000);
   hHitPatternPID   = H1_BOOK("HitPatternPID","Hits/Chan (PID selected)",1000,0,1000);
   hHitPatternX1    = H1_BOOK("HitPatternX1","Hits/Wire X1 ",X_WIRES+1,0,X_WIRES+1);
   hHitPatternX2    = H1_BOOK("HitPatternX2","Hits/Wire X2 ",X_WIRES+1,0,X_WIRES+1);
   hHitPatternU1    = H1_BOOK("HitPatternU1","Hits/Wire U1 ",U_WIRES+1,0,U_WIRES+1);
   hHitPatternU2    = H1_BOOK("HitPatternU2","Hits/Wire U2 ",U_WIRES+1,0,U_WIRES+1);

   hChanVsTimeRaw    = H2_BOOK("Chan_vs_RawTime","TDC Channel vs time (raw)", 4000, 0, 14999, 896, 0, 896 );
   hChanVsTimeRef    = H2_BOOK("Chan_vs_RefTime","TDC Channel vs time (ref times incl)", 4000, 0, 14999, 896, 0, 896);
   hChanVsTimeOffset = H2_BOOK("Chan_vs_OffsetTime","TDC Channel vs time (cablelenghts offsets incl)", 4000, 0, 14999, 896, 0, 896);
   hChanVsTimeOffsetWire = H2_BOOK("Chan_vs_OffsetTime_Wire","Wire Channel vs time (cablelenghts offsets incl)", 4000, 0, 14999, 1000, 0, 1000);

   hDriftTimeAllRaw  = H1_BOOK("DriftTime_All_Raw","All drifttimes (before subtracting ref times), 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hDriftTimeAllRef  = H1_BOOK("DriftTime_All_Ref","All drifttimes (ref time corrected), 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   hDriftTimeX1Ref    = H1_BOOK("DriftTime_X1_ref","X1 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hDriftTimeX2Ref    = H1_BOOK("DriftTime_X2_ref","X2 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hDriftTimeU1Ref    = H1_BOOK("DriftTime_U1_ref","U1 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hDriftTimeU2Ref    = H1_BOOK("DriftTime_U2_ref","U2 drifttimes (ref time corrected) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   hDriftTimeX1Offset = H1_BOOK("DriftTime_X1_Off","X1 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hDriftTimeX2Offset = H1_BOOK("DriftTime_X2_Off","X2 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hDriftTimeU1Offset = H1_BOOK("DriftTime_U1_Off","U1 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hDriftTimeU2Offset = H1_BOOK("DriftTime_U2_Off","U2 drifttimes (cablelenghts offsets incl) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
   hX1_eff_ID    = H1_BOOK("X1_eff_ID","VDC efficiency calculation: X1 ",20,0,20);
   hU1_eff_ID    = H1_BOOK("U1_eff_ID","VDC efficiency calculation: U1 ",20,0,20);
   hX2_eff_ID    = H1_BOOK("X2_eff_ID","VDC efficiency calculation: X2 ",20,0,20);
   hU2_eff_ID    = H1_BOOK("U2_eff_ID","VDC efficiency calculation: U2 ",20,0,20);

   hX1_driftlength = H1_BOOK("X1_driftlength","Driftlength X1 (mm)  ",110,-1,10);
   hX2_driftlength = H1_BOOK("X2_driftlength","Driftlength X2 (mm)  ",110,-1,10);
   hU1_driftlength = H1_BOOK("U1_driftlength","Driftlength U1 (mm)  ",110,-1,10);
   hU2_driftlength = H1_BOOK("U2_driftlength","Driftlength U2 (mm)  ",110,-1,10);

   hX1_HitsG  = H1_BOOK("X1_HitsG","X1 wires/event (good event)",20,0,20);
   hX2_HitsG  = H1_BOOK("X2_HitsG","X2 wires/event (good event)",20,0,20);
   hU1_HitsG  = H1_BOOK("U1_HitsG","U1 wires/event (good event)",20,0,20);
   hU2_HitsG  = H1_BOOK("U2_HitsG","U2 wires/event (good event)",20,0,20);

   hDriftTimeX1Good = H1_BOOK("X1_DriftTime_Good","X1 drifttimes (good events) 100ps/bin",TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);

   hX1_pos    = H1_BOOK("X1_pos","FP position: X1 ",8000,0,800);
   hX1_pos_angGated = H1_BOOK("X1_pos_angGated","FP position: X1  (for theta cut)",8000,0,800);
   hX1_pos_flag1 = H1_BOOK("X1_pos_flag1","FP position: X1 for raytrace flag=1",8000,0,800);
   hX1_theta  = H1_BOOK("X1_theta","X1 angle ",900,0,90);
   hX1_chisq  = H1_BOOK("X1_chisq","Chi squared: X1 ",500,0,15);
   hX1_th_vs_pos = H2_BOOK("X1_th_vs_pos","X1: theta vs position", 2400,0.,800.,600,0.,60.);
   hX1_th_vs_posflag1 = H2_BOOK("X1_th_vs_posflag1","X1: theta vs position", 2400,0.,800.,600,0.,60.);
   hX1_eff    = H1_BOOK("X1_eff","VDC efficiency: X1 ",1000,0,100);

   hU1_pos    = H1_BOOK("U1_pos","FP position from U1 ",8000,0,800);
   hU1_pos_flag1  = H1_BOOK("U1_pos_flag1","FP position: U1 for raytrace flag=1",8000,0,800);
   hU1_theta  = H1_BOOK("U1_theta","U1 angle ",900,0,90);
   hU1_chisq  = H1_BOOK("U1_chisq","Chi squared: U1 ",500,0,15);
   hU1_th_vs_pos = H2_BOOK("U1_th_vs_pos","U1: theta vs position", 2400,0.,800.,180,0,90.);
   hU1_eff    = H1_BOOK("U1_eff","VDC efficiency: U1 ",1000,0,100);

   hX2_pos    = H1_BOOK("X2_pos","FP position: X2 ",8000,0,800);
   hX2_pos_flag1 = H1_BOOK("X2_pos_flag1","FP position: X2 for raytrace flag=1",8000,0,800);
   hX2_theta  = H1_BOOK("X2_theta","X2 angle ",900,0,90);
   hX2_chisq  = H1_BOOK("X2_chisq","Chi squared: X2 ",500,0,15);
   hX2_th_vs_pos = H2_BOOK("X2_th_vs_pos","X2: theta vs position", 2400,0.,800.,600,0,60.);
   hX2_eff    = H1_BOOK("X2_eff","VDC efficiency: X2 ",1000,0,100);

   hU2_pos    = H1_BOOK("U2_pos","FP position from U2 ",8000,0,800);
   hU2_pos_flag1 = H1_BOOK("U2_pos_flag1","FP position: U2 for raytrace flag=1",8000,0,800);
   hU2_theta  = H1_BOOK("U2_theta","U2 angle ",900,0,90);
   hU2_chisq  = H1_BOOK("U2_chisq","Chi squared: U2 ",500,0,15);
   hU2_th_vs_pos = H2_BOOK("U2_th_vs_pos","U2: theta vs position", 2400,0.,800.,180,0,90.);
   hU2_eff    = H1_BOOK("U2_eff","VDC efficiency: U2 ",1000,0,100);

   hresX1_2d_RCNP = H2_BOOK("resX1_2d_RCNP","RCNP style resolution plot: X1", 200,-3,3,200,-3.,3.);
   hresX1_2d_ITL = H2_BOOK("resX1_2d_ITL","ITL style resolution plot for X1 (xaxis=pos-int(pos)) ", 200,-0.1,1.1,200,-3.,3.);
   hresX1 = H1_BOOK("resX1","X1 position resolution",400,-8.,8.);
   hresU1_2d_RCNP = H2_BOOK("resU1_2d_RCNP","Min.drift-Estimated VS Min.drift: U1", 200,-3,3,200,-3.,3.);
   hresU1_2d_ITL = H2_BOOK("resU1_2d_ITL","ITL style resolution plot: U1", 200,-0.1,1.1,200,-3.,3.);
   hresU1 = H1_BOOK("resU1","U1 position resolution (5 wire events only)",400,-8.,8.);
   hresX2_2d_RCNP = H2_BOOK("resX2_2d_RCNP","Min.drift-Estimated VS Min.drift: X2", 200,-3,3,200,-3.,3.);
   hresX2_2d_ITL = H2_BOOK("resX2_2d_ITL","ITL style resolution plot: X2", 200,-0.1,1.1,200,-3.,3.);
   hresX2 = H1_BOOK("resX2","X2 position resolution (5 wire events only)",400,-8.,8.);
   hresU2_2d_RCNP = H2_BOOK("resU2_2d_RCNP","Min.drift-Estimated VS Min.drift: U2", 200,-3,3,200,-3.,3.);
   hresU2_2d_ITL = H2_BOOK("resU2_2d_ITL","ITL style resolution plot: U2", 200,-0.1,1.1,200,-3.,3.);
   hresU2 = H1_BOOK("resU2","U2 position resolution (5 wire events only)",400,-8.,8.);

   hY1  = H1_BOOK("Y1","Y1 ",2000,-100,100);
   hY2  = H1_BOOK("Y2","Y2 ",2000,-100,100);
   hX1Y1= H2_BOOK("X1Y1","X1 vs Y1", 1600,0.,800.,500,-100.,100.);
   hX2Y2= H2_BOOK("X2Y2","X2 vs Y2", 1600,0.,800.,500,-100.,100.);

   hFP_theta  = H1_BOOK("FP_theta","FP theta (using X1 and X2)",900,0,90);
   hFP_phi  = H1_BOOK("FP_phi","FP phi (using X1 and X2)",1000,-50,50);
   hFP_theta_VS_pos = H2_BOOK("FP_theta_VS_pos","FP theta VS FP position", 2400,0.,800.,450,0.,90.);
   hFP_theta_VS_Y = H2_BOOK("FP_theta_VS_Y","Y VS FP theta", 400,20.,70.,700,-70.,70.);
   hFP_theta_VS_phi = H2_BOOK("FP_theta_VS_phi","FP phi VS FP theta",  450,0.,90.,1800,-40.,40.);
   hFP_phi_VS_pos = H2_BOOK("FP_phi_VS_pos","FP phi VS FP position", 2400,0.,800.,360,-90.,90.);

   hSCAT_theta  = H1_BOOK("SCAT_theta","Scattering angle (using X1 and X2)",1600,-20,20);
   hSCAT_theta_VS_pos = H2_BOOK("SCAT_theta_VS_pos","SCAT theta VS FP position", 2400,0.,800.,600,-60.,60.);

   hPaddlePIDg  = H2_BOOK("Pad_PID_Gated","Paddle1(y-axis) vs Paddle2(x-axis) (gated)",  1024, 0, 4096, 1024,0 , 4096);

   open_subfolder("Individual TDC chan");
     open_subfolder("X1_Refs");
     for(i=0;i<TDC_CHANNELS;i++){
	sprintf(name,"TDCchan_%d",i);
	sprintf(title,"TDC channel # %d (reftimes incl) ",i);
   	hTDC_REF[i] = H1_BOOK(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
     }
     close_subfolder();
   close_subfolder();


   /*
   open_subfolder("X1:Individual TDC chan");
     open_subfolder("X1_Raw");
     for(i=0;i<X_WIRES;i++){
	sprintf(name,"TDC_X1_%d",i);
	sprintf(title,"Chamber X1 wire # %d",i);
   	hTDC_X1_RAW[i] = H1_BOOK(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
     }
     close_subfolder(); 
     open_subfolder("X1_Refs+Offset");
     for(i=0;i<X_WIRES;i++){
	sprintf(name,"TDC_X1_%d_corr",i);
	sprintf(title,"Chamber X1 wire # %d (reftimes and offsets incl) ",i);
   	hTDC_X1_CORR[i] = H1_BOOK(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
     }
     close_subfolder();
   close_subfolder();

   open_subfolder("X2:Individual TDC chan");
     open_subfolder("X2_Raw");
     for(i=0;i<X_WIRES;i++){
	sprintf(name,"TDC_X2_%d",i);
	sprintf(title,"Chamber X2 wire # %d",i);
   	hTDC_X2_RAW[i]=H1_BOOK(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
     }
     close_subfolder(); 
     open_subfolder("X2_Refs+Offset");
     for(i=0;i<X_WIRES;i++){
	sprintf(name,"TDC_X2_%d_corr",i);
	sprintf(title,"Chamber X2 wire # %d (reftimes and offsets incl) ",i);
   	hTDC_X2_CORR[i] = H1_BOOK(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
     }
     close_subfolder();
   close_subfolder();

   open_subfolder("U1:Individual TDC chan");
     open_subfolder("U1_Raw");
     for(i=0;i<U_WIRES;i++){
	sprintf(name,"TDC_U1_%d",i);
	sprintf(title,"Chamber U1 wire # %d",i);
   	hTDC_U1_RAW[i]=H1_BOOK(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
     }
     close_subfolder(); 

     open_subfolder("U1_Refs+Offset");
     for(i=0;i<U_WIRES;i++){
	sprintf(name,"TDC_U1_%d_corr",i);
	sprintf(title,"Chamber U1 wire # %d (reftimes and offsets incl) ",i);
   	hTDC_U1_CORR[i] = H1_BOOK(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
     }
     close_subfolder();
   close_subfolder();

   open_subfolder("U2:Individual TDC chan");
     open_subfolder("U2_Raw");
     for(i=0;i<U_WIRES;i++){
	sprintf(name,"TDC_U2_%d",i);
	sprintf(title,"Chamber U2 wire # %d",i);
   	hTDC_U2_RAW [i]=H1_BOOK(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
     }
     close_subfolder(); 
     open_subfolder("U2_Refs+Offset");
     for(i=0;i<U_WIRES;i++){
	sprintf(name,"TDC_U2_%d_corr",i);
	sprintf(title,"Chamber U2 wire # %d (reftimes and offsets incl) ",i);
   	hTDC_U2_CORR[i] = H1_BOOK(name,title,TDC_N_BINS,TDC_MIN_TIME,TDC_MAX_TIME);
     }
     close_subfolder();
   close_subfolder();   
   */



   return SUCCESS;
}


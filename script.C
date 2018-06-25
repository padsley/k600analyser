{
    //TF1 *fNoise = new TF1("fNoise", "pol0");
    //TF1 *fLine = new TF1("fLine", "pol1");
    
    hTDC_bkgSub_631->Fit("fLine", "", "", 8300.0, 8360.0);
    
    a = fLine->GetParameter(0);
    b = fLine->GetParameter(1);
    
    offset = (-a/b) - 8000.0;
    
    std::cout << std::endl;
    std::cout << "offset: " << offset << std::endl;
    std::cout << std::endl;
}

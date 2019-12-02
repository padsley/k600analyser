{
    /*
    hTDC_bkgSub_57->SetStats(0);
    hTDC_bkgSub_57->Draw();
    */
    
    TF1 *f = new TF1("f", "pol1", 0.0, 10000.0);
    f->SetParameter(0, 110078.0);
    f->SetParameter(1, -25.9562);
    f->SetLineWidth(3);
    
    /*
    hTDC_bkgSub_57->GetListOfFunctions()->Clear();
    hTDC_bkgSub_57->Draw("SAME");
    
    f->Draw("SAME");
    */
}

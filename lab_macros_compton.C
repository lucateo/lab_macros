#include "lab_macros.C"

/*
This macro has to be executed only if you are acquiring with at least 2 channels.
It creates a tree, which is essentially a clone of the input one, in a new root file
after fixing possible missing coincidences between different acquisition channels.
For any problems or doubts contact Franco Galtarossa (franco.galtarossa@lnl.infn.it).
*/

/*struct slimport_data_t {
    ULong64_t	timetag;
    UInt_t	baseline;
    UShort_t	qshort;
    UShort_t	qlong;
    UShort_t	pur;
    UShort_t	samples[4096];
};*/

// for tagger, channel 0
TH1F* calibrateTagger(char *file_na, int numBins, double minX, double maxX ) { 

	TH1F *h_na = getHistoForChannelFromTree(file_na, 0, numBins, minX, maxX);
		
	// call the function to calibrate the histogram
	CalibrateHisto(h_na, 0.055, -8.059);
	
	// Personalization things
	h_na->SetXTitle("Energy (keV)");
	h_na->SetYTitle("counts");
	
	h_na->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	
	//h_na->Draw();
	return h_na;

}


TH1F* calibrateScatterer(char *file_na,int numBins, double minX, double maxX ) { 

	TH1F *h_na = getHistoForChannelFromTree(file_na, 1, numBins, minX, maxX);
		
	// call the function to calibrate the histogram
	CalibrateHisto(h_na, 0.0581, -7.937);
	
	// Personalization things
	h_na->SetXTitle("Energy (keV)");
	h_na->SetYTitle("counts");
	
	h_na->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	
	//h_na->Draw();
	return h_na;

}


TH1F* calibrateDetector(char *file_na,int numBins, double minX, double maxX ) { 

	TH1F *h_na = getHistoForChannelFromTree(file_na, 2, numBins, minX, maxX);
		
	// call the function to calibrate the histogram
	CalibrateHisto(h_na, 0.0529, -6.302);
	
	// Personalization things
	h_na->SetXTitle("Energy (keV)");
	h_na->SetYTitle("counts");
	
	h_na->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	
	//h_na->Draw();
	return h_na;

}


//this is the one I will use personally
void getHistoCompton(char *name_file, int numBins, double minX, double maxX) {
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("acq_tree_0");
	
	TBranch *inbranch0 = intree->GetBranch("acq_ch0");
	TBranch *inbranch1 = intree->GetBranch("acq_ch1");
	TBranch *inbranch2 = intree->GetBranch("acq_ch2");
	
	inbranch0->SetAddress(&indata.timetag);
	inbranch1->SetAddress(&indata.timetag);
	inbranch2->SetAddress(&indata.timetag);
		
	TH1F *h0 = new TH1F("spectrum","Tagger spectrum",numBins,minX,maxX);
	TH1F *h1 = new TH1F("spectrum","Scatterer spectrum",numBins,minX,maxX);
	TH1F *h2 = new TH1F("spectrum","Detector spectrum",numBins,minX,maxX);
	 
	// histogram filling
	for (int i=0; i<inbranch0->GetEntries(); i++) {
		inbranch0->GetEntry(i);
		h0->Fill(indata.qlong);
	}
	
	for (int i=0; i<inbranch1->GetEntries(); i++) {
		inbranch1->GetEntry(i);
		h1->Fill(indata.qlong);
	}
	
	for (int i=0; i<inbranch2->GetEntries(); i++) {
		inbranch2->GetEntry(i);
		h2->Fill(indata.qlong);
	}
		
	
	h0->SetXTitle("channels");
	h0->SetYTitle("counts");
	h0->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	h1->SetXTitle("channels");
	h1->SetYTitle("counts");
	h1->GetYaxis()->SetTitleOffset(1.4);
	h2->SetXTitle("channels");
	h2->SetYTitle("counts");
	h2->GetYaxis()->SetTitleOffset(1.4);
	
	
	//h_spectrum->Fit("gaus");
	gStyle->SetOptStat("emi"); //to show what I want in the panel, see reference (the letters specify which information will be included)
	
	
	TCanvas *c0 = new TCanvas("c0");
	c0->Divide(2,2);
	c0->cd(1);
	h0->Draw();
	c0->cd(2);
	h1->Draw();
	c0->cd(3);
	h2->Draw();
	// return
	//return h_spectrum;
}


void getHistoComptonCalibrated(char *name_file, int numBins, double minX, double maxX) {
	// variables
	
	TH1F *h0 = calibrateTagger(name_file, numBins,minX,maxX);
	TH1F *h1 = calibrateScatterer(name_file,numBins,minX,maxX);
	TH1F *h2 = calibrateDetector(name_file,numBins,minX,maxX);
	h0->SetTitle("Tagger");
	h1->SetTitle("Scatterer");
	h2->SetTitle("Detector");
	gStyle->SetOptStat(""); //to show what I want in the panel, see reference (the letters specify which information will be included)
		
	TCanvas *c0 = new TCanvas("c0");
	h0->Draw();
	TCanvas *c1 = new TCanvas("c1");
	h1->Draw();
	TCanvas *c2 = new TCanvas("c2");
	h2->Draw();
}





//fit with 2 guassian, to do the calibration graphs
void multifit2compton(char *file) {
	
	TCanvas *se = new TCanvas;
	// remember TH1F* getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)
 
	TH1F *h = getHistoForChannelFromTree(file, 2, 1000, 4000, 27000);
	
   Double_t par[6]; // I need 6 parameters for two gaussians
   
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","gaus",8000,12000); //this is to find the proper parameters from the single fits
   TF1 *g2    = new TF1("g2","gaus",22000,26000); //and then puth them on the total fit
   
   TF1 *total = new TF1("total","gaus(0)+gaus(3)",3500,10000); // notice that its range contains all the previous
																	// two ranges
   //total->SetLineColor(3);
   
   h->Fit(g1,"R"); //"R" Use the Range specified in the function range   

   h->Fit(g2,"R+"); //"+" Add this new fitted function to the list of fitted functions (by default, 
					//any previous function is deleted)  

   g1->GetParameters(&par[0]); // I think he puts the parameters in the array par, for every fit
   g2->GetParameters(&par[3]); //there are three parameters: p0 = normalization factor, p1 = mean, p2 = sigma, 
   
   total->SetParameters(par);
   h->Fit(total,"R+");
   
   h->SetTitle("Calibration detector with Na source");
   h->SetXTitle("bins");
   h->SetYTitle("counts");
   h->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
      
   //all the things down here are to properly insert the stats of the second fit
   
   
   gStyle->SetOptStat("e");
   gStyle->SetOptFit(11);
   
   h->Draw();
   se->Update();
   
   // Retrieve the stat box
   TPaveStats *ps = (TPaveStats*)se->GetPrimitive("stats");
   ps->SetName("mystats");
   TList *listOfLines = ps->GetListOfLines();
   
   // Add a new line in the stat box.
   // Note that "=" is a control character
	TLatex *p0 = new TLatex(0,0,"Constant2 = 260.39 #pm 3.02"); // Tlatex is like latex but with # instead of \ for special characters
											 // the numbers before the text specify the position of the thing
	TLatex *p1 = new TLatex(0,0,"Mean2 = 24260.4 #pm 1.4 "); //add by hand the parameters of the second gaussian
	TLatex *p2 = new TLatex(0,0,"Sigma2 = 976.3 #pm 1.4"); // p0 is constant, p1 mean, p2 sigma
	
	//set of the lines
	p0->SetTextSize(0.04);
	p1->SetTextSize(0.04);
	p2->SetTextSize(0.04);
	p0->SetTextFont(42);
	p1->SetTextFont(42);
	p2->SetTextFont(42);
	
	//add the lines
    //listOfLines->Add(p0);
    listOfLines->Add(p1);
    listOfLines->Add(p2);
    
    // the following line is needed to avoid that the automatic redrawing of stats
    h->SetStats(0);
    //reupdate the canvas
    se->Modified();
   
}



//fit with 2 guassian, to do the calibration graphs
void multifit2comptonCalibrated(char *file) {
	
	TCanvas *se = new TCanvas;
	// remember TH1F* getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)
	
	//REMEMBER to change the type of calibration!!!
 	TH1F *h = calibrateDetector(file, 1000, 0, 26000); 
	
   Double_t par[6]; // I need 6 parameters for two gaussians
   
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","gaus",450,590); //this is to find the proper parameters from the single fits
   TF1 *g2    = new TF1("g2","gaus",1200,1330); //and then puth them on the total fit
   
   TF1 *total = new TF1("total","gaus(0)+gaus(3)",400,1400); // notice that its range contains all the previous
																	// two ranges
   //total->SetLineColor(3);
   
   h->Fit(g1,"R"); //"R" Use the Range specified in the function range   

   h->Fit(g2,"R+"); //"+" Add this new fitted function to the list of fitted functions (by default, 
					//any previous function is deleted)  

   g1->GetParameters(&par[0]); // I think he puts the parameters in the array par, for every fit
   g2->GetParameters(&par[3]); //there are three parameters: p0 = normalization factor, p1 = mean, p2 = sigma, 
   
   total->SetParameters(par);
   h->Fit(total,"R+");
   h->SetTitle("Calibrated Na spectrum (Detector)");
     
   //all the things down here are to properly insert the stats of the second fit
     
   gStyle->SetOptStat("e");
   gStyle->SetOptFit(11);
   
   h->Draw();
   se->Update();
   
   // Retrieve the stat box
   TPaveStats *ps = (TPaveStats*)se->GetPrimitive("stats");
   ps->SetName("mystats");
   TList *listOfLines = ps->GetListOfLines();
   
   // Add a new line in the stat box.
   // Note that "=" is a control character
	TLatex *p0 = new TLatex(0,0,"Constant2 = 1005.81 #pm 1.84"); // Tlatex is like latex but with # instead of \ for special characters
											 // the numbers before the text specify the position of the thing
	TLatex *p1 = new TLatex(0,0,"Mean2 = 1272.88 #pm 0.44 "); //add by hand the parameters of the second gaussian
	TLatex *p2 = new TLatex(0,0,"Sigma2 = 32.41 #pm 0.42"); // p0 is constant, p1 mean, p2 sigma
	
	//set of the lines
	p0->SetTextSize(0.04);
	p1->SetTextSize(0.04);
	p2->SetTextSize(0.04);
	p0->SetTextFont(42);
	p1->SetTextFont(42);
	p2->SetTextFont(42);
	
	//add the lines
    listOfLines->Add(p0);
    listOfLines->Add(p1);
    listOfLines->Add(p2);
    
    // the following line is needed to avoid that the automatic redrawing of stats
    h->SetStats(0);
    //reupdate the canvas
    se->Modified();
   
   
}


//fit with 1 guassian, to do the calibration graphs
void multifit1comptonCalibrated(char *file) {
	
	TH1F *h = calibrateScatterer(file, 500, 0, 26000); //REMEMBER to change the type of calibration!!!
	
     
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","gaus",10,170); //this is to find the proper parameters from the single fits
     
   h->Fit(g1,"R"); //"R" Use the Range specified in the function range   
      
   gStyle->SetOptStat("i");
   gStyle->SetOptFit(11);
   
   h->SetTitle("30 degrees scatterer");
   h->Draw();
   
   
}


//fit with 1 guassian, to do the calibration for Americium
void multifit1compton(char *file) {
	
	// remember TH1F* getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)
 
 // or put subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX);
 // instead of getHistoForChannelFromTree
	TH1F *h = getHistoForChannelFromTree(file, 2, 500, 700, 4000);
	
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","gaus",800,1600); //this is to find the proper parameters from the single fits
  
   //total->SetLineColor(3);
   
   h->Fit(g1,"R"); //"R" Use the Range specified in the function range   

     
   h->SetTitle("Calibration detector with Am source");
   h->SetXTitle("bins");
   h->SetYTitle("counts");
   h->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
   gStyle->SetOptStat(0);
   gStyle->SetOptFit(1);  
   h->Draw();   
}




// this draws the calibration line for three points
void calibrationLine(float peak0, float peak1, float peak2) {

	
	float bin[3];
	bin[0] = peak0; //americium peak in bins
	bin[1] = peak1; // Sodium photopeak energy in bins
	bin[2] = peak2; // Sodium photopeak energy in bins

	float kev[3];
	kev[0] = 59.; //americium peak
	kev[1] = 511.; // Sodium photopeak energy in keV
	kev[2] = 1275.; // Sodium photopeak energy in keV

	// Define a TGraphErrors which contains the experimental points in x
	// and the theoretical points in y; I semplify by not taking into account errors.
	
	//graph without errors
	//TGraphErrors *graphErr = new TGraphErrors(3,bin,kev);
	
	//graph with errors
	 float ex[] = {0, 0, 0}; //errors in x, I take the sigma
	 float ey[] = {0,0,0}; // errors in energy, obviously I don't have errors in the energy
	 TGraphErrors *graphErr = new TGraphErrors(3,bin,kev,ex,ey);
	 


	
	// Define a function which fits the points (does not work with errors)
	//TF1 *fitfun = new TF1("calfitfun1","pol1",bin[0],bin[2]);

	// Graphical options to draw the fit
	graphErr->SetMarkerStyle(21);
	graphErr->SetLineColor(2);
	graphErr->SetTitle("Calibration Line detector");
	graphErr->GetXaxis()->SetTitle("bins");
	graphErr->GetYaxis()->SetTitle("energy");
	
	// Fit
	graphErr->Fit("pol1"); //with this it works even with the error option
	graphErr->Draw("AP"); //A = Axis are drawn around the graph 
							// L = A simple polyline is drawn 
							// P= The current marker is plotted at each point 
							// (can be useful) C= a smooth Curve is drawn
	//the errors are in the minimizer menù, p0 is the intercept, p1 is the angular coefficient
	
	gStyle->SetOptFit(0101);   // default is SetOptFit(pcev)
								//p= 1;  print Probability
								//c = 1;  print Chisquare/Number of degrees of freedom
								//e = 1;  print errors (if e=1, v must be 1)
								//v = 1;  print name/values of parameters
	
}



TH1F* getIntegral(char *name_file) {  //a particular class to determine the integral under a peak
	
	
	// remember subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX)
	TH1F *h_spectrum = getHistoForChannelFromTree(name_file, 0, 400, 10250, 11900);
	//I don't know why but it works only if I specify the range in the histogram and then use Integral()
	/*
	h_spectrum->SetMaximum(4500.);        //to set the proper y range (Check if it counts only the area in this range!! 
    h_spectrum->SetMinimum(500.); */
    h_spectrum->SetMinimum(50.);
	h_spectrum->SetXTitle("channels");
	h_spectrum->SetYTitle("counts");
	h_spectrum->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	//h_spectrum->Fit("gaus");
	
	double integral = h_spectrum->Integral();
	
		
	gStyle->SetOptStat("mie"); //to show what I want in the panel, see reference (the letters specify which information will be included), i is
	h_spectrum->Draw();			// for the integral
	// return
	
	cout << h_spectrum->Integral() << endl;
	return h_spectrum;
}



//Optimized by me using three channels
void fix_coinc(const char *name_infile, const char *name_outfile){
    
    slimport_data_t indataA, indataB, indataC, indataD;
    //offset are the correct data in coincidence to register, ent is...
    int offsetA=0, offsetB=0, offsetC=0, offsetD=0, ent=0;
    int tolerance = 25; // in timestamp units, define the tolerance in which one can consider 
						//the events as in coincidence
    TFile *infile = new TFile(name_infile);
    TTree *intree = (TTree*)infile->Get("acq_tree_0");
    
//If you are using channels in a different order change here!
    TBranch *inbranchA = intree->GetBranch("acq_ch0");
    TBranch *inbranchB = intree->GetBranch("acq_ch1");
    TBranch *inbranchC = intree->GetBranch("acq_ch2");
    //TBranch *inbranchD = intree->GetBranch("acq_ch3");
    inbranchA->SetAddress(&indataA.timetag);
    inbranchB->SetAddress(&indataB.timetag);
    inbranchC->SetAddress(&indataC.timetag);
    //inbranchD->SetAddress(&indataD.timetag);

    TFile *outfile = new TFile(name_outfile, "RECREATE");
    TTree *outtree = intree->CloneTree(0); //so you create a new root file with the proper coincidences

 if(inbranchC->GetEntries()==0){ // If you are acquiring with just 2 channels (ch0 and ch1)
                                                      
     while(ent+offsetA<inbranchA->GetEntries() && ent+offsetB<inbranchB->GetEntries()){
         inbranchA->GetEntry(ent+offsetA);
         inbranchB->GetEntry(ent+offsetB);
         if(indataA.timetag < indataB.timetag - tolerance){
	     offsetA++;
	     continue;
	 } else if(indataA.timetag > indataB.timetag + tolerance){
	     offsetB++;
	     continue;
	 }
                                                         
         ent++;                                                                                         
         outtree->Fill();
                                                          
         if(ent == 1) cout << endl << "Let's fix the coincidences!" << endl << endl;
         if(ent % 25000 == 0 && ent != 0){
             cout << ent << " entries analyzed. Only " << inbranchA->GetEntries() - ent << " entries left." << endl;
             cout.flush();
         }
                                                         
     }

 }  else { // If you are acquiring with 3 channels (ch0, ch1, ch2)

    while(ent+offsetA<inbranchA->GetEntries() && ent+offsetB<inbranchB->GetEntries() && ent+offsetC<inbranchC->GetEntries()){
        
        inbranchA->GetEntry(ent+offsetA);
        inbranchB->GetEntry(ent+offsetB);
        inbranchC->GetEntry(ent+offsetC);
       // inbranchD->GetEntry(ent+offsetD);

        if(indataA.timetag < indataB.timetag - tolerance || indataA.timetag < indataC.timetag - tolerance){
            offsetA++;
            continue;
        } else if(indataB.timetag < indataA.timetag - tolerance || indataB.timetag < indataC.timetag - tolerance){
            offsetB++;
            continue;
        } else if(indataC.timetag < indataA.timetag - tolerance || indataC.timetag < indataB.timetag - tolerance){
           	offsetC++;
            continue;
        } 
 
        ent++;
        outtree->Fill();
            
        if(ent == 1) cout << endl << "Let's fix the coincidences!" << endl << endl;
        if(ent % 25000 == 0 && ent != 0){
            cout << ent << " entries analyzed. Only " << inbranchA->GetEntries() - ent << " entries left." << endl;
            cout.flush();
        }
    
    }

 }
    
    outfile->cd();
    outtree->Write();
    outfile->Close();
    delete outfile;

}


/*
This macro has to be executed only if you are acquiring with just 2 channels.
For any problems or doubts contact Franco Galtarossa (franco.galtarossa@lnl.infn.it).
*/

/*struct slimport_data_t {
    ULong64_t	timetag;
    UInt_t	baseline;
    UShort_t	qshort;
    UShort_t	qlong;
    UShort_t	pur;
    UShort_t	samples[4096];
};*/

void fix_coinc2(const char *name_infile, const char *name_outfile){
    
    slimport_data_t indataA, indataB;
    int offsetA=0, offsetB=0, ent=0;
    int tolerance = 25; // in timestamp units
    TFile *infile = new TFile(name_infile);
    TTree *intree = (TTree*)infile->Get("acq_tree_0");
    
//If you are using channels in a different order change here!
    TBranch *inbranchA = intree->GetBranch("acq_ch0");
    TBranch *inbranchB = intree->GetBranch("acq_ch1");
    inbranchA->SetAddress(&indataA.timetag);
    inbranchB->SetAddress(&indataB.timetag);
                 
    while(ent+offsetA<inbranchA->GetEntries() && ent+offsetB<inbranchB->GetEntries()){
         inbranchA->GetEntry(ent+offsetA);
         inbranchB->GetEntry(ent+offsetB);
         if(indataA.timetag < indataB.timetag - tolerance){
	     offsetA++;
	     continue;
	 } else if(indataA.timetag > indataB.timetag + tolerance){
	     offsetB++;
	     continue;
	 }
                                                         
         ent++;                                                                                         

//Here you can do your analysis
                                          
    }

}



void tableEnergy()
{
	float angle[4] = {0,30,50,90}; 
	float egamma[4] = {483, 436.1, 370.3, 274.9};
	float egammath[4]; //theoretical values
	float sigmagamma[4] = {35.47,53.94,62.46,44.25}; //from the gaussian fit
	float eelectron[4] = {23.48, 66.7, 128.4, 233.7};
	float eelectronth[4];//theoretical values
	float sigmaelectron[4] = {19.78,36.3,53.38,49.52};
	float sumenergy[4];
	float errorsum[4];
	for (int i=0; i < 4; i++){
		sumenergy[i] = egamma[i] + eelectron[i];
		errorsum[i] = sigmagamma[i] + sigmaelectron[i];
	}
	
	//calculus of theoretical values
	for (int i=0; i < 4; i++){
		egammath[i] = 511.0 / (1 +  (1 - TMath::Cos(angle[i] * 0.017 ) )); //all in keV!!! And the angle in rad
		eelectronth[i] = 511.0 - egammath[i];
	}
	
	
	//write data
	writeLines(searchLine("%TABLE_ENERGY", "physical_lab_reports.tex"), "physical_lab_reports.tex","physical_lab_reports2.tex");
		ofstream outfile;
		outfile.open("physical_lab_reports2.tex", std::ios_base::app);
	for (int i=0; i < 4; i++){
		outfile << fixed << setprecision(0) << angle[i] << " & \t" << setprecision(2) << egammath[i] << " & \t" 
		<< eelectronth[i] << " & \t" << egamma[i]  << " $\\pm$ " << sigmagamma[i] 
		<< " & \t" <<  eelectron[i]  << " $\\pm$ " << sigmaelectron[i] << " & \t" << sumenergy[i] << " $\\pm$ " 
		<<  errorsum[i] << " \\\\" << endl;
	}
		writeFinalLines(searchLine("%TABLE_ENERGY", "physical_lab_reports.tex"), "physical_lab_reports.tex","physical_lab_reports2.tex");
		outfile.close();
}

void tableNumber()
{
	float angle[4] = {0,30,50,90};
	//float ngamma[4] = {4513, 5372, 6524, 227600}; //without filter
	float ngamma[4] = {2796, 3501, 4195, 144800};
	float nelectron[4] = {2202, 3937, 5229, 188200};
	float sigmaelectron[4];
	float sigmagamma[4];
	for (int i=0; i < 4; i++){
		sigmaelectron[i] = TMath::Sqrt(nelectron[i]);
		sigmagamma[i] = TMath::Sqrt(ngamma[i]);
	}
	//write data
		writeLines(searchLine("%TABLE_NUMBER", "physical_lab_reports.tex"), "physical_lab_reports.tex","physical_lab_reports2.tex");
		ofstream outfile;
		outfile.open("physical_lab_reports2.tex", std::ios_base::app);
	for (int i=0; i < 4; i++){
		outfile << fixed << setprecision(0) <<
		angle[i] << "\t" << "&" << "\t" << ngamma[i] << "\t" << "$\\pm$" << "\t" << setprecision(2) << sigmagamma[i]
		 << "\t" << "&" << "\t" << setprecision(0) << nelectron[i]  <<"\t" << "$\\pm$" << "\t" 
		 << setprecision(2) << sigmaelectron[i] << "\t" << "\\\\" << endl;
	}
		writeFinalLines(searchLine("%TABLE_NUMBER", "physical_lab_reports.tex"), "physical_lab_reports.tex","physical_lab_reports2.tex");
		outfile.close();
	
}



//macro for selecting a gap in energy
void getHistoWithFilterCompton(char *name_file, int numBins, double minX, double maxX) {
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("acq_tree_0");
	TBranch *inbranch0 = intree->GetBranch("acq_ch0");
	TBranch *inbranch1 = intree->GetBranch("acq_ch1");
	TBranch *inbranch2 = intree->GetBranch("acq_ch2");
		
	inbranch0->SetAddress(&indata.timetag);
	inbranch1->SetAddress(&indata.timetag);
	inbranch2->SetAddress(&indata.timetag);
	
	TH1F *h0 = new TH1F("h_spectrum","Tagger",numBins,minX,maxX);
	TH1F *h1 = new TH1F("h_spectrum","Scatterer 90 degrees",numBins,minX,maxX);
	TH1F *h2 = new TH1F("h_spectrum","Detector 90 degrees",numBins,minX,maxX);
	
	// values of trigger with calibration of tagger, they correspond to 460-560
	double lowThr = 8510;
	double highThr = 10328;
	// histogram filling
	for (int i=0; i<inbranch0->GetEntries(); i++) {
		inbranch0->GetEntry(i);
		if (indata.qlong>lowThr && indata.qlong<highThr) {  // This fills only if the data is in the range I gave with lowThr and highThr
			h0->Fill(indata.qlong);
			inbranch1->GetEntry(i);
			h1->Fill(indata.qlong);
			inbranch2->GetEntry(i);
			h2->Fill(indata.qlong);
		}
	}
	
	CalibrateHisto(h0, 0.055, -8.059);
	CalibrateHisto(h1, 0.0581, -7.937);
	CalibrateHisto(h2, 0.0529, -6.302);
	
	TF1 *g1 = new TF1("g1","gaus",100,400); //this is to find the proper parameters from the single fits
    h1->Fit(g1,"R"); //"R" Use the Range specified in the function range   
    TF1 *g2 = new TF1("g1","gaus",200,400); //this is to find the proper parameters from the single fits
    h2->Fit(g2,"R"); //"R" Use the Range specified in the function range 
    
    gStyle->SetOptStat("i");
    gStyle->SetOptFit(11);
	
	//personalization
	h0->SetXTitle("Energy (keV)");
	h0->SetYTitle("counts");
	h0->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	h1->SetXTitle("Energy (keV)");
	h1->SetYTitle("counts");
	h1->GetYaxis()->SetTitleOffset(1.4);
	h2->SetXTitle("Energy (keV)");
	h2->SetYTitle("counts");
	h2->GetYaxis()->SetTitleOffset(1.4);
	
	TCanvas *c0 = new TCanvas("c0");
	h0->Draw();
	TCanvas *c1 = new TCanvas("c1");
	h1->Draw();
	TCanvas *c2 = new TCanvas("c2");
	h2->Draw();
}

//to make the sum in energy of the same events
void histoSumEnergy(char *name_file, int numBins, double minX, double maxX) {
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("acq_tree_0");
	TBranch *inbranch0 = intree->GetBranch("acq_ch0");
	TBranch *inbranch1 = intree->GetBranch("acq_ch1");
	TBranch *inbranch2 = intree->GetBranch("acq_ch2");
		
	inbranch0->SetAddress(&indata.timetag);
	inbranch1->SetAddress(&indata.timetag);
	inbranch2->SetAddress(&indata.timetag);
	
	TH1F *h0 = new TH1F("h_spectrum","Tagger",numBins,minX,maxX);
	TH1F *h1 = new TH1F("h_spectrum","Scatterer",numBins,minX,maxX);
	TH1F *h2 = new TH1F("h_spectrum","Detector",numBins,minX,maxX);
	TH1F *hsum = new TH1F("h_spectrum","",numBins,0,1400); //sum of the energies
	
	// values of trigger with calibration of tagger, they correspond to 460-560
	double lowThr = 8510;
	double highThr = 10328;
	double dataScat = 0;
	double dataDet = 0;
	// histogram filling
	for (int i=0; i<inbranch0->GetEntries(); i++) {
		inbranch0->GetEntry(i);
		if (indata.qlong>lowThr && indata.qlong<highThr) {  // This fills only if the data is in the range I gave with lowThr and highThr
			h0->Fill(indata.qlong);
			inbranch1->GetEntry(i);
			dataScat = 0.0581*indata.qlong -7.937 ;
			h1->Fill(indata.qlong);
			inbranch2->GetEntry(i);
			dataDet = 0.0529*indata.qlong -6.302; //calibrate in the moment
			h2->Fill(indata.qlong);
			hsum->Fill(dataScat + dataDet);
		}
	}
	
	CalibrateHisto(h0, 0.055, -8.059);
	CalibrateHisto(h1, 0.0581, -7.937);
	CalibrateHisto(h2, 0.0529, -6.302);
	
	gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
	
	//personalization
	h0->SetXTitle("Energy (keV)");
	h0->SetYTitle("counts");
	h0->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	h1->SetXTitle("Energy (keV)");
	h1->SetYTitle("counts");
	h1->GetYaxis()->SetTitleOffset(1.4);
	h2->SetXTitle("Energy (keV)");
	h2->SetYTitle("counts");
	h2->GetYaxis()->SetTitleOffset(1.4);
	h2->SetLineColor(kRed);
	hsum->SetXTitle("Energy (keV)");
	hsum->SetYTitle("counts");
	hsum->GetYaxis()->SetTitleOffset(1.4);
	hsum->SetTitle("Sum energies 90 degrees");
		
	TCanvas *c0 = new TCanvas("c0");
	h1->Draw();
	h2->Draw("SAME");
	c0->BuildLegend();
	h1->SetTitle("Scatterer + Detector 90 degrees");
	TCanvas *c1 = new TCanvas("c1");
	hsum->Draw();
	//auto legend = new TLegend(0.1,0.7,0.48,0.9);
	//legend->SetLegendFillColor(0);
	//legend->AddEntry(h1,"Scatterer (electron)","");
	//legend->AddEntry(h2,"Detector (photon)","");
	//legend->Draw();
	
}


void histo2DCompton(char *name_file, double numBins, double minX, double maxX  )
{
   TCanvas *c1 = new TCanvas("c1", "c1",900,900);
   gStyle->SetOptStat(0);
      
   // Create, fill and project a 2D histogram.
   //this strange choice of min and max range was to properly center the numbers
   TH2F *h2d = new TH2F("h2","",numBins,minX,maxX,numBins,minX,maxX); //(name, nBinsX, minX, maxX, nBinsY, minY, maxY)
    
    slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("acq_tree_0");
	TBranch *inbranch0 = intree->GetBranch("acq_ch0");
	TBranch *inbranch1 = intree->GetBranch("acq_ch1");
	TBranch *inbranch2 = intree->GetBranch("acq_ch2");
		
	inbranch0->SetAddress(&indata.timetag);
	inbranch1->SetAddress(&indata.timetag);
	inbranch2->SetAddress(&indata.timetag);
	
		
	// values of trigger with calibration of tagger, they correspond to 460-560
	double lowThr = 8510;
	double highThr = 10328;
	double sigmatot = 100;
	double electronHigh = 180;
	// histogram filling
	for (int i=0; i<inbranch0->GetEntries(); i++) {
		inbranch0->GetEntry(i);
		if (indata.qlong>lowThr && indata.qlong<highThr) {  // This fills only if the data is in the range I gave with lowThr and highThr
			inbranch1->GetEntry(i);
			double dataScat = 0.0581*indata.qlong -7.937 ;
			inbranch2->GetEntry(i);
			double dataDet = 0.0529*indata.qlong -6.302; //calibrate in the moment
			double datasum = dataDet + dataScat;
			//if (dataScat < electronHigh && datasum < 511 + sigmatot  && datasum > 511 - sigmatot)
			h2d->Fill(dataDet, dataScat);
		}
	}
	
	h2d->SetTitle("2D Energy spectrum 50 degrees (no gates)");
   // Drawing
   h2d->SetXTitle("Energy Detector (keV)");
   h2d->SetYTitle("Energy Scatterer (keV)");
   h2d->GetYaxis()->SetTitleOffset(1.4);
  	
	gStyle->SetPalette(49); //to set the range of colors, until 9 it arrives until grey
								// The default palette defines:
							//   index 0->9   : grey colors from light to dark grey
							//   index 10->19 : "brown" colors
							//   index 20->29 : "blueish" colors
							//   index 30->39 : "redish" colors
							//   index 40->49 : basic colors
   h2d->Draw("COLZ"); //COL = coloured, Z gives the legend
    
}


//graph for comparison theoretical values vs experimental
void histo2DEnergyCompton()
{
   TCanvas *c1 = new TCanvas("c1", "c1",900,900);
     
   TMultiGraph *mg = new TMultiGraph();
   mg->SetTitle("Theoretical and experimental values comparison");
	//angles
	float binx[4]= {0, 30, 50, 90 };
	float errAngle[4]= {0,0,0,0};
	//insert by hand this, energy photon
	float biny[4] = {483, 436, 370.3, 274.9};
	float err1[4] = {35, 53, 62, 44 };
	
	//insert by hand this, energy electron
	float biny2[4] = {23.48, 66.70, 128.4, 233.7};
	float err2[4] = { 19, 36, 53, 49  };
	
	//insert by hand this, energy photon theo
	float biny3[4] = {511, 453.31, 381.34, 260.82};
	//insert by hand this, energy electron theo
	float biny4[4] = {0, 57.69, 129.66, 250.18};

	int nPoints = 4;
	// Define a TGraphErrors which contains the experimental points in x
	// and the theoretical points in y; I semplify by not taking into account errors.
	TGraphErrors *graphErr = new TGraphErrors(nPoints,binx,biny, errAngle, err1);
	TGraphErrors *graphErr2 = new TGraphErrors(nPoints,binx,biny2, errAngle , err2);	
	TGraphErrors *graphTheo = new TGraphErrors(nPoints,binx,biny3);
	TGraphErrors *graphTheo2 = new TGraphErrors(nPoints,binx,biny4);
	
	
	graphErr->SetMarkerStyle(21);
	graphErr->SetMarkerColor(2);
	graphErr->SetLineColor(2);
	graphErr->SetTitle("photon energy");
	//graphErr->SetTitle("Theoretical and experimental values comparison");
	graphErr->GetXaxis()->SetTitle("Angle");
	graphErr->GetYaxis()->SetTitle("Energy (keV)");
	graphErr->GetYaxis()->SetTitleOffset(1.2);
    graphErr->SetDrawOption("AP");
    
    graphErr2->SetMarkerStyle(20);
    graphErr2->SetMarkerColor(4);
	graphErr2->SetLineColor(4);
	graphErr2->SetTitle("electron energy");
    graphErr2->SetDrawOption("P");
        
    graphTheo->SetLineColor(2);
	graphTheo->SetTitle("photon energy (th)");
    graphTheo->SetDrawOption("PL");
   
    graphTheo2->SetLineColor(4);
	graphTheo2->SetTitle("electron energy (th)");
    graphTheo2->SetDrawOption("PL");
    
   mg->Add(graphErr, "P");
   mg->Add(graphErr2, "P");
   mg->Add(graphTheo, "C");
   mg->Add(graphTheo2, "C");
    mg->Draw("APC"); 
    //all this must go after the Draw option
    mg->GetXaxis()->SetTitle("Angle");
    mg->GetYaxis()->SetTitleOffset(1.2);
   mg->GetYaxis()->SetTitle("Energy (keV)");
    c1->BuildLegend(); //after the Draw option!
    //graphErr->Draw("AP"); //COL = coloured, Z gives the legend
    //graphErr2->Draw("P");
    //c1->Modified();
    //c1->Update();
}

//to fix coincidence with tagger and detector
void fix_coinc2channels(const char *name_infile, const char *name_outfile){
    
    slimport_data_t indataA, indataB, indataC, indataD;
    //offset are the correct data in coincidence to register, ent is...
    int offsetA=0, offsetB=0, offsetC=0, offsetD=0, ent=0;
    int tolerance = 25; // in timestamp units, define the tolerance in which one can consider 
						//the events as in coincidence
    TFile *infile = new TFile(name_infile);
    TTree *intree = (TTree*)infile->Get("acq_tree_0");
    
//If you are using channels in a different order change here!
    TBranch *inbranchA = intree->GetBranch("acq_ch0");
    //TBranch *inbranchB = intree->GetBranch("acq_ch1");
    TBranch *inbranchC = intree->GetBranch("acq_ch2");
    //TBranch *inbranchD = intree->GetBranch("acq_ch3");
    inbranchA->SetAddress(&indataA.timetag);
    //inbranchB->SetAddress(&indataB.timetag);
    inbranchC->SetAddress(&indataC.timetag);
    //inbranchD->SetAddress(&indataD.timetag);

    TFile *outfile = new TFile(name_outfile, "RECREATE");
    TTree *outtree = intree->CloneTree(0); //so you create a new root file with the proper coincidences

  // If you are acquiring with just 2 channels (ch0 and ch2)
                                                      
     while(ent+offsetA<inbranchA->GetEntries() && ent+offsetC<inbranchC->GetEntries()){
         inbranchA->GetEntry(ent+offsetA);
         inbranchC->GetEntry(ent+offsetC);
         if(indataA.timetag < indataC.timetag - tolerance){
	     offsetA++;
	     continue;
	 } else if(indataA.timetag > indataC.timetag + tolerance){
	     offsetC++;
	     continue;
	 }
                                                         
         ent++;                                                                                         
         outtree->Fill();
                                                          
         if(ent == 1) cout << endl << "Let's fix the coincidences!" << endl << endl;
         if(ent % 25000 == 0 && ent != 0){
             cout << ent << " entries analyzed. Only " << inbranchA->GetEntries() - ent << " entries left." << endl;
             cout.flush();
         }
                                                         
     

 }  

 
    
    outfile->cd();
    outtree->Write();
    outfile->Close();
    delete outfile;

}

//macro for computing the cross section
void crossSection() //MISSING: ERR I ERR SIGMA; WITH THIS TRIAL DATA THE RESULT IS CLOSE
{
	//all in grams and centimeter
	float Sigma = 2630;
	float SigmaErr = TMath::Sqrt(Sigma);
	float epsilon =  0.682;
	float epsilonErr = 0.001;
	float rho = 2.7;
	float heightAl = 0.7;		//valore di prova
	float surfaceAl = 9.07;		//valore di prova
	
	//the other factor for the computation of N
	float factorN = 2.899; //*10^23
	float V = heightAl * surfaceAl;
	float N = V * factorN * rho;
	float NErr = 0.1 * N / heightAl;
	
	//for the solid angle of the detector
	float distance = 22;
	float height = 3.75; //valore di prova
	float surface = height * height * 3.14;
	float angle = TMath::ATan(height/ distance);
	//solid angles
	float sangle = 6.28*(1-TMath::Cos(angle));
	float a = 14 + ( distance * distance); //argument of square root
	float sangleErr =  6.28 * ( (TMath::Sqrt( a) - ( (distance * distance) /(TMath::Sqrt( a)) ))/a );
	
		
	float At = 525451;
	float AtErr = TMath::Sqrt(At);
	float A511 = 158609;
	float A511Err = TMath::Sqrt(A511);
	float F511 = A511/At;
	float F511Err = TMath::Sqrt( ( A511*A511*A511Err*A511Err) + (At*At*AtErr*AtErr)) / (At*At);
	//float Nscaler = 108570000;	//valore di prova
	float Nscaler = 19438743;
	float NscalerErr = TMath::Sqrt(Nscaler);
	float I = Nscaler * F511;
	float IErr = TMath::Sqrt(( Nscaler * F511Err) * ( Nscaler * F511Err) + (NscalerErr *F511 ) * (NscalerErr * F511 ));
	
	//calculus of sigma
	float sigma = Sigma / (epsilon * N * sangle * (I/surface));
		
	float sigmaErr = SigmaErr/(epsilon * N * sangle * (I/surface));
	
	cout << "Volume " << V  <<  " SigmaErr " << SigmaErr << endl;
	cout << "Number " << N << " error " << NErr << endl;
	cout << "Solid angle " << sangle << " error " << sangleErr << endl;
	cout << "error At " << AtErr << " error A511 " << A511Err << " error F511 " << F511Err << endl;
	cout << "error N scaler "  << NscalerErr << endl;
	cout << "I " << I << " error " << IErr << endl;
	//remember that there is a 10^-23 factor!!!
	cout << "sigma " << sigma << "  error " <<  sigmaErr << endl;
	
	
	
	//classical radius electron squared
	float requadro =  7.89; //*10^-30 m^2
	float Ei = 511;
	float Ef = 260;
	//sigma theoretical
	float sigmath = requadro/2 * (Ef/Ei)*(Ef/Ei)*( (Ef/Ei) + (Ei/Ef) -1);
	cout << "theoretical value " << sigmath << endl;
	
	
}

//histogram for relation error/statistic
void makeHistogramStatistic() { // I try the calibration fo Europium
	
	// in this example they are the europium energies
	float binx[4]= {3840, 11584, 51135, 101504 };
	
	//insert by hand this
	float y[4] = {25.01, 8.16, 3.84, 2.74 };
	float biny[4];
	//the real ones
	for (int i = 0; i<4; i++)
	{
		biny[i]	= (y[i] / binx[i])* 100;	
	}

	int nPoints = 4;
	// Define a TGraphErrors which contains the experimental points in x
	// and the theoretical points in y; I semplify by not taking into account errors.
	TGraphErrors *graphErr = new TGraphErrors(nPoints,binx,biny);

	// Define a function which fits the points, [0] is the parameter (don't use p0, p1 ecc)
	TF1 *fitfun = new TF1("calfitfun1","[0]*1/sqrt(x)",binx[0],binx[3]); //change pol(nPoints-1) and the maximum bin value
																// in binx[nPoints-1]

	// Graphical options to draw the fit
	graphErr->SetMarkerStyle(21);
	graphErr->SetLineColor(2);
	graphErr->SetTitle("Relation centroid error-statistic");
	graphErr->GetXaxis()->SetTitle("Entries");
	graphErr->GetYaxis()->SetTitle("error");
	// Fit
	graphErr->Fit(fitfun, "R");
	graphErr->Draw("AP"); //A = Axis are drawn around the graph 
							// L = A simple polyline is drawn 
							// P= The current marker is plotted at each point 
							// (can be useful) C= a smooth Curve is drawn

	gStyle->SetOptStat("");
	gStyle->SetOptFit(11);
	//If I want to evaluate the function at different points (here at 600)
	cout << fitfun->Eval(511) << endl;
}


//does not work
void histo3DCompton(char *name_file, double numBins, double minX, double maxX)
{
   TCanvas *c1 = new TCanvas("c1", "c1",900,900);
   gStyle->SetOptStat(0);
      
   // Create, fill and project a 2D histogram.
   //this strange choice of min and max range was to properly center the numbers
   TH3F *h3 = new TH3F("h3","",numBins,minX,maxX,numBins,minX,maxX,100,0,1000); //(name, nBinsX, minX, maxX, nBinsY, minY, maxY, nBinsZ, minZ, maxZ )
   
   slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("acq_tree_0");
	TBranch *inbranch0 = intree->GetBranch("acq_ch0");
	TBranch *inbranch1 = intree->GetBranch("acq_ch1");
	TBranch *inbranch2 = intree->GetBranch("acq_ch2");
		
	inbranch0->SetAddress(&indata.timetag);
	inbranch1->SetAddress(&indata.timetag);
	inbranch2->SetAddress(&indata.timetag);
	
		
	// values of trigger with calibration of tagger, they correspond to 460-560
	double lowThr = 8510;
	double highThr = 10328;
	//change this depending on the histogram
	double sigmatot = 100;
	double electronHigh = 180;
	// histogram filling
	for (int i=0; i<inbranch0->GetEntries(); i++) {
		inbranch0->GetEntry(i);
		if (indata.qlong>lowThr && indata.qlong<highThr) {  // This fills only if the data is in the range I gave with lowThr and highThr
			inbranch1->GetEntry(i);
			double dataScat = 0.0581*indata.qlong -7.937 ;
			inbranch2->GetEntry(i);
			double dataDet = 0.0529*indata.qlong -6.302; //calibrate in the moment
			double datasum = dataDet + dataScat;
			if (dataScat < electronHigh && datasum < 511 + sigmatot  && datasum > 511 - sigmatot )
			h3->Fill(dataDet, dataScat, 0);
		}
	}
   
   h3->SetTitle("3D plot scatterer-detector");
   // Drawing
   
   h3->SetXTitle("Detector (keV)");
   h3->SetYTitle("Scatterer (keV)");
   h3->SetZTitle("Events");
   h3->GetXaxis()->SetTitleOffset(1.8);
   h3->GetYaxis()->SetTitleOffset(2);
   h3->GetZaxis()->SetTitleOffset(1.5);
   
   //h3->GetXaxis()->CenterTitle();
   //h3->GetYaxis()->CenterTitle();
   
   //h2->GetXaxis()->SetLabelSize(0); //this removes the label, i.e. the numbers from the axis
   	
   h3->Draw("ISO"); //ISO = draws the iso surface
    
}








//Functions for calculations

//macro for calculate the resolution 
float resolution(float mean, float sigma)
{
	//resolution
	float fwhm = 2*TMath::Sqrt( 2*TMath::Log(2) )*sigma;
	float resol = fwhm/mean;
	
	cout << resol << endl;
	return resol;
	
}






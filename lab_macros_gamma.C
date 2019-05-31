#include "lab_macros.C"
#include <iomanip>
#include <fstream>

TH1F* calibrateNaI(char *file_na, char *file_bg,int numBins, double minX, double maxX ) { 
	//the error are told in the minimizer

	// canvas (I suppose that if I put draw option at the end, the canvas option is redundant)
	//TCanvas *c0 = new TCanvas("c0");

	// read the histos
	// remember subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX)
	TH1F *h_na = subtract_bg(file_na, file_bg, 0, numBins, minX, maxX);
	
	// call the function to calibrate the histogram
	CalibrateHisto(h_na, 0.2584, -9.537);
	
	// Personalization things
	h_na->SetXTitle("Energy (keV)");
	h_na->SetYTitle("counts");
	
	//for europium
	//h_na->GetXaxis()->SetRange(minX,maxX);
	h_na->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	
	//the i is for the integral
	//gStyle->SetOptStat("i"); //to show what I want in the panel, see reference 
								//(the letters specify which information will be included)
	/* Europium personalization things							
	h_na->SetTitle("Europium spectrum");
	gPad->SetLogy(); */
	
    
    
	h_na->Draw();
	return h_na;

}



TH1F* calibrateHPGe(char *file_na, char *file_bg,int numBins, double minX, double maxX ) { 
	
	// read the histos
	// remember subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX)
	TH1F *h_na = subtract_bg(file_na, file_bg, 1, numBins, minX, maxX);
	/*
	// fit the peaks with the Na energies
	float na_bin[3];
	
	 // uncomment this and comment the next for HPGe calibration
	//insert on hand this, these are for HPGe calibration
	na_bin[0] = 3815.; 
	na_bin[1] = 9517.;
	na_bin[2] = 445.; //americium 
	
	 // uncomment this and comment the previous for NaI calibration
	//insert on hand this, these are for NaI calibration
	na_bin[0] = 2043.; 
	na_bin[1] = 4942.;
	na_bin[2] = 264.; //americium */
	/*
	//insert on hand this, these are for HPGe calibration
	float na_kev[3]; 
	na_kev[0] = 511.; 
	na_kev[1] = 1275.;
	na_kev[2] = 59; //americium
	
	//add here other calibration things if you have any
	
	int nPeaks = 3; // The 2 of sodium and the one for americium for example
		
	TGraphErrors *graphErr = new TGraphErrors(nPeaks,na_bin,na_kev);
	TF1 *fitfun = new TF1("calfitfun1","pol1",na_bin[2],na_bin[1]); // na_bin[2] is the minimum x value,
																	// na_bin[1] is the maximum x value 
	graphErr->Fit(fitfun); //this finds the fitfun

	// get the fit parameters
	float m = fitfun->GetParameter(1);
	float q = fitfun->GetParameter(0);
*/
	// call the function to calibrate the histogram
	CalibrateHisto(h_na, 0.134, -0.6153);
	
	// Personalization things
	h_na->SetXTitle("Energy (keV)");
	h_na->SetYTitle("counts");
	
	//for europium
	//h_na->GetXaxis()->SetRange(minX,maxX);
	h_na->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	
	//the i is for the integral
	gStyle->SetOptStat("i"); //to show what I want in the panel, see reference 
								//(the letters specify which information will be included)
	/* Europium personalization things							
	h_na->SetTitle("Europium spectrum");
	gPad->SetLogy(); */
	
    
    
	h_na->Draw();
	return h_na;

}


void peaksearch(char *file_na, char *file_bg, short chan, float sigma, double minratio) {

	// creare il canvas
	/*
	TCanvas *c0 = new TCanvas("c0");
	c0->Divide(2,2); // to draw more histos on a Canvas (see slides)
	
	TH1F *h_na = getHistoForChannelFromTree(file_na, 0, 4000, 10, 16000);
	TH1F *h_bg = getHistoForChannelFromTree(file_bg, 0, 4000, 10, 16000);
	*/
	TH1F *h_subtr;
	if (chan == 0)
		TH1F *h_subtr = calibrateNaI(file_na, file_bg, 250, 0, 8000); //REMEMBER to change the calibration!!!
	
	if (chan == 1)
		TH1F *h_subtr = calibrateHPGe(file_na, file_bg, 250, 0, 14000);	
	
	
	TH1F *h_peaks = (TH1F*)h_subtr->Clone(); // copia l'istogramma
/*
	h_na->SetTitle("Measured spectrum");
	h_bg->SetTitle("Measure of background");
	h_subtr->SetTitle("Subtracted background");*/
	h_peaks->SetTitle("Reference Canister spectrum HPGe");
	
//draw the histograms on multiple canvas
	/*c0->cd(1);
	h_na->Draw();
	c0->cd(2);
	h_bg->Draw();
	c0->cd(3);
	h_subtr->Draw();
	c0->cd(4);*/
	h_peaks->Draw();

	TSpectrum *s = new TSpectrum(30);
	int nPeaks;
	float *xPeaks; //array that contains the coordinate of the peak
	
	//play with sigma and minratio to get the right results
	//float sigma = 1;
	//double minratio = 0.01;	// minimum ratio between a peak and the main peak

	nPeaks = s->Search(h_peaks,sigma,"",minratio);

	xPeaks = (float*)s->GetPositionX();
	for (int p = 0; p<nPeaks; p++) {
		cout << "Peak #" << p << " @ channel " << xPeaks[p] << endl;	
	}
	
	
	//personalization things
	//h_peaks->SetMinimum(0); //this is because subtact_bg does not properly eliminate negative entries
	//gPad->SetLogy();
	  gStyle->SetOptStat("ie");
	
	//Print the result as png image
	//c0->Print(Form("peaks_sigma%i.png",sigma));

}



//macro to do a generic histogram by hand
void makeHistogramEu() { // I try the calibration fo Europium
	
	// in this example they are the europium energies
	float binx[7]= {121.8, 244.7, 344.3, 778.9, 964., 1112.1, 1408 };
	
	//insert by hand this
	float biny[7] = {8.4, 7.1, 3.9, 2.1, 1.6, 1.5,1. };
	

	int nPoints = 7;
	// Define a TGraphErrors which contains the experimental points in x
	// and the theoretical points in y; I semplify by not taking into account errors.
	TGraphErrors *graphErr = new TGraphErrors(nPoints,binx,biny);

	// Define a function which fits the points
	TF1 *fitfun = new TF1("calfitfun1","expo",binx[0],binx[6]); //change pol(nPoints-1) and the maximum bin value
																// in binx[nPoints-1]

	// Graphical options to draw the fit
	graphErr->SetMarkerStyle(21);
	graphErr->SetLineColor(2);
	graphErr->SetTitle("HPGe relative efficiency curve");
	graphErr->GetXaxis()->SetTitle("Energy (keV)");
	graphErr->GetYaxis()->SetTitle("relative efficiency");
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

//macro to do efficiency curve for NaI
void makeHistogramNaI() { // I try the calibration fo Europium
	
	TCanvas *se = new TCanvas;

	//energies
	float binx[3]= {59, 511, 1275 };
	
	//insert by hand this
	float biny[3] = {0.502, 0.571, 0.208};
	

	int nPoints = 3;
	// Define a TGraphErrors which contains the experimental points in x
	// and the theoretical points in y; I semplify by not taking into account errors.
	TGraphErrors *graphErr = new TGraphErrors(nPoints,binx,biny);
		   
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","pol1",binx[0],binx[1]); //this is to find the proper parameters from the single fits
   TF1 *g2    = new TF1("g2","expo",binx[1],binx[2]); //and then puth them on the total fit
   
     
   graphErr->Fit(g1,"R"); //"R" Use the Range specified in the function range   

   graphErr->Fit(g2,"R+"); //"+" Add this new fitted function to the list of fitted functions (by default, 
					//any previous function is deleted)  

	// Graphical options to draw the fit
	graphErr->SetMarkerStyle(21);
	graphErr->SetLineColor(2);
	graphErr->SetTitle("NaI efficiency curve");
	graphErr->GetXaxis()->SetTitle("Energy (keV)");
	graphErr->GetYaxis()->SetTitle("Intrinsic efficiency");
	gStyle->SetOptStat(0);
	gStyle->SetOptFit(11);
	graphErr->Draw("AP"); //A = Axis are drawn around the graph 
							// L = A simple polyline is drawn 
							// P= The current marker is plotted at each point 
							// (can be useful) C= a smooth Curve is drawn

	 se->Update();
   
   // Retrieve the stat box
   TPaveStats *ps = (TPaveStats*)se->GetPrimitive("stats");
   ps->SetName("mystats");
   TList *listOfLines = ps->GetListOfLines();
   
   // Add a new line in the stat box.
   // Note that "=" is a control character
	TLatex *p0 = new TLatex(0,0,"Constant = 0.11 #pm 4.41"); // Tlatex is like latex but with # instead of \ for special characters
											 // the numbers before the text specify the position of the thing
	TLatex *p1 = new TLatex(0,0,"Slope = -0.0013 #pm 0.0068 "); //add by hand the parameters of the second gaussian
	
	//set of the lines
	p0->SetTextSize(0.04);
	p1->SetTextSize(0.04);
	p0->SetTextFont(42);
	p1->SetTextFont(42);
	
	//add the lines
    listOfLines->Add(p0);
    listOfLines->Add(p1);
    
    // the following line is needed to avoid that the automatic redrawing of stats
    //graphErr->SetStats(0);
    //reupdate the canvas
    ps->DrawClone();

	
}



//fit with 2 guassian, to do the calibration graphs
void multifit2gamma(char *file, char *file_bg) {
	
	// remember TH1F* getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)
 
 // or put subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX);
 // instead of getHistoForChannelFromTree
	TH1F *h = subtract_bg(file, file_bg, 1, 1000, 3500, 10500);
	
   Double_t par[6]; // I need 6 parameters for two gaussians
   
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","gaus",3600,4000); //this is to find the proper parameters from the single fits
   TF1 *g2    = new TF1("g2","gaus",9500,10000); //and then puth them on the total fit
   
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
   
   h->SetTitle("Calibration HPGe with Na source");
   h->SetXTitle("bins");
   h->SetYTitle("counts");
   h->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
   gStyle->SetOptStat(0);
   gStyle->SetOptFit(0);  
   h->Draw();   
}



//fit with 2 guassian, to do the calibration graphs
void multifit2gammaCalibrated(char *file, char *file_bg) {
	
	TCanvas *se = new TCanvas;
	// remember TH1F* getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)
 
 // or put subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX);
 // instead of getHistoForChannelFromTree
	TH1F *h = calibrateNaI(file, file_bg, 1000, 0, 6000); //REMEMBER to change the type of calibration!!!
	
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
   h->SetTitle("Calibrated Na spectrum (NaI)");
     
   //all the things down here are to properly insert the stats of the second fit
   
   
   gStyle->SetOptStat(0);
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
	TLatex *p1 = new TLatex(0,0,"Mean2 = 1272.73 #pm 0.33 "); //add by hand the parameters of the second gaussian
	TLatex *p2 = new TLatex(0,0,"Sigma2 = 43.7 #pm 0.4"); // p0 is constant, p1 mean, p2 sigma
	
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
void multifit1gammaCalibrated(char *file, char *file_bg) {
	
		
	TH1F *h = calibrateHPGe(file, file_bg, 500, 0, 3000); //REMEMBER to change the type of calibration!!!
	
     
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","gaus",20,90); //this is to find the proper parameters from the single fits
     
   h->Fit(g1,"R"); //"R" Use the Range specified in the function range   
      
   gStyle->SetOptStat(0);
   gStyle->SetOptFit(11);
   
   h->SetTitle("Calibrated Am spectrum (HPGe)");
   h->Draw();
   
   
}


//fit with 1 guassian, to do the calibration for Americium
void multifit1gamma(char *file, char *file_bg) {
	
	// remember TH1F* getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)
 
 // or put subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX);
 // instead of getHistoForChannelFromTree
	TH1F *h = subtract_bg(file, file_bg, 1, 500, 10, 3000);
	
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","gaus",10,500); //this is to find the proper parameters from the single fits
  
   //total->SetLineColor(3);
   
   h->Fit(g1,"R"); //"R" Use the Range specified in the function range   

     
   h->SetTitle("Calibration HPGe with Am source");
   h->SetXTitle("bins");
   h->SetYTitle("counts");
   h->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
   gStyle->SetOptStat(0);
   gStyle->SetOptFit(0);  
   h->Draw();   
}






//does not work properly
TH1F* peakIntegral(char *file_na, char *file_bg, short chan, float sigma, double minratio ) { 
	
	TH1F *h_na;
	
	if ( chan ==0)
		TH1F *h_na = calibrateNaI(file_na, file_bg, 250, 0, 12000);
		
	if( chan ==1)
		TH1F *h_na = calibrateHPGe(file_na, file_bg, 4000, 0, 12000);
		
	
	TH1F *h_peaks = (TH1F*)h_na->Clone(); // copia l'istogramma

	h_peaks->SetTitle("Reference Canister spectrum HPGe");
	
	h_peaks->Draw();

	TSpectrum *s = new TSpectrum(30);
	int nPeaks;
	float *xPeaks; //array that contains the coordinate of the peak
	
	//play with sigma and minratio to get the right results
	//float sigma = 1;
	//double minratio = 0.01;	// minimum ratio between a peak and the main peak

	nPeaks = s->Search(h_peaks,sigma,"",minratio);

	xPeaks = (float*)s->GetPositionX();
		
 //ordinamento dei picchi in ordine crescente
 
	for(int i=0; i<nPeaks-1; i++)
	{
	int min = i;

	for(int j=i+1; j<nPeaks; j++){
		if(xPeaks[j] < xPeaks[min]) //cambiare questa condizione per invertire l'ordine
		{	min = j;}
	}
	int temp=xPeaks[min];
	xPeaks[min]=xPeaks[i];
	xPeaks[i]=temp;
	}
		
	for (int p = 0; p<nPeaks; p++) {
		float min_range, max_range;
		cout << "Peak #" << p << " @ channel " << xPeaks[p] << endl;
		if (p==0){
		float min_range = 0;
		float max_range = (xPeaks[1] + xPeaks[0]) / 2;	
			
		} 
		
		else if (p== nPeaks -1){
		float min_range = (xPeaks[p-1] + xPeaks[p])/2;
		float max_range = 12000;		
		} 
		
		else {
		float min_range = (xPeaks[p-1] + xPeaks[p])/2;
		float max_range = (xPeaks[p] + xPeaks[p+1]) / 2;		
		} 
		
		
		TF1 *g1    = new TF1("g","gaus",min_range,max_range);
		h_peaks->Fit(g1,"R"); //"R" Use the Range specified in the function range
		cout << "Integral " << g1->Integral(min_range,max_range) << endl;
		
	}
		
	// Personalization things
	h_na->SetXTitle("Energy (keV)");
	h_na->SetYTitle("counts");
	h_na->SetMinimum(0);
	
	h_na->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	
	//the i is for the integral
	gStyle->SetOptStat("i"); //to show what I want in the panel, see reference 
								//(the letters specify which information will be included)
		
	return h_peaks;

}












//functions to calculate things

//calculation of the efficiency
void solidAngle(float distance1 , float distance2) {
	// all in centimeters
	float height_NaI = 3.75;
	float height_HPGe = 1.95;
	
	float angle_NaI = TMath::ATan(height_NaI / distance1) ;
	float angle_HPGe = TMath::ATan(height_HPGe / distance2);
	
	//solid angles
	float sangle_NaI = 6.28*(1-TMath::Cos(angle_NaI)) ;
	float sangle_HPGe = 6.28*(1-TMath::Cos(angle_HPGe));

	cout << "Angle NaI" << sangle_NaI << endl;
	cout << "Angle HPGe" << sangle_HPGe << endl;

}


//calculation of the efficiency
float solidAngleNaI(float distance1) {
	// all in centimeters
	float height_NaI = 3.75;
		
	float angle_NaI = TMath::ATan(height_NaI / distance1) ;
		
	//solid angles
	float sangle_NaI = 6.28*(1-TMath::Cos(angle_NaI)) ;
	
	return sangle_NaI;
}


//calculation of the efficiency
float solidAngleHPGe(float distance2) {
	// all in centimeters
	float height_HPGe = 1.95;
	float angle_HPGe = TMath::ATan(height_HPGe / distance2);
	
	//solid angles
	float sangle_HPGe = 6.28*(1-TMath::Cos(angle_HPGe));
	
	return sangle_HPGe;
}





void intrinsicEfficiency()
{   // Corrected:   Am(23cm)	Na(511)(20cm) Na(1275)(6.5cm) Am (20cm) Na(511)(23cm)	Na(1275)(23cm)      
	// Order: 		Am(23cm)	Na(511)(6.5cm) Na(1275)(6.5cm) Am (20cm) Na(511)(23cm)	Na(1275)(23cm) 
	// parameters
	float time[6] = {359,285,285,600,172,172};
	float sangle_NaI[6]; //solid angle
	float sangle_HPGe[6]; //solid angle
	
	float activity[6]= {380,2.2, 2.2, 380, 2.2, 2.2 }; //old activities
	//float activity[6]= {363,0.0106, 0.0106, 363, 0.0106, 0.0106  }; //new activities, with these all the results are wrong
	float fraction[6]= {0.36,1.8,1,0.36,1.8,1}; //fraction of gamma in a given transition
	float integralNaI[6]= {310000, 89000, 18000, 352000, 2910, 680}; //number of counts under a peak NaI
	float integralHPGe[6]= {28360, 3815, 990, 43400, 380, 103 }; //number of counts under a peak HPGe
	
	//to find the solid angles (these are the original ones)
	//float distance_NaI[6] = {6.5, 6.5, 6.5, 10 , 23, 23 }; //??????
	//float distance_HPGe[6] = {6.5, 6.5, 6.5, 20, 20, 20};//??????
	
	float distance_NaI[6] = {6.5, 6.5, 6.5, 10 , 23, 23 }; //??????
	float distance_HPGe[6] = {6.5, 6.5, 6.5, 20, 20, 20};//??????
	
	for (int i = 0; i<6; i++)
	{
		sangle_NaI[i] = solidAngleNaI(distance_NaI[i]);
		sangle_HPGe[i] = solidAngleHPGe(distance_HPGe[i]);
		
		}
		
	
	float N_NaI[6];
	float N_HPGe[6];
	float efficiencyNaI[6];
	float efficiencyHPGe[6];

	
	for (int i = 0; i<6; i++)
	{	//remember that the activity is in kiloBq!!!!
		N_NaI[i]= 1000*(time[i]*sangle_NaI[i]*activity[i]*fraction[i])/ 12.56; //number of gamma, 12.56 = 4 \pi
		cout << i << ")" << " Number of gamma NaI " << "\t" << N_NaI[i] << endl;
		
		//I will assume the error as the square root of counts	
		efficiencyNaI[i] = integralNaI[i]/N_NaI[i];
		float error1 = TMath::Sqrt(integralNaI[i])/N_NaI[i];
		cout << i << ") efficiency NaI " << "\t" << efficiencyNaI[i] << "\t" << "error" << "\t" << error1 << endl;
		cout << endl;
		
		
		N_HPGe[i]= 1000*(time[i]*sangle_HPGe[i]*activity[i]*fraction[i]) / 12.56; //number of gamma
		cout << i << ") Number gamma HPGe " << "\t" << N_HPGe[i] << endl;
			
		efficiencyHPGe[i] = integralHPGe[i]/N_HPGe[i];
		float error2 = TMath::Sqrt(integralHPGe[i])/N_HPGe[i];
		cout << i << ") efficiency HPGe " << "\t" << efficiencyHPGe[i] << "\t" << "error" << "\t" << error2 << endl;
		cout << endl;
		}
	}
	
//to convert the number in a string
std::string converterHPGe(int i)
{
	std::string s0 = "Am";	
	std::string s1 = "Na1";
	std::string s2 = "Na2";
	std::string s3 = "Na1(23)";
	std::string s4 = "Na2(23)";
			
	if (i == 0)
		return s0;
	if (i == 1)
		return s1;
	if (i == 2)
		return s2;
	if (i == 3)
		return s3;
	if (i == 4)
		return s4;
	else //devo mettere una scappatoia altrimenti il compiler rompe i coglioni
		return s0;
}


//to convert the number in a string
std::string converterNaI(int i)
{
	std::string s0 = "Na1";	
	std::string s1 = "Na2";
	std::string s2 = "Am";
				
	if (i == 0)
		return s0;
	if (i == 1)
		return s1;
	if (i == 2)
		return s2;
	else //devo mettere una scappatoia altrimenti il compiler rompe i coglioni
		return s0;
	}



void intrinsicEfficiency2()
{   // original:   		Am(23cm)	Na(511)(6.5cm) Na(1275)(6.5cm) Am (20cm) Na(511)(23cm)	Na(1275)(23cm)      
	// Order NaI: 		Na(511)(6.5cm) Na(1275)(6.5cm) Am (20cm)   levare primo e ultimi due
	//Order HPGe: 		Am(23cm)	Na(511)(6.5cm) Na(1275)(6.5cm)  Na(511)(23cm)	Na(1275)(23cm) levare quarto
	// parameters
	float timeNaI[3] = {285,285,600};
	float timeHPGe[5] = {359,285,285,172,172};

	float sangle_NaI[3]; //solid angle
	float sangle_HPGe[5]; //solid angle
	
	//float activityNaI[3]= {2.2, 2.2, 380 }; //old activities
	//float activityHPGe[5]= {380,2.2, 2.2, 2.2, 2.2 }; //old activities
		
	float activityNaI[3]= {1.62, 1.62, 379  }; //new activities, with these all the results are wrong
	float activityHPGe[5]= {379, 1.62, 1.62, 1.62, 1.62  }; //new activities, with these all the results are wrong
	
	
	float fractionNaI[3]= {1.8,1,0.36}; //fraction of gamma in a given transition
	float fractionHPGe[5]= {0.36,1.8,1,1.8,1}; //fraction of gamma in a given transition
	
	
	float integralNaI[3]= { 89000, 18000, 352000}; //number of counts under a peak NaI
	float integralHPGe[5]= {28360, 3815, 990, 380, 103 }; //number of counts under a peak HPGe
	
	//to find the solid angles (these are the original ones)
	//float distance_NaI[6] = {6.5, 6.5, 6.5, 10 , 23, 23 }; //??????
	//float distance_HPGe[6] = {6.5, 6.5, 6.5, 20, 20, 20};//??????
	
	float distance_NaI[3] = { 3, 3, 20 }; //with these it seems I have coherent values
	float distance_HPGe[5] = {23, 7, 7, 18, 18};//??????
	
	for (int i = 0; i<3; i++)
		sangle_NaI[i] = solidAngleNaI(distance_NaI[i]);
		
	for (int i = 0; i<6; i++)
		sangle_HPGe[i] = solidAngleHPGe(distance_HPGe[i]);
		
		
	
	float N_NaI[3];
	float N_HPGe[5];
	float efficiencyNaI[3];
	float efficiencyHPGe[5];

//setw has to be used every time whereas setprecision and fixed are persistent (fixed to make setprecision act only on decimals)

	cout  << "type"  << "\t" << setw(10) << "N gamma" << "\t" << setw(10) << "error" << "\t" << 
	setw(10) << "efficic" << "\t" << "error" << endl;
	for (int i = 0; i<3; i++)
	{	//remember that the activity is in kiloBq!!!!
		N_NaI[i] = 1000*(timeNaI[i]*sangle_NaI[i]*activityNaI[i]*fractionNaI[i])/ 12.56; //number of gamma, 12.56 = 4 \pi
		
		//I will assume the error as the square root of counts and 1 cm error for the distance
		float a = 14 + ( distance_NaI[i] * distance_NaI[i]); //argument of square root
		float error =  6.28 * ( (TMath::Sqrt( a) - ( (distance_NaI[i] * distance_NaI[i]) /(TMath::Sqrt( a)) ))/a );
		// i put an error of 0.1 cm (in reports say that the error is 0.2)
		float error1 = 0.1 * error * 1000*(timeNaI[i]*activityNaI[i]*fractionNaI[i])/ 12.56;		
		efficiencyNaI[i] = integralNaI[i]/N_NaI[i] ;
		float errorcount = TMath::Sqrt(integralNaI[i]);
		float error2 = TMath::Sqrt((integralNaI[i] * integralNaI[i] * errorcount * errorcount ) +
						(N_NaI[i] * N_NaI[i] * error1 * error1  )) / (N_NaI[i] * N_NaI[i]);
						
		cout  << converterNaI(i) << "\t" << setw(10) << fixed << setprecision(0) << N_NaI[i] << "\t" << setw(10)  << error1 << "\t" << 
		setw(10) << setprecision(3) <<  efficiencyNaI[i] << "\t" << error2 << endl;
		
		}
	//HPGe part
	cout << "type"  << "\t"  << setw(10) << "N gamma" << "\t" << setw(10) << "error" << 
	"\t" << setw(10) << "efficic" << "\t" << "error" << endl;
	
		for (int i = 0; i<5; i++) {
			
		N_HPGe[i]= 1000*(timeHPGe[i]*sangle_HPGe[i]*activityHPGe[i]*fractionHPGe[i]) / 12.56; //number of gamma
		float a = 3.8 + ( distance_HPGe[i] * distance_HPGe[i]); //argument of square root
		float error = 6.28 * ((TMath::Sqrt(a) - ( (distance_HPGe[i] * distance_HPGe[i]) /(TMath::Sqrt( a)) ) )/a );
		// i put an error of 0.1 cm (in reports say that the error is 0.2)
		float error1 = 0.1 * error * 1000*(timeHPGe[i]*activityHPGe[i]*fractionHPGe[i]) / 12.56;
		efficiencyHPGe[i] = integralHPGe[i]/N_HPGe[i];
		float errorcount = TMath::Sqrt(integralHPGe[i]);
		float error2 = TMath::Sqrt((integralHPGe[i] * integralHPGe[i] * errorcount * errorcount ) +
						(N_HPGe[i] * N_HPGe[i] * error1 * error1  )) / (N_HPGe[i] * N_HPGe[i]);
		
		cout << converterHPGe(i) << "\t" << setw(10) << fixed << setprecision(0) << N_HPGe[i] << "\t" << setw(10) 
		<< error1 << "\t" << setw(10) << setprecision(3) << efficiencyHPGe[i] << "\t" << error2 << endl;
		
		}
	
	
	}



//macro for propagation of errors in the division
void errorsDivision()
{
	//values (x numerator, y denominator)
	float x = 514900;
	float y = 754900;
	
	//errors
	float ex = TMath::Sqrt(x);
	float ey = TMath::Sqrt(y);
	
	float error = TMath::Sqrt( ( x*x*ex*ex) + (y*y*ey*ey)) / (y*y);
	
	cout << error << endl;
	
	
}


//macro for calculate the resolution 
float resolution(float mean, float sigma)
{
	//resolution
	float fwhm = 2*TMath::Sqrt( 2*TMath::Log(2) )*sigma;
	float resol = fwhm/mean;
	
	cout << resol << endl;
	return resol;
	
}

//to write the errors for europium
void errorCounts()
{
	//for europium relative efficiencies (divide by 250 the original value)
	float counts_eu[18] = {1190,260,500,48,70,70,34,45,45,42,132,58,113,16,97,15,11,100};
	
	// took from the table given
	float real_values[18] = {141, 36.6, 127.2, 4.19, 10.71, 15, 1.984, 2.24, 2.296, 4.12, 62.6, 20.54, 70.4, 3.57, 
								65, 6.67, 7.76, 100};
	float error_realValues[18] = {4, 1.1, 1.3, 0.04, 0.11, 0.15, 0.023, 0.05, 0.028, 0.04, 0.6, 0.21, 0.7, 0.07,
								 0.7, 0.07, 0.08, 1};
	
	float error_eu[18];
	float eff[18];
	float error_eff[18];

	cout  << "counts"  << "\t" << "error" << "\t"  << "rel eff" << "\t" << 
	 "error" << endl;
	for ( int i=0; i < 18; i++)
	{
		error_eu[i] = 0.0632*TMath::Sqrt(counts_eu[i]); //0.0632 = sqrt(250)/250
		error_eff[i] = TMath::Sqrt( ( counts_eu[i]*counts_eu[i]* error_eu[i] * error_eu[i]) + 
						(real_values[i] * real_values[i]*error_realValues[i] * error_realValues[i])) / (real_values[i] * real_values[i]);
		eff[i] = counts_eu[i] / real_values[i];				
		cout << counts_eu[i] << "\t" << fixed << setprecision(1) << error_eu[i] << "\t" << setprecision(2) <<
		eff[i] << "\t" << error_eff[i] << endl;
		
		}
	
	
	
	
}



//to convert the number in a string
std::string converterActivity(int i)
{
	std::string s0 = "Pellet NaI";	
	std::string s1 = "Pellet HPGe";
	std::string s2 = "Plaster NaI";
	std::string s3 = "Plaster HPGe";
	std::string s4 = "background canister NaI";
	std::string s5 = "background canister HPGe";	
	std::string s6 = "reference canister NaI";
	std::string s7 = "reference canister HPGe";
	std::string s8 = "exposed NaI";
	std::string s9 = "exposed HPGe";
	std::string s10 = "autunite NaI";	
	std::string s11 = "autunite HPGe";
	
			
	if (i == 0)
		return s0;
	if (i == 1)
		return s1;
	if (i == 2)
		return s2;
	if (i == 3)
		return s3;
	if (i == 4)
		return s4;	
		
	if (i == 5)
		return s5;
	if (i == 6)
		return s6;
	if (i == 7)
		return s7;
	if (i == 8)
		return s8;
	if (i == 9)
		return s9;	
		
	if (i == 10)
		return s10;
	if (i == 11)
		return s11;
	else //devo mettere una scappatoia altrimenti il compiler rompe i coglioni
		return s0;
		
}




//to calculate the activities, you can try to change this as attenuationCoeffCalculus() of imaging
void Activity()
{
	//useful commands: cat provvisorio.txt | awk '{print $3}' | sed ':l; N; s/\n/,/; tl'    the last to change \n with ,
	
	//order: pellet, plaster, background canister, reference canister, exposed canister, autunite
	// all materials occupy two rows, one for NaI, the other for HPGe, for example 1 = NaI pellet, 2 = HPGe pellet
	float peaks[12][14] = {
		{85, 225, 621, 745, 1009, 1125, 1280, 1636,0,0,0,0,0,0},
		{108, 180, 235, 325, 512, 767, 870, 1380,0,0,0,0,0,0},
		{77, 242, 357, 605, 976, 1446,0,0,0,0,0,0,0,0},
		{93, 242, 400, 580, 805, 1149, 1456,0,0,0,0,0,0,0},
		{300, 580, 853, 960, 1075, 1183, 1447, 1910,0,0,0,0,0,0},
		{86, 220, 310, 392, 565, 856, 1036, 1182, 1396, 1508,0,0,0,0},
		{241, 291, 349, 605, 778, 1108, 1380, 1735,0,0,0,0,0,0},
		{78, 145, 183, 243, 295, 347, 609, 767, 931, 1036, 1119, 1238, 1373, 1508},
		{250, 300, 357, 613, 778, 1116, 1727,0,0,0,0,0,0,0},
		{79, 152, 240, 288, 348, 407, 602, 767, 977, 1120, 1381, 1500,0,0},
		{241, 1166, 1438,0,0,0,0,0,0,0,0,0,0,0},
		{100, 228, 415, 534, 684, 850, 1037, 1216, 1456,0,0,0,0,0},
		
	};
	
	
	float counts[12][14] = {
		{272,456,131,45,40,31,23,20,0,0,0,0,0,0},
		{237,129,67,89,55,40,38,30,0,0,0,0,0,0},
		{2980,2578,1100,926,715,481,0,0,0,0,0,0,0,0},
		{409,121,146,145,76,98,88,0,0,0,0,0,0,0},
		{146,148,53,48,32,69,124,31,0,0,0,0,0,0},
		{550,270,143,134,166,105,101,43,33,31,0,0,0,0},
		{12310,15280,23160,17330,5465,5200,4140,3340,0,0,0,0,0,0},
		{4081,3703,3158,1545,1844,2394,2044,670,450,344,539,355,308,279},
		{8266,12070,17650,13840,4876,4400,2926,0,0,0,0,0,0,0},
		{1820,2394,1389,1113,1412,902,1150,396,295,404,129,143,0,0},
		{8600,8427,10330,0,0,0,0,0,0,0,0,0,0,0},
		{1844,858,489,325,205,392,407,796,518,0,0,0,0,0},
		
	};
	
		//float efficiency[12][14] = { //original ones
		//{0.506, 0.527, 0.494, 0.419, 0.296, 0.254, 0.207, 0.129, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.972,0.837,0.747,0.620,0.421,0.248,0.201,0.070,0.000,0.000,0.000,0.000,0.000,0.000,},
		//{0.505, 0.530, 0.547, 0.504, 0.309, 0.166, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.998,0.736,0.531,0.366,0.230,0.113,0.060,0.000,0.000,0.000,0.000,0.000,0.000,0.000,},
		//{0.539, 0.521, 0.363, 0.315, 0.271, 0.235, 0.166, 0.090, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.998,0.771,0.640,0.540,0.377,0.207,0.142,0.105,0.068,0.054,0.000,0.000,0.000,0.000,},
		//{0.530, 0.537, 0.546, 0.504, 0.401, 0.259, 0.181, 0.113, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.998,0.900,0.832,0.735,0.660,0.592,0.344,0.248,0.177,0.142,0.120,0.094,0.071,0.054,},
		//{0.531, 0.539, 0.547, 0.499, 0.401, 0.257, 0.114, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.998,0.887,0.739,0.669,0.591,0.523,0.350,0.248,0.161,0.120,0.070,0.054,0.000,0.000,},
		//{0.530, 0.240, 0.168, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.988,0.758,0.515,0.402,0.295,0.209,0.142,0.098,0.060,0.000,0.000,0.000,0.000,0.000,},		

	//};
	
	//float efficiency[12][14] = { //modified by hand for too high values
		//{0.506, 0.527, 0.494, 0.419, 0.296, 0.254, 0.207, 0.129, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.851,0.837,0.747,0.620,0.421,0.248,0.201,0.070,0.000,0.000,0.000,0.000,0.000,0.000,},
		//{0.505, 0.530, 0.547, 0.504, 0.309, 0.166, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.752,0.736,0.531,0.366,0.230,0.113,0.060,0.000,0.000,0.000,0.000,0.000,0.000,0.000,},
		//{0.539, 0.521, 0.363, 0.315, 0.271, 0.235, 0.166, 0.090, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.779,0.771,0.640,0.540,0.377,0.207,0.142,0.105,0.068,0.054,0.000,0.000,0.000,0.000,},
		//{0.530, 0.537, 0.546, 0.504, 0.401, 0.259, 0.181, 0.113, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.909,0.900,0.832,0.735,0.660,0.592,0.344,0.248,0.177,0.142,0.120,0.094,0.071,0.054,},
		//{0.531, 0.539, 0.547, 0.499, 0.401, 0.257, 0.114, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.891,0.887,0.739,0.669,0.591,0.523,0.350,0.248,0.161,0.120,0.070,0.054,0.000,0.000,},
		//{0.530, 0.240, 0.168, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		//{0.773,0.758,0.515,0.402,0.295,0.209,0.142,0.098,0.060,0.000,0.000,0.000,0.000,0.000,},		

	//};
	float efficiency[12][14] = {
	{0.505969, 0.527341, 0.493731, 0.419091, 0.295638, 0.253612, 0.20663, 0.129072, 0, 0, 0, 0, 0, 0 },
{0.575565, 0.495886, 0.442536, 0.367331, 0.249451, 0.147162, 0.11891, 0.0413846, 0, 0, 0, 0, 0, 0 },
{0.504748, 0.529936, 0.547491, 0.504284, 0.308819, 0.165921, 0, 0, 0, 0, 0, 0, 0, 0 },
{0.593713, 0.436171, 0.314521, 0.216704, 0.136032, 0.066751, 0.0353615, 0, 0, 0, 0, 0, 0, 0 },
{0.53879, 0.521227, 0.363339, 0.315419, 0.27094, 0.234896, 0.165702, 0.0898551, 0, 0, 0, 0, 0, 0 },
{0.602376, 0.456489, 0.378913, 0.319771, 0.223537, 0.122406, 0.0843375, 0.0623445, 0.0400367, 0.0317537, 0, 0, 0, 0 },
{0.529783, 0.537416, 0.54627, 0.504284, 0.401204, 0.259376, 0.181046, 0.11324, 0, 0, 0, 0, 0, 0 },
{0.612432, 0.533138, 0.492817, 0.43527, 0.39086, 0.350982, 0.204081, 0.147162, 0.104808, 0.0843375, 0.0710267, 0.0555221, 0.0419885, 0.0317537 },
{0.531157, 0.53879, 0.547491, 0.49898, 0.401204, 0.256647, 0.114444, 0, 0, 0, 0, 0, 0, 0 },
{0.611166, 0.52547, 0.437981, 0.396564, 0.350256, 0.309997, 0.207059, 0.147162, 0.0952904, 0.0708799, 0.041299, 0.0322838, 0, 0 },
{0.529783, 0.240234, 0.167685, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{0.585174, 0.448994, 0.304907, 0.238348, 0.174741, 0.123935, 0.0841632, 0.0581085, 0.0353615, 0, 0, 0, 0, 0 },
};
	
	
	
	float activity[12][14]; 
	float time[12] = {740, 740, 1130, 1130, 1180, 1180, 1185, 1185, 1200, 1200, 1120, 1120 };
	float weight[12] = {0.194, 0.194, 0.322, 0.322, 0.155, 0.155, 0.170, 0.170, 0.159, 0.159, 0.029, 0.029};
	float errorcounts[12][14];
	float err_eff[12][14] = { 
		{0.020, 0.021, 0.020, 0.017, 0.012, 0.010, 0.008, 0.005, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.069, 0.060, 0.053, 0.044, 0.030, 0.018, 0.014, 0.005, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.020, 0.021, 0.022, 0.020, 0.012, 0.007, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.071, 0.053, 0.038, 0.026, 0.016, 0.008, 0.004, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.022, 0.021, 0.015, 0.013, 0.011, 0.009, 0.007, 0.004, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.073, 0.055, 0.046, 0.038, 0.027, 0.015, 0.010, 0.008, 0.005, 0.004, 0.000, 0.000, 0.000, 0.000},
		{0.021, 0.021, 0.022, 0.020, 0.016, 0.010, 0.007, 0.005, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.074, 0.064, 0.059, 0.052, 0.047, 0.042, 0.025, 0.018, 0.013, 0.010, 0.009, 0.007, 0.005, 0.004},
		{0.021, 0.022, 0.022, 0.020, 0.016, 0.010, 0.005, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.074, 0.063, 0.053, 0.048, 0.042, 0.037, 0.025, 0.018, 0.011, 0.009, 0.005, 0.004, 0.000, 0.000},
		{0.021, 0.010, 0.007, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.070, 0.054, 0.037, 0.029, 0.021, 0.015, 0.010, 0.007, 0.004, 0.000, 0.000, 0.000, 0.000, 0.000},


	};
	float solidNaI = 2.35; // 3 cm
	float solidHPGe = 0.23; // 7 cm
	float solidangle[12] ={solidNaI,solidHPGe,solidNaI,solidHPGe,solidNaI,solidHPGe,solidNaI,solidHPGe,solidNaI,solidHPGe,solidNaI,solidHPGe}; 
	
 	//for weight and time assume errors 1 g and 1 s
 		
 	for (int i=0; i< 12; i++)
	{	
		for (int j=0; j< 14; j++)
			errorcounts[i][j] = TMath::Sqrt(counts[i][j]);
		}
 		
	for (int i=0; i< 12; i++)
	{	
		for (int j=0; j< 14; j++)
		{
			if (counts[i][j]==0)
				activity[i][j] = 0;
			else
				activity[i][j] = (counts[i][j] * 12.56)/(efficiency[i][j] * time[i] * weight[i]*solidangle[i]);
				
			}
						
		}
		
	float activityTot[12]= {0,0,0,0,0,0,0,0,0,0,0,0};
	float err_actTot[12]= {0,0,0,0,0,0,0,0,0,0,0,0} ;
	for (int i=0; i< 12; i++)
	{
		for (int j=0; j< 14; j++)
			{
				activityTot[i] = activityTot[i] + activity[i][j];
			}
		
		}
	
	//percentual a given transition yields
	float percent[12][14];
	for (int i=0; i< 12; i++)
	{
		for (int j=0; j< 14; j++)
			percent[i][j] = activity[i][j] / activityTot[i] ;
		}
	
	
	
	float err_Activity[12][14];	
	float sigmay = 1; //error on time
	float sigmaz = 0.0001; // error on weight
	float sigmasolid =0.1; //error on sigmasolid, I don't take it into account otherwise too great error
	for (int i=0; i< 12; i++)
	{ // I didn't count the error for resolution
		for (int j=0; j< 14; j++) {
		if (efficiency[i][j]==0)
			err_Activity[i][j]=0;
		else
		{
			err_Activity[i][j] = (12.56/efficiency[i][j])*(TMath::Sqrt( (errorcounts[i][j] * errorcounts[i][j] )/(solidangle[i]*solidangle[i]*time[i]*time[i]*weight[i]*weight[i]) + 
					(counts[i][j]*counts[i][j]*sigmay*sigmay)/( solidangle[i]*solidangle[i]*weight[i]*weight[i]*TMath::Power(time[i], 4))
					+ (counts[i][j]*counts[i][j]*sigmaz*sigmaz)/( solidangle[i]*solidangle[i]*time[i]*time[i]*TMath::Power(weight[i], 4)   ) /*+
				(counts[i][j]*counts[i][j]*sigmasolid*sigmasolid)/( TMath::Power(solidangle[i], 4)*time[i]*time[i]*weight[i]*weight[i]   )*/	   ) );
			}
		}
	}
	
	for (int i=0; i< 12; i++)
	{
		for (int j=0; j< 14; j++)
			{
				err_actTot[i] = err_actTot[i] + err_Activity[i][j];
			}
		
		}
	
	
	cout  << "peaks" << "\t" << "counts"  << "\t" << setw(10) << "error" << "\t"  << "eff abs" << "\t" << 
	 "error" << "\t" << setw(12) <<"Activ" << "\t" << setw(10) << "error" << "\t" << "percent" << endl;
	for (int i=0; i< 12; i++)
	{
		cout << converterActivity(i) << endl;
		for (int j=0; j< 14; j++) {
			cout  << fixed << setprecision(0) << peaks[i][j] << "\t" << counts[i][j] << "\t" << setw(10) << setprecision(1) << errorcounts[i][j] << "\t"  << 
			 setprecision(3) << efficiency[i][j] << "\t" << err_eff[i][j] << "\t" << setw(12) <<  setprecision(2) << activity[i][j] << "\t" << setw(10) << err_Activity[i][j] << "\t" << percent[i][j] << endl;
		}
		cout << "Total activity " << activityTot[i] << " $\\pm$ " << err_actTot[i] << endl;
	}
	//gSystem->ChangeDirectory("~/physics_laboratory");
	int status = system("bash scriptLatex.sh activity");
	//gSystem->cd("~/physics_laboratory_data");

		
	}
	
	
void EfficiencyNaI()
{
	//energies
	float binx2[3]= {59, 511, 1275 };
	
	//insert by hand this
	float biny2[3] = {0.502, 0.571, 0.208};
	
	int nPoints2 = 3;
	
	TGraphErrors *graphErr2 = new TGraphErrors(nPoints2,binx2,biny2);

	 TF1 *g1    = new TF1("g1","pol1",binx2[0],binx2[1]); //this is to find the proper parameters from the single fits
   TF1 *g2    = new TF1("g2","expo",binx2[1],binx2[2]); //and then puth them on the total fit
   
     
   graphErr2->Fit(g1,"R"); //"R" Use the Range specified in the function range   

   graphErr2->Fit(g2,"R+"); //"+" Add this new fitted function to the list of fitted functions (by default, 
					//any previous function is deleted)
	
	
	
}

// the scriptLatex equivalent is efficiency
void Efficiency()
{
	
	//HPGe part
	// in this example they are the europium energies
	float binx[7]= {121.8, 244.7, 344.3, 778.9, 964., 1112.1, 1408 };
	
	//insert by hand this
	float biny[7] = {8.4, 7.1, 3.9, 2.1, 1.6, 1.5,1. };
	

	int nPoints = 7;
	// Define a TGraphErrors which contains the experimental points in x
	// and the theoretical points in y; I semplify by not taking into account errors.
	TGraphErrors *graphErr = new TGraphErrors(nPoints,binx,biny);

	// Define a function which fits the points
	TF1 *fitfun = new TF1("calfitfun1","expo",binx[0],binx[6]); //change pol(nPoints-1) and the maximum bin value
																// in binx[nPoints-1]
	// Fit
	graphErr->Fit(fitfun);
	
	//parameters
	//float eff_ass = 0.422 ; //absolute efficiency for HPGe at 511 keV OLD VALUES
	//float eff_rel = 3.683 ; // relative efficiency for HPGe at 511 keV
	float eff_ass = 0.250 ; //absolute efficiency for HPGe at 511 keV
	float eff_rel = 3.683 ; // relative efficiency for HPGe at 511 keV
	float conv = eff_ass / eff_rel;
	
	float err_ass = 0.021;
	float err_rel = 0.03;
	float err_conv = TMath::Sqrt( ( eff_ass*eff_ass*err_ass*err_ass) + (eff_rel*eff_rel*err_rel*err_rel)) / (eff_rel*eff_rel);
	float efficiencies[12][14];
	float err_eff[12][14];
	float peaks[12][14] = {
		{85, 225, 621, 745, 1009, 1125, 1280, 1636,0,0,0,0,0,0},
		{108, 180, 235, 325, 512, 767, 870, 1380,0,0,0,0,0,0},
		{77, 242, 357, 605, 976, 1446,0,0,0,0,0,0,0,0},
		{93, 242, 400, 580, 805, 1149, 1456,0,0,0,0,0,0,0},
		{300, 580, 853, 960, 1075, 1183, 1447, 1910,0,0,0,0,0,0},
		{86, 220, 310, 392, 565, 856, 1036, 1182, 1396, 1508,0,0,0,0},
		{241, 291, 349, 605, 778, 1108, 1380, 1735,0,0,0,0,0,0},
		{78, 145, 183, 243, 295, 347, 609, 767, 931, 1036, 1119, 1238, 1373, 1508},
		{250, 300, 357, 613, 778, 1116, 1727,0,0,0,0,0,0,0},
		{79, 152, 240, 288, 348, 407, 602, 767, 977, 1120, 1381, 1500,0,0},
		{241, 1166, 1438,0,0,0,0,0,0,0,0,0,0,0},
		{100, 228, 415, 534, 684, 850, 1037, 1216, 1456,0,0,0,0,0},
		
	};	
		
		for (int i=0; (2*i + 1) < 12; i++)
		{	
		for (int j=0; j< 14; j++)
		{	
			if (peaks[(2*i + 1)][j] == 0)
				efficiencies[(2*i + 1)][j] = 0;
			else if ( (conv * fitfun->Eval(peaks[(2*i + 1)][j])) > 1 ) //avoid efficiencies greater than one
				efficiencies[(2*i + 1)][j] = 0.998;
			else
				efficiencies[(2*i + 1)][j] = conv * fitfun->Eval(peaks[(2*i + 1)][j]);	
			}
		}
		
		//print
		
		for (int i=0; (2*i + 1) < 12; i++)
		{	
			cout << "{" ;
		for (int j=0; j< 14; j++)
		{	
				cout << efficiencies[(2*i + 1)][j] << ",";
			}
		cout << "}," << endl;
		}
		
		for (int i=0; (2*i + 1) < 12; i++)
		{	

		for (int j=0; j< 14; j++)
		{	
			if (peaks[(2*i + 1)][j] == 0)
				err_eff[(2*i + 1)][j] = 0;
			else
				err_eff[(2*i + 1)][j] = err_conv * fitfun->Eval(peaks[(2*i + 1)][j]);	
			}
		}
		
		cout << endl;
		//print
		for (int i=0; (2*i + 1) < 12; i++)
		{
			cout << "{" ;
	
		for (int j=0; j< 14; j++)
		{	
				cout << err_eff[(2*i + 1)][j] << ", ";
			}
			cout << "}," << endl;

		}
				
		


	//NaI part	
	//energies
	float binx2[3]= {59, 511, 1275 };
	
	//insert by hand this
	float biny2[3] = {0.502, 0.571, 0.208};	
	int nPoints2 = 3;	
	TGraphErrors *graphErr2 = new TGraphErrors(nPoints2,binx2,biny2);
	TF1 *g1    = new TF1("g1","pol1",binx2[0],binx2[1]); //this is to find the proper parameters from the single fits
	TF1 *g2    = new TF1("g2","expo",binx2[1],binx2[2]); //and then puth them on the total fit
        
   graphErr2->Fit(g1,"R"); //"R" Use the Range specified in the function range   
   graphErr2->Fit(g2,"R+"); //"+" Add this new fitted function to the list of fitted functions (by default, 
					//any previous function is deleted)
					
	float err_Na = 0.04; //this is pretty arbitrary				
		
	for (int i=0; (2*i) < 12; i++)
		{	
		for (int j=0; j< 14; j++)
		{	
			if (peaks[(2*i)][j] == 0)
				efficiencies[(2*i)][j] = 0;
			else if (peaks[(2*i)][j] < binx2[1])
				efficiencies[(2*i)][j] = g1->Eval(peaks[(2*i)][j]);
			else
				efficiencies[(2*i)][j] = g2->Eval(peaks[(2*i)][j]);	
			}
		}
		
		
		
		cout << endl;
		
		for (int i=0; (2*i) < 12; i++)
		{	
		for (int j=0; j< 14; j++)
		{	
			if (peaks[(2*i)][j] == 0)
				err_eff[(2*i)][j] = 0;
			else
				err_eff[(2*i)][j] = err_Na * efficiencies[(2*i)][j];	
			}
		}
	
	
		//print the values
		//for (int i=0; (2*i) < 12; i++)
		//{
			//cout << "{" ;	
			
		//for (int j=0; j< 14; j++)
			//cout << efficiencies[(2*i)][j] << ", ";
		 //cout << "}," << endl;	
		//}
		
		//cout << endl;
		
		
		for (int i=0; i < 12; i++)
		{
			cout << "{" ;	
			
		for (int j=0; j< 14; j++)
			cout << efficiencies[i][j] << ", ";
		 cout << "}," << endl;	
		}
		
		cout << endl;
		
		for (int i=0; i < 12; i++)
		{	
			cout << "{" ;
		for (int j=0; j< 14; j++)
			cout << err_eff[i][j] << ", ";
		 cout << "}," << endl;
		}
		
		
					
}







//macro to measure the activity today
void todayActivity()
{
	//the initial activity (kBq)
	//float A_Na = 2.2;
	//float A_Am = 380;
	//float A_Eu = 102.5;
	//the other activity (kBq)
	float A_Na = 370;
	float A_Am = 397;
	float A_Eu = 395;
	
	
	//Half time in years
	float halfTime_Na = 2.6;
	float halfTime_Am = 432.2;
	float halfTime_Eu = 13.52;
	
	//time passed (old)
	//float time_Na = 20.1;
	//float time_Am = 28.17;
	//float time_Eu = 26.84;
			
	//time passed (if the sources refer to September)
	//float time_Na = 1.15;
	//float time_Am = 1.15;
	//float time_Eu = 1.15;
	
	//time passed (with the nominal activity)
	float time_Na = 19;
	float time_Am = 27.1;
	float time_Eu = 25.8;
	
			
	//Activity today
	float todayA_Na = A_Na*TMath::Exp(-(time_Na * 0.69)/halfTime_Na);
	float todayA_Am = A_Am*TMath::Exp(-(time_Am * 0.69)/halfTime_Am);
	float todayA_Eu = A_Eu*TMath::Exp(-(time_Eu * 0.69)/halfTime_Eu);
	
	
	//here put the one you want printed
	std::cout <<  todayA_Na << std::endl;
	std::cout <<  todayA_Am << std::endl;
	std::cout <<  todayA_Eu << std::endl;
	
	}


void RNcalculus() // todo: put in red the peaks relative to radon, errors here, identification of radionuclides.
{
	//order of peaks: NaI	300 580 853 960 1075 1183  
	//				  HPGe  220 310 392 565 856 1036 1182 
	
	//data for background canister
	float arraybackNai[4] = {146,100,101,40};
	float arraybackHPGe[4] = {150,140,170,101};
	//float arrayrefNai[5] = {15280,23160,17330,5200,3340};
	float arrayrefNai[4] = {15280,23160,17330,5200};
	float arrayrefHPGe[4] = {1844, 2394, 2044, 539};
	//float arrayexpNai[5] = {12070, 17650,13840,4400,2926};
	float arrayexpNai[4] = {12070, 17650,13840,4400};
	float arrayexpHPGe[4] = {1113,1412,1150,404};
			
	float backgroundNai = 0;
	float backgroundHPGe = 0;
	float backgroundTime = 1180;
	
	float exposedNai = 0;
	float exposedHPGe = 0;
	float exposedTime = 1200;
	
	float referenceNai = 0;
	float referenceHPGe = 0;
	float referenceTime = 1185;
	
	for (int i = 0; i<4; i++)
	{backgroundNai = backgroundNai + arraybackNai[i];}
	for (int i = 0; i<4; i++)
	{backgroundHPGe = backgroundHPGe + arraybackHPGe[i];}
	for (int i = 0; i<4; i++)
	{exposedNai = exposedNai + arrayexpNai[i];}
	for (int i = 0; i<4; i++)
	{exposedHPGe = exposedHPGe + arrayexpHPGe[i];}
	for (int i = 0; i<4; i++)
	{referenceNai = referenceNai + arrayrefNai[i];}
	for (int i = 0; i<4; i++)
	{referenceHPGe = referenceHPGe + arrayrefHPGe[i];}
	
	
	float N_NaI = 2 * ( (exposedNai * exposedTime) - (backgroundNai * backgroundTime)) /((backgroundTime + exposedTime) * exposedTime );
	float N_HPGe = 2 * ( (exposedHPGe * exposedTime) - (backgroundHPGe * backgroundTime)) /((backgroundTime + exposedTime) * exposedTime);
	float E_NaI = ( (referenceNai * referenceTime) - (backgroundNai * backgroundTime)) /((backgroundTime + referenceTime) * 260 * referenceTime);
	float E_HPGe = ( (referenceHPGe * referenceTime) - (backgroundHPGe * backgroundTime)) /((backgroundTime + referenceTime)*260 * referenceTime);
	
	float errN_NaI = TMath::Sqrt(exposedNai) / exposedTime;
	float errN_HPGe = TMath::Sqrt(exposedHPGe) / exposedTime;
	float errE_NaI = TMath::Sqrt(referenceNai) / (exposedTime * 260);
	float errE_HPGe = TMath::Sqrt(referenceHPGe) / ( exposedTime * 260 );
	
	float T = 1440; //time of exposure
	float CF = 0.12;
	float DF = 1;
	float RN_NaI = N_NaI /(DF * CF * T * E_NaI);
	float RN_HPGe = N_HPGe /(DF * CF * T * E_HPGe);
	
	float errRN_HPGe = (TMath::Sqrt( ( errN_HPGe*errN_HPGe*N_HPGe*N_HPGe) + (E_HPGe*E_HPGe*errE_HPGe*errE_HPGe)))
						/(DF * CF * T);
	float errRN_NaI = (TMath::Sqrt( ( errN_NaI*errN_NaI*N_NaI*N_NaI) + (E_NaI*E_NaI*errE_NaI*errE_NaI)) 
						/ (E_NaI*E_NaI))/(DF * CF * T);
		
	float RN_activity_NaI = 556 /(DF * CF * T);
	float RN_activity_HPGe = 822 /(DF * CF * T );
	
	float errRN2_NaI = TMath::Sqrt(556)/(DF * CF * T);
	float errRN2_HPGe = TMath::Sqrt(822)/(DF * CF * T);
	
	//write data
	cout << "RN NaI  " << RN_NaI << " err " << errRN_NaI << endl;
	cout << "RN_HPGe  " << RN_HPGe << " err " << errRN_HPGe << endl;
	cout << "E NaI  " <<  E_NaI << " err " << errE_NaI << endl;
	cout << "E HPGe  "  << E_HPGe << " err " << errE_HPGe << endl;
	cout << "N NaI  " << N_NaI << " err " << errN_NaI << endl;
	cout << "N_HPGe  " << N_HPGe << " err " << errN_HPGe << endl;
	cout << "RN NaI Activity  " << RN_activity_NaI << " err " << errRN2_NaI << endl;
	cout << "RN HPGe Activity  " << RN_activity_HPGe << " err " << errRN2_HPGe << endl;
	cout << "" << endl;
	cout << "" << endl;
	cout << "" << endl;
	cout << "" << endl;
	cout << "" << endl;
	
}


















//prova per scrivere direttamente in un file di testo, ma non riesco a scrivere in mezzo a un file
void ActivityProva()
{
	//order: pellet, plaster, background canister, reference canister, exposed canister, autunite
	// all materials occupy two rows, one for NaI, the other for HPGe, for example 1 = NaI pellet, 2 = HPGe pellet
	float peaks[12][14] = {
		{85, 225, 621, 745, 1009, 1125, 1280, 1636,0,0,0,0,0,0},
		{108, 180, 235, 325, 512, 767, 870, 1380,0,0,0,0,0,0},
		{77, 242, 357, 605, 976, 1446,0,0,0,0,0,0,0,0},
		{93, 242, 400, 580, 805, 1149, 1456,0,0,0,0,0,0,0},
		{300, 580, 853, 960, 1075, 1183, 1447, 1910,0,0,0,0,0,0},
		{86, 220, 310, 392, 565, 856, 1036, 1182, 1396, 1508,0,0,0,0},
		{241, 291, 349, 605, 778, 1108, 1380, 1735,0,0,0,0,0,0},
		{78, 145, 183, 243, 295, 347, 609, 767, 931, 1036, 1119, 1238, 1373, 1508},
		{250, 300, 357, 613, 778, 1116, 1727,0,0,0,0,0,0,0},
		{79, 152, 240, 288, 348, 407, 602, 767, 977, 1120, 1381, 1500,0,0},
		{241, 1166, 1438,0,0,0,0,0,0,0,0,0,0,0},
		{100, 228, 415, 534, 684, 850, 1037, 1216, 1456,0,0,0,0,0},
		
	};
	
	
	float counts[12][14] = {
		{272,456,131,45,40,31,23,20,0,0,0,0,0,0},
		{237,129,67,89,55,40,38,30,0,0,0,0,0,0},
		{2980,2578,1100,926,715,481,0,0,0,0,0,0,0,0},
		{409,121,146,145,76,98,88,0,0,0,0,0,0,0},
		{146,148,53,48,32,69,124,31,0,0,0,0,0,0},
		{550,270,143,134,166,105,101,43,33,31,0,0,0,0},
		{12310,15280,23160,17330,5465,5200,4140,3340,0,0,0,0,0,0},
		{4081,3703,3158,1545,1844,2394,2044,670,450,344,539,355,308,279},
		{8266,12070,17650,13840,4876,4400,2926,0,0,0,0,0,0,0},
		{1820,2394,1389,1113,1412,902,1150,396,295,404,129,143,0,0},
		{8600,8427,10330,0,0,0,0,0,0,0,0,0,0,0},
		{1844,858,489,325,205,392,407,796,518,0,0,0,0,0},
		
	};
	
		
	float efficiency[12][14] = { //modified by hand for too high values
		{0.506, 0.527, 0.494, 0.419, 0.296, 0.254, 0.207, 0.129, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.851,0.837,0.747,0.620,0.421,0.248,0.201,0.070,0.000,0.000,0.000,0.000,0.000,0.000,},
		{0.505, 0.530, 0.547, 0.504, 0.309, 0.166, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.752,0.736,0.531,0.366,0.230,0.113,0.060,0.000,0.000,0.000,0.000,0.000,0.000,0.000,},
		{0.539, 0.521, 0.363, 0.315, 0.271, 0.235, 0.166, 0.090, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.779,0.771,0.640,0.540,0.377,0.207,0.142,0.105,0.068,0.054,0.000,0.000,0.000,0.000,},
		{0.530, 0.537, 0.546, 0.504, 0.401, 0.259, 0.181, 0.113, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.909,0.900,0.832,0.735,0.660,0.592,0.344,0.248,0.177,0.142,0.120,0.094,0.071,0.054,},
		{0.531, 0.539, 0.547, 0.499, 0.401, 0.257, 0.114, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.891,0.887,0.739,0.669,0.591,0.523,0.350,0.248,0.161,0.120,0.070,0.054,0.000,0.000,},
		{0.530, 0.240, 0.168, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.773,0.758,0.515,0.402,0.295,0.209,0.142,0.098,0.060,0.000,0.000,0.000,0.000,0.000,},		

	};
	
	
	
	
	
	float activity[12][14]; 
	float time[12] = {740, 740, 1130, 1130, 1180, 1180, 1185, 1185, 1200, 1200, 1120, 1120 };
	float weight[12] = {0.194, 0.194, 0.322, 0.322, 0.155, 0.155, 0.155, 0.155, 0.159, 0.159, 0.029, 0.029};
	float errorcounts[12][14];
	float err_eff[12][14] = { 
		{0.020, 0.021, 0.020, 0.017, 0.012, 0.010, 0.008, 0.005, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.069, 0.060, 0.053, 0.044, 0.030, 0.018, 0.014, 0.005, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.020, 0.021, 0.022, 0.020, 0.012, 0.007, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.071, 0.053, 0.038, 0.026, 0.016, 0.008, 0.004, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.022, 0.021, 0.015, 0.013, 0.011, 0.009, 0.007, 0.004, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.073, 0.055, 0.046, 0.038, 0.027, 0.015, 0.010, 0.008, 0.005, 0.004, 0.000, 0.000, 0.000, 0.000},
		{0.021, 0.021, 0.022, 0.020, 0.016, 0.010, 0.007, 0.005, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.074, 0.064, 0.059, 0.052, 0.047, 0.042, 0.025, 0.018, 0.013, 0.010, 0.009, 0.007, 0.005, 0.004},
		{0.021, 0.022, 0.022, 0.020, 0.016, 0.010, 0.005, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.074, 0.063, 0.053, 0.048, 0.042, 0.037, 0.025, 0.018, 0.011, 0.009, 0.005, 0.004, 0.000, 0.000},
		{0.021, 0.010, 0.007, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000},
		{0.070, 0.054, 0.037, 0.029, 0.021, 0.015, 0.010, 0.007, 0.004, 0.000, 0.000, 0.000, 0.000, 0.000},


	};
 	//for weight and time assume errors 1 g and 1 s
 		
 	for (int i=0; i< 12; i++)
	{	
		for (int j=0; j< 14; j++)
			errorcounts[i][j] = TMath::Sqrt(counts[i][j]);
		}
 		
	for (int i=0; i< 12; i++)
	{	
		for (int j=0; j< 14; j++)
		{
			if (counts[i][j]==0)
				activity[i][j] = 0;
			else
				activity[i][j] = counts[i][j]/(efficiency[i][j] * time[i] * weight[i]);
				
			}
						
		}
		
	float activityTot[12]= {0,0,0,0,0,0,0,0,0,0,0,0};
	float err_actTot[12]= {0,0,0,0,0,0,0,0,0,0,0,0} ;
	for (int i=0; i< 12; i++)
	{
		for (int j=0; j< 14; j++)
			{
				activityTot[i] = activityTot[i] + activity[i][j];
			}
		
		}
	
	//percentual a given transition yields
	float percent[12][14];
	for (int i=0; i< 12; i++)
	{
		for (int j=0; j< 14; j++)
			percent[i][j] = activity[i][j] / activityTot[i] ;
		}
	
	
	
	float err_Activity[12][14];	
	float sigmay = 1; //error on time
	float sigmaz = 0.0001; // error on weight
	for (int i=0; i< 12; i++)
	{ // I didn't count the error for resolution
		for (int j=0; j< 14; j++)
			err_Activity[i][j] = TMath::Sqrt( (errorcounts[i][j] * errorcounts[i][j] )/(time[i]*time[i]*weight[i]*weight[i]) + 
					(counts[i][j]*counts[i][j]*sigmay*sigmay)/( weight[i]*weight[i]*TMath::Power(time[i], 4))
					+ (counts[i][j]*counts[i][j]*sigmaz*sigmaz)/( time[i]*time[i]*TMath::Power(weight[i], 4)   ) ) ;
		}
	
	for (int i=0; i< 12; i++)
	{
		for (int j=0; j< 14; j++)
			{
				err_actTot[i] = err_actTot[i] + err_Activity[i][j];
			}
		
		}
	
	
	//start of writing
		writeLines(searchLine("%TABLEHPGe", "provvisorio3.txt"), "provvisorio3.txt","provvisorio4.txt");
		cout << searchLine("%TABLEHPGe", "provvisorio3.txt") << endl;
		ofstream outfile;
		outfile.open("provvisorio.txt", std::ios_base::app);
	
	
	outfile  << "peaks" << "\t" << "counts"  << "\t" << setw(10) << "error" << "\t"  << "eff abs" << "\t" << 
	 "error" << "\t" << "Activ" << "\t" << setw(10) << "error" << "\t" << "percent" << endl;
	for (int i=0; i< 12; i++)
	{
		outfile << converterActivity(i) << endl;
		for (int j=0; j< 14; j++) {
			outfile  << fixed << setprecision(0) << peaks[i][j] << "\t" << counts[i][j] << "\t" << setw(10) << setprecision(1) << errorcounts[i][j] << "\t"  << 
			 setprecision(3) << efficiency[i][j] << "\t" << err_eff[i][j] << "\t" << setprecision(2) << activity[i][j] << "\t" << setw(10) << err_Activity[i][j] << "\t" << percent[i][j] << endl;
		}
		outfile << "Total activity " << activityTot[i] << " $\\pm$ " << err_actTot[i] << endl;
	}
	//gSystem->ChangeDirectory("~/physics_laboratory");
	outfile.close(); //Otherwise it won't save the output and the next program does not work	
	int status = system("bash scriptLatex.sh activity >> provvisorio4.txt"); //I don't know why I have to change file...
		writeFinalLines(searchLine("%TABLEHPGe","provvisorio3.txt"), "provvisorio3.txt","provvisorio4.txt");
		outfile.close();
	}




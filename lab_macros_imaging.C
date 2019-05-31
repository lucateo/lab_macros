#include "lab_macros.C"
#include <iomanip>
//Ch 0 is detector 1
//Ch 1 is detector 4
//Ch 2 is detector 5
//Ch 3 is pattern unit

//this is the master root macro
void getHistoImaging(char *name_file, int numBins, double minX, double maxX) {

	ntuple_data_t indata;
	TFile *infile = new TFile(name_file);
	TNtuple *intuple = (TNtuple*)infile->Get("pjmca");
	TBranch *inbranch0 = intuple->GetBranch("ch0");
	TBranch *inbranch1 = intuple->GetBranch("ch1");
	TBranch *inbranch2 = intuple->GetBranch("ch2");
	TBranch *inbranch3 = intuple->GetBranch("ch3");
	inbranch0->SetAddress(&indata.ch0);
	inbranch1->SetAddress(&indata.ch1);
	inbranch2->SetAddress(&indata.ch2);
	inbranch3->SetAddress(&indata.ch3);

	TH1F *h0 = new TH1F("h0","h0",numBins,minX,maxX);
	TH1F *h1 = new TH1F("h1","h1",numBins,minX,maxX);
	TH1F *h2 = new TH1F("h2","h2",numBins,minX,maxX);
	TH1F *h3 = new TH1F("h3","h3",numBins,minX,maxX);
		
	for (int i=0; i<intuple->GetEntries(); i++) {
		intuple->GetEntry(i);
		h0->Fill(indata.ch0);
		h1->Fill(indata.ch1);
		h2->Fill(indata.ch2);
		h3->Fill(indata.ch3);
	}
	TCanvas *c0 = new TCanvas("c0");
	c0->Divide(2,2);
	c0->cd(1);
	h0->Draw();
	c0->cd(2);
	h1->Draw();
	c0->cd(3);
	h2->Draw();
	c0->cd(4);
	h3->Draw();

}

//I will use this in the other macros
TH1F* getHistoFromChannelImaging(char *name_file, short chan, int numBins, double minX, double maxX) {

	ntuple_data_t indata;
	TCanvas *se = new TCanvas("c0"); 
	TFile *infile = new TFile(name_file);
	TNtuple *intuple = (TNtuple*)infile->Get("pjmca");
	TBranch *inbranch = intuple->GetBranch(Form("ch%d",chan));
	TH1F *h0 = new TH1F("","Pattern unit blackbox 5",numBins,minX,maxX);
	
	if (chan ==0)
	{
	inbranch->SetAddress(&indata.ch0);
		for (int i=0; i<intuple->GetEntries(); i++) {
		intuple->GetEntry(i);
		h0->Fill(indata.ch0);
		}
	}
	
	if (chan ==1)
	{
	inbranch->SetAddress(&indata.ch1);
		for (int i=0; i<intuple->GetEntries(); i++) {
		intuple->GetEntry(i);
		h0->Fill(indata.ch1);
		}
	}
	
	
	if (chan ==2)
	{
	inbranch->SetAddress(&indata.ch2);
		for (int i=0; i<intuple->GetEntries(); i++) {
		intuple->GetEntry(i);
		h0->Fill(indata.ch2);
		}
	}
	
	
	
	if (chan ==3)
	{
	inbranch->SetAddress(&indata.ch3);
		for (int i=0; i<intuple->GetEntries(); i++) {
		intuple->GetEntry(i);
		h0->Fill(indata.ch3);
		}
	}
	gStyle->SetOptStat("");
	h0->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	h0->SetXTitle("channel");
	h0->SetYTitle("counts");
	h0->Draw();
	return h0;
}

//try to fix the coincidence corresponding to the 511 keV peak (does not work)
TH1F* fixCoincImaging(char *name_file, int numBins, double minX, double maxX) {

	ntuple_data_t indata;
	TCanvas *se = new TCanvas; 
	TFile *infile = new TFile(name_file);
	TNtuple *intuple = (TNtuple*)infile->Get("pjmca");
	TBranch *inbranch1 = intuple->GetBranch("ch0");
	TBranch *inbranch2 = intuple->GetBranch("ch3");
	TH1F *h0 = new TH1F(name_file,"Detector 1",numBins,minX,maxX);
	TH1F *h1 = new TH1F(name_file,"Pattern unit",numBins,minX,maxX);
	inbranch1->SetAddress(&indata.ch0);
	inbranch2->SetAddress(&indata.ch3); //pattern unit
	
	//calibrated triggers, they correspond to 400-600 keV
	double lowThr = 498;
	double highThr =769;
	
	for (int i=0; i<intuple->GetEntries(); i++) {
		intuple->GetEntry(i);
		if (indata.ch0>lowThr && indata.ch0<highThr) {  // This fills only if the data is in the range I gave with lowThr and highThr
			h0->Fill(indata.ch0);
			h1->Fill(indata.ch1);
		}
	}	
	
	gStyle->SetOptStat("in");
	h0->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	h0->SetXTitle("channel");
	h0->SetYTitle("counts");
	se->Divide(2,1);
	se->cd(1);
	h0->Draw();
	se->cd(2);
	h1->Draw();
	
	return h0;
}




//fit with 2 guassian, to do the calibration graphs
void multifit2Imaging(char *file, short chan) {
		
	//TCanvas *se = new TCanvas("c0");
		
	TH1F *h = getHistoFromChannelImaging(file, chan, 1000, 0, 2000);
	
   Double_t par[6]; // I need 6 parameters for two gaussians
   
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","gaus",200,690); //this is to find the proper parameters from the single fits
   TF1 *g2    = new TF1("g2","gaus",950,1400); //and then puth them on the total fit
   
   TF1 *total = new TF1("total","gaus(0)+gaus(3)",200,1400); // notice that its range contains all the previous
																	// two ranges
   //total->SetLineColor(3);
   
   h->Fit(g1,"R"); //"R" Use the Range specified in the function range   

   h->Fit(g2,"R+"); //"+" Add this new fitted function to the list of fitted functions (by default, 
					//any previous function is deleted)  

   g1->GetParameters(&par[0]); // I think he puts the parameters in the array par, for every fit
   g2->GetParameters(&par[3]); //there are three parameters: p0 = normalization factor, p1 = mean, p2 = sigma, 
   
   total->SetParameters(par);
   //h->Fit(total,"R+");
   
   h->SetTitle("Calibration Detector 8 with Na source");
   h->SetXTitle("bins");
   h->SetYTitle("counts");
   h->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
   gStyle->SetOptStat("");
   gStyle->SetOptFit(0);  
   h->Draw();   
   
  // se->Update();
   
   //// Retrieve the stat box
   //TPaveStats *ps = (TPaveStats*)se->GetPrimitive("stats");
   ////ps->SetName("mystats");
   //TList *listOfLines = ps->GetListOfLines();
   
   //// Add a new line in the stat box.
   //// Note that "=" is a control character
	//TLatex *p0 = new TLatex(0,0,"Constant2 = 42.22 #pm 0.6"); // Tlatex is like latex but with # instead of \ for special characters
											 //// the numbers before the text specify the position of the thing
	//TLatex *p1 = new TLatex(0,0,"Mean2 = 1669.1 #pm 2.25 "); //add by hand the parameters of the second gaussian
	//TLatex *p2 = new TLatex(0,0,"Sigma2 = 157.94 #pm 2.2"); // p0 is constant, p1 mean, p2 sigma
	
	////set of the lines
	//p0->SetTextSize(0.04);
	//p1->SetTextSize(0.04);
	//p2->SetTextSize(0.04);
	//p0->SetTextFont(42);
	//p1->SetTextFont(42);
	//p2->SetTextFont(42);
	
	////add the lines
    //listOfLines->Add(p0);
    //listOfLines->Add(p1);
    //listOfLines->Add(p2);
    
    //// the following line is needed to avoid that the automatic redrawing of stats
    //h->SetStats(0);
    ////reupdate the canvas
    //se->Modified();
   
   
}


//calibrated histogram for detector 1
TH1F* calibrateImaging(char *file_na, short chan,int numBins, double minX, double maxX ) { 
	
	// remember subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX)
	TH1F *h_na = getHistoFromChannelImaging(file_na, chan, numBins, minX, maxX);
	
	
	// call the function to calibrate the histogram
	CalibrateHisto(h_na, 0.74, 30.96);
	
	// Personalization things
	h_na->SetXTitle("Energy (keV)");
	h_na->SetYTitle("counts");
	h_na->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	
	//the i is for the integral
	gStyle->SetOptStat("i"); //to show what I want in the panel, see reference 
	
	h_na->Draw();
	return h_na;

}




//fit with 2 guassian, to do the calibration graphs
void multifit2ImagingCalibrated(char *file, short chan) {
	
	TCanvas *c0 = new TCanvas("c0");
	// remember TH1F* getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)
	
	//REMEMBER to change the type of calibration!!!
 	TH1F *h = calibrateImaging(file, chan, 1000, 0, 2000); 
	
   Double_t par[6]; // I need 6 parameters for two gaussians
   
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","gaus",400,800); //this is to find the proper parameters from the single fits
   TF1 *g2    = new TF1("g2","gaus",1100,1400); //and then puth them on the total fit
   
   TF1 *total = new TF1("total","gaus(0)+gaus(3)",400,1400); // notice that its range contains all the previous
																	// two ranges
   //total->SetLineColor(3);
   
   h->Fit(g1,"R"); //"R" Use the Range specified in the function range   

   h->Fit(g2,"R+"); //"+" Add this new fitted function to the list of fitted functions (by default, 
					//any previous function is deleted)  

   g1->GetParameters(&par[0]); // I think he puts the parameters in the array par, for every fit
   g2->GetParameters(&par[3]); //there are three parameters: p0 = normalization factor, p1 = mean, p2 = sigma, 
   
   total->SetParameters(par);
   //h->Fit(total,"R+");
   h->SetTitle("Calibrated Na spectrum (Detector 1)");
     
   //all the things down here are to properly insert the stats of the second fit
     
   gStyle->SetOptStat("e");
   gStyle->SetOptFit(11);
   
   h->Draw();
   c0->Update();
   
   // Retrieve the stat box
   TPaveStats *ps = (TPaveStats*)c0->GetPrimitive("stats");
   //ps->SetName("mystats");
   TList *listOfLines = ps->GetListOfLines();
   
   // Add a new line in the stat box.
   // Note that "=" is a control character
	TLatex *p0 = new TLatex(0,0,"Constant2 = 40.61 #pm 0.63"); // Tlatex is like latex but with # instead of \ for special characters
											 // the numbers before the text specify the position of the thing
	TLatex *p1 = new TLatex(0,0,"Mean2 = 1274.22 #pm 2.01 "); //add by hand the parameters of the second gaussian
	TLatex *p2 = new TLatex(0,0,"Sigma2 = 10.91 #pm 2.73"); // p0 is constant, p1 mean, p2 sigma
	
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
    c0->Modified();
   
   
}
















// for 2D histogram master root uses this struct, maybe it is important

//struct slimport_data_t {
//	UInt_t	timestamp;
//	UInt_t	baseline;
//	UShort_t	qshort;
//	UShort_t	qlong;
//	UShort_t	pur;
//	UShort_t	scope[144];
//};


void analysis(char *name_file) {//for 2D histogram

	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("datatree");
	TBranch *inbranch = intree->GetBranch("ACQ_ch0");
	inbranch->SetAddress(&indata.timetag);
	TH2F *histo2D = new TH2F("hs_2D","PSD plot",256,0,12288,64,0,0.4);
	float ratio;

	// histogram filling
	for (int i=0; i<inbranch->GetEntries(); i++) {
		inbranch->GetEntry(i);
		ratio = (float)(indata.qlong-indata.qshort)/(indata.qlong);
		if (ratio>0.15 && indata.qlong>2000) {
			histo2D->Fill( indata.qlong, ratio );
		}
	}

	TCanvas *c0 = new TCanvas("c0");
	histo2D->Draw("colz");

}


// this draws the calibration line for three points
void calibrationLine(float peak0, float peak1) {

	
	float bin[2];
	bin[0] = peak0; //americium peak in bins
	bin[1] = peak1; // Sodium photopeak energy in bins
	//bin[2] = peak2; // Sodium photopeak energy in bins

	float kev[2];
	//kev[0] = 59.; //americium peak
	kev[0] = 511.; // Sodium photopeak energy in keV
	kev[1] = 1275.; // Sodium photopeak energy in keV

	// Define a TGraphErrors which contains the experimental points in x
	// and the theoretical points in y; I semplify by not taking into account errors.
	
	//graph without errors
	//TGraphErrors *graphErr = new TGraphErrors(3,bin,kev);
	
	//graph with errors
	 float ex[] = {0, 0}; //errors in x, I take the sigma
	 float ey[] = {0,0}; // errors in energy, obviously I don't have errors in the energy
	 TGraphErrors *graphErr = new TGraphErrors(2,bin,kev,ex,ey);
	 


	
	// Define a function which fits the points (does not work with errors)
	//TF1 *fitfun = new TF1("calfitfun1","pol1",bin[0],bin[2]);

	// Graphical options to draw the fit
	graphErr->SetMarkerStyle(21);
	graphErr->SetLineColor(2);
	graphErr->SetTitle("Calibration Line detector 1");
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


//errors due to poisson in the counts
float errorPoisson(float num, float den)
{
	float err_num = TMath::Sqrt(num);
	float err_den = TMath::Sqrt(den);
	float error = TMath::Sqrt( ( num*num*err_num*err_num) + (den*den*err_den*err_den)) / (den*den);
	
	return error;
}


//to convert the number in a string
std::string converter(int i)
{
	std::string s0 = "aluminium";	
	std::string s1 = "lead";
	std::string s2 = "polyethylene";
	std::string s3 = "iron";
	std::string s4 = "graphite";
			
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
	else //se non lo metto rompe i coglioni
		return s0;
}


void attenuationCoeffCalculus()
{
	
	//home for the various detectors
	float I0[7] = {211,0,2086,726,870,0,0};
	
	//currents for the various detectors on the various positions 
	float I[7][5] ={ // order: aluminium, lead, poleth, iron, graphite (as columns!)
	   {172,149,236,190,210},
	   {0,0,0,0,0},
	   {1106,876,1336,1093,1623},
	   {380,312,404,350,459},
	   {417,339,482,399,530}, 
	   {0,0,0,0,0},  
	   {0,0,0,0,0}  
	   	   
   } ;
	
	//data
	float density[5] = {2.69, 11.35, 0.93, 7.87,1.70};	
	float x[5]= {2, 0.5, 2, 1, 2};		// thickness
	float corrections[7] = {1.019, 1.008, 1.002, 1.000, 1.002, 1.008, 1.019} ;
	
	std::cout << "Det"   << "\t" << setw(16) << "Aluminium" << "\t" << setw(16) << "Lead    " << "\t" << setw(16) << 
	"Polyethylene"  << "\t"	<< setw(16) << "Iron"  << "\t" << setw(16) << "graphite" << std::endl;
	cout << fixed << setprecision(2);
	 for (int i = 0; i < 7; i++){  
		 std::cout << i+2;
		for(int k = 0; k < 5; k++){
			
			if (I0[i] == 0)
				std::cout << setw(10) << "-" << " $\\pm$ "  << "-" << "\t";
			
			else
				//to align with tab
				std::cout << setw(10) << I[i][k] / I0[i] << " $\\pm$ "  << errorPoisson(I[i][k], I0[i]) << "\t";
		}
				cout << endl;

	}
	
	
	std::cout << "attenuation coefficient" << std::endl;
	std::cout << "Det"   << "\t" << setw(22) << "Aluminium" << "\t" << setw(22) << "Lead    " << "\t" << setw(22) << 
	"Polyethylene"  << "\t"	<< setw(22) << "Iron"  << "\t" << setw(22) << "graphite" << std::endl;
	 cout << fixed << setprecision(2);
	 for (int i = 0; i < 7; i++){  
		 std::cout << i+2 ;
		for(int k = 0; k < 5; k++){
				 
			if (I0[i] == 0)
				std::cout << setw(12) << "----" << " $\\pm$ " 	<< "----" <<  "\t";
			else
				//to align with tab
				std::cout << setw(12) << TMath::Log(I0[i]/I[i][k])/(density[k] * x[k] * corrections[i] ) << " $\\pm$ " 
				<< (errorPoisson(I0[i],I[i][k])/(I0[i]/I[i][k]) ) / (density[k] * x[k] * corrections[i] )
				<< "\t";
		
		}
		cout << endl;
	}
	
	
	std::cout << std::endl;



	
}




//script associated: attcoeff
void attenuationBlackBox()
{
	//change for the various detectors
	//home for the various detectors
	float I0[7] = {211,0,2086,726,870,0,0};
	
	//currents for the various detectors on the various positions 
	float I[7][5] ={ //the order for the detectors in on columns!!!
	   {280,293,214,234,233},
	   {0,0,0,0,0},
	   {776,735,1766,1882,1779},
	   {237,207,519,608,560},
	   {263,280,630,678,631}, 
	   {0,0,0,0,0},  
	   {0,0,0,0,0}  
	   	   
   } ;
		
		
	std::cout << "Det"   << "\t" << setw(16) << "blackbox 1" << "\t" << setw(16) << "blackbox 2 " << "\t" << setw(16) << 
	"blackbox 3"  << "\t"	<< setw(16) << "blackbox 4"  << "\t" << setw(16) << "blackbox 5" << std::endl;
	cout << fixed << setprecision(2);
	 for (int i = 0; i < 7; i++){  
		 std::cout << i+2;
		for(int k = 0; k < 5; k++){
			
			if (I0[i] == 0)
				std::cout << setw(10) << "-" << " $\\pm$ "  << "-" << "\t";
			
			else
				//to align with tab
				std::cout << setw(10) << I[i][k] / I0[i] << " $\\pm$ "  << errorPoisson(I[i][k], I0[i]) << "\t";
		}
				cout << endl;

	}
	
}




void histo2DBlackBox()
{
   TCanvas *c1 = new TCanvas("c1", "c1",900,900);
   gStyle->SetOptStat(0);
      
   // Create, fill and project a 2D histogram.
   //this strange choice of min and max range was to properly center the numbers
   TH2F *h2 = new TH2F("h2","",5,1,6,7,2,9); //(name, nBinsX, minX, maxX, nBinsY, minY, maxY)
   
  //substitute here the attenuations      
   float max[7][5] = { 
		{ 1.33 ,1.39 ,1.01 ,1.11 ,1.10 },
  { 0  ,0  ,0  ,0  ,0  },
  { 0.37 ,0.35 ,0.85 ,0.90 ,0.85 },
  { 0.33 ,0.29 ,0.71 ,0.84 ,0.77 },
  { 0.30 ,0.32 ,0.72 ,0.78 ,0.73 },
  { 0  ,0  ,0  ,0  ,0  },
  { 0  ,0  ,0  ,0  ,0  },
	   	   
   };   
      
    for (float i = 0; i < 7; i++){  
		for(float k = 0; k < 5; k++){
					
				h2->SetBinContent(k+1.5, i + 1.5, max[(int)i][(int)k]);
			}
		}

   h2->SetTitle("Black Box");
   // Drawing
   h2->SetXTitle("Steps");
   h2->SetYTitle("Detectors");
   
   h2->GetXaxis()->CenterTitle();
   h2->GetYaxis()->CenterTitle();
   
   //h2->GetXaxis()->SetLabelSize(0); //this removes the label, i.e. the numbers from the axis
   h2->GetXaxis()->SetTickLength(0); //removes the grid of the axis
   h2->GetYaxis()->SetTickLength(0);
	h2->GetXaxis()->SetNdivisions(5); //sets the number of divisions of the axis
	h2->GetXaxis()->CenterLabels();
	h2->GetYaxis()->CenterLabels();
	
	gStyle->SetPalette(9); //to set the range of colors, until 9 it arrives until grey
								// The default palette defines:
							//   index 0->9   : grey colors from light to dark grey
							//   index 10->19 : "brown" colors
							//   index 20->29 : "blueish" colors
							//   index 30->39 : "redish" colors
							//   index 40->49 : basic colors
   h2->Draw("COLZ"); //COL = coloured, Z gives the legend
    
}


//attempt to do a 3D histogram
void histo3DBlackBox()
{
   TCanvas *c1 = new TCanvas("c1", "c1",900,900);
   gStyle->SetOptStat(0);
      
   // Create, fill and project a 2D histogram.
   //this strange choice of min and max range was to properly center the numbers
   TH3F *h3 = new TH3F("h3","",5,1,6,7,2,9,20,0,1); //(name, nBinsX, minX, maxX, nBinsY, minY, maxY, nBinsZ, minZ, maxZ )
   
  //substitute here the attenuations      
   float max[7][5] = { 
		{ 0 ,0 ,0 ,0 ,0 },
  { 0  ,0  ,0  ,0  ,0  },
  { 0.37 ,0.35 ,0.85 ,0.90 ,0.85 },
  { 0.33 ,0.29 ,0.71 ,0.84 ,0.77 },
  { 0.30 ,0.32 ,0.72 ,0.78 ,0.73 },
  { 0  ,0  ,0  ,0  ,0  },
  { 0  ,0  ,0  ,0  ,0  },
	   	   
   };   
      
    for (float i = 0; i < 7; i++){  
		for(float k = 0; k < 5; k++){
					
				h3->Fill(k+1.5, i + 1.5, max[(int)i][(int)k]);
			}
		}

   h3->SetTitle("Black Box");
   // Drawing
   h3->SetXTitle("Steps");
   h3->SetYTitle("Detectors");
   
   h3->GetXaxis()->CenterTitle();
   h3->GetYaxis()->CenterTitle();
   
   //h2->GetXaxis()->SetLabelSize(0); //this removes the label, i.e. the numbers from the axis
   h3->GetXaxis()->SetTickLength(0); //removes the grid of the axis
   h3->GetYaxis()->SetTickLength(0);
	h3->GetXaxis()->SetNdivisions(5); //sets the number of divisions of the axis
	h3->GetXaxis()->CenterLabels();
	h3->GetYaxis()->CenterLabels();
		
   h3->Draw("ISO"); //ISO = draws the iso surface
    
}

//to say the possible thickness of the material inside the black box
void possibleThickness()
{
	float density[5] = {2.69, 11.35, 0.93, 7.87,1.70};
	float mu[5] = {0.084, 0.16, 0.24, 0.084,0.087};
	for (int i=0; i<5; i++)
		cout << TMath::Log(1/0.3)/(density[i] * mu[i]) << " error " << ((1/0.3) * 0.04)/(density[i] * mu[i]) << endl;
	
	
}


void resolCalibration()
{
	float mean1 = 644;
	float sigma1 = 128;
	float mean2 = 1669;
	float sigma2 = 157;
	
	calibrationLine(mean1, mean2);
	float fwhm1 = 2*TMath::Sqrt( 2*TMath::Log(2) )*sigma1;
	float resol1 = fwhm1/mean1;
	float fwhm2 = 2*TMath::Sqrt( 2*TMath::Log(2) )*sigma2;
	float resol2 = fwhm2/mean2;
	
	cout << resol1 << " " << resol2 << endl;
}



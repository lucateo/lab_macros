#include "TH1.h"
#include "TF1.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// these are to write in the middle of a text file directly using a program or whatever  
//The code stands for the word in the file that identifies the point where one should write
int searchLine(string code, const char* openFile)
{
	int numberLines = 0;
	string input = "";
	ifstream myfile(openFile);

    if(myfile.is_open()){
        while(!myfile.eof()){
            getline(myfile,input);
           // cout<< input << endl;
            numberLines++;
            if (input ==code) // you must not put any space in the original file!!!!!!!
				break;
        }
        myfile.close();
    
          }
   
    //cout << numberLines << endl;
    return numberLines;	
}
// n is the number of the line, to find using searchLine
void writeLines(int n, const char* openFile, const char* outFile)
{
	string input = "";
	ifstream file(openFile);
	ofstream outfile;
	outfile.open(outFile); //this rewrites the file
    for (int i = 0; i < n; i++) {
		getline(file, input);
		outfile << input << endl;
    }
   file.close();
   outfile.close();
}

void writeFinalLines(int n, const char* openFile, const char* outFile)
{
	int i = 1;
	string input = "";
	ifstream file(openFile);
	ofstream outfile;
	outfile.open(outFile, std::ios_base::app); //with the second option append the text to the file and	
	while(!file.eof()){											//does not overwrite it
    	getline(file, input);
		if (i > n) //if we are beyond the delimiter line, start writing 
			outfile << input << endl;
		i++;	
    }
   file.close();
   outfile.close();
}

//prova for writelines
void prova()
{
	    // at first write the lines before the %Table delimiter from provvisorio2.txt to provvisorio.txt
		writeLines(searchLine("%Table", "provvisorio2.txt"), "provvisorio2.txt","provvisorio.txt");
		ofstream outfile;
		outfile.open("provvisorio.txt", std::ios_base::app);
		// here write what you want after the initial lines always in provvisorio.txt
		outfile << "BELLAAAAAA" << endl;
		// writes lines after the delimiter %Table
		writeFinalLines(searchLine("%Table", "provvisorio2.txt"), "provvisorio2.txt","provvisorio.txt");
		outfile.close();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










//Digitizer data from the LAB

struct slimport_data_t {    //This is for Verdi acquisition
	ULong64_t	timetag; //time stamp
	UInt_t		baseline;
	UShort_t	qshort; //integration with shorter time
	UShort_t	qlong; //integration with longer time
	UShort_t	pur;
	UShort_t	samples[4096];
};

struct ntuple_data_t { // I think this is for Imaging acquisition
	
	Float_t	ch0;
	Float_t	ch1;
	Float_t	ch2;
	Float_t	ch3;
};


//read the tree and the time of acquisition
void readtree(const char *fileName) {
	
	slimport_data_t	inc_data;	// declare the struct

	TFile *input_file = new TFile(fileName,"READ"); // open the file in reading mode
	TTree *input_tree = (TTree*)input_file->Get("acq_tree_0"); //read the tree
        // TFile->Get() returns a generic object which must
	// be explicitly converted putting (TTree*) before

	input_tree->Print(); //check on the screen the structure of the tree

	// I get the branch and associate it to the struct
	TBranch *branch = input_tree->GetBranch("acq_ch0");
	branch->SetAddress(&inc_data.timetag);

    branch->GetEntry(1);
    long long int first_time = (long long int)inc_data.timetag;
    cout << inc_data.timetag << endl;
    long long int nEntries = (long long int)branch->GetEntries();
    branch->GetEntry(nEntries -1);
    long long int last_time = (long long int)inc_data.timetag;
    cout << inc_data.timetag << endl;
    cout << nEntries << endl;

    cout << "result " << (last_time - first_time)/100000000 << endl;


/*   root master things
	// I read the entries (leaves) and print the data
	cout << "Ev. #\tTime\tSamples" << endl;
	for (int i=0; i<18; i++) {
		branch->GetEntry(i); // this fills the struct
		cout << "#" << inc_data.qlong << "\t";
		cout << inc_data.timetag << "\t";
		for (int k=0; k<8; k++)
		    cout << inc_data.samples[k] << " ";
		cout << endl;
	} */

	//input_file->Close();
}

//to read the time
long long int readtime(const char *fileName) {
	
	slimport_data_t	inc_data;	// declare the struct

	TFile *input_file = new TFile(fileName,"READ"); // open the file in reading mode
	TTree *input_tree = (TTree*)input_file->Get("acq_tree_0"); //read the tree
        // TFile->Get() returns a generic object which must
	// be explicitly converted putting (TTree*) before

	// I get the branch and associate it to the struct
	TBranch *branch = input_tree->GetBranch("acq_ch0"); //channel 0 or 1 does not change much, the time is the same
	branch->SetAddress(&inc_data.timetag);

    branch->GetEntry(1);
    long long int first_time = (long long int)inc_data.timetag;
   
    long long int nEntries = (long long int)branch->GetEntries();
    branch->GetEntry(nEntries -1);
    long long int last_time = (long long int)inc_data.timetag;
    
    long long int result = (last_time - first_time)/100000000;
    
    return result;

}

//macro di prova
void looktime(const char *fileName1, const char *fileName2)
{
	long long int time_na = readtime(fileName1);
	long long int time_bg = readtime(fileName2);
	double scale = (double)time_na / (double)time_bg;
	cout << scale << endl;
}

TH1F* getHistoFromTree(char *name_file, int numBins, double minX, double maxX) {
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("acq_tree_0");
	TBranch *inbranch = intree->GetBranch("acq_ch1");
	inbranch->SetAddress(&indata.timetag);
	TH1F *h_spectrum = new TH1F("h_spectrum","Tagger Sodium first peak",numBins,minX,maxX);
	// histogram filling
	for (int i=0; i<inbranch->GetEntries(); i++) {
		inbranch->GetEntry(i);
		h_spectrum->Fill(indata.qlong);
	}
	// return
	h_spectrum->Draw();
	return h_spectrum;
}


//this is the one I will use in other macros
TH1F* getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX) {
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("acq_tree_0");
	TBranch *inbranch = intree->GetBranch(Form("acq_ch%d",chan));
	inbranch->SetAddress(&indata.timetag);
	TH1F *h_spectrum = new TH1F("spectrum","",numBins,minX,maxX); //remember to change the name!
	// histogram filling
	for (int i=0; i<inbranch->GetEntries(); i++) {
		inbranch->GetEntry(i);
		h_spectrum->Fill(indata.qlong);
	}
	
	//h_spectrum->Draw();
	// return
	return h_spectrum;
}


//this is the one I will use personally
TH1F* getHistoForChannelFromTreePersonal(char *name_file, short chan, int numBins, double minX, double maxX) {
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("acq_tree_0");
	TBranch *inbranch = intree->GetBranch(Form("acq_ch%d",chan));
	inbranch->SetAddress(&indata.timetag);
	TH1F *h_spectrum = new TH1F("spectrum","Detector sodium second peak",numBins,minX,maxX); //remember to change the name!
	// histogram filling
	for (int i=0; i<inbranch->GetEntries(); i++) {
		inbranch->GetEntry(i);
		h_spectrum->Fill(indata.qlong);
	}
	h_spectrum->SetXTitle("channels");
	h_spectrum->SetYTitle("counts");
	h_spectrum->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	//h_spectrum->Fit("gaus");
	gStyle->SetOptStat("emi"); //to show what I want in the panel, see reference (the letters specify which information will be included)
	h_spectrum->Draw();
	// return
	return h_spectrum;
}


TH1F* getHistoWithFilter(char *name_file, int numBins, double minX, double maxX, double lowThr = 0, double highThr = 999999) {
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("acq_tree_0");
	TBranch *inbranch = intree->GetBranch("acq_ch0");
	inbranch->SetAddress(&indata.timetag);
	TH1F *h_spectrum = new TH1F("h_spectrum","Total spectrum",numBins,minX,maxX);
	// histogram filling
	for (int i=0; i<inbranch->GetEntries(); i++) {
		inbranch->GetEntry(i);
		if (indata.qlong>lowThr && indata.qlong<highThr) {  // This fills only if the data is in the range I gave with lowThr and highThr
			h_spectrum->Fill(indata.qlong);
		}
	}
	// return
	return h_spectrum;
}


TH1F* subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX) { 
	// to subtract the background


	// canvas
	//TCanvas *c0 = new TCanvas("c0");
	
// remember getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)
// you should normalize the background time acquisition to the time of the source spectrum, try by dividing the data with the maximum value
//for each histogram, subtract them and then returning to the original value
	TH1F *h_na = getHistoForChannelFromTree(file_na, chan, numBins, minX, maxX);  //the proper file
	TH1F *h_bg = getHistoForChannelFromTree(file_bg, chan, numBins, minX, maxX); //the background file
	
	//take the time from the exposition
	long long int time_na = readtime(file_na);
	long long int time_bg = readtime(file_bg);
	double scale = (double)time_na / (double)time_bg;
	
	//scale the background
	h_bg->Scale(scale);
	h_bg->Draw();
	//the (TH1F*) is needed for compatibility with root 6
	TH1F *h_subtr = (TH1F*)h_na->Clone(); //this clones the h_na file and puts it on h_subtr
	h_subtr->Add(h_bg,-1); //this subtracts the background
	
	//I want to set to zero negative bins, this is because in the subtraction can happen that some bins are below zero
	//but it seems that it does not work...
	for (int i=0; i<h_subtr->GetEntries(); i++) {
		if ( h_subtr->GetBinContent(i) < 0 ) {
		h_subtr->SetBinContent(i,0); //set it to zero
		}
	}
	
	
	//h_subtr->SetMinimum(0); // the one I used before, wrong because negative values still remain
/*
	h_na->Draw();
	h_subtr->SetLineColor(2); //the subtracted background will have a different color*/
	h_subtr->Draw(); //In order to keep the previous drawing and draw on top of it the option SAME should be use
	//c0->Print("subtr_bg.png"); 
	return h_subtr;
}


TH1F* getIntegral(char *name_file, char *file_bg) {  //a particular class to determine the integral under a peak
	
	
	// remember subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX)
	TH1F *h_spectrum = subtract_bg(name_file, file_bg, 0, 1000, 0, 16850);
	//I don't know why but it works only if I specify the range in the histogram and then use Integral()
	/*
	h_spectrum->SetMaximum(4500.);        //to set the proper y range (Check if it counts only the area in this range!! 
    h_spectrum->SetMinimum(500.); */
	h_spectrum->SetXTitle("channels");
	h_spectrum->SetYTitle("counts");
	h_spectrum->GetYaxis()->SetTitleOffset(1.4); //to put the y title in the right position
	//h_spectrum->Fit("gaus");
	
	double integral = h_spectrum->Integral();
	
		
	gStyle->SetOptStat("mi"); //to show what I want in the panel, see reference (the letters specify which information will be included), i is
	h_spectrum->Draw();			// for the integral
	// return
	
	cout << h_spectrum->Integral() << endl;
	return h_spectrum;
}



//Imagine you want to plot two or more histograms on a single canvas:
void saveMultipleCanvas(char *file_one, char *file_two) {
	
	TCanvas *c0 = new TCanvas("c0");
	
	// remember getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)

	TH1F *h1 = getHistoForChannelFromTree(file_one, 0, 4000, 0, 16000);  //first histo
	TH1F *h2 = getHistoForChannelFromTree(file_two, 0, 4000, 0, 16000); //second histo
	
	c0->Divide(2,1); //depends on the number of histos
	c0->cd(1);
	h1->Draw();
	c0->cd(2);
	h2->Draw(); //and repeat for the number of histos
	c0->Print("some_histos.png");
}

//fit with three gaussians
void multifit3(char *file) {
 
  // remember TH1F* getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)
 
	TH1F *h = getHistoForChannelFromTree(file, 0, 4000, 15, 16000);
	
   Double_t par[9]; // the nine parameters of the three gaussians
   TF1 *g1    = new TF1("g1","gaus",85,95); //this is to find the proper parameters from the single fits
   TF1 *g2    = new TF1("g2","gaus",98,108); //and then puth them on the total fit
   TF1 *g3    = new TF1("g3","gaus",110,121);
   TF1 *total = new TF1("total","gaus(0)+gaus(3)+gaus(6)",85,125); // notice that its range contains all the previous
																	// three ranges
   //total->SetLineColor(3);
   h->Fit(g1,"R"); //"R" Use the Range specified in the function range
   h->Fit(g2,"R+"); //"R" Use the Range specified in the function range
   h->Fit(g3,"R+");
   g1->GetParameters(&par[0]); // I think he puts the parameters in the array par, for every fit
   g2->GetParameters(&par[3]); //there are three parameters: p0 = normalization factor, p1 = mean, p2 = sigma, 
   g3->GetParameters(&par[6]);
   total->SetParameters(par);
   h->Fit(total,"R+");
   h->Draw();
}



//fit with 2 guassian
void multifit2(char *file) {
	
	// remember TH1F* getHistoForChannelFromTree(char *name_file, short chan, int numBins, double minX, double maxX)
 
 // or put subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX);
 // instead of getHistoForChannelFromTree
	TH1F *h = getHistoForChannelFromTree(file, 0, 4000, 0, 16000);
	
   Double_t par[6]; // I need 6 parameters for two gaussians
   
   //you have to change the ranges needed for the experiment
   TF1 *g1    = new TF1("g1","gaus",1500,2500); //this is to find the proper parameters from the single fits
   TF1 *g2    = new TF1("g2","gaus",4500,5500); //and then puth them on the total fit
   
   TF1 *total = new TF1("total","gaus(0)+gaus(3)",1500,5500); // notice that its range contains all the previous
																	// two ranges
   //total->SetLineColor(3);
   
   h->Fit(g1,"R"); //"R" Use the Range specified in the function range
   h->Fit(g2,"R+"); //"+" Add this new fitted function to the list of fitted functions (by default, 
					//any previous function is deleted)
   g1->GetParameters(&par[0]); // I think he puts the parameters in the array par, for every fit
   g2->GetParameters(&par[3]); //there are three parameters: p0 = normalization factor, p1 = mean, p2 = sigma, 
   
   total->SetParameters(par);
   h->Fit(total,"R+");
   h->Draw();
}




// this draws the calibration line for two points
void fit2(float peak0, float peak1) {

	float m=1., q=0.; // default values for calibration (m=1, q=0 means "no calibration")

	float na_bin[2];
	na_bin[0] = peak0; // Sodium photopeak energy in bins
	na_bin[1] = peak1; // Sodium photopeak energy in bins

	float na_kev[2];
	na_kev[0] = 511.; // Sodium photopeak energy in keV
	na_kev[1] = 1275.; // Sodium photopeak energy in keV

	// Define a TGraphErrors which contains the experimental points in x
	// and the theoretical points in y; I semplify by not taking into account errors.
	
	//graph without errors
	TGraphErrors *graphErr = new TGraphErrors(2,na_bin,na_kev);
	
	//graph with errors
	//TGraphErrors *graphErr = new TGraphErrors(2,na_bin,na_kev);


	// Define a function which fits the points
	TF1 *fitfun = new TF1("calfitfun1","pol1",na_bin[0],na_bin[1]);

	// Graphical options to draw the fit
	graphErr->SetMarkerStyle(21);
	graphErr->SetLineColor(2);
	graphErr->Draw("ALP"); //A = Axis are drawn around the graph 
							// L = A simple polyline is drawn 
							// P= The current marker is plotted at each point 
							// (can be useful) C= a smooth Curve is drawn

	// Fit
	graphErr->Fit(fitfun);

	// Get the parameters
	m = fitfun->GetParameter(1);
	q = fitfun->GetParameter(0);

	cout << "Slope = " << m << ", Intercept = " << q << endl;

}


// this draws the calibration line for three points
void fit3(float peak0, float peak1, float peak2) {

	
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
	 float ex[] = {4.2, 9.2, 6.7}; //errors in x, I take the sigma
	 float ey[] = {0,0,0}; // errors in energy, obviously I don't have errors in the energy
	 TGraphErrors *graphErr = new TGraphErrors(3,bin,kev,ex,ey);
	 


	
	// Define a function which fits the points (does not work with errors)
	//TF1 *fitfun = new TF1("calfitfun1","pol1",bin[0],bin[2]);

	// Graphical options to draw the fit
	graphErr->SetMarkerStyle(21);
	graphErr->SetLineColor(2);
	graphErr->SetTitle("Calibration Line HPGe");
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


// not optimized yet
void gaussian_fit(char *file_na) {

	// Canvas
	TCanvas *c[3];
	for (int i=0; i<3; i++)
	   c[i] = new TCanvas(Form("c%i",i));

	// Get a histo with gaussian peaks
	TH1F *h_na = getHistoFromTree(file_na,1026,0,16384);
	c[0]->cd();
	h_na->Draw();
	
	// Get the two peaks of Na
	TSpectrum *s = new TSpectrum(30);
	int nPeaks;
	float *xPeaks;
	nPeaks = s->Search(h_na,1,"goff",0.12);
	xPeaks = (float*)s->GetPositionX();

	// I fit the two peaks with a Gaussian to get better values of centroids and sigmas

	int npoints;
	TH1F *h_fitted[2];
	float left_mk, right_mk, x_fit_min, x_fit_max, chisqr, mean, sigma;
	int apertura_sx = 250; // arbitrary values. Per un calcolo raffinato si può fare un secondo
	int apertura_dx = 450; // fit dove questi valori corrispondono alle sigma del primo fit...
	for (int p=0; p<2; p++) {
		c[p+1]->cd(); // change canvas
		h_fitted[p] = (TH1F*)h_na->Clone(); // Clone on a new TH1F to show separately the fits
		x_fit_min = xPeaks[p] - left_mk; // fit left margin
		x_fit_max = xPeaks[p] + right_mk; // fit right margin
		// fit function
		TF1 *fit = new TF1("fit","gaus",x_fit_min,x_fit_max);
		fit->SetParameters(1,xPeaks[p]); // Fix the centroid from the already known x
		fit->SetNpx(1000); // Number of points to define the function
		// fit
		h_fitted[p]->Fit("fit","","",x_fit_min,x_fit_max);
		npoints = (fit->GetNumberFitPoints())-3;
		if (npoints>0) 
		    chisqr = fit->GetChisquare()/npoints;
		else
		    chisqr = 9999;
	//	const = fit->GetParameter(0);  //in a gaussian, p0 = normalization, p1= mean, p2 = sigma
		mean = fit->GetParameter(1);
		sigma = fit->GetParameter(2);
		// Results
		cout << "Fit the peak n." << p << ": with TSpectrum x = " << xPeaks[p] << endl;
		cout << "                    with the fit    x = " << mean << " and sigma = " << sigma << endl;
		fit->Delete();
	}

	// http://root.cern.ch/viewvc/trunk/tutorials/fit/multifit.C?view=markup

}




void CalibrateHisto(char *name_file, float m, float q)
{
	// put here the code to obtain the noncalibrated histogram
	TH1F *h_uncal = getHistoForChannelFromTree(name_file,0,514,0,16384);
	
	
	int max_bin = h_uncal->GetNbinsX(); // This method returns the number of bins in x of the histogram
	float max_kev = h_uncal->GetBinCenter(max_bin)*m + q;
	h_uncal->GetXaxis()->SetLimits(q,max_kev);
	if (m!=1 && q!=0) //This means that I actually changed the calibration!
	{
	    h_uncal->SetXTitle("keV");
	    h_uncal->Draw();
	 }
}

// to use directly as an histogram output in following functions
void CalibrateHisto(TH1F *h_uncal, float m, float q) { //Re-scaling of axis, as in the slides

//BE CAREFUL! TO MAKE IT WORK, YOU MUST START ALWAYS FROM ZERO!
	int max_bin = h_uncal->GetNbinsX(); // This method returns the number of bins in x of the histogram
	float max_kev = h_uncal->GetBinCenter(max_bin)*m + q; //energy corresponding to the last channel
	h_uncal->GetXaxis()->SetLimits(q,max_kev);
	if (m!=1 && q!=0) //This means that I actually changed the calibration!
	    h_uncal->SetXTitle("keV");

}


//calibrated histogram
TH1F* calibrate(char *file_na, char *file_bg, short chan,int numBins, double minX, double maxX ) { 
	//the error are told in the minimizer

	// canvas (I suppose that if I put draw option at the end, the canvas option is redundant)
	//TCanvas *c0 = new TCanvas("c0");

	// read the histos
	// remember subtract_bg(char *file_na, char *file_bg, short chan, int numBins, double minX, double maxX)
	TH1F *h_na = subtract_bg(file_na, file_bg, chan, numBins, minX, maxX);
	
	/* I'll do it on hand
	// search the peaks
	TSpectrum *s = new TSpectrum(30);
	int nPeaks;
	float *na_bin;
	nPeaks = s->Search(h_na,1,"",0.1);
 	na_bin = s->GetPositionX(); */

	// fit the peaks with the Na energies
	float na_bin[3];
	
	/* // uncomment this and comment the next for HPGe calibration
	//insert on hand this, these are for HPGe calibration
	na_bin[0] = 3815.; 
	na_bin[1] = 9517.;
	na_bin[2] = 445.; //americium */
	
	 // uncomment this and comment the previous for NaI calibration
	//insert on hand this, these are for NaI calibration
	na_bin[0] = 2043.; 
	na_bin[1] = 4942.;
	na_bin[2] = 264.; //americium 
	
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

	// call the function to calibrate the histogram
	CalibrateHisto(h_na, m, q);
	
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





//macro to do a generic histogram by hand
void makeHistogram() { // I try the calibration fo Europium
	
	// in this example they are the europium energies
	float binx[10]= {244., 344., 411., 443, 778, 867, 964, 1085, 1112, 1408 };
	
	//insert by hand this
	float biny[10] = {1.25, 1, 0.9, 0.85, 0.45, 0.4, 0.38, 0.373, 0.371, 0.25 };
	

	int nPoints = 10;
	// Define a TGraphErrors which contains the experimental points in x
	// and the theoretical points in y; I semplify by not taking into account errors.
	TGraphErrors *graphErr = new TGraphErrors(nPoints,binx,biny);

	// Define a function which fits the points
	TF1 *fitfun = new TF1("calfitfun1","pol10",binx[0],binx[9]); //change pol(nPoints) and the maximum bin value
																// in binx[nPoints-1]

	// Graphical options to draw the fit
	graphErr->SetMarkerStyle(21);
	graphErr->SetLineColor(2);
	graphErr->SetTitle("Calibration Line");
	graphErr->GetXaxis()->SetTitle("Energy (keV)");
	graphErr->GetYaxis()->SetTitle("relative efficiency");
	// Fit
	graphErr->Fit(fitfun);
	graphErr->Draw("AP"); //A = Axis are drawn around the graph 
							// L = A simple polyline is drawn 
							// P= The current marker is plotted at each point 
							// (can be useful) C= a smooth Curve is drawn

	gStyle->SetOptStat("");
	
	//If I want to evaluate the function at different points (here at 600)
	cout << fitfun->Eval(600) << endl;
}







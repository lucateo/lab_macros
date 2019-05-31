#include "lab_macros_gamma.C"
#include <iomanip>
#include <fstream>
//this is to write directly on the latex file, trial programs that use writelines/searchline idea to write 
//things directly in the latex file the way I want


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
	float distance_HPGe[5] = {23, 9.2, 9.2, 23, 23};//??????
	
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
		{78, 145, 183, 243, 295, 347, 609, 767, 931, 1036, 1119, 1238, 1373, 1508}
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
		{4081,3703,3158,1545,1844,2394,2044,670,450,344,539,355,308,279}
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
	
	
	//write
	writeLines(searchLine("%TABLE_ENERGY", "physical_lab_reports.tex"), "physical_lab_reports.tex","physical_lab_reports2.tex");
		ofstream outfile;
		outfile.open("physical_lab_reports2.tex", std::ios_base::app);
	cout  << "peaks" << "\t" << "counts"  << "\t" << setw(10) << "error" << "\t"  << "eff abs" << "\t" << 
	 "error" << "\t" << setw(13) <<  "Activ" << "\t" << setw(10) << "error" << "\t" << "percent" << endl;
	for (int i=0; i< 12; i++)
	{
		cout << converterActivity(i) << endl;
		for (int j=0; j< 14; j++) {
			cout  << fixed << setprecision(0) << peaks[i][j] << "\t" << counts[i][j] << "\t" << setw(10) << setprecision(1) << errorcounts[i][j] << "\t"  << 
			 setprecision(3) << efficiency[i][j] << "\t" << err_eff[i][j] << "\t" << setprecision(2) << setw(13) << activity[i][j] << "\t" << setw(10) << err_Activity[i][j] << "\t" << percent[i][j] << endl;
		}
		cout << "Total activity " << activityTot[i] << " $\\pm$ " << err_actTot[i] << endl;
	}
	
	writeFinalLines(searchLine("%TABLE_ENERGY", "physical_lab_reports.tex"), "physical_lab_reports.tex","physical_lab_reports2.tex");
		outfile.close();
	//gSystem->ChangeDirectory("~/physics_laboratory");
	int status = system("bash scriptLatex.sh activity");
	//gSystem->cd("~/physics_laboratory_data");

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
	float eff_ass = 0.422 ; //absolute efficiency for HPGe at 511 keV
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
		{78, 145, 183, 243, 295, 347, 609, 767, 931, 1036, 1119, 1238, 1373, 1508}
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
		for (int i=0; (2*i) < 12; i++)
		{
			cout << "{" ;	
			
		for (int j=0; j< 14; j++)
			cout << efficiencies[(2*i)][j] << ", ";
		 cout << "}," << endl;	
		}
		
		cout << endl;
		
		for (int i=0; (2*i) < 12; i++)
		{	
			cout << "{" ;
		for (int j=0; j< 14; j++)
			cout << err_eff[(2*i)][j] << ", ";
		 cout << "}," << endl;
		}
		
		
					
}



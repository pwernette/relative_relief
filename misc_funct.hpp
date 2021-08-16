#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <map>
#include <vector>
#include <random>

//#include "subsurface data structures.hpp"

using namespace std;

//float roundToInteger(float x){
//	return floor( x + .5);
//}
//
//float roundToTenths(float x){
//	x /=10;
//	return floor(x + 0.5) * 10;
//}
//
//float roundToHundredths(float x){
//	x /=100;
//	return floor(x + 0.5) * 100;
//}
//
//float roundToThousandths(float x){
//	x /=1000;
//	return floor(x + 0.5) * 1000;
//}

//void Header::defaultHeader(){
//	Header.ncols = 0;
//	Header.nlines = 0;
//	Header.bands = 1;
//	Header.headeroffset = 0;
//	Header.datatype = 0;
//	Header.xres = 0;
//	Header.yres = 0;
//	Header.ulx = 0;
//	Header.uly = 0;
//	Header.ymin = 999999999;
//	Header.xmax = -999999999;
//	Header.zmin = 99999;
//	Header.zmax = -99999;
//}


//extracts the output data type, based on the input vector data type
//int datout(vector<float> outdat){
//	//map all possible data types
//	map<string, int> dattypes;
//	int second;
//
//	//initialize data types
//	dattypes["unsigned"]=1;
//	dattypes["unsigned int"]=1;
//	dattypes["int"]=2;
//	dattypes["long"]=3;
//	dattypes["float"]=4;
//	dattypes["double"]=5;
//
//	second = dattypes[outdat];
//
//	return(second);
//}
//int datout(vector<unsigned int> outdat){
//	return(1);
//}
//int datout(vector<int> outdat){
//	return(2);
//}
//int datout(vector<long> outdat){
//	return(3);
//}
//int datout(vector<float> outdat){
//	return(4);
//}
//int datout(vector<double> outdat){
//	return(5);
//}

//function to compute the relative relief
void Raster::computeRelativeRelief(int i, int j, int buf, Header hdr){
//	cout << "Raster size (z): " << Raster::z.size() << endl;
//	cout << "Raster size (rr): " << Raster::avg.size() << endl;

	register int ii, jj, a;
	int index = (i*hdr.ncols)+j;
	float z1 = 0;
	vector<float> tmp;

	//cout << i << ", " << j << " = " << index << endl;
	for(a=buf; a<(buf+9); ++a){
		float z_min = Raster::z[index];
		float z_max = Raster::z[index];

		for(ii=i-a; ii<i+a+1; ++ii){
			if(ii>=0 && ii<hdr.nlines){
				for(jj=j-a; jj<j+a+1; ++jj){
					if(jj>=0 && jj<hdr.ncols){
						int index2 = (ii*hdr.ncols)+jj;

						if(Raster::z[index2] > -100){
//							cout << index2 << " ==> " << Raster::z[index2] << endl;

							// calculate parameters for relative relief
							if(Raster::z[index2] > z_max){
								z_max = Raster::z[index2];
//								cout << index << " - " << index2 << " (" << a << ")" << " max -> " << z_max << endl;
							} else if(Raster::z[index2] < z_min){
								z_min = Raster::z[index2];
//								cout << index << " - " << index2 << " (" << a << ")" << " min -> " << z_min << endl;
							}
						}
					}
				}
			}
		}
		
//		cout << (Raster::z[index] - z_min) << endl;
//		float rrval = ((Raster::z[index] - z_min)/(z_max - z_min));
		
//		cout << "RR (" << i << "," << j << "," << a << "): " << (Raster::z[index] - z_min)/(z_max - z_min) << endl;

		tmp.push_back((Raster::z[index] - z_min)/(z_max - z_min));
		Raster::res[index] = tmp[0];

//		if(a == buf){			// IF window size +1, THEN write window + 0
//			Raster::res[index] = rrval;
////			cout << " a: " << a << " - " << Raster::res[index] << endl;
//		}
//		if(a == (buf+1)){		// IF window size +1, THEN write window + 1
//			Raster::res_plus1[index] = rrval;
////			cout << "a: " << a << " - " << Raster::res_plus1[index] << endl;
//		}
//		if(a == (buf+2)){		// IF window size +1, THEN write window + 2
//			Raster::res_plus2[index] = rrval;
////			cout << "a: " << a << " - " << Raster::res_plus2[index] << endl;
//		}
//		if(a=(buf+3)){		// IF window size +1, THEN write window + 3
//			rast[index].relative_relief.res_plus3 = ((rast[index].dem.z - z_min)/(z_max - z_min));
////			cout << "a: " << a << " - " << rast[index].relative_relief.res_plus3 << endl;
//		}
//		if(a=(buf+4)){		// IF window size +1, THEN write window + 4
//			rast[index].relative_relief.res_plus4 = ((rast[index].dem.z - z_min)/(z_max - z_min));
////			cout << "a: " << a << " - " << rast[index].relative_relief.res_plus4 << endl;
//		}
	}

//	cout << "accumulate function (" << i << "," << j << "," << a << ") --> " << accumulate(tmp.begin(), tmp.end(), 0.0)/tmp.size() << endl;

	Raster::avg[index] = accumulate(tmp.begin(), tmp.end(), 0.0)/tmp.size();

//	cout << "  stored value: " << Raster::avg[index] << endl;

//	Raster::avg[index] = (Raster::res[index] * Raster::res_plus1[index] * Raster::res_plus2[index]);
//	rast[index].relative_relief.avg = (rast[index].relative_relief.res + rast[index].relative_relief.res_plus1 + rast[index].relative_relief.res_plus2 + rast[index].relative_relief.res_plus3 + rast[index].relative_relief.res_plus4)/5;
}
/*
void computeSlopeAngle(string outprod, const int i, const int j, const int buf, Header& hdr, Raster *rast){
	if(outprod.compare("slope")!=0 &&
			outprod.compare("slope1")!=0 &&
			outprod.compare("slope2")!=0 &&
//			outprod.compare("slope3")!=0 &&
//			outprod.compare("slope4")!=0 &&
			outprod.compare("avgslope")!=0 &&
			outprod.compare("slopes")!=0 &&
			outprod.compare("azimuth")!=0 &&
			outprod.compare("all")!=0){
	} else{
		register int ii, jj, a;

		int index = (i*hdr.ncols)+j;

		double x1 = rast[index].x;
		double y1 = rast[index].y;
		double z1 = rast[index].dem.z;

		for(a=buf; a<(buf+5); ++a){
			int n = 0;

			double slope_angle = 0.0;
			double azimuth = 0.0;
			double slope_min = 9999;	// used to calculate azimuth

			for(ii=i-buf; ii<i+buf; ++ii){
				for(jj=j-buf; jj<j+buf; ++jj){
					int index2 = (ii*hdr.ncols)+jj;

					if(rast[index2].dem.z > -100 && index!=index2){
						double x2 = rast[index2].x;
						double y2 = rast[index2].y;
						double z2 = rast[index2].dem.z;

						// calculate the distance between pixels (planimetric distance = 2D distance)
						double dist = sqrt(pow((x2-x1), 2.0)+pow((y2-y1), 2.0));

						// calculate SLOPE ANGLE
						slope_angle += atan((z2 - z1)/dist)*(180/pi);

						// calculate SLOPE AZIMUTH ANGLE
						//
						// Azimuth angle was calculated based on the following diagram:
						//
						//           * (X2, Y2)
						//           ^
						//           |
						//           |
						//           |
						//           |
						//  (X1, Y1) * - - - - >* (X3, Y3)
						//
						if(atan((z2 - z1)/dist)*(180/pi) < slope_min){
							slope_min = atan((z2 - z1)/dist)*(180/pi);
							double x3 = x1+0;
							double y3 = y1+1;

							// the inner angle
							double dx21 = x2-x1;		// X2 to X1 diatnce
							double dx31 = x3-x1;		// X3 to X1 distance
							double dy21 = y2-y1;		// Y2 to Y1 distance
							double dy31 = y3-y1;		// Y3 to Y1 distance

							double x12 = sqrt(pow(dx21,2)+pow(dy21,2));	// straightline distance from point 2 to point 1
							double x13 = sqrt(pow(dx31,2)+pow(dy31,2));	// straightline distance from point 3 to point 1

							// if the inner angle is to the west of the N-S line, then add 180 degrees
							if(x2-x1 < 0){
								// 180 - internal angle = deviation from North
								// deviation from North + 180 = clockwise orientation from North
								azimuth = (180-(fabs(acos((dx21*dx31+dy21*dy31)/(x12*x13))*180/pi))+180);
							} else{
								// internal angle from North = clockwise orientation from North
								azimuth = acos((dx21*dx31+dy21*dy31)/(x12*x13))*180/pi;
							}
						}

						// increment the pixel counter
						++n;
					}
				}
			}

			if(a=buf){			// IF window size +0, THEN write window + 0
				rast[index].slope_angle.res = slope_angle/n;
				rast[index].slope_azimuth = azimuth;
			}
			if(a=buf+1){		// IF window size +1, THEN write window + 1
				rast[index].slope_angle.res_plus1 = slope_angle/n;
			}
			if(a=buf+2){		// IF window size +2, THEN write window + 2
				rast[index].slope_angle.res_plus2 = slope_angle/n;
			}
//			if(a=buf+3){		// IF window size +2, THEN write window + 3
//				rast[index].slope_angle.res_plus3 = slope_angle/n;
//			}
//			if(a=buf+4){		// IF window size +2, THEN write window + 4
//				rast[index].slope_angle.res_plus4 = slope_angle/n;
//			}
		}

		rast[index].slope_angle.avg = (rast[index].slope_angle.res + rast[index].slope_angle.res_plus1 + rast[index].slope_angle.res_plus2)/3;
//		rast[index].slope_angle.avg = (rast[index].slope_angle.res + rast[index].slope_angle.res_plus1 + rast[index].slope_angle.res_plus2 + rast[index].slope_angle.res_plus3 + rast[index].slope_angle.res_plus4)/5;
	}
}
*/
/*
void computeSlopeSD(string outprod, const int i, const int j, const int buf, Header& hdr, Raster *rast){
	if(outprod.compare("sd")!=0 &&
			outprod.compare("sd1")!=0 &&
			outprod.compare("sd2")!=0 &&
//			outprod.compare("sd3")!=0 &&
//			outprod.compare("sd4")!=0 &&
			outprod.compare("avgsd")!=0 &&
			outprod.compare("sds")!=0 &&
			outprod.compare("all")!=0){
	} else{
		register int ii, jj, a;

		int index = (i*hdr.ncols)+j;

		double x1 = rast[index].x;
		double y1 = rast[index].y;
		double z1 = rast[index].dem.z;

		for(a=buf; a<(buf+5); ++a){
			int n = 0;

			double slope_min = 9999;	// used to calculate azimuth
			double sum_samp_sq = 0;		// used to calculate slope angle standard deviation
			double sum = 0;

			for(ii=i-buf; ii<i+buf; ++ii){
				for(jj=j-buf; jj<j+buf; ++jj){
					int index2 = (ii*hdr.ncols)+jj;

					if(rast[index2].dem.z > -100 && index!=index2){
						double x2 = rast[index2].x;
						double y2 = rast[index2].y;
						double z2 = rast[index2].dem.z;

						// calculate the distance between pixels (planimetric distance = 2D distance)
						double dist = sqrt(pow((x2-x1), 2.0)+pow((y2-y1), 2.0));

						sum_samp_sq += pow(atan((z2 - z1)/dist)*(180/pi), 2.0);
						sum += atan((z2 - z1)/dist)*(180/pi);

						// increment the pixel counter
						++n;
					}
				}
			}

			if(a=buf){			// IF window size +0, THEN write window + 0
				rast[index].sd.res = sqrt((sum_samp_sq - (pow(sum, 2.0)/n))/(n));
			}
			if(a=buf+1){		// IF window size +1, THEN write window + 1
				rast[index].sd.res_plus1 = sqrt((sum_samp_sq - (pow(sum, 2.0)/n))/(n));
			}
			if(a=buf+2){		// IF window size +2, THEN write window + 2
				rast[index].sd.res_plus2 = sqrt((sum_samp_sq - (pow(sum, 2.0)/n))/(n));
			}
//			if(a=buf+3){		// IF window size +2, THEN write window + 3
//				rast[index].sd.res_plus3 = sqrt((sum_samp_sq - (pow(sum, 2.0)/n))/(n));
//			}
//			if(a=buf+4){		// IF window size +2, THEN write window + 4
//				rast[index].sd.res_plus4 = sqrt((sum_samp_sq - (pow(sum, 2.0)/n))/(n));
//			}
		}

		rast[index].sd.avg = (rast[index].sd.res + rast[index].sd.res_plus1 + rast[index].sd.res_plus2)/3;

//		rast[index].sd.avg = (rast[index].sd.res + rast[index].sd.res_plus1 + rast[index].sd.res_plus2 + rast[index].sd.res_plus3 + rast[index].sd.res_plus4)/5;
	}
}
*/
//void getProj(string output_dat_type, Header& head){
//	////////////////////////////////////////////////
//	// GET PROJECTION INFORMATION FROM USER INPUT //
//	////////////////////////////////////////////////
//	if(output_dat_type.compare("envi")==0 || output_dat_type.compare("both")==0){
//		cout << "Please enter the following coordinate system information.\n";
//
//		// projection system (e.g. UTM) and convert it to lowercase
//		cout << "Projection System: ";
//		getline(cin, head.coordsys);
//		transform(head.coordsys.begin(), head.coordsys.end(), head.coordsys.begin(), ::toupper);
//
//		// utm zone information (optional)
//		if(head.coordsys.compare("UTM")==0){
//			cout << "UTM zone number: ";
//			getline(cin, head.utm_zone_number);
//
//			cout << "UTM Hemisphere ('North' or 'South'): ";
//			getline(cin, head.utm_zone_band);
//			transform(head.utm_zone_band.begin(), head.utm_zone_band.end(), head.utm_zone_band.begin(), ::tolower);
//			if((head.utm_zone_band).compare("n")==0 || head.utm_zone_band.compare("north")==0){
//				head.utm_zone_band = "N";
//			} else if(head.utm_zone_band.compare("s")==0 || head.utm_zone_band.compare("south")==0){
//				head.utm_zone_band = "S";
//			} else{
//				cout << "Invalid hemisphere! Please re-run program and enter 'N' or 'S'.\n";
//				exit(1);
//			}
//
//			// datum information
//			cout << "Datum ('WGS-84' or 'NAD-83'): ";
//			getline(cin, head.datum);
//			if(head.datum.compare("WGS-84")==0 || head.datum.compare("wgs-84")==0 || head.datum.compare("WGS84")==0 || head.datum.compare("wgs84")==0){
//				head.datum = "WGS-84";
//			} else if(head.datum.compare("NAD-83")==0 || head.datum.compare("nad-83")==0 || head.datum.compare("NAD83")==0 || head.datum.compare("nad83")==0){
//				head.datum = "NAD-83";
//			} else{
//				cout << "Invalid datum! Please re-run program and enter 'WGS-84' or 'NAD-83' only.\n";
//				exit(1);
//			}
//
//			// units information
//			cout << "Units ('m' or 'ft'): ";
//			getline(cin, head.units);
//			transform(head.units.begin(), head.units.end(), head.units.begin(), ::tolower);
//			if(head.units.compare("m")==0 || head.units.compare("meters")==0 || head.units.compare("M")==0 || head.units.compare("Meters")==0){
//				head.units = "Meters";
//				cout << "\n";
//			} else if(head.units.compare("ft")==0 || head.units.compare("feet")==0 || head.units.compare("Feet")==0 || head.units.compare("Feet")==0){
//				head.units = "Feet";
//				cout << "\n";
//			} else{
//				cout << "Invalid units! Please re-run program and enter 'm' or 'ft'.\n";
//				exit(1);
//			}
//		} else if(head.coordsys.compare("PADRE")==0 || head.coordsys.compare("NPI")==0 || head.coordsys.compare("N")==0){
//			head.coordsys = "utm";
//			head.utm_zone_number = "14";
//			head.utm_zone_band = "N";
//			head.datum = "WGS-84";
//			head.units = "Meters";
//		} else if(head.coordsys.compare("")==0){
//			cout << "ERROR: No projection system entered! Please enter a coordinate system and re-run program.\n";
//			exit(1);
//		}
//	}
//}
//
//
//void getThresholds(Thresholds& thresh, string shoreline_indicator, string default_threshold_values){
//	// check to use default values?
//	cout << "Use default thresholds? (default is 'yes'): ";
//	getline(cin, default_threshold_values);
//	transform(default_threshold_values.begin(), default_threshold_values.end(), default_threshold_values.begin(), ::tolower);
//
//	// get user input and check for a valid data type
//	bool valid = false;
//
//	if(default_threshold_values.compare("y")==0 || default_threshold_values.compare("yes")==0){
//		thresh.shoreline = 0.2;
//		thresh.dune_toe = 0.2;
//		thresh.dune_crest = 0.8;
//		thresh.dune_heel = 0.4;
//		thresh.backbarrier_shoreline = 1.0;
//		thresh.dune_setback.min = 20;
//		thresh.dune_setback.max = 100;
//		thresh.crest_setback.min = 1;
//		thresh.crest_setback.max = 50;
//		thresh.heel_setback.min = 1;
//		thresh.heel_setback.max = 50;
//	} else{
//		/////////////////////////////////////
//		//   SHORELINE THRESHOLD INFORMATION
//		/////////////////////////////////////
//		cout << "Use 0 as the shoreline threshold? (default is 'yes'): ";
//		getline(cin, shoreline_indicator);
//		transform(shoreline_indicator.begin(), shoreline_indicator.end(), shoreline_indicator.begin(), ::tolower);
//
//		if(shoreline_indicator.compare("y")!=0 && shoreline_indicator.compare("yes")!=0){
//			cout << "Please enter the following threshold values: \n";
//
//			while(!valid){
//				valid = true;
//
//				cout << "Enter value for shoreline elevation: ";
//				cin >> thresh.shoreline;
//
//				if(cin.fail() || !(cin)){
//					cin.clear();
//					cin.ignore(99999, '\n');
//
//					cout << "ERROR: Invalid value! Please enter a float." << endl;
//					valid = false;
//				}
//			}
//			cin.sync();	// sync the program and inputs
//		} else{
//			thresh.shoreline = 0.2;
//		}
//
//		/////////////////////////////////////
//		//   DUNE TOE THRESHOLD INFORMATION
//		/////////////////////////////////////
//		// Set the dune toe setback minimum distance
//		valid = false;
//		while(!valid){
//			valid = true;
//
//			cout << "Minimum distance (in meters) from shoreline to dune toe: ";
//			cin >> thresh.dune_setback.min;
//
//			if(cin.fail() || !(cin)){
//				cin.clear();
//				cin.ignore(99999, '\n');
//
//				cout << "ERROR: Invalid value! Please enter a float." << endl;
//				valid = false;
//			}
//		}
//		cin.sync();	// sync the program and inputs
//
//		// Set the dune toe setback maximum distance
//		valid = false;
//		while(!valid){
//			valid = true;
//
//			cout << "Maximum distance (in meters) from shoreline to dune toe: ";
//			cin >> thresh.dune_setback.max;
//
//			if(cin.fail() || !(cin)){
//				cin.clear();
//				cin.ignore(99999, '\n');
//
//				cout << "ERROR: Invalid value! Please enter a float." << endl;
//				valid = false;
//			}
//		}
//		cin.sync();	// sync the program and inputs
//
//		// Set the dune toe RR threshold
//		valid = false;
//		while(!valid){
//			valid = true;
//
//			cout << "Dune Toe relative relief?\n    ranges from 0.000 to 1.000 (suggested: 0.2): ";
//			cin >> thresh.dune_toe;
//
//			if(cin.fail() || !(cin)){
//				cin.clear();
//				cin.ignore(99999, '\n');
//
//				cout << "ERROR: Invalid value! Please enter a float." << endl;
//				valid = false;
//			}
//		}
//		cin.sync();	// sync the program and inputs
//
//		/////////////////////////////////////
//		//   DUNE CREST THRESHOLD INFORMATION
//		/////////////////////////////////////
//		// Set the dune crest setback minimum distance
//		valid = false;
//		while(!valid){
//			valid = true;
//
//			cout << "Minimum distance (in meters) from dune toe to dune crest: ";
//			cin >> thresh.crest_setback.min;
//
//			if(cin.fail() || !(cin)){
//				cin.clear();
//				cin.ignore(99999, '\n');
//
//				cout << "ERROR: Invalid value! Please enter a float." << endl;
//				valid = false;
//			}
//		}
//		cin.sync();
//
//		// Set the dune crest setback maximum distance
//		valid = false;
//		while(!valid){
//			valid = true;
//
//			cout << "Maximum distance (in meters) from dune toe to dune crest: ";
//			cin >> thresh.crest_setback.max;
//
//			if(cin.fail() || !(cin)){
//				cin.clear();
//				cin.ignore(99999, '\n');
//
//				cout << "ERROR: Invalid value! Please enter a float." << endl;
//				valid = false;
//			}
//		}
//		cin.sync();
//
//		// Set the dune crest RR threshold
//		valid = false;
//		while(!valid){
//			valid = true;
//
//			cout << "Dune Crest relative relief?\n    ranges from 0.000 to 1.000 (suggested: 0.8): ";
//			cin >> thresh.dune_crest;
//
//			if(cin.fail() || !(cin)){
//				cin.clear();
//				cin.ignore(99999, '\n');
//
//				cout << "ERROR: Invalid value! Please enter a float." << endl;
//				valid = false;
//			}
//		}
//		cin.sync();	// sync the program and inputs
//
//		/////////////////////////////////////
//		//   DUNE HEEL THRESHOLD INFORMATION
//		/////////////////////////////////////
//		// Set the dune heel setback minimum distance
//		valid = false;
//		while(!valid){
//			valid = true;
//
//			cout << "Minimum distance (in meters) from dune crest to dune heel: ";
//			cin >> thresh.heel_setback.min;
//
//			if(cin.fail() || !(cin)){
//				cin.clear();
//				cin.ignore(99999, '\n');
//
//				cout << "ERROR: Invalid value! Please enter a float." << endl;
//				valid = false;
//			}
//		}
//		cin.sync();
//
//		// Set the dune heel setback maximum distance
//		valid = false;
//		while(!valid){
//			valid = true;
//
//			cout << "Maximum distance (in meters) from dune crest to dune heel: ";
//			cin >> thresh.heel_setback.max;
//
//			if(cin.fail() || !(cin)){
//				cin.clear();
//				cin.ignore(99999, '\n');
//
//				cout << "ERROR: Invalid value! Please enter a float." << endl;
//				valid = false;
//			}
//		}
//		cin.sync();
//
//		// Set the dune heel RR threshold
//		valid = false;
//		while(!valid){
//			valid = true;
//
//			cout << "Dune Heel relative relief?\n    ranges from 0.000 to 1.000 (suggested: 0.4): ";
//			cin >> thresh.dune_heel;
//
//			if(cin.fail() || !(cin)){
//				cin.clear();
//				cin.ignore(99999, '\n');
//
//				cout << "ERROR: Invalid value! Please enter a float." << endl;
//				valid = false;
//			}
//		}
//		cin.sync();	// sync the program and inputs
//
//		/////////////////////////////////////
//		//   BACKBARRIER SHORELINE THRESHOLD INFORMATION
//		/////////////////////////////////////
//		// Set the backbarrier shoreline elevation threshold
//		valid = false;
//		while(!valid){
//			valid = true;
//
//			cout << "Enter value for backbarrier shoreline elevation: ";
//			cin >> thresh.backbarrier_shoreline;
//
//			if(cin.fail() || !(cin)){
//				cin.clear();
//				cin.ignore(99999, '\n');
//
//				cout << "ERROR: Invalid value! Please enter a float." << endl;
//				valid = false;
//			}
//		}
//
//		//sync the program and inputs
//		cin.sync();
//	}
//}

/*void DEMwithEMI(Header& header, Raster *rast, string filename){
	int i;
	string ofemi = "DATA_INTEGRATED_";
	ofemi.append(filename);
	ofemi.append(".txt");
	
	//cout << header.ulx << " - " << header.xmax << " - " << header.ymin << " - " << header.uly << endl;
	// initialize the file pointer
//	FILE *wr;
	FILE *wr2;

	// open the file
//	wr = fopen(of.c_str(), "w");
	wr2 = fopen(ofemi.c_str(), "w");
	if(!wr || !wr2){
		cout << "ERROR: Cannot write new file!\n";
		exit(1);
	}
	
//	(void) fprintf(wr, "UTM_x, UTM_y, Elevation, Cond_Freq1, Cond_Freq2, Cond_Freq3\n");
	(void) fprintf(wr2, "UTM_x, UTM_y, Elevation, Cond_Freq1, Cond_Freq2, Cond_Freq3\n");
	
	for(i=0; i<header.npix; i++){
		if(rast[i].dem.z > -9999){
			(void) fprintf(wr, "%le, %le, %le, %f, %f, %f\n", rast[i].x, rast[i].y, rast[i].dem.z, rast[i].emi.cond_freq1, rast[i].emi.cond_freq2, rast[i].emi.cond_freq3);
		}
		
		// write DEM and EMI values to file only if there is an EMI value		
		if(rast[i].emi.cond_freq1 > 0){
			(void) fprintf(wr2, "%le, %le, %le, %f, %f, %f\n", rast[i].x, rast[i].y, rast[i].dem.z, rast[i].emi.cond_freq1, rast[i].emi.cond_freq2, rast[i].emi.cond_freq3);
		}
	}
	
//	fclose(wr);
	fclose(wr2);
}*/


//void compare2list(string outprod){
////	switch(outprod)
////	{
////	case "dem":
////	case "rr":
////	case "rr1":
////	case "rr2":
////	case "avgrr":
////	case "shoreline":
////	case "dunetoe":
////	case "dunecrest":
////	case "duneheel":
////	case "backbarrier":
////	case "island":
////	case "all":
////	default:
////		cout << "Invalid output product! Please select an appropriate output product and re-run the program." << endl;
////		exit(1);
////	}
//	if(outprod.compare("dem")!=0 &&
////			outprod.compare("slope")!=0 &&
////			outprod.compare("slope1")!=0 &&
////			outprod.compare("slope2")!=0 &&
////			outprod.compare("slope3")!=0 &&
////			outprod.compare("slope4")!=0 &&
////			outprod.compare("avgslope")!=0 &&
////			outprod.compare("slopes")!=0 &&
////			outprod.compare("azimuth")!=0 &&
////			outprod.compare("sd")!=0 &&
////			outprod.compare("sd1")!=0 &&
////			outprod.compare("sd2")!=0 &&
////			outprod.compare("sd3")!=0 &&
////			outprod.compare("sd4")!=0 &&
////			outprod.compare("avgsd")!=0 &&
////			outprod.compare("sds")!=0 &&
//			outprod.compare("rr")!=0 &&
//			outprod.compare("rr1")!=0 &&
//			outprod.compare("rr2")!=0 &&
////			outprod.compare("rr3")!=0 &&
////			outprod.compare("rr4")!=0 &&
//			outprod.compare("avgrr")!=0 &&
////			outprod.compare("rrs")!=0 &&
//			outprod.compare("shoreline")!=0 &&
//			outprod.compare("dunetoe")!=0 &&
//			outprod.compare("dunecrest")!=0 &&
//			outprod.compare("duneheel")!=0 &&
//			outprod.compare("backbarrier")!=0 &&
//			outprod.compare("island")!=0 &&
//			outprod.compare("all")!=0){
//
//		cout << "Invalid output product! Please select an appropriate output product and re-run the program." << endl;
//		exit(1);
//	}
//}
/*void compare2list_stockdon_2009(string outprod){
	if(outprod.compare("shoreline")!=0 &&
			outprod.compare("dunetoe")!=0 &&
			outprod.compare("dunecrest")!=0 &&
			outprod.compare("duneheel")!=0 &&
			outprod.compare("island")!=0 &&
			outprod.compare("all")!=0){

		cout << "Invalid output product! Please select an appropriate output product and re-run the program." << endl;
		exit(1);
	}
}*/

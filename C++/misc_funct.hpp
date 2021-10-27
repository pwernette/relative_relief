#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <map>
#include <vector>
#include <random>

using namespace std;

//function to compute the relative relief
void Raster::computeRelativeRelief(int i, int j, int buf, Header hdr){
	register int ii, jj, a;
	int index = (i*hdr.ncols)+j;
	float z1 = 0;
	vector<float> tmp;

	// iterate through all pixels in the specified window size
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

		// add the relative relief value to the data structure
		tmp.push_back((Raster::z[index] - z_min)/(z_max - z_min));
		Raster::res[index] = tmp[0];
	}

	// compute the average relative relief for all 3 scales
	Raster::avg[index] = accumulate(tmp.begin(), tmp.end(), 0.0)/tmp.size();
}

/*
 * The purpose of this program is to import a DEM and perform simple statistical
 * and geospatial analysis. The program requires the following inputs:
 * 		1) Input filename (excluding extension)
 *
 * 		2) window size to calculate statistics
 *
 * 		3) desired product:
 * 			rr --> outputs relative relief at 3 scales:
 * 							initial window size
 *							initial window size + 2
 *							initial window size + 4
 *							average relative relief of all 3 scales
 *
 * 			shoreline --> outputs shoreline
 * 			dunetoe --> outputs dune toe
 * 			dunecrest --> outputs dune crest
 * 			duneheel --> outputs dune heel
 * 			backbarrier --> outputs backbarrier
 * 			landforms --> outputs all geomorphic feature parameters
 *
 * 			all --> outputs all products
 *
 * 		4) output data types:
 * 			ascii --> only outputs ascii file
 * 			envi --> only outputs ENVI format raster
 *
 * 			both --> output both ascii and ENVI files
 *
 * 	Example Usage:
 * 		program.exe sample_ENVI_raster_filename 25 all both
 *		program.exe sample_ENVI_raster_filename 11 rr envi
 * 		program.exe sample_ENVI_raster_filename 3 landforms both
 *
 * This program assumes that the grid resolution is relatively small, and therefore
 * a simple pythagorean theorem distance is appropriate when calculating metrics.
 * NOTE: This asusmption is not valid for distances larger than ~12 miles (~19 km),
 * 		 since the curvature of the Earth will affect the 3D distance.
 */

// include the appropriate libraries
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <vector>

// library with data structure objects
#include "data_structures.hpp"

// library with miscellaneous functions
#include "misc_funct.hpp"

using namespace std;

// MAIN PROGRAM
int main (){
	Params prms;
	Header hdr;

	//indexing variables
	register int i, j, k;

	//projection information
	double utm_east, utm_north;
	int utm_zone;

	//load in the parameters for the program
	if (!prms.Initialize()) return false;

	//load in the header information from the input file (pulled from the Params info
	if (!hdr.Initialize(prms.iFile)) return false;

	// based on the specified window size, determine the buffer radius
	int buffer = (prms.iWindowSize-1)/2;

	// Import DEM as Raster object
	Raster data;
	data.Initialize(prms, hdr);

	// Define threshold values
	string shoreline_indicator, default_threshold_values;

	// output ASCII file pointer
	FILE *landforms_metrics;

	////////////////////////////////////////////////////////
	cout << "Processing the input data" << endl;
	
	////////////////////////////////////////////////////////
	if(prms.oProduct.compare("rr")!=0){
		if(prms.oFormat.compare("ascii")==0 || prms.oFormat.compare("both")==0){
			string ascii_outname = prms.iFile.substr(0, prms.iFile.find_last_of("."));
			ascii_outname.append("_ISLAND_METRICS.csv");

			landforms_metrics = fopen(ascii_outname.c_str(), "w+");
			if(!landforms_metrics){
				cout << "ERROR: Cannot write ascii data file: " << ascii_outname << endl;
				exit(1);
			} else{
				cout << "Creating/writing ascii data file: " << ascii_outname << "\n" << endl;
			}

			// write the hdr_info
			(void) fprintf(landforms_metrics, "ycoordinate, ");
			if(prms.oProduct.compare("shoreline")==0){
				(void) fprintf(landforms_metrics, "shorelineX, shorelineZ\n");
			}
			if(prms.oProduct.compare("dunetoe")==0){
				(void) fprintf(landforms_metrics, "dunetoeX, dunetoeZ\n");
			}
			if(prms.oProduct.compare("dunecrest")==0){
				(void) fprintf(landforms_metrics, "dunecrestX, dunecrestZ\n");
			}
			if(prms.oProduct.compare("duneheel")==0){
				(void) fprintf(landforms_metrics, "duneheelX, duneheelZ\n");
			}
			if(prms.oProduct.compare("backbarrier")==0){
				(void) fprintf(landforms_metrics, "backbarrierX, backbarrierZ\n");
			}
			if(prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
				(void) fprintf(landforms_metrics, "shorelineX, shorelineZ, ");
				(void) fprintf(landforms_metrics, "dunetoeX, dunetoeZ, ");
				(void) fprintf(landforms_metrics, "dunecrestX, dunecrestZ, ");
				(void) fprintf(landforms_metrics, "duneheelX, duneheelZ, ");
				(void) fprintf(landforms_metrics, "backbarrierX, backbarrierZ, ");
				(void) fprintf(landforms_metrics, "beach_width, beach_vol, dune_height, dune_vol, island_width, island_volume\n");
			}
		}

		////////////////////////////////////////////
		// Calculate DEM stats (including RR values) for every pixel
		///////////////////////////////////////////
		if(prms.transect_direction.compare("W")==0 || prms.transect_direction.compare("E")==0){
			for(i=0; i<hdr.nlines; ++i){
				// define variables for extraction
				float shorelinex, dunetoex, dunecrestx, duneheelx, backbarrierx;
				double shorelinez, dunetoez, dunecrestz, duneheelz, backbarrierz;

				// variables used to track feature position
				float shoreline_pos = 0;
				float dunetoe_pos = 0;
				float dunecrest_pos = 0;
				float duneheel_pos = 0;
				float backbarrier_pos = 0;

				double beach_vol = 0;
				double dune_vol = 0;
				double island_vol = 0;

				int index1;			// index for analyzing the data
				double ycoord = 0;
				
				// iterate through row to compute relative relief and other metrics given buffer size
				for(j=0; j<hdr.ncols; ++j){
					index1 = (i*hdr.ncols)+j;
					// cout << index1 << endl;

					// IF the center pixel is within the buffer distance to the image edge
					if(i<buffer || i>hdr.nlines-buffer || j<buffer || j>hdr.ncols-buffer){
						data.res[index1] = -9999;
						data.res_plus1[index1] = -9999;
						data.res_plus2[index1] = -9999;
						data.avg[index1] = -9999;

						data.shoreline[index1] = 0;
						data.dune_toe_line[index1] = 0;
						data.dune_ridge_line[index1] = 0;
						data.dune_heel_line[index1] = 0;
						data.backbarrier_line[index1] = 0;
					}

					// IF the center pixel is NOT NULL, then continue...
					else if(data.z[index1] > -100){
						// compute relative relief
						data.computeRelativeRelief(i, j, buffer, hdr);

						// set the landform indices to zero (i.e. edge of raster)
						data.shoreline[index1] = 0;
						data.dune_toe_line[index1] = 0;
						data.dune_ridge_line[index1] = 0;
						data.dune_heel_line[index1] = 0;
						data.backbarrier_line[index1] = 0;
					}

					// IF the center pixel contains a NULL value, then set all the calculated attributes to NULL.
					else{
						data.res[index1] = -9999;
						data.res_plus1[index1] = -9999;
						data.res_plus2[index1] = -9999;
						data.avg[index1] = -9999;

						data.shoreline[index1] = 0;
						data.dune_toe_line[index1] = 0;
						data.dune_ridge_line[index1] = 0;
						data.dune_heel_line[index1] = 0;
						data.backbarrier_line[index1] = 0;
					}
				}
					
				if(prms.transect_direction.compare("E")==0){
					///////////////////////
					// extract SHORELINE
					///////////////////////
					for(j=0; j<hdr.ncols; ++j){		// read RIGHT to LEFT starting at edge of the image
						index1 = (i*hdr.ncols)+j;

						// IF the center pixel is within the limits of the image...
						if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1){
							// search for a location along the transect where criteria is met
							if(data.z[index1+1] < prms.tShoreline
									&& data.z[index1] >= prms.tShoreline){
								data.shoreline[index1] = 1;
								shoreline_pos = j;
								shorelinex = data.x[index1];
								shorelinez = data.z[index1];
								ycoord = data.y[index1];
								break;
							} else{
								////////////////////////////////////////////////////////
								//  This block of code is VERY IMPORTANT because it
								//	limits the data to only those rows where the
								//	shoreline can be extracted. If there is not a
								//	shoreline extracted, then the program will not
								//	attempt to calculate any landform parameters
								////////////////////////////////////////////////////////
								shoreline_pos = 0;
								shorelinex = 0;
								shorelinez = -99999;
							}
						}
					}

					//////////////////////
					// extract DUNE TOE
					//////////////////////
					if(prms.oProduct.compare("dunetoe")==0 || prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(j=shoreline_pos; j<hdr.ncols; ++j){		// read RIGHT to LEFT starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1
									// if pixel is greater than minimum distance back from the shoreline
									&& (shoreline_pos-j)*hdr.xres > prms.tDuneDistMin
									// if pixel is less than maximum distance back from the shoreline
									&& (shoreline_pos-j)*hdr.xres < prms.tDuneDistMax){
								// search for a location along the transect where criteria is met
								if(data.avg[index1+1] < prms.tDT
										&& data.avg[index1] >= prms.tDT){
									data.dune_toe_line[index1] = 1;
									dunetoe_pos = j;
									dunetoex = data.x[index1];
									dunetoez = data.z[index1];
									break;
								} else{
									dunetoe_pos = 0;
									dunetoex = 0;
									dunetoez = -99999;
								}
							}
						}
					}

					////////////////////////
					// extract DUNE CREST
					////////////////////////
					if(prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(j=dunetoe_pos; j<hdr.ncols; ++j){		// read RIGHT to LEFT starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1 && j<dunetoe_pos){
								// search for a location along the transect where criteria is met
								if(data.avg[index1] >= prms.tDC
										&& data.avg[index1-1] < prms.tDC
										&& data.avg[index1-1]!=-9999
										&& data.avg[index1]!=-9999
										&& data.z[index1]!=-9999
										// if pixel is greater than minimum distance back from the dune toe
										&& (dunetoe_pos-j)*hdr.xres > prms.tCrestDistMin
										// if pixel is less than maximum distance back from the dune toe
										&& (dunetoe_pos-j)*hdr.xres < prms.tCrestDistMax
										// If pixel is higher than dune toe
										&& dunetoez<data.z[index1]){
									data.dune_ridge_line[index1] = 1;
									dunecrest_pos = j;
									dunecrestx = data.x[index1];
									dunecrestz = data.z[index1];
									break;
								} else{
									dunecrest_pos = 0;
									dunecrestx = 0;
									dunecrestz = -99999;
								}
							}
						}
					}

					///////////////////////
					// extract DUNE HEEL
					///////////////////////
					if(prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(j=dunecrest_pos; j<hdr.ncols; ++j){		// read RIGHT to LEFT starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1 && j<dunecrest_pos){
								if(data.avg[index1] >= prms.tDH
										&& data.avg[index1-1] < prms.tDH
										&& data.avg[index1-1]!=-9999
										&& data.avg[index1]!=-9999
										&& data.z[index1]!=-9999
										// if pixel is greater than minimum distance back from the dune crest
										&& (dunecrest_pos-j)*hdr.xres > prms.tHeelDistMin
										// if pixel is less than maximum distance back from the dune crest
										&& (dunecrest_pos-j)*hdr.xres < prms.tHeelDistMax){
									data.dune_heel_line[index1] = 1;
									duneheel_pos = j;
									duneheelx = data.x[index1];
									duneheelz = data.z[index1];
									break;
								} else{
									duneheel_pos = 0;
									duneheelx = 0;
									duneheelz = -99999;
								}
							}
						}
					}

					/////////////////////////////
					// extract BACKBARRIER EDGE
					/////////////////////////////
					if(prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						int backstart;
						if(duneheel_pos!=0){
							backstart = duneheel_pos;
						} else if(dunecrest_pos!=0){
							backstart = dunecrest_pos;
						} else if(dunetoe_pos!=0){
							backstart = dunetoe_pos;
						} else{
							backstart = shoreline_pos;
						}

						for(j=backstart; j<hdr.ncols; ++j){		// read RIGHT to LEFT starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1){
								//cout << j << " - " << index1 << " - " << data[index1-1].z << endl;
								if(data.z[index1-1] < prms.tBB
										&& data.z[index1] >= prms.tBB
										&& data.z[index1]!=-9999){
									data.backbarrier_line[index1] = 1;
									backbarrier_pos = j;
									backbarrierx = data.x[index1];
									backbarrierz = data.z[index1];
									break;
								} else{
									backbarrier_pos = 0;
									backbarrierx = 0;
									backbarrierz = -99999;
								}
							}
						}
					}

					//////////////////////
					// calculate VOLUMES
					//////////////////////
					for(j=0; j<hdr.ncols; ++j){		// read RIGHT to LEFT starting at shoreline
						index1 = (i*hdr.ncols)+j;
						int a = 0;
						///////////////////////////
						// calculate BEACH VOLUME
						///////////////////////////
						if(prms.oProduct.compare("dunetoe")==0 || prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							if(j<=shoreline_pos
									&& j>=dunetoe_pos
									&& dunetoe_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								if(a<=(shorelinex-dunetoex)*hdr.xres){
									beach_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
								}
							} else{
								beach_vol += 0;
							}
						}

						///////////////////////////
						// calculate DUNE VOLUME
						///////////////////////////
						if(prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							// IF the center pixel is within the limits of the image...
							if(j<=dunetoe_pos
									&& j>=duneheel_pos
									&& duneheel_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								dune_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
							} else{
								dune_vol += 0;
							}
						}

						///////////////////////////
						// calculate ISLAND VOLUME
						///////////////////////////
						if(prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							// IF the center pixel is within the limits of the image...
							if(j<=shoreline_pos
									&& j>=backbarrier_pos
									&& backbarrierz!=-99999
									&& shoreline_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								island_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
							} else{
								island_vol += 0;
							}
						}
					}
				}
				else if(prms.transect_direction.compare("W")==0){
					///////////////////////
					// extract SHORELINE
					///////////////////////
					for(j=hdr.ncols; j>-1; --j){		// read RIGHT to LEFT starting at edge of the image
						index1 = (i*hdr.ncols)+j;

						// IF the center pixel is within the limits of the image...
						if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1){
							// search for a location along the transect where criteria is met
							if(data.z[index1+1] < prms.tShoreline
									&& data.z[index1] >= prms.tShoreline){
								data.shoreline[index1] = 1;
								shoreline_pos = j;
								shorelinex = data.x[index1];
								shorelinez = data.z[index1];
								ycoord = data.y[index1];
								break;
							} else{
								////////////////////////////////////////////////////////
								//  This block of code is VERY IMPORTANT because it
								//	limits the data to only those rows where the
								//	shoreline can be extracted. If there is not a
								//	shoreline extracted, then the program will not
								//	attempt to calculate any landform parameters
								////////////////////////////////////////////////////////
								shoreline_pos = 0;
								shorelinex = 0;
								shorelinez = -99999;
							}
						}
					}

					//////////////////////
					// extract DUNE TOE
					//////////////////////
					if(prms.oProduct.compare("dunetoe")==0 || prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(j=shoreline_pos; j>-1; --j){		// read RIGHT to LEFT starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1
									// if pixel is greater than minimum distance back from the shoreline
									&& (shoreline_pos-j)*hdr.xres > prms.tDuneDistMin
									// if pixel is less than maximum distance back from the shoreline
									&& (shoreline_pos-j)*hdr.xres < prms.tDuneDistMax){
								// search for a location along the transect where criteria is met
								if(data.avg[index1+1] < prms.tDT
										&& data.avg[index1] >= prms.tDT){
									data.dune_toe_line[index1] = 1;
									dunetoe_pos = j;
									dunetoex = data.x[index1];
									dunetoez = data.z[index1];
									break;
								} else{
									dunetoe_pos = 0;
									dunetoex = 0;
									dunetoez = -99999;
								}
							}
						}
					}

					////////////////////////
					// extract DUNE CREST
					////////////////////////
					if(prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(j=dunetoe_pos; j>-1; --j){		// read RIGHT to LEFT starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1 && j<dunetoe_pos){
								// search for a location along the transect where criteria is met
								if(data.avg[index1] >= prms.tDC
										&& data.avg[index1-1] < prms.tDC
										&& data.avg[index1-1]!=-9999
										&& data.avg[index1]!=-9999
										&& data.z[index1]!=-9999
										// if pixel is greater than minimum distance back from the dune toe
										&& (dunetoe_pos-j)*hdr.xres > prms.tCrestDistMin
										// if pixel is less than maximum distance back from the dune toe
										&& (dunetoe_pos-j)*hdr.xres < prms.tCrestDistMax
										// If pixel is higher than dune toe
										&& dunetoez<data.z[index1]){
									data.dune_ridge_line[index1] = 1;
									dunecrest_pos = j;
									dunecrestx = data.x[index1];
									dunecrestz = data.z[index1];
									break;
								} else{
									dunecrest_pos = 0;
									dunecrestx = 0;
									dunecrestz = -99999;
								}
							}
						}
					}

					///////////////////////
					// extract DUNE HEEL
					///////////////////////
					if(prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(j=dunecrest_pos; j>-1; --j){		// read RIGHT to LEFT starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1 && j<dunecrest_pos){
								if(data.avg[index1] >= prms.tDH
										&& data.avg[index1-1] < prms.tDH
										&& data.avg[index1-1]!=-9999
										&& data.avg[index1]!=-9999
										&& data.z[index1]!=-9999
										// if pixel is greater than minimum distance back from the dune crest
										&& (dunecrest_pos-j)*hdr.xres > prms.tHeelDistMin
										// if pixel is less than maximum distance back from the dune crest
										&& (dunecrest_pos-j)*hdr.xres < prms.tHeelDistMax){
									data.dune_heel_line[index1] = 1;
									duneheel_pos = j;
									duneheelx = data.x[index1];
									duneheelz = data.z[index1];
									break;
								} else{
									duneheel_pos = 0;
									duneheelx = 0;
									duneheelz = -99999;
								}
							}
						}
					}

					/////////////////////////////
					// extract BACKBARRIER EDGE
					/////////////////////////////
					if(prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						int backstart;
						if(duneheel_pos!=0){
							backstart = duneheel_pos;
						} else if(dunecrest_pos!=0){
							backstart = dunecrest_pos;
						} else if(dunetoe_pos!=0){
							backstart = dunetoe_pos;
						} else{
							backstart = shoreline_pos;
						}

						for(j=backstart; j>-1; --j){		// read RIGHT to LEFT starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1){
								//cout << j << " - " << index1 << " - " << data[index1-1].z << endl;
								if(data.z[index1-1] < prms.tBB
										&& data.z[index1] >= prms.tBB
										&& data.z[index1]!=-9999){
									data.backbarrier_line[index1] = 1;
									backbarrier_pos = j;
									backbarrierx = data.x[index1];
									backbarrierz = data.z[index1];
									break;
								} else{
									backbarrier_pos = 0;
									backbarrierx = 0;
									backbarrierz = -99999;
								}
							}
						}
					}

					//////////////////////
					// calculate VOLUMES
					//////////////////////
					for(j=hdr.ncols; j>-1; --j){		// read RIGHT to LEFT starting at shoreline
						index1 = (i*hdr.ncols)+j;
						int a = 0;
						///////////////////////////
						// calculate BEACH VOLUME
						///////////////////////////
						if(prms.oProduct.compare("dunetoe")==0 || prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							if(j<=shoreline_pos
									&& j>=dunetoe_pos
									&& dunetoe_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								if(a<=(shorelinex-dunetoex)*hdr.xres){
									beach_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
								}
							} else{
								beach_vol += 0;
							}
						}

						///////////////////////////
						// calculate DUNE VOLUME
						///////////////////////////
						if(prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							// IF the center pixel is within the limits of the image...
							if(j<=dunetoe_pos
									&& j>=duneheel_pos
									&& duneheel_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								dune_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
							} else{
								dune_vol += 0;
							}
						}

						///////////////////////////
						// calculate ISLAND VOLUME
						///////////////////////////
						if(prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							// IF the center pixel is within the limits of the image...
							if(j<=shoreline_pos
									&& j>=backbarrier_pos
									&& backbarrierz!=-99999
									&& shoreline_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								island_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
							} else{
								island_vol += 0;
							}
						}
					}
				}
				
				/////////////////////////////////////////////////////
				// output ASCII format text files (if requested by user input)
				/////////////////////////////////////////////////////
				if(prms.oFormat.compare("ascii")==0 || prms.oFormat.compare("both")==0){
					// write out the desired products to the ascii file
					if(prms.oProduct.compare("shoreline")==0 && shorelinez>=hdr.zmin && shorelinex>hdr.ulx && shorelinex<=hdr.xmax){
						(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)shorelinex, (float)shorelinez);
					}
					if(prms.oProduct.compare("dunetoe")==0 && dunetoez>=hdr.zmin && dunetoex>hdr.ulx && dunetoex<hdr.xmax){
						(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)dunetoex, (float)dunetoez);
					}
					if(prms.oProduct.compare("dunecrest")==0 && dunecrestz>=hdr.zmin && dunecrestx>hdr.ulx && dunecrestx<hdr.xmax){
						(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)dunecrestx, (float)dunecrestz);
					}
					if(prms.oProduct.compare("duneheel")==0 && duneheelz>=hdr.zmin && duneheelx>hdr.ulx && duneheelx<hdr.xmax){
						(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)duneheelx, (float)duneheelz);
					}
					if(prms.oProduct.compare("backbarrier")==0 && backbarrierz>=hdr.zmin && backbarrierx>=hdr.ulx && backbarrierx<hdr.xmax){
						(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)backbarrierx, (float)backbarrierz);
					}
					if((prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0) && ycoord!=0){
							double dh, bw, iw;

							// check against negative dune elevations
							if((dunecrestz-dunetoez)>0 && (dunecrestz-dunetoez)<300){
								dh = dunecrestz-dunetoez;
							} else{
								dh = -99999;
							}

							if(beach_vol <= 0){
								beach_vol = -99999;
							}
							if(dune_vol <= 0){
								dune_vol = -99999;
							}
							if(island_vol <= 0){
								island_vol = -99999;
							}

							// compute beach width
							if(abs(shorelinex-dunetoex)*hdr.xres>=0){
								bw = abs(shorelinex-dunetoex)*hdr.xres;
							} else{
								bw = -99999;
							}
							
							// compute island width
							if(abs(shorelinex-backbarrierx)*hdr.xres>0){
								iw = abs(shorelinex-backbarrierx)*hdr.xres;
							} else{
								iw = -99999;
							}
							
							// write values to the log file
							(void) fprintf(landforms_metrics, "%lf.10, ", (float)ycoord);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)shorelinex, (float)shorelinez);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)dunetoex, (float)dunetoez);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)dunecrestx, (float)dunecrestz);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)duneheelx, (float)duneheelz);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)backbarrierx, (float)backbarrierz);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10, %lf.10, %lf.10, %lf.10\n",
								(float)bw,
								(float)beach_vol,
								(float)dh,
								(float)dune_vol,
								(float)iw,
								(float)island_vol);
					}
				}
			}
		}
		else if(prms.transect_direction.compare("S")==0 || prms.transect_direction.compare("N")==0){
			for(j=0; j<hdr.ncols; ++j){
				// define variables for extraction
				float shoreliney, dunetoey, dunecresty, duneheely, backbarriery;
				double shorelinez, dunetoez, dunecrestz, duneheelz, backbarrierz;

				// variables used to track feature position
				float shoreline_pos = 0;
				float dunetoe_pos = 0;
				float dunecrest_pos = 0;
				float duneheel_pos = 0;
				float backbarrier_pos = 0;

				double beach_vol = 0;
				double dune_vol = 0;
				double island_vol = 0;

				int index1;			// index for analyzing the data
				double xcoord = 0;
				
				// iterate through row to compute relative relief and other metrics given buffer size
				for(i=0; i<hdr.nlines; ++i){
					index1 = (i*hdr.ncols)+j;
					// cout << index1 << endl;

					// IF the center pixel is within the buffer distance to the image edge
					if(i<buffer || i>hdr.nlines-buffer || j<buffer || j>hdr.ncols-buffer){
						data.res[index1] = -9999;
						data.res_plus1[index1] = -9999;
						data.res_plus2[index1] = -9999;
						data.avg[index1] = -9999;

						data.shoreline[index1] = 0;
						data.dune_toe_line[index1] = 0;
						data.dune_ridge_line[index1] = 0;
						data.dune_heel_line[index1] = 0;
						data.backbarrier_line[index1] = 0;
					}

					// IF the center pixel is NOT NULL, then continue...
					else if(data.z[index1] > -100){
						// compute relative relief
						data.computeRelativeRelief(i, j, buffer, hdr);

						// set the landform indices to zero (i.e. edge of raster)
						data.shoreline[index1] = 0;
						data.dune_toe_line[index1] = 0;
						data.dune_ridge_line[index1] = 0;
						data.dune_heel_line[index1] = 0;
						data.backbarrier_line[index1] = 0;
					}

					// IF the center pixel contains a NULL value, then set all the calculated attributes to NULL.
					else{
						data.res[index1] = -9999;
						data.res_plus1[index1] = -9999;
						data.res_plus2[index1] = -9999;
						data.avg[index1] = -9999;

						data.shoreline[index1] = 0;
						data.dune_toe_line[index1] = 0;
						data.dune_ridge_line[index1] = 0;
						data.dune_heel_line[index1] = 0;
						data.backbarrier_line[index1] = 0;
					}
				}
					
				if(prms.transect_direction.compare("N")==0){
					///////////////////////
					// extract SHORELINE
					///////////////////////
					for(i=0; i<hdr.nlines; ++i){		// read BOTTOM to TOP starting at edge of the image
						index1 = (i*hdr.ncols)+j;

						// IF the center pixel is within the limits of the image...
						if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1){
							// search for a location along the transect where criteria is met
							if(data.z[index1+1] < prms.tShoreline
									&& data.z[index1] >= prms.tShoreline){
								data.shoreline[index1] = 1;
								shoreline_pos = i;
								shoreliney = data.x[index1];
								shorelinez = data.z[index1];
								xcoord = data.y[index1];
								break;
							} else{
								////////////////////////////////////////////////////////
								//  This block of code is VERY IMPORTANT because it
								//	limits the data to only those rows where the
								//	shoreline can be extracted. If there is not a
								//	shoreline extracted, then the program will not
								//	attempt to calculate any landform parameters
								////////////////////////////////////////////////////////
								shoreline_pos = 0;
								shoreliney = 0;
								shorelinez = -99999;
							}
						}
					}

					//////////////////////
					// extract DUNE TOE
					//////////////////////
					if(prms.oProduct.compare("dunetoe")==0 || prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(i=shoreline_pos; i<hdr.nlines; ++i){		// read BOTTOM to TOP starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1
									// if pixel is greater than minimum distance back from the shoreline
									&& abs(shoreline_pos-i)*hdr.yres > prms.tDuneDistMin
									// if pixel is less than maximum distance back from the shoreline
									&& abs(shoreline_pos-i)*hdr.yres < prms.tDuneDistMax){
								// search for a location along the transect where criteria is met
								if(data.avg[index1+1] < prms.tDT
										&& data.avg[index1] >= prms.tDT){
									data.dune_toe_line[index1] = 1;
									dunetoe_pos = i;
									dunetoey = data.y[index1];
									dunetoez = data.z[index1];
									break;
								} else{
									dunetoe_pos = 0;
									dunetoey = 0;
									dunetoez = -99999;
								}
							}
						}
					}

					////////////////////////
					// extract DUNE CREST
					////////////////////////
					if(prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(i=dunetoe_pos; i<hdr.nlines; ++i){		// read BOTTOM to TOP starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1 && i>dunetoe_pos){
								// search for a location along the transect where criteria is met
								if(data.avg[index1] >= prms.tDC
										&& data.avg[index1-1] < prms.tDC
										&& data.avg[index1-1]!=-9999
										&& data.avg[index1]!=-9999
										&& data.z[index1]!=-9999
										// if pixel is greater than minimum distance back from the dune toe
										&& abs(dunetoe_pos-i)*hdr.yres > prms.tCrestDistMin
										// if pixel is less than maximum distance back from the dune toe
										&& abs(dunetoe_pos-i)*hdr.yres < prms.tCrestDistMax
										// If pixel is higher than dune toe
										&& dunetoez<data.z[index1]){
									data.dune_ridge_line[index1] = 1;
									dunecrest_pos = i;
									dunecresty = data.y[index1];
									dunecrestz = data.z[index1];
									break;
								} else{
									dunecrest_pos = 0;
									dunecresty = 0;
									dunecrestz = -99999;
								}
							}
						}
					}

					///////////////////////
					// extract DUNE HEEL
					///////////////////////
					if(prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(i=dunecrest_pos; i<hdr.nlines; ++i){		// read BOTTOM to TOP starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1 && i>dunecrest_pos){
								if(data.avg[index1] >= prms.tDH
										&& data.avg[index1-1] < prms.tDH
										&& data.avg[index1-1]!=-9999
										&& data.avg[index1]!=-9999
										&& data.z[index1]!=-9999
										// if pixel is greater than minimum distance back from the dune crest
										&& abs(dunecrest_pos-i)*hdr.yres > prms.tHeelDistMin
										// if pixel is less than maximum distance back from the dune crest
										&& abs(dunecrest_pos-i)*hdr.yres < prms.tHeelDistMax){
									data.dune_heel_line[index1] = 1;
									duneheel_pos = i;
									duneheely = data.y[index1];
									duneheelz = data.z[index1];
									break;
								} else{
									duneheel_pos = 0;
									duneheely = 0;
									duneheelz = -99999;
								}
							}
						}
					}

					/////////////////////////////
					// extract BACKBARRIER EDGE
					/////////////////////////////
					if(prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						int backstart;
						if(duneheel_pos!=0){
							backstart = duneheel_pos;
						} else if(dunecrest_pos!=0){
							backstart = dunecrest_pos;
						} else if(dunetoe_pos!=0){
							backstart = dunetoe_pos;
						} else{
							backstart = shoreline_pos;
						}

						for(i=backstart; i<hdr.nlines; ++i){		// read BOTTOM to TOP starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1){
								//cout << j << " - " << index1 << " - " << data[index1-1].z << endl;
								if(data.z[index1-1] < prms.tBB
										&& data.z[index1] >= prms.tBB
										&& data.z[index1]!=-9999){
									data.backbarrier_line[index1] = 1;
									backbarrier_pos = i;
									backbarriery = data.y[index1];
									backbarrierz = data.z[index1];
									break;
								} else{
									backbarrier_pos = 0;
									backbarriery = 0;
									backbarrierz = -99999;
								}
							}
						}
					}
					
					//////////////////////
					// calculate VOLUMES
					//////////////////////
					for(i=0; i<hdr.nlines; ++i){
						index1 = (i*hdr.ncols)+j;
						int a = 0;
						///////////////////////////
						// calculate BEACH VOLUME
						///////////////////////////
						if(prms.oProduct.compare("dunetoe")==0 || prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							if(i>=shoreline_pos
									&& i<=dunetoe_pos
									&& dunetoe_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								if(a<=(shoreliney-dunetoey)*hdr.xres){
									beach_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
								}
							} else{
								beach_vol += 0;
							}
						}

						///////////////////////////
						// calculate DUNE VOLUME
						///////////////////////////
						if(prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							// IF the center pixel is within the limits of the image...
							if(i>=dunetoe_pos
									&& i<=duneheel_pos
									&& duneheel_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								dune_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
							} else{
								dune_vol += 0;
							}
						}

						///////////////////////////
						// calculate ISLAND VOLUME
						///////////////////////////
						if(prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							// IF the center pixel is within the limits of the image...
							if(i>=shoreline_pos
									&& i<=backbarrier_pos
									&& backbarrierz!=-99999
									&& shoreline_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								island_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
							} else{
								island_vol += 0;
							}
						}
					}
				}
				else if(prms.transect_direction.compare("S")==0){
					///////////////////////
					// extract SHORELINE
					///////////////////////
					for(i=hdr.nlines; i>-1; --i){		// read TOP to BOTTOM starting at edge of the image
						index1 = (i*hdr.ncols)+j;

						// IF the center pixel is within the limits of the image...
						if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1){
							// search for a location along the transect where criteria is met
							if(data.z[index1+1] < prms.tShoreline
									&& data.z[index1] >= prms.tShoreline){
								data.shoreline[index1] = 1;
								shoreline_pos = i;
								shoreliney = data.y[index1];
								shorelinez = data.z[index1];
								xcoord = data.x[index1];
								break;
							} else{
								////////////////////////////////////////////////////////
								//  This block of code is VERY IMPORTANT because it
								//	limits the data to only those rows where the
								//	shoreline can be extracted. If there is not a
								//	shoreline extracted, then the program will not
								//	attempt to calculate any landform parameters
								////////////////////////////////////////////////////////
								shoreline_pos = 0;
								shoreliney = 0;
								shorelinez = -99999;
							}
						}
					}

					//////////////////////
					// extract DUNE TOE
					//////////////////////
					if(prms.oProduct.compare("dunetoe")==0 || prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(i=shoreline_pos; i>-1; --i){		// read TOP to BOTTOM starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1
									// if pixel is greater than minimum distance back from the shoreline
									&& abs(shoreline_pos-i)*hdr.yres > prms.tDuneDistMin
									// if pixel is less than maximum distance back from the shoreline
									&& abs(shoreline_pos-i)*hdr.yres < prms.tDuneDistMax){
								// search for a location along the transect where criteria is met
								if(data.avg[index1+1] < prms.tDT
										&& data.avg[index1] >= prms.tDT){
									data.dune_toe_line[index1] = 1;
									dunetoe_pos = i;
									dunetoey = data.y[index1];
									dunetoez = data.z[index1];
									break;
								} else{
									dunetoe_pos = 0;
									dunetoey = 0;
									dunetoez = -99999;
								}
							}
						}
					}

					////////////////////////
					// extract DUNE CREST
					////////////////////////
					if(prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(i=dunetoe_pos; i>-1; --i){		// read TOP to BOTTOM starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1 && i>dunetoe_pos){
								// search for a location along the transect where criteria is met
								if(data.avg[index1] >= prms.tDC
										&& data.avg[index1-1] < prms.tDC
										&& data.avg[index1-1]!=-9999
										&& data.avg[index1]!=-9999
										&& data.z[index1]!=-9999
										// if pixel is greater than minimum distance back from the dune toe
										&& abs(dunetoe_pos-i)*hdr.yres > prms.tCrestDistMin
										// if pixel is less than maximum distance back from the dune toe
										&& abs(dunetoe_pos-i)*hdr.yres < prms.tCrestDistMax
										// If pixel is higher than dune toe
										&& dunetoez<data.z[index1]){
									data.dune_ridge_line[index1] = 1;
									dunecrest_pos = i;
									dunecresty = data.y[index1];
									dunecrestz = data.z[index1];
									break;
								} else{
									dunecrest_pos = 0;
									dunecresty = 0;
									dunecrestz = -99999;
								}
							}
						}
					}

					///////////////////////
					// extract DUNE HEEL
					///////////////////////
					if(prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						for(i=dunecrest_pos; i>-1; --i){		// read TOP to BOTTOM starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1 && i>dunecrest_pos){
								if(data.avg[index1] >= prms.tDH
										&& data.avg[index1-1] < prms.tDH
										&& data.avg[index1-1]!=-9999
										&& data.avg[index1]!=-9999
										&& data.z[index1]!=-9999
										// if pixel is greater than minimum distance back from the dune crest
										&& abs(dunecrest_pos-i)*hdr.yres > prms.tHeelDistMin
										// if pixel is less than maximum distance back from the dune crest
										&& abs(dunecrest_pos-i)*hdr.yres < prms.tHeelDistMax){
									data.dune_heel_line[index1] = 1;
									duneheel_pos = i;
									duneheely = data.y[index1];
									duneheelz = data.z[index1];
									break;
								} else{
									duneheel_pos = 0;
									duneheely = 0;
									duneheelz = -99999;
								}
							}
						}
					}

					/////////////////////////////
					// extract BACKBARRIER EDGE
					/////////////////////////////
					if(prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						int backstart;
						if(duneheel_pos!=0){
							backstart = duneheel_pos;
						} else if(dunecrest_pos!=0){
							backstart = dunecrest_pos;
						} else if(dunetoe_pos!=0){
							backstart = dunetoe_pos;
						} else{
							backstart = shoreline_pos;
						}

						for(i=backstart; i>-1; --i){		// read TOP to BOTTOM starting at shoreline
							index1 = (i*hdr.ncols)+j;

							// IF the center pixel is within the limits of the image...
							if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1){
								//cout << j << " - " << index1 << " - " << data[index1-1].z << endl;
								if(data.z[index1-1] < prms.tBB
										&& data.z[index1] >= prms.tBB
										&& data.z[index1]!=-9999){
									data.backbarrier_line[index1] = 1;
									backbarrier_pos = i;
									backbarriery = data.y[index1];
									backbarrierz = data.z[index1];
									break;
								} else{
									backbarrier_pos = 0;
									backbarriery = 0;
									backbarrierz = -99999;
								}
							}
						}
					}
					//////////////////////
					// calculate VOLUMES
					//////////////////////
					for(i=0; i<hdr.nlines; ++i){
						index1 = (i*hdr.ncols)+j;
						int a = 0;
						///////////////////////////
						// calculate BEACH VOLUME
						///////////////////////////
						if(prms.oProduct.compare("dunetoe")==0 || prms.oProduct.compare("dunecrest")==0 || prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							if(i<=shoreline_pos
									&& i>=dunetoe_pos
									&& dunetoe_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								if(a<=(shoreliney-dunetoey)*hdr.yres){
									beach_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
								}
							} else{
								beach_vol += 0;
							}
						}

						///////////////////////////
						// calculate DUNE VOLUME
						///////////////////////////
						if(prms.oProduct.compare("duneheel")==0 || prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
							// IF the center pixel is within the limits of the image...
							if(i<=dunetoe_pos
									&& i>=duneheel_pos
									&& duneheel_pos!=0
									&& data.z[index1]>=prms.tShoreline){
								dune_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
							} else{
								dune_vol += 0;
							}
						}

						///////////////////////////
						// calculate ISLAND VOLUME
						///////////////////////////
						if(prms.oProduct.compare("backbarrier")==0 || prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
						// IF the center pixel is within the limits of the image...
						if(i<=shoreline_pos
								&& i>=backbarrier_pos
								&& backbarrierz!=-99999
								&& shoreline_pos!=0
								&& data.z[index1]>=prms.tShoreline){
							island_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres;
						} else{
							island_vol += 0;
						}
					}
				}

				/////////////////////////////////////////////////////
				// output ASCII format text files (if requested by user input)
				/////////////////////////////////////////////////////
				if(prms.oFormat.compare("ascii")==0 || prms.oFormat.compare("both")==0){
					// write out the desired products to the ascii file
					if(prms.oProduct.compare("shoreline")==0 && shorelinez>=hdr.zmin && shoreliney<hdr.uly && shoreliney>hdr.ymin){
						(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)shoreliney, (float)shorelinez);
					}
					if(prms.oProduct.compare("dunetoe")==0 && dunetoez>=hdr.zmin && dunetoey<hdr.uly && dunetoey>hdr.ymin){
						(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)dunetoey, (float)dunetoez);
					}
					if(prms.oProduct.compare("dunecrest")==0 && dunecrestz>=hdr.zmin && dunecresty<hdr.uly && dunecresty>hdr.ymin){
						(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)dunecresty, (float)dunecrestz);
					}
					if(prms.oProduct.compare("duneheel")==0 && duneheelz>=hdr.zmin && duneheely<hdr.uly && duneheely>hdr.ymin){
						(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)duneheely, (float)duneheelz);
					}
					if(prms.oProduct.compare("backbarrier")==0 && backbarrierz>=hdr.zmin && backbarriery<hdr.uly && backbarriery>hdr.ymin){
						(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)backbarriery, (float)backbarrierz);
					}
					if((prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0) && xcoord!=0){
							double dh, bw, iw;

							// check against negative dune elevations
							if((dunecrestz-dunetoez)>0 && (dunecrestz-dunetoez)<500){
								dh = dunecrestz-dunetoez;
							} else{
								dh = -99999;
							}

							if(beach_vol <= 0){
								beach_vol = -99999;
							}
							if(dune_vol <= 0){
								dune_vol = -99999;
							}
							if(island_vol <= 0){
								island_vol = -99999;
							}

							// compute beach width
							if(abs(shoreliney-dunetoey)*hdr.yres>=0){
								bw = abs(shoreliney-dunetoey)*hdr.yres;
							} else{
								bw = -99999;
							}
							
							// compute island width
							if(abs(shoreliney-backbarriery)*hdr.yres>0){
								iw = abs(shoreliney-backbarriery)*hdr.yres;
							} else{
								iw = -99999;
							}
							
							// write values to the log file
							(void) fprintf(landforms_metrics, "%lf.10, ", (float)xcoord);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)shoreliney, (float)shorelinez);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)dunetoey, (float)dunetoez);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)dunecresty, (float)dunecrestz);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)duneheely, (float)duneheelz);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)backbarriery, (float)backbarrierz);
							(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10, %lf.10, %lf.10, %lf.10\n",
								(float)bw,
								(float)beach_vol,
								(float)dh,
								(float)dune_vol,
								(float)iw,
								(float)island_vol);
						}
					}
				}
			}
		}
	}
	// if output Products is specified as "rr" only, then no need to compute all landform metrics
	// the following block only computes the relative relief values for 3 scales + an average value
	else if(prms.oProduct.compare("rr")==0){
		////////////////////////////////////////////
		// Calculate Relative Relief
		///////////////////////////////////////////
		// iterate through all columns from top to bottom
		for(i=0; i<hdr.nlines; ++i){
			int index1;			// index for analyzing the data
			double ycoord = 0;

			// iterate through all rows from left to right
			for(j=0; j<hdr.ncols; ++j){
				index1 = (i*hdr.ncols)+j;

				// IF the center pixel is within the buffer distance to the image edge
				if(i<buffer || i>hdr.nlines-buffer || j<buffer || j>hdr.ncols-buffer){
					data.res[index1] = -9999;
					data.res_plus1[index1] = -9999;
					data.res_plus2[index1] = -9999;
					data.avg[index1] = -9999;
				}

				// IF the center pixel is NOT NULL, then continue...
				else if(data.z[index1] > -100){
					// compute relative relief
					data.computeRelativeRelief(i, j, buffer, hdr);
				}

				// IF the center pixel contains a NULL value, then set all the calculated attributes to NULL.
				else{
					data.res[index1] = -9999;
					data.res_plus1[index1] = -9999;
					data.res_plus2[index1] = -9999;
					data.avg[index1] = -9999;
				}
			}
		}
	}

	cout << "   Processing successful!\n" << endl;

	if(prms.oFormat.compare("ascii")==0 || prms.oFormat.compare("both")==0){
		// close the output ascii file
		fclose(landforms_metrics);
		cout << "Successfully wrote landform metrics to CSV file." << endl;
	}

	/////////////////////////////////////////////////////
	// output ENVI format rasters (if requested by user input)
	/////////////////////////////////////////////////////
	if(prms.oFormat.compare("envi")==0 || prms.oFormat.compare("both")==0 || prms.oProduct.compare("rr")==0){
		cout << "Writing out ENVI format products." << endl;

		//write out the ENVI products specified
		data.writeENVIs(prms.iFile, hdr, prms);
	}
	return 0;
}

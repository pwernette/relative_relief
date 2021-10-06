'''
  The purpose of this program is to import a DEM and perform simple statistical and
  geospatial analysis. The program requires the following inputs:
  		1) Input filename (excluding extension)

  		2) window size to calculate statistics

  		3) desired product:
  			dem --> outputs the raw DEM (can only be used with "envi" and "both" output data types)

  			rr --> outputs relative relief 1 (window size + 0)
  			rr1 --> outputs relative relief 2 (window size + 1)
  			rr2 --> outputs relative relief 3 (window size + 2)
  			avgrr --> outputs average relative relief
 			rrs --> outputs all relative relief products (including averag rr)

  			shoreline --> outputs shoreline
  			dunetoe --> outputs dune toe
  			dunecrest --> outputs dune crest
  			duneheel --> outputs dune heel
  			backbarrier --> outputs backbarrier
  			landforms --> outputs all geomorphic feature parameters

  			all --> outputs all products

  		4) output data types:
  			ascii --> only outputs ascii file
  			envi --> only outputs ENVI format raster

  			both --> output both ascii and ENVI files

  	Example Usage:
  		program sample.txt 3 rel both
  					OR
  		program.exe npilidar.txt 13 all both

  This program assumes that the grid resolution is relatively small, and therefore
  a simple pythagorean theorem distance is appropriate when calculating metrics.
  NOTE: This asusmption is not valid for distances larger than ~12 miles (~19 km),
  		 since the curvature of the Earth will affect the 3D distance.
'''


# include the appropriate libraries
import os
import math

# library with data structure objects
from "data_structures.py" import *

# library with miscellaneous functions
from "misc_funct.py" import *

# MAIN PROGRAM
# execute main() if script call
if __name__ == '__main__':
	Params prms
	Header hdr

    # read in raster using rasterio
    ifile = rio.open(Fname)

	#indexing variables
	register int i, j, k

	#projection information
	double utm_east, utm_north
	int utm_zone

	#load in the parameters for the program
	if (!prms.Initialize()) return false

	#load in the header information from the input file (pulled from the Params info
	if (!hdr.Initialize(prms.iFile)) return false

	# based on the specified window size, determine the buffer radius
	int buffer = (prms.iWindowSize-1)/2

	# Import DEM as Raster object
	Raster data
	data.Load(prms, hdr)

	# Define threshold values
	string shoreline_indicator, default_threshold_values

	# output ASCII file pointer
	FILE *landforms_metrics

	############################
	if(prms.oFormat.compare("ascii")==0 || prms.oFormat.compare("both")==0){
		string ascii_outname = prms.iFile.substr(0, prms.iFile.find_last_of("."))
		ascii_outname.append("_ISLAND_METRICS.csv")

		landforms_metrics = fopen(ascii_outname.c_str(), "w+")
		if(!landforms_metrics){
			cout << "ERROR: Cannot write ascii data file: " << ascii_outname << endl
			exit(1)
		} else{
			cout << "Creating/writing ascii data file: " << ascii_outname << "\n" << endl
		}

		# write the hdr_info
		(void) fprintf(landforms_metrics, "ycoordinate, ")
		if(prms.oProduct.compare("shoreline")==0){
			(void) fprintf(landforms_metrics, "shorelineX, shorelineZ\n")
		}
		if(prms.oProduct.compare("dunetoe")==0){
			(void) fprintf(landforms_metrics, "dunetoeX, dunetoeZ\n")
		}
		if(prms.oProduct.compare("dunecrest")==0){
			(void) fprintf(landforms_metrics, "dunecrestX, dunecrestZ\n")
		}
		if(prms.oProduct.compare("duneheel")==0){
			(void) fprintf(landforms_metrics, "duneheelX, duneheelZ\n")
		}
		if(prms.oProduct.compare("backbarrier")==0){
			(void) fprintf(landforms_metrics, "backbarrierX, backbarrierZ\n")
		}
		if(prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0){
			(void) fprintf(landforms_metrics, "shorelineX, shorelineZ, ")
			(void) fprintf(landforms_metrics, "dunetoeX, dunetoeZ, ")
			(void) fprintf(landforms_metrics, "dunecrestX, dunecrestZ, ")
			(void) fprintf(landforms_metrics, "duneheelX, duneheelZ, ")
			(void) fprintf(landforms_metrics, "backbarrierX, backbarrierZ, ")
			(void) fprintf(landforms_metrics, "beach_width, beach_vol, dune_height, dune_vol, island_width, island_volume\n")
		}
	}
	############################
	cout << "Processing the input data" << endl

	######################
	# Calculate DEM STATISTICS
	#####################/
	for(i=0 i<hdr.nlines ++i){
		# define variables for extraction
		float shorelinex, dunetoex, dunecrestx, duneheelx, backbarrierx
		double shorelinez, dunetoez, dunecrestz, duneheelz, backbarrierz

		# variables used to track feature position
		float shoreline_pos = 0
		float dunetoe_pos = 0
		float dunecrest_pos = 0
		float duneheel_pos = 0
		float backbarrier_pos = 0

		double beach_vol = 0
		double dune_vol = 0
		double island_vol = 0

		int index1			# indicies for analyzing the data
		double ycoord = 0

		#cout << i << " - " << hdr.nlines << endl

		# Read through the row from Left to Right...
		for(j=0 j<hdr.ncols ++j){
			index1 = (i*hdr.ncols)+j
			# cout << index1 << endl

			# IF the center pixel is within the buffer distance to the image edge
			if(i<buffer || i>hdr.nlines-buffer || j<buffer || j>hdr.ncols-buffer){
				data.res[index1] = -9999
				data.res_plus1[index1] = -9999
				data.res_plus2[index1] = -9999
				data.avg[index1] = -9999

				data.shoreline[index1] = 0
				data.dune_toe_line[index1] = 0
				data.dune_ridge_line[index1] = 0
				data.dune_heel_line[index1] = 0
				data.backbarrier_line[index1] = 0
			}

			# IF the center pixel is NOT NULL, then continue...
			else if(data.z[index1] > -100){
				# compute relative relief
				data.computeRelativeRelief(i, j, buffer, hdr)

				# set the landform indices to zero (i.e. edge of raster)
				data.shoreline[index1] = 0
				data.dune_toe_line[index1] = 0
				data.dune_ridge_line[index1] = 0
				data.dune_heel_line[index1] = 0
				data.backbarrier_line[index1] = 0
			}

			# IF the center pixel contains a NULL value, then set all the calculated attributes to NULL.
			else{
				data.res[index1] = -9999
				data.res_plus1[index1] = -9999
				data.res_plus2[index1] = -9999
				data.avg[index1] = -9999

				data.shoreline[index1] = 0
				data.dune_toe_line[index1] = 0
				data.dune_ridge_line[index1] = 0
				data.dune_heel_line[index1] = 0
				data.backbarrier_line[index1] = 0
			}
		}


		###########/
		# extract SHORELINE
		###########/
		for(j=hdr.ncols j>-1 --j){		# read RIGHT to LEFT starting at edge of the image
			index1 = (i*hdr.ncols)+j

			# IF the center pixel is within the limits of the image...
			if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1){
				# search for a location along the transect where criteria is met
				if(data.z[index1+1] < prms.tShoreline
						&& data.z[index1] >= prms.tShoreline){
					data.shoreline[index1] = 1
					shoreline_pos = j
					shorelinex = data.x[index1]
					shorelinez = data.z[index1]
					ycoord = data.y[index1]
					break
				} else{
					############################
					#  This block of code is VERY IMPORTANT because it
					#	limits the data to only those rows where the
					#	shoreline can be extracted. If there is not a
					#	shoreline extracted, then the program will not
					#	attempt to calculate any landform parameters
					############################
					shoreline_pos = 0
					shorelinex = 0
					shorelinez = -99999
				}
			}
		}

		###########
		# extract DUNE TOE
		###########
		for(j=shoreline_pos j>-1 --j){		# read RIGHT to LEFT starting at shoreline
			index1 = (i*hdr.ncols)+j

			# IF the center pixel is within the limits of the image...
			if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1
					# if pixel is greater than minimum distance back from the shoreline
					&& (shoreline_pos-j)*hdr.xres > prms.tDuneDistMin
					# if pixel is less than maximum distance back from the shoreline
					&& (shoreline_pos-j)*hdr.xres < prms.tDuneDistMax){
				# search for a location along the transect where criteria is met
				if(data.avg[index1+1] < prms.tDT
						&& data.avg[index1] >= prms.tDT){
					data.dune_toe_line[index1] = 1
					dunetoe_pos = j
					dunetoex = data.x[index1]
					dunetoez = data.z[index1]
					break
				} else{
					dunetoe_pos = 0
					dunetoex = 0
					dunetoez = -99999
				}
			}
		}

		############
		# extract DUNE CREST
		############
		for(j=dunetoe_pos j>-1 --j){		# read RIGHT to LEFT starting at shoreline
			index1 = (i*hdr.ncols)+j

			# IF the center pixel is within the limits of the image...
			if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1 && j<dunetoe_pos){
				# search for a location along the transect where criteria is met
				if(data.avg[index1] >= prms.tDC
						&& data.avg[index1-1] < prms.tDC
						&& data.avg[index1-1]!=-9999
						&& data.avg[index1]!=-9999
						&& data.z[index1]!=-9999
						# if pixel is greater than minimum distance back from the dune toe
						&& (dunetoe_pos-j)*hdr.xres > prms.tCrestDistMin
						# if pixel is less than maximum distance back from the dune toe
						&& (dunetoe_pos-j)*hdr.xres < prms.tCrestDistMax
						# If pixel is higher than dune toe
						&& dunetoez<data.z[index1]){
					data.dune_ridge_line[index1] = 1
					dunecrest_pos = j
					dunecrestx = data.x[index1]
					dunecrestz = data.z[index1]
					break
				} else{
					dunecrest_pos = 0
					dunecrestx = 0
					dunecrestz = -99999
				}
			}
		}

		###########/
		# extract DUNE HEEL
		###########/
		for(j=dunecrest_pos j>-1 --j){		# read RIGHT to LEFT starting at shoreline
			index1 = (i*hdr.ncols)+j

			# IF the center pixel is within the limits of the image...
			if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1 && j<dunecrest_pos){
				if(data.avg[index1] >= prms.tDH
						&& data.avg[index1-1] < prms.tDH
						&& data.avg[index1-1]!=-9999
						&& data.avg[index1]!=-9999
						&& data.z[index1]!=-9999
						# if pixel is greater than minimum distance back from the dune crest
						&& (dunecrest_pos-j)*hdr.xres > prms.tHeelDistMin
						# if pixel is less than maximum distance back from the dune crest
						&& (dunecrest_pos-j)*hdr.xres < prms.tHeelDistMax){
					data.dune_heel_line[index1] = 1
					duneheel_pos = j
					duneheelx = data.x[index1]
					duneheelz = data.z[index1]
					break
				} else{
					duneheel_pos = 0
					duneheelx = 0
					duneheelz = -99999
				}
			}
		}

		##############/
		# extract BACKBARRIER EDGE
		##############/
		int backstart
		if(duneheel_pos!=0){
			backstart = duneheel_pos
		} else if(dunecrest_pos!=0){
			backstart = dunecrest_pos
		} else if(dunetoe_pos!=0){
			backstart = dunetoe_pos
		} else{
			backstart = shoreline_pos
		}

		for(j=backstart j>-1 --j){		# read RIGHT to LEFT starting at shoreline
			index1 = (i*hdr.ncols)+j

			# IF the center pixel is within the limits of the image...
			if(i>buffer && i<hdr.nlines-buffer && j>buffer+1 && j<hdr.ncols-buffer-1){
				#cout << j << " - " << index1 << " - " << data[index1-1].z << endl
				if(data.z[index1-1] < prms.tBB
						&& data.z[index1] >= prms.tBB
						&& data.z[index1]!=-9999){
					data.backbarrier_line[index1] = 1
					backbarrier_pos = j
					backbarrierx = data.x[index1]
					backbarrierz = data.z[index1]
					break
				} else{
					backbarrier_pos = 0
					backbarrierx = 0
					backbarrierz = -99999
				}
			}
		}

		###########
		# calculate VOLUMES
		###########
		for(j=hdr.ncols j>-1 --j){		# read RIGHT to LEFT starting at shoreline
			index1 = (i*hdr.ncols)+j
			int a = 0
			#############/
			# calculate BEACH VOLUME
			#############/
			if(j<=shoreline_pos
					&& j>=dunetoe_pos
					&& dunetoe_pos!=0
					&& data.z[index1]>=prms.tShoreline){
				if(a<=(shorelinex-dunetoex)*hdr.xres){
					beach_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres
				}
			} else{
				beach_vol += 0
			}

			#############/
			# calculate DUNE VOLUME
			#############/
			# IF the center pixel is within the limits of the image...
			if(j<=dunetoe_pos
					&& j>=duneheel_pos
					&& duneheel_pos!=0
					&& data.z[index1]>=prms.tShoreline){
				dune_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres
			} else{
				dune_vol += 0
			}

			#############/
			# calculate ISLAND VOLUME
			#############/
			# IF the center pixel is within the limits of the image...
			if(j<=shoreline_pos
					&& j>=backbarrier_pos
					&& backbarrierz!=-99999
					&& shoreline_pos!=0
					&& data.z[index1]>=prms.tShoreline){
				island_vol += (data.z[index1]-prms.tShoreline)*hdr.xres*hdr.yres
			} else{
				island_vol += 0
			}
		}

		##########################/
		# output ASCII format text files (if requested by user input)
		##########################/
		if(prms.oFormat.compare("ascii")==0 || prms.oFormat.compare("both")==0){
			# write out the desired products to the ascii file
			if(prms.oProduct.compare("shoreline")==0 && shorelinez>=hdr.zmin && shorelinex>hdr.ulx && shorelinex<=hdr.xmax){
				(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)shorelinex, (float)shorelinez)
			}
			if(prms.oProduct.compare("dunetoe")==0 && dunetoez>=hdr.zmin && dunetoex>hdr.ulx && dunetoex<hdr.xmax){
				(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)dunetoex, (float)dunetoez)
			}
			if(prms.oProduct.compare("dunecrest")==0 && dunecrestz>=hdr.zmin && dunecrestx>hdr.ulx && dunecrestx<hdr.xmax){
				(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)dunecrestx, (float)dunecrestz)
			}
			if(prms.oProduct.compare("duneheel")==0 && duneheelz>=hdr.zmin && duneheelx>hdr.ulx && duneheelx<hdr.xmax){
				(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)duneheelx, (float)duneheelz)
			}
			if(prms.oProduct.compare("backbarrier")==0 && backbarrierz>=hdr.zmin && backbarrierx>=hdr.ulx && backbarrierx<hdr.xmax){
				(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10\n", (i*hdr.yres)+hdr.ulx, (float)backbarrierx, (float)backbarrierz)
			}
			if((prms.oProduct.compare("landforms")==0 || prms.oProduct.compare("all")==0) && ycoord!=0){
					double dh, bw, iw

					# check against negative dune elevations
					if((dunecrestz-dunetoez)>0 && (dunecrestz-dunetoez)<300){
						dh = dunecrestz-dunetoez
					} else{
						dh = -99999
					}

					if(beach_vol <= 0){
						beach_vol = -99999
					}
					if(dune_vol <= 0){
						dune_vol = -99999
					}
					if(island_vol <= 0){
						island_vol = -99999
					}

					if((shorelinex-dunetoex)*hdr.xres>=0){
						bw = (shorelinex-dunetoex)*hdr.xres
					} else{
						bw = -99999
					}

					if((shorelinex-backbarrierx)*hdr.xres>0){
						iw = (shorelinex-backbarrierx)*hdr.xres
					} else{
						iw = -99999
					}

					(void) fprintf(landforms_metrics, "%lf.10, ", (float)ycoord)
					(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)shorelinex, (float)shorelinez)
					(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)dunetoex, (float)dunetoez)
					(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)dunecrestx, (float)dunecrestz)
					(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)duneheelx, (float)duneheelz)
					(void) fprintf(landforms_metrics, "%lf.10, %lf.10, ", (float)backbarrierx, (float)backbarrierz)
					(void) fprintf(landforms_metrics, "%lf.10, %lf.10, %lf.10, %lf.10, %lf.10, %lf.10\n",
						(float)bw,
						(float)beach_vol,
						(float)dh,
						(float)dune_vol,
						(float)iw,
						(float)island_vol)
			}
		}
	}

	cout << "   Processing successful!\n" << endl

	if(prms.oFormat.compare("ascii")==0 || prms.oFormat.compare("both")==0){
		# close the output ascii file
		fclose(landforms_metrics)
		cout << "Successfully wrote landform metrics to CSV file." << endl
	}

	##########################/
	# output ENVI format rasters (if requested by user input)
	##########################/
	cout << "Writing out ENVI format products." << endl

	#write out the ENVI products specified
	data.writeENVIs(prms.iFile, hdr, prms)

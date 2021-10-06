import numpy as np
import rasterio as rio

# function to compute the relative relief
def computeRelativeRelief(inrast, buf):
	# create empty numpy arrays to contain the relative relief values
	rr = np.empty_like(inrast.shape, dtype=np.float32)
	rr3 = np.empty_like(inrast.shape, dtype=np.float32)
	rr6 = np.empty_like(inrast.shape, dtype=np.float32)

	# iterate through all pixels in Raster object
	for i in range(0, inrast.width, 1):
		for j in range(0, inrast.height, 1):
			for a in range(buf, buf+6, 3):
				# initially set the min and max value for the relative relief computations
				z_min = inrast[i][j]
				z_max = inrast[i][j]
				for ii in range(i-a, i+a, 1):  # check that the X index is within the raster bounds
					if ii>=0 and ii<inrast.width:
						for jj in range(j-a, j+a, 1):  # check that the Y index is within the raster bounds
							if jj>=0 and jj<inrast.height:
								if inrast[ii][jj] > -1000:  # check that the raster value is not -9999
									# calculate parameters for relative relief
									if inrast[ii][jj] > z_max:
										z_max = inrast[ii][jj]
									if inrast[ii][jj] < z_min:
										z_min = inrast[ii][jj]
				# compute relative relief
				if a = buf:
					rr[i][j] = np.divide((inrast[i][j]-z_min), (z_max-z_min), out=-9999, where=(z_max-zmin)>0.0)
				if a = buf+3:
					rr3[i][j] = np.divide((inrast[i][j]-z_min), (z_max-z_min), out=-9999, where=(z_max-zmin)>0.0)
				if a = buf+6:
					rr6[i][j] = np.divide((inrast[i][j]-z_min), (z_max-z_min), out=-9999, where=(z_max-zmin)>0.0)
	# average the relative relief arrays
	avgrr = np.mean([rr, rr3, rr6], axis=0)
	# return the rr, rr3, rr6, and avgrr numpy arrays
	return rr,rr3,rr6,avgrr

def writeENVIs(filename, inrast, outputcrs, pm):
	if "shoreline" in pm.oProduct or "landforms" in pm.oProduct or "all" in pm.oProduct:
		#######/
		# SHORELINE #
		#######/
		tmpname = filename;
		tmpname.append("_shoreline.tif");

		# write out shoreline raster
		newdat = rio.open(tmpname, 'w',
						driver='GTiff',
						height=inrast.height,
						width=inrast.width,
						count=1,
						dtype=inrast.dtype,
						crs=outcrs)
		# save and close the output raster
		newdat.write(inrast, 1)
		newdat.close()
		newdat.open(tmpname,'r+')
		newdat.nodata=-9999
		newdat.close()
	if "dunetoe" in pm.oProduct or "landforms" in pm.oProduct or "all" in pm.oProduct:
		#######
		# DUNE TOE #
		#######
		string tmpname = filename;
		tmpname.append("_dune_toe");

		# write out dune toe ENVI raster
		hdr.writeHDR(tmpname, Raster::dune_toe_line);
		hdr.writeDAT(tmpname, Raster::dune_toe_line);
	if "dunecrest" in pm.oProduct or "landforms" in pm.oProduct or "all" in pm.oProduct:
		########
		# DUNE RIDGE #
		########
		string tmpname = filename;
		tmpname.append("_dune_crest");

		# write out dune crest ENVI raster
		hdr.writeHDR(tmpname, Raster::dune_ridge_line);
		hdr.writeDAT(tmpname, Raster::dune_ridge_line);
	if "duneheel" in pm.oProduct or "landforms" in pm.oProduct or "all" in pm.oProduct:
		#######/
		# DUNE HEEL #
		#######/
		string tmpname = filename;
		tmpname.append("_dune_heel");

		# write out dune heel ENVI raster
		hdr.writeHDR(tmpname, Raster::dune_heel_line);
		hdr.writeDAT(tmpname, Raster::dune_heel_line);
	if "backbarrier" in pm.oProduct or "landforms" in pm.oProduct or "all" in pm.oProduct:
		#############/
		# BACKBARRIER SHORELINE #
		#############/
		string tmpname = filename;
		tmpname.append("_backbarrier_shoreline");

		# write out back barrier shoreline ENVI raster
		hdr.writeHDR(tmpname, Raster::backbarrier_line);
		hdr.writeDAT(tmpname, Raster::backbarrier_line);
	if "rr" in pm.oProduct or "all" in pm.oProduct:
		##########/
		# RELATIVE RELIEF #
		##########/
		string tmpname = filename;
		tmpname.append("_rr"+to_string(pm.iWindowSize));

		# write out relative relief ENVI rasters
		hdr.writeHDR(tmpname, Raster::res);
		hdr.writeDAT(tmpname, Raster::res);

		tmpname = filename;
		tmpname.append("_rr"+to_string(pm.iWindowSize+2));

		# write out relative relief ENVI rasters
		hdr.writeHDR(tmpname, Raster::res_plus1);
		hdr.writeDAT(tmpname, Raster::res_plus1);

		tmpname = filename;
		tmpname.append("_rr"+to_string(pm.iWindowSize+4));

		# write out relative relief ENVI rasters
		hdr.writeHDR(tmpname, Raster::res_plus2);
		hdr.writeDAT(tmpname, Raster::res_plus2);

		tmpname = filename;
		tmpname.append("_rr_avg");

		# write out average relative relief ENVI raster
		hdr.writeHDR(tmpname, Raster::avg);
		hdr.writeDAT(tmpname, Raster::avg);

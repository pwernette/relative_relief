import numpy as np
import rasterio as rio
import sys

###############################
# HEADER INFORMATION
###############################
class Header:
	def __init__(self):
		self.description = string()
		self.ncols = int() 			# number of columns
		self.nlines = int()			# number of lines
		self.npix = int()		# number of pixels
		self.bands = int()		# number of bands
		self.headeroffset = int()
		self.filetype = string()
		self.datatype = int()
		self.interleave = string()		# data interleave format (bsq, bip, or bil)
		self.sensortype = string()
		self.coordsys = string()		# coordinate system information
		self.xres = float()			# resolution (x direction)
		self.yres = float()			# resolution (y direction)
		self.ulx = float()			# upper-left x coordinate
		self.uly = float()			# upper-left y coordinate
		self.ymin = float()
		self.xmax = float()
		self.zmin = float()
		self.zmax = float()
		self.utm_zone_number = string()
		self.utm_zone_band = string()
		self.datum = string()
		self.units = string()		# measurement units
		self.proj_string = string()	# projection string

	def LoadInParameters(self, ifile):
		# get number of columns
		self.ncols = ifile.width
		# get number of lines
		self.nlines = ifile.height
		# get number of bands
		self.bands = ifile.count
		# get data type
		self.datatype = ifile.dtypes[ifile.count]
		# get upper left coordinates
		self.ulx, self.uly = ifile.transform*(0,0)
		# get x and y resolution (i.e. pixel size)
		self.xres = ifile.transform[0]
		self.yres = -ifile.transform[4]
		# get coordinate system information
		self.coordsys = ifile.crs
		self.units = ifile.crs.linear_units
		self.proj_string = ifile.crs.wkt

###############################/
# THRESHOLDS INFORMATION
###############################/
class Params:
	def __init__(self):
		#input filename
		self.iFile = string()
		#initial window size (pixels in data units)
		self.iWindowSize = int()
		#specify the output product
		self.oProduct = string()
		#output product format
		self.oFormat = string()
		#-------------------------------------------------------------------
		#  used to define the thresholds
		#-------------------------------------------------------------------
		self.tShoreline = float()
		self.tDT = float()
		self.tDC = float()
		self.tDH = float()
		self.tBB = float()
		self.tDuneDistMin = float()
		self.tDuneDistMax = float()
		self.tCrestDistMin = float()
		self.tCrestDistMax = float()
		self.tHeelDistMin = float()
		self.tHeelDistMax = float()

	def LoadInParameters(self, infile):
		# open the file
		infi = open(infile, 'r')
		# read the file line-by-line
		lines = infi.readlines()
		for line in lines:
			if "iFile" in line:
				self.iFile = line.split(' ',1)[1]
			if "iWindowSize" in line:
				self.iWindowSize = int(line.split(' ',1)[1])
				if not self.iWindowSize % 2 = 1:
					print("ERROR: Window size is set to an even number. Window size will be increased by one to {}".format(self.iWindowSize+1))
			if "oProduct" in line:
				self.oProduct = line.split(' ',1)[1]
			if "oFormat" in line:
				self.oFormat = line.split(' ',1)[1]
			if "tShoreline" in line:
				self.tShoreline = float(line.split(' ',1)[1])
			if "tDT" in line:
				self.tDT = float(line.split(' ',1)[1])
			if "tDC" in line:
				self.tDC = float(line.split(' ',1)[1])
			if "tDH" in line:
				self.tDH = float(line.split(' ',1)[1])
			if "tBB" in line:
				self.tBB = float(line.split(' ',1)[1])
			if "tDuneDistMin" in line:
				self.tDuneDistMin = float(line.split(' ',1)[1])
			if "tDuneDistMax" in line:
				self.tDuneDistMax = float(line.split(' ',1)[1])
			if "tCrestDistMin" in line:
				self.tCrestDistMin = float(line.split(' ',1)[1])
			if "tCrestDistMax" in line:
				self.tCrestDistMax = float(line.split(' ',1)[1])
			if "tHeelDistMin" in line:
				self.tHeelDistMin = float(line.split(' ',1)[1])
			if "tHeelDistMax" in line:
				self.tHeelDistMax = float(line.split(' ',1)[1])

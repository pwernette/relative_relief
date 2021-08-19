import numpy as np
import rasterio as rio

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

	def Initialize(self, Fname):
		Fname.append(".hdr")
		if(!LoadInParameters(Fname)):
			print("ERROR: Cannot find '{}''".format(Fname))
			return false
		return true

	def LoadInParametersENVI(self, Fname):
		# attempt to open specified input file
		ifile = open(Fname, 'r')
		if not ifile:
			print("ERROR: Cannot open {}".format(Fname))
			break

		print("Reading header information from {} ...".format(Fname))

		# iterate through all lines in the input file
		for line in ifile.readlines(){
			# split the identifier from the information
			item = line.substr(0, (line.find_first_of("="))-1)

			# get description of raster
			if "description" in item:
				self.description = (item.split("{",1)[1]).split("}",1)[1]

			#get number of columns (aka: samples) in raster
			if "samples" in item:
				self.ncols = int(line.split("=",1)[1])

			# get number of lines in raster
			if "lines" in item:
				self.nlines = int(line.split("=",1)[1])

			# get number of bands
			if "bands" in item:
				self.bands = int(line.split("=",1)[1])

			# header offset information
			if "header offset" in item:
				self.headeroffset = int(line.split("=",1)[1])

			#get file type info
			if "file type" in item:
				self.filetype = line.split("=",1)[1]

			# get data type
			if "data type" in item:		# e.g 5 = double
				self.datatype = int(line.split("=",1)[1])

			# get interleave format
			if "interleave" in item:		# "BSQ" | "BIL" | "BIP"
				self.interleave = line.split("=",1)[1]

			# get sensor type
			if "sensor type" in item:
				self.sensortype = line.split("=",1)[1]

			# get map information (i.e. coordinate and projection information
			if "map info" in item:
				# get coordinate system info
				size_t com = line.find_first_of("{")+1
				size_t com2 = line.find(",", com+1)
				self.coordsys = line.substr(com, com2-com)

				# get upper-left x coordinate
				com = line.find(",", com2)
				com2 = line.find(",", com+1)
				com = line.find(",", com2)
				com2 = line.find(",", com+1)
				com = line.find(",", com2)
				com2 = line.find(",", com+1)
				self.ulx = atof((line.substr(com+2, com2-com)).c_str())

				# get upper-left y coordinate
				com = line.find(",", com2)
				com2 = line.find(",", com+1)-2
				self.uly = atof((line.substr(com+2, com2-com)).c_str())

				# get spatial resolution in the x direction
				com = line.find(",", com2)
				com2 = line.find(",", com+1)-2
				self.xres = atof((line.substr(com+2, com2-com)).c_str())

				# get spatial resolution in the y direction
				com = line.find(",", com2)
				com2 = line.find(",", com+1)-2
				self.yres = atof((line.substr(com+2, com2-com)).c_str())

				# get UTM zone and band combination (if UTM coordinate system)
				if(self.coordsys.compare("UTM") == 0):
					# extract the UTM zone number
					com = line.find(",", com2)
					com2 = line.find(",", com+1)-2
					self.utm_zone_number = (line.substr(com+2, com2-com))

					# extract the UTM zone band
					com = line.find(",", com2)
					com2 = line.find(",", com+1)-2
					self.utm_zone_band = (line.substr(com+2, com2-com))

				# get datum information
				com = line.find(",", com2)
				com2 = line.find(",", com+1)-2
				self.datum = (line.substr(com+2, com2-com))

				# get map units
				com = line.find("units=", com2)+6
				com2 = line.find_last_of("}")
				self.units = (line.substr(com, com2-com))

			# get projection string information
			if(item.compare("coordinate system string") == 0):
				self.proj_string = line.substr(line.find_first_of("{")+1, line.find_first_of("}")-line.find_first_of("{")-1)

	def LoadInParameters(self, Fname):
		# read in raster using rasterio
		ifile = rio.open(Fname)

		# populate Header with info from raster
		self.

	#function to write binary data file
	def writeDAT(string fn, vector<float> outdat):
	void writeDAT(string fn, vector<unsigned int> outdat)
	void writeDAT(string fn, vector<int> outdat)
	void writeDAT(string fn, vector<long int> outdat)
	void writeDAT(string fn, vector<double> outdat)

	#function to write header file
	void writeHDR(string fn, vector<float> outinfo)
	void writeHDR(string fn, vector<unsigned int> outinfo)
	void writeHDR(string fn, vector<int> outinfo)
	void writeHDR(string fn, vector<long int> outinfo)
	void writeHDR(string fn, vector<double> outinfo)
}

###############################/
# THRESHOLDS INFORMATION
###############################/
class Params
{
	public:
	#-------------------------------------------------------------------
	#  input parameters
	#-------------------------------------------------------------------

	#input filename
	string iFile

	#initial window size (pixels in data units)
	int iWindowSize

	#specify the output product
	string oProduct

	#output product format
	string oFormat


	#-------------------------------------------------------------------
	#  used to define the thresholds
	#-------------------------------------------------------------------
	float tShoreline
	float tDT
	float tDC
	float tDH
	float tBB
	float tDuneDistMin
	float tDuneDistMax
	float tCrestDistMin
	float tCrestDistMax
	float tHeelDistMin
	float tHeelDistMax

	bool Initialize()
	{
	if(!LoadInParameters("params_rr.ini"))
		{
			cout << "ERROR: Cannot find 'params_rr.ini'" << endl
			#MessageBox(NULL, "Cannot find 'params_rr.ini'", "Error", 0)

			return false
		}

		if(fmod(Params::iWindowSize, 2) != 1){ # exit the program if the specified window size is not an odd number
			cout << "ERROR: Invalid window size --> Window size must be an odd number!" << endl
			exit(1)
		} else if(Params::iWindowSize < 3){
			cout << "ERROR: Invalid window size --> Window size must be at least 3!" << endl
			exit(1)
		}

		return true
	}
	bool LoadInParameters(const char* szFileName)
}


###############################/
# STORE RASTER VALUES AND METRICS
###############################/
class Raster
{
public:
	#integer corresponding to the number of pixels
	int size

	#DEM information
	vector<float> x			# x coordinate
	vector<float> y			# y coordinate
	vector<float> z			# z coordinate

	#binary indicating whether the location has data for all data types
	vector<unsigned int> complete

	#relative relief variables (per pixel)
	vector<float> res
	vector<float> res_plus1
	vector<float> res_plus2
	vector<float> avg

	#binary indicators of feature position
	vector<float> shoreline
	vector<float> dune_toe_line
	vector<float> dune_ridge_line
	vector<float> dune_heel_line
	vector<float> backbarrier_line

	void Initialize(Params prms, Header hdr)

	void Init(int m_size)

	bool readDAT(string Fname, Header hdr)

	void writeENVIs(string filename, Header hdr, Params pm)

	void computeRelativeRelief(int i, int j, int buf, Header hdr)
}

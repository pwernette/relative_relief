import numpy as np

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

	def Initialize(Fname):
		Fname.append(".hdr")
		if(!LoadInParameters(Fname)):
			print("ERROR: Cannot find '{}''".format(Fname))
			return false
		return true

	def LoadInParameters(Fname):
		register int i;
		string line;
	
		// attempt to open specified input file
		ifstream infile(filename.c_str(), ios::in);
		if(!infile){
			cerr << "ERROR: Cannot open " << filename << endl;
			return false;
		}

		cout << "Reading header information from " << filename.c_str() << "..." << endl;

		while(getline(infile, line)){
			// split the identifier from the information
			string item = line.substr(0, (line.find_first_of("="))-1);

			// get description of raster
			if(item.compare("description") == 0){
				Header::description = line.substr(line.find_first_of("{"), line.find_first_of("}"));
			}

			//get number of columns (aka: samples) in raster
			if(item.compare("samples") == 0){
				Header::ncols = atoi(line.substr(line.find_first_of("=")+2, line.length()-1).c_str());
				//cout << header.ncols << endl;
			}

			// get number of lines in raster
			if(item.compare("lines") == 0){
				Header::nlines = atoi(line.substr(line.find_first_of("=")+2, line.length()-1).c_str());
				//cout <<  header.nlines << endl;
			}

			// get number of bands
			if(item.compare("bands") == 0){
				Header::bands = atoi(line.substr(line.find_first_of("=")+2, line.length()-1).c_str());
			}

			// header offset information
			if(item.compare("header offset") == 0){
				Header::headeroffset = atoi(line.substr(line.find_first_of("=")+2, line.length()-1).c_str());
			}

			//get file type info
			if(item.compare("file type") == 0){
				Header::filetype = line.substr(line.find_first_of("=")+2, line.length()-1);
			}

			// get data type
			if(item.compare("data type") == 0){		// e.g 5 = double
				Header::datatype = atoi(line.substr(line.find_first_of("=")+2, line.length()-1).c_str());
			}

			// get interleave format
			if(item.compare("interleave") == 0){		// "BSQ" | "BIL" | "BIP"
				Header::interleave = line.substr(line.find_first_of("=")+2, line.length()-1);
			}

			// get sensor type
			if(item.compare("sensor type") == 0){
				Header::sensortype = line.substr(line.find_first_of("=")+2, line.length()-1);
			}

			// get map information (i.e. coordinate and projection information
			if(item.compare("map info") == 0){
				// get coordinate system info
				size_t com = line.find_first_of("{")+1;
				size_t com2 = line.find(",", com+1);
				Header::coordsys = line.substr(com, com2-com);

				// get upper-left x coordinate
				com = line.find(",", com2);
				com2 = line.find(",", com+1);
				com = line.find(",", com2);
				com2 = line.find(",", com+1);
				com = line.find(",", com2);
				com2 = line.find(",", com+1);
				Header::ulx = atof((line.substr(com+2, com2-com)).c_str());

				// get upper-left y coordinate
				com = line.find(",", com2);
				com2 = line.find(",", com+1)-2;
				Header::uly = atof((line.substr(com+2, com2-com)).c_str());

				// get spatial resolution in the x direction
				com = line.find(",", com2);
				com2 = line.find(",", com+1)-2;
				Header::xres = atof((line.substr(com+2, com2-com)).c_str());

				// get spatial resolution in the y direction
				com = line.find(",", com2);
				com2 = line.find(",", com+1)-2;
				Header::yres = atof((line.substr(com+2, com2-com)).c_str());

				// get UTM zone and band combination (if UTM coordinate system)
				if(Header::coordsys.compare("UTM") == 0){
					// extract the UTM zone number
					com = line.find(",", com2);
					com2 = line.find(",", com+1)-2;
					Header::utm_zone_number = (line.substr(com+2, com2-com));

					// extract the UTM zone band
					com = line.find(",", com2);
					com2 = line.find(",", com+1)-2;
					Header::utm_zone_band = (line.substr(com+2, com2-com));
				}

				// get datum information
				com = line.find(",", com2);
				com2 = line.find(",", com+1)-2;
				Header::datum = (line.substr(com+2, com2-com));

				// get map units
				com = line.find("units=", com2)+6;
				com2 = line.find_last_of("}");
				Header::units = (line.substr(com, com2-com));
			}

			// get projection string information
			if(item.compare("coordinate system string") == 0){
				Header::proj_string = line.substr(line.find_first_of("{")+1, line.find_first_of("}")-line.find_first_of("{")-1);
			}

	#function to write binary data file
	def writeDAT(string fn, vector<float> outdat):
	void writeDAT(string fn, vector<unsigned int> outdat);
	void writeDAT(string fn, vector<int> outdat);
	void writeDAT(string fn, vector<long int> outdat);
	void writeDAT(string fn, vector<double> outdat);

	#function to write header file
	void writeHDR(string fn, vector<float> outinfo);
	void writeHDR(string fn, vector<unsigned int> outinfo);
	void writeHDR(string fn, vector<int> outinfo);
	void writeHDR(string fn, vector<long int> outinfo);
	void writeHDR(string fn, vector<double> outinfo);
};

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
	string iFile;

	#initial window size (pixels in data units)
	int iWindowSize;

	#specify the output product
	string oProduct;

	#output product format
	string oFormat;


	#-------------------------------------------------------------------
	#  used to define the thresholds
	#-------------------------------------------------------------------
	float tShoreline;
	float tDT;
	float tDC;
	float tDH;
	float tBB;
	float tDuneDistMin;
	float tDuneDistMax;
	float tCrestDistMin;
	float tCrestDistMax;
	float tHeelDistMin;
	float tHeelDistMax;

	bool Initialize()
	{
	if(!LoadInParameters("params_rr.ini"))
		{
			cout << "ERROR: Cannot find 'params_rr.ini'" << endl;
			#MessageBox(NULL, "Cannot find 'params_rr.ini'", "Error", 0);

			return false;
		}

		if(fmod(Params::iWindowSize, 2) != 1){ # exit the program if the specified window size is not an odd number
			cout << "ERROR: Invalid window size --> Window size must be an odd number!" << endl;
			exit(1);
		} else if(Params::iWindowSize < 3){
			cout << "ERROR: Invalid window size --> Window size must be at least 3!" << endl;
			exit(1);
		}

		return true;
	}
	bool LoadInParameters(const char* szFileName);
};


###############################/
# STORE RASTER VALUES AND METRICS
###############################/
class Raster
{
public:
	#integer corresponding to the number of pixels
	int size;

	#DEM information
	vector<float> x;			# x coordinate
	vector<float> y;			# y coordinate
	vector<float> z;			# z coordinate

	#binary indicating whether the location has data for all data types
	vector<unsigned int> complete;

	#relative relief variables (per pixel)
	vector<float> res;
	vector<float> res_plus1;
	vector<float> res_plus2;
	vector<float> avg;

	#binary indicators of feature position
	vector<float> shoreline;
	vector<float> dune_toe_line;
	vector<float> dune_ridge_line;
	vector<float> dune_heel_line;
	vector<float> backbarrier_line;

	void Initialize(Params prms, Header hdr);

	void Init(int m_size);

	bool readDAT(string Fname, Header hdr);

	void writeENVIs(string filename, Header hdr, Params pm);

	void computeRelativeRelief(int i, int j, int buf, Header hdr);
};

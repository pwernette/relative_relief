#include <math.h>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <vector>

using namespace std;

///////////////////////////////////////////////////////////////
// HEADER INFORMATION
///////////////////////////////////////////////////////////////
class Header
{
public:
	string description;
	int ncols; 			// number of columns
	int nlines; 			// number of lines
	int npix;  			// number of pixels
	int bands; 			// number of bands
	int headeroffset;
	string filetype;
	int datatype;
	string interleave;		// data interleave format (bsq, bip, or bil)
	string sensortype;
	string coordsys;		// coordinate system information
	float xres;			// resolution (x direction)
	float yres;			// resolution (y direction)
	double ulx;			// upper-left x coordinate
	double uly;			// upper-left y coordinate
	double ymin;
	double xmax;
	double zmin;
	double zmax;
	string utm_zone_number;
	string utm_zone_band;
	string datum;
	string units;		// measurement units
	string proj_string;	// projection string

	bool Initialize(string Fname)
	{
		Fname.append(".hdr");

		if(!LoadInParameters(Fname))
		{
			cout << "ERROR: Cannot find '" << Fname << endl;

			return false;
		}

		return true;
	}

	void defaultHeader();

	bool LoadInParameters(string Fname);

	//function to write binary data file
	void writeDAT(string fn, vector<float> outdat);
	void writeDAT(string fn, vector<unsigned int> outdat);
	void writeDAT(string fn, vector<int> outdat);
	void writeDAT(string fn, vector<long int> outdat);
	void writeDAT(string fn, vector<double> outdat);

	//function to write header file
	void writeHDR(string fn, vector<float> outinfo);
	void writeHDR(string fn, vector<unsigned int> outinfo);
	void writeHDR(string fn, vector<int> outinfo);
	void writeHDR(string fn, vector<long int> outinfo);
	void writeHDR(string fn, vector<double> outinfo);
};

///////////////////////////////////////////////////////////////
// THRESHOLDS INFORMATION
///////////////////////////////////////////////////////////////
class Params
{
	public:
	//-------------------------------------------------------------------
	//  input parameters
	//-------------------------------------------------------------------

	//input filename
	string iFile;

	//initial window size (pixels in data units)
	int iWindowSize;

	//specify the output product
	string oProduct;

	//output product format
	string oFormat;


	//-------------------------------------------------------------------
	//  used to define the thresholds
	//-------------------------------------------------------------------
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
	
	// transect direction
	string transect_direction;

	bool Initialize()
	{
	if(!LoadInParameters("params_rr.ini"))
		{
			cout << "ERROR: Cannot find 'params_rr.ini'" << endl;
			//MessageBox(NULL, "Cannot find 'params_rr.ini'", "Error", 0);

			return false;
		}

		if(fmod(Params::iWindowSize, 2) != 1){ // exit the program if the specified window size is not an odd number
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


///////////////////////////////////////////////////////////////
// STORE RASTER VALUES AND METRICS
///////////////////////////////////////////////////////////////
class Raster
{
public:
	//integer corresponding to the number of pixels
	int size;

	//DEM information
	vector<float> x;			// x coordinate
	vector<float> y;			// y coordinate
	vector<float> z;			// z coordinate

	//binary indicating whether the location has data for all data types
	vector<unsigned int> complete;

	//relative relief variables (per pixel)
	vector<float> res;
	vector<float> res_plus1;
	vector<float> res_plus2;
	vector<float> avg;

	//binary indicators of feature position
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

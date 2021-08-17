#include "data_structures.hpp"
#include <string.h>
#include <limits>
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;

///////////////////////////////////////////////////////////////
// GET PARAMETERS (INCLUDING THRESHOLDS) FROM INIT FILE
///////////////////////////////////////////////////////////////

//this function loads in the parameters from a given file name. Returns
//false if there is a problem opening the file.
bool Params::LoadInParameters(const char* iFileName)
{
  ifstream grab(iFileName);

  //check file exists
  if (!grab)
  {
	cerr << "ERROR: Cannot open " << iFileName << endl;
    return false;
  }

  //load in from the file
  char ParamDescription[40];

  grab >> ParamDescription;
  grab >> iFile;
  grab >> ParamDescription;
  grab >> iWindowSize;
  grab >> ParamDescription;
  grab >> oProduct;
  grab >> ParamDescription;
  grab >> oFormat;
  grab >> ParamDescription;
  grab >> tShoreline;
  grab >> ParamDescription;
  grab >> tDT;
  grab >> ParamDescription;
  grab >> tDC;
  grab >> ParamDescription;
  grab >> tDH;
  grab >> ParamDescription;
  grab >> tBB;
  grab >> ParamDescription;
  grab >> tDuneDistMin;
  grab >> ParamDescription;
  grab >> tDuneDistMax;
  grab >> ParamDescription;
  grab >> tCrestDistMin;
  grab >> ParamDescription;
  grab >> tCrestDistMax;
  grab >> ParamDescription;
  grab >> tHeelDistMin;
  grab >> ParamDescription;
  grab >> tHeelDistMax;

  return true;
};


///////////////////////////////////////////////////////////////
// HEADER INFORMATION
///////////////////////////////////////////////////////////////

////this function loads in the parameters from a given file name. Returns
////false if there is a problem opening the file.
bool Header::LoadInParameters(string filename){
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
	}

	// compute total number of pixels
	Header::npix = Header::ncols * Header::nlines;
	Header::xmax = -99999;
	Header::ymin = 9999999999;

	infile.close();

	return true;
}




//// Functions to write ENVI format header (.h) file
//// NOTE that this function is overloaded with several options for data input types
void Header::writeHDR(string fn, vector<unsigned int> outinfo){
	string tmp = fn;
	tmp.append(".hdr");

	// initialize the file pointer
	FILE *wrhead;

	// open the file
	wrhead = fopen(tmp.c_str(), "w");
	if(!wrhead){
		cout << "ERROR: Cannot write header file!\n";
		exit(1);
	}

	Header::interleave = "bsq";  // band sequential format

	// write the ascii header file
	(void) fprintf(wrhead, "ENVI\n");
	(void) fprintf(wrhead, "description = {%s}\n", Header::description.c_str()); // file description
	(void) fprintf(wrhead, "samples = %i\n", Header::ncols); // number of columns
	(void) fprintf(wrhead, "lines = %i\n", Header::nlines); // number of lines
	(void) fprintf(wrhead, "bands = %i\n", Header::bands); // number of bands
	(void) fprintf(wrhead, "header offset = 0\n");
	(void) fprintf(wrhead, "file type = ENVI Standard\n");
	(void) fprintf(wrhead, "data type = %i\n", 1); // outpud data type
	(void) fprintf(wrhead, "interleave = %s\n", Header::interleave.c_str()); // interleave format (bsq, bil, or bip)
	(void) fprintf(wrhead, "sensor type = Unknown\n");
	(void) fprintf(wrhead, "byte order = 0\n");
	(void) fprintf(wrhead, "map info = {%s, 1.00000, 1.00000, %le, %le, %le, %le, %s, %s, %s, units=%s}\n", Header::coordsys.c_str(), Header::ulx, Header::uly, Header::xres, Header::yres, Header::utm_zone_number.c_str(), Header::utm_zone_band.c_str(), Header::datum.c_str(), Header::units.c_str());
	(void) fprintf(wrhead, "coordinate system string = {PROJCS[\"UTM_Zone_14N\",GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-99.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]}\n");
	(void) fprintf(wrhead, "data ignore value = -9999\n");
	(void) fprintf(wrhead, "wavelength units = Unknown");

	// close the header file
	fclose(wrhead);

	cout << "Successfully wrote header file: " << tmp.c_str() << endl;
}
void Header::writeHDR(string fn, vector<int> outinfo){
	string tmp = fn;
	tmp.append(".hdr");

	// initialize the file pointer
	FILE *wrhead;

	// open the file
	wrhead = fopen(tmp.c_str(), "w");
	if(!wrhead){
		cout << "ERROR: Cannot write header file!\n";
		exit(1);
	}

	Header::interleave = "bsq";

	// write the ascii header file
	(void) fprintf(wrhead, "ENVI\n");
	(void) fprintf(wrhead, "description = {%s}\n", Header::description.c_str()); // file description
	(void) fprintf(wrhead, "samples = %i\n", Header::ncols); // number of columns
	(void) fprintf(wrhead, "lines = %i\n", Header::nlines); // number of lines
	(void) fprintf(wrhead, "bands = %i\n", Header::bands); // number of bands
	(void) fprintf(wrhead, "header offset = 0\n");
	(void) fprintf(wrhead, "file type = ENVI Standard\n");
	(void) fprintf(wrhead, "data type = %i\n", 2); // outpud data type
	(void) fprintf(wrhead, "interleave = %s\n", Header::interleave.c_str()); // interleave format (bsq, bil, or bip)
	(void) fprintf(wrhead, "sensor type = Unknown\n");
	(void) fprintf(wrhead, "byte order = 0\n");
	(void) fprintf(wrhead, "map info = {%s, 1.00000, 1.00000, %le, %le, %le, %le, %s, %s, %s, units=%s}\n", Header::coordsys.c_str(), Header::ulx, Header::uly, Header::xres, Header::yres, Header::utm_zone_number.c_str(), Header::utm_zone_band.c_str(), Header::datum.c_str(), Header::units.c_str());
	(void) fprintf(wrhead, "coordinate system string = {PROJCS[\"UTM_Zone_14N\",GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-99.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]}\n");
	(void) fprintf(wrhead, "data ignore value = -9999\n");
	(void) fprintf(wrhead, "wavelength units = Unknown");

	// close the header file
	fclose(wrhead);

	cout << "Successfully wrote header file: " << tmp.c_str() << endl;
}
void Header::writeHDR(string fn, vector<float> outinfo){
	string tmp = fn;
	tmp.append(".hdr");

	// initialize the file pointer
	FILE *wrhead;

	// open the file
	wrhead = fopen(tmp.c_str(), "w");
	if(!wrhead){
		cout << "ERROR: Cannot write header file!\n";
		exit(1);
	}

	Header::interleave = "bsq";

	// write the ascii header file
	(void) fprintf(wrhead, "ENVI\n");
	(void) fprintf(wrhead, "description = {%s}\n", Header::description.c_str()); // file description
	(void) fprintf(wrhead, "samples = %i\n", Header::ncols); // number of columns
	(void) fprintf(wrhead, "lines = %i\n", Header::nlines); // number of lines
	(void) fprintf(wrhead, "bands = %i\n", Header::bands); // number of bands
	(void) fprintf(wrhead, "header offset = 0\n");
	(void) fprintf(wrhead, "file type = ENVI Standard\n");
	(void) fprintf(wrhead, "data type = %i\n", 4); // outpud data type
	(void) fprintf(wrhead, "interleave = %s\n", Header::interleave.c_str()); // interleave format (bsq, bil, or bip)
	(void) fprintf(wrhead, "sensor type = Unknown\n");
	(void) fprintf(wrhead, "byte order = 0\n");
	(void) fprintf(wrhead, "map info = {%s, 1.00000, 1.00000, %le, %le, %le, %le, %s, %s, %s, units=%s}\n", Header::coordsys.c_str(), Header::ulx, Header::uly, Header::xres, Header::yres, Header::utm_zone_number.c_str(), Header::utm_zone_band.c_str(), Header::datum.c_str(), Header::units.c_str());
	(void) fprintf(wrhead, "coordinate system string = {PROJCS[\"UTM_Zone_14N\",GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-99.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]}\n");
	(void) fprintf(wrhead, "data ignore value = -9999\n");
	(void) fprintf(wrhead, "wavelength units = Unknown");

	// close the header file
	fclose(wrhead);

	cout << "Successfully wrote header file: " << tmp.c_str() << endl;
}
void Header::writeHDR(string fn, vector<double> outinfo){
	string tmp = fn;
	tmp.append(".hdr");

	// initialize the file pointer
	FILE *wrhead;

	// open the file
	wrhead = fopen(tmp.c_str(), "w");
	if(!wrhead){
		cout << "ERROR: Cannot write header file!\n";
		exit(1);
	}

	Header::interleave = "bsq";

	// write the ascii header file
	(void) fprintf(wrhead, "ENVI\n");
	(void) fprintf(wrhead, "description = {%s}\n", Header::description.c_str()); // file description
	(void) fprintf(wrhead, "samples = %i\n", Header::ncols); // number of columns
	(void) fprintf(wrhead, "lines = %i\n", Header::nlines); // number of lines
	(void) fprintf(wrhead, "bands = %i\n", Header::bands); // number of bands
	(void) fprintf(wrhead, "header offset = 0\n");
	(void) fprintf(wrhead, "file type = ENVI Standard\n");
	(void) fprintf(wrhead, "data type = %i\n", 5); // outpud data type
	(void) fprintf(wrhead, "interleave = %s\n", Header::interleave.c_str()); // interleave format (bsq, bil, or bip)
	(void) fprintf(wrhead, "sensor type = Unknown\n");
	(void) fprintf(wrhead, "byte order = 0\n");
	(void) fprintf(wrhead, "map info = {%s, 1.00000, 1.00000, %le, %le, %le, %le, %s, %s, %s, units=%s}\n", Header::coordsys.c_str(), Header::ulx, Header::uly, Header::xres, Header::yres, Header::utm_zone_number.c_str(), Header::utm_zone_band.c_str(), Header::datum.c_str(), Header::units.c_str());
	(void) fprintf(wrhead, "coordinate system string = {PROJCS[\"UTM_Zone_14N\",GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-99.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0]]}\n");
	(void) fprintf(wrhead, "data ignore value = -9999\n");
	(void) fprintf(wrhead, "wavelength units = Unknown");

	// close the header file
	fclose(wrhead);

	cout << "Successfully wrote header file: " << tmp.c_str() << endl;
}




//// Functions to write ENVI format data (.dat) file
//// The .dat file is a binary file used to efficently store large data volumes.
//// NOTE that this function is overloaded with several options for data input types
void Header::writeDAT(string fn, vector<unsigned int> outdat){
	string tmp = fn;
	tmp.append(".dat");

	// write the binary data to file
	if(!outdat.empty()){
		// initialize the file pointer
		ofstream fout;
		fout.open(tmp, ios::out | ios::binary);

//		cout << "outdat is NOT empty." << endl;
//		size_t size = outdat.size()*sizeof(outdat);
		fout.write(reinterpret_cast<char*>(outdat.data()), outdat.size()*sizeof(BYTE));
//		(void) fwrite((float)outdat, sizeof(float), Header::npix, wrdat);

		cout << "Successfully wrote data to binary file: " << tmp.c_str() << endl;
		cout << endl;
		// close the file
	//	fclose(fout);
		fout.close();
	}
}
void Header::writeDAT(string fn, vector<int> outdat){
	string tmp = fn;
	tmp.append(".dat");
	// write the binary data to file
	if(!outdat.empty()){
		// initialize the file pointer
		ofstream fout;
		fout.open(tmp, ios::out | ios::binary);

//		size_t size = outdat.size()*sizeof(outdat);
		fout.write(reinterpret_cast<char*>(&outdat[0]), outdat.size()*sizeof(int));
//		(void) fwrite((float)outdat, sizeof(float), Header::npix, wrdat);

		cout << "Successfully wrote data to binary file: " << tmp.c_str() << endl;
		cout << endl;
		// close the file
	//	fclose(fout);
		fout.close();
	}
}
void Header::writeDAT(string fn, vector<float> outdat){
	string tmp = fn;
	tmp.append(".dat");

	// write the binary data to file
	if(!outdat.empty()){
		// initialize the file pointer
		ofstream FILE;
		FILE.open(tmp, ios::out | ios::binary);

//		size_t size = outdat.size()*sizeof(outdat);
		FILE.write(reinterpret_cast<char*>(&outdat[0]), outdat.size()*sizeof(float));
//		(void) fwrite((float)outdat, sizeof(float), Header::npix, wrdat);

		cout << "Successfully wrote data to binary file: " << tmp.c_str() << endl;
		cout << endl;

		// close the file
		FILE.close();
	}
}
void Header::writeDAT(string fn, vector<double> outdat){
	string tmp = fn;
	tmp.append(".dat");

	// initialize the file pointer
	ofstream fout;
	fout.open(tmp, ios::out | ios::binary);

	// write the binary data to file
	if(!outdat.empty()){
//		size_t size = outdat.size()*sizeof(outdat);
		fout.write(reinterpret_cast<char*>(&outdat[0]), outdat.size()*sizeof(double));
//		(void) fwrite((float)outdat, sizeof(float), Header::npix, wrdat);

		cout << "Successfully wrote data to binary file: " << tmp.c_str() << endl;
		cout << endl;
	}
	// close the file
//	fclose(fout);
	fout.close();
}



///////////////////////////////////////////////////////////////
// RASTER INFORMATION
///////////////////////////////////////////////////////////////

// Function to initialize raster of specified size
void Raster::Init(int m_size)
{
	Raster::x.resize(m_size);
	Raster::y.resize(m_size);
	Raster::z.resize(m_size);
	Raster::complete.resize(m_size);
	Raster::res.resize(m_size);
	Raster::avg.resize(m_size);
	Raster::shoreline.resize(m_size);
	Raster::dune_toe_line.resize(m_size);
	Raster::dune_ridge_line.resize(m_size);
	Raster::dune_heel_line.resize(m_size);
	Raster::backbarrier_line.resize(m_size);

//	cout << "Raster size (init): " << Raster::z.size() << endl;  // for debugging purposes
}


// Function to initialze a raster (wrapper for several functions)
void Raster::Initialize(Params prms, Header hdr)
{
	Raster::Init(hdr.npix);  // initialize the Raster object with size npix

	if(!readDAT((prms.iFile), hdr))  // if unable to read input data file(s)
	{
		cout << "Input filename: " << prms.iFile << endl;
		cout << "ERROR: Cannot find '" << prms.iFile << ".dat'" << endl;
	}
}

// Function to read data file
bool Raster::readDAT(string fn, Header hdr){
	register int count, t, s, idx;
	ifstream f;

	// open the file
	f.open(fn.append(".dat"), ios::binary | ios::in);
	if(!f){
		cerr << "ERROR: Cannot open " << fn << endl;
		return false;
	}

	cout << "Reading data from " << fn << "..." << endl;

	if(hdr.datatype == 4){
		f.read(reinterpret_cast<char*> (Raster::z.data()), Raster::z.size()*sizeof(float));

		for(s=0; s<hdr.nlines; s++){
			for(t=0; t<hdr.ncols; t++){
				idx = s*hdr.ncols + t;

				Raster::x[idx] = hdr.ulx + t*hdr.xres;
				Raster::y[idx] = hdr.uly - s*hdr.yres;

				if(Raster::x[idx] > hdr.xmax){
					hdr.xmax = Raster::x[idx];
				}
				if(Raster::y[idx] < hdr.ymin){
					hdr.ymin = Raster::y[idx];
				}

				if(Raster::z[idx] > -9999){
					if(hdr.zmin > Raster::z[idx] && Raster::z[idx] > -100){
						hdr.zmin = Raster::z[idx];
					} else if(Raster::z[idx] > hdr.zmax){
						hdr.zmax = Raster::z[idx];
					}
				}
			}
		}
	} else cout << "Invalid data type." << endl;

	// print info about the file to the screen
	cout << "FILE INFORMATION:" << endl;
	cout << "Upper Left (" << hdr.ulx << ", " << hdr.uly << ")" << endl;
	cout << "Lower Right (" << hdr.xmax << ", " << hdr.ymin << ")" << endl;
	cout << "Resolution (X, Y): (" << hdr.xres << ", " << hdr.yres << ")" << endl;
	cout << "Z min & max: " << hdr.zmin << " - " << hdr.zmax << endl;
	cout << "Rows: " << hdr.nlines << ", Columns: " << hdr.ncols << ", Pixels: " << hdr.npix << "\n" << endl;

	// close the file
	f.close();

	return true;
}

void Raster::writeENVIs(string filename, Header hdr, Params pm){
	if(pm.oProduct.compare("shoreline")==0 || pm.oProduct.compare("all")==0){
		///////////////
		// SHORELINE //
		///////////////
		string tmpname = filename;
		tmpname.append("_shoreline");

		// write out shoreline ENVI raster
		hdr.writeHDR(tmpname, Raster::shoreline);
		hdr.writeDAT(tmpname, Raster::shoreline);
	}
	if(pm.oProduct.compare("dunetoe")==0 || pm.oProduct.compare("all")==0){
		//////////////
		// DUNE TOE //
		//////////////
		string tmpname = filename;
		tmpname.append("_dune_toe");

		// write out dune toe ENVI raster
		hdr.writeHDR(tmpname, Raster::dune_toe_line);
		hdr.writeDAT(tmpname, Raster::dune_toe_line);
	}
	if(pm.oProduct.compare("dunecrest")==0 || pm.oProduct.compare("all")==0){
		////////////////
		// DUNE RIDGE //
		////////////////
		string tmpname = filename;
		tmpname.append("_dune_crest");

		// write out dune crest ENVI raster
		hdr.writeHDR(tmpname, Raster::dune_ridge_line);
		hdr.writeDAT(tmpname, Raster::dune_ridge_line);
	}
	if(pm.oProduct.compare("duneheel")==0 || pm.oProduct.compare("all")==0){
		///////////////
		// DUNE HEEL //
		///////////////
		string tmpname = filename;
		tmpname.append("_dune_heel");

		// write out dune heel ENVI raster
		hdr.writeHDR(tmpname, Raster::dune_heel_line);
		hdr.writeDAT(tmpname, Raster::dune_heel_line);
	}
	if(pm.oProduct.compare("backbarrier")==0 || pm.oProduct.compare("all")==0){
		///////////////////////////
		// BACKBARRIER SHORELINE //
		///////////////////////////
		string tmpname = filename;
		tmpname.append("_backbarrier_shoreline");

		// write out back barrier shoreline ENVI raster
		hdr.writeHDR(tmpname, Raster::backbarrier_line);
		hdr.writeDAT(tmpname, Raster::backbarrier_line);
	}
	if(pm.oProduct.compare("rr")==0 || pm.oProduct.compare("all")==0){
		/////////////////////
		// RELATIVE RELIEF //
		/////////////////////
		string tmpname = filename;
		tmpname.append("_rr"+to_string(pm.iWindowSize));

		// write out relative relief ENVI rasters
		hdr.writeHDR(tmpname, Raster::res);
		hdr.writeDAT(tmpname, Raster::res);

		tmpname = filename;
		tmpname.append("_rr"+to_string(pm.iWindowSize+2));

		// write out relative relief ENVI rasters
		hdr.writeHDR(tmpname, Raster::res_plus1);
		hdr.writeDAT(tmpname, Raster::res_plus1);

		tmpname = filename;
		tmpname.append("_rr"+to_string(pm.iWindowSize+4));

		// write out relative relief ENVI rasters
		hdr.writeHDR(tmpname, Raster::res_plus2);
		hdr.writeDAT(tmpname, Raster::res_plus2);

		tmpname = filename;
		tmpname.append("_rr_avg");

		// write out average relative relief ENVI raster
		hdr.writeHDR(tmpname, Raster::avg);
		hdr.writeDAT(tmpname, Raster::avg);
	}
}

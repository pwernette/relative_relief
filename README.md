# Relative Relief and Automated Barrier Island Landform Feature Extraction from DEMs

This program implements the approach detailed in:

> Wernette, P., C. Houser, and M.P. Bishop. (2016) An automated approach for extracting barrier island morphology from digital elevation models. Geomorphology, 262(1), 1-7. http://doi.org/10.1016/j.geomorph.2016.02.024.

![Graphical abstract from *Geomorphology* paper](/images/featureextractiongraphicalabstract.png)

## Compiling the Program

If compiling this program on a **linux OS** or **unix OS**, you do not need to include the ".exe" suffix at the end of the output program name and can compile the program using either of the following options:
```
g++ *.cpp -lm -O2 -o programname
g++ *.cpp -lm -O2 -static -o programname
```

If compiling this program on a **Windows OS**, make sure to include the ".exe" suffix at the end of the output program name. For example, `programname.exe`.
```
g++ *.cpp -lm -O2 -o programname.exe
g++ *.cpp -lm -O2 -static -o programname.exe
```

Including the `-static` flag during compilation will ensure that everything required for the program is wrapped into a single executable that can be moved from one computer to another and still function properly. If this flag is not included, then the resulting executable may not function properly or at all when moving computers.

**NOTE: Compiling with** ```relative_relief.cpp``` **will not work since data_structures.cpp is not also compiled in the process.**

## Inputs

1. **Input filename** (excluding extension)
2. **Window Size**: This is the moving window diameter (*i.e.* kernel size) **in picxels** used to compute relative relief. Two additional scales of relative relief will also be computed by adding 2 to this user-specified window size.
3. **Desired Output Product**:
	- `rr`: outputs relative relief rasters only
	- `shoreline`: outputs shoreline
	- `dunetoe`: outputs dune toe
	- `dunecrest`: outputs dune crest
	- `duneheel`: outputs dune heel
	- `backbarrier`: outputs backbarrier
	- `landforms`: outputs all landform rasters AND landform metrics table
	- `all`: outputs all landform rasters, landform metrics table, AND relative relief rasters
4. **Output Data Type(s)**:
	- `ascii`: only outputs ascii file
	- `envi`:  only outputs ENVI format raster
	- `both`: output both ascii and ENVI files


## Defaults

The default thresholds are as follows:
	
	- Shoreline:    0.02 masl
	- Dune Toe:     0.2
	- Dune Crest:   0.8
	- Dune Heel:    0.4
	- BB Shoreline: 1.0 masl

Any of the default values can be changed by altering the ```params_rr.ini``` file that accompanies this program.

## Usage
To run the program:
1. Make sure you have successfully compiled the program. If you are on a Windows OS, you will need to specify the executable as a .exe file during compilation. Otherwise, with unix and linux systems, the ".exe" suffix is not required.
2. Place a copy of the `params_rr.ini` file in the same directory on your local machine as the ENVI format rasters you wish to analyze.
3. Update the `params_rr.ini` file with the correct information (i.e. filename, initial kernel size, thresholds, desired output products and format, etc) for analysis.
4. Open a terminal window and mavigate to the directory with your .ini file and rasters.
5. Run the executable you created when you compiled the program.

## Program Function

This program functions by (1) importing an ENVI format DEM (.dat and .h file), (2) computing relative relief across 3 spatial scales, and (3) (OPTIONAL) extracting beach, dune, and island landscape features, and computing landform morphometrics (i.e. height, width, volume) from these landscape features.

Relative relief (RR) of every pixel in the input DEM is first computed \***using a 2D moving window**\*, where the window size is specified by the ```params_rr.ini``` file.

![Side profile (1D) profile of a transect through a 2D moving window](/images/Figure2.png)

The 2D window size will significant affect the ability to extract different landform morphometrics and subsequent landforms.

![Effect of 2D moving window size on relative relief values](/images/Figure3withDI.png)

Landform morphometrics (*i.e.* dune toe, dune crest, dune heel, and all additional derivatives) are extracted along transects of the 2D computed relative relief values.

![Planview map of where landform morphometrics from this paper](/images/Figure5.png)

![Profile view of where landform morphometrics from this paper align with other approaches](/images/Figure6.png)

## Questions and Feedback

This program was developed by Phil Wernette as part of his PhD Thesis from 2017 ("Assessing the Role of Framework Geology on Barrier Island Geomorphology") and *Geomorphology* paper from 2016 (Wernette, P., C. Houser, and M.P. Bishop. (2016) An automated approach for extracting barrier island morphology from digital elevation models. Geomorphology, 262(1), 1-7. http://doi.org/10.1016/j.geomorph.2016.02.024.)

For questions related to this program, please contact:

Phil Wernette [pwernette@usgs.gov]()

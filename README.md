# Relative Relief and Automated Barrier Island Landform Feature Extraction from DEMs

This program implements the approach detailed in:

> Wernette, P., C. Houser, and M.P. Bishop. (2016) An automated approach for extracting barrier island morphology from digital elevation models. Geomorphology, 262(1), 1-7. http://doi.org/10.1016/j.geomorph.2016.02.024.

<img src='/images/featureextractiongraphicalabstract.png' alt='Graphical abstract from *Geomorphology* paper'>

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

## Usage
To run the program:
1. Make sure you have successfully compiled the program. If you are on a Windows OS, you will need to specify the executable as a .exe file during compilation. Otherwise, with unix and linux systems, the ".exe" suffix is not required.
2. Place a copy of the `params_rr.ini` file in the same directory on your local machine as the ENVI format rasters you wish to analyze.
3. Update the `params_rr.ini` file with the correct information (i.e. filename, initial kernel size, thresholds, desired output products and format, etc) for analysis.
4. Open a terminal window and mavigate to the directory with your .ini file and rasters.
5. Run the executable you created when you compiled the program.

## Inputs

The program draws all input information (including input file name and thresholds) from the `params_rr.ini` file. This .ini file includes the following information:
1. **iFile** [default: rr_example]: The default input DEM file name (without file extension).
2. **iWindowSize** [default: 21] Size of the kernel **in picxels** used to compute relative relief. Two additional scales of relative relief will also be computed by adding 2 to this user-specified window/kernel size.
3. **oProduct** [default: all] Specify the output products
	a. `all`: Computes relative relief and all landform parameters.
	b. `rr`: Compute the relative relief only. (ONLY OUTPUTS ENVI FORMAT RASTERS)
	c. `landforms`: Writes out all landform metrics information.
	d. `shoreline`: Writes out only the shoreline information.
	e. `dunetoe`: Writes out only the dune toe information.
	f. `dunecrest`: Writes out only the dune crest information.
	g. `duneheel`: Writes out only the dune heel information.
	h. `backbarrier`: Writes out only the backbarrier shoreline information.
4. **oFormat** [default: both]: Output file format(s).
	a. `ascii`: Write out only ascii landform metrics.
	b. `envi`: Only output ENVI format rasters.
	c. `both`: Write ascii landform metrics AND ENVI format rasters.
5. **tShoreline** [default: 0.2]: Elevation threshold used to define the shoreline position.
6. **tDT** [default: 0.22]: Relative relief threshold used to define the dune toe position.
7. **tDC** [default: 0.75]: Relative relief threshold used to define the dune crest position.
8. **tDH** [default: 0.4]: Relative relief threshold used to define the dune heel position.
9. **tBB** [default: 1.0]: Elevation threshold used to define the backbarrier shoreline position.
10. **tDuneDistMin** [default: 15]: Minimum distance between the shoreline and dune toe.
11. **tDuneDistMax** [default: 100]: Maximum distance between the shoreline and dune toe.
12. **tCrestDistMin** [default: 1]: Minimum distance between the dune toe and dune crest.
13. **tCrestDistMax** [default: 20]: Maximum distance between the dune toe and dune crest.
14. **tHeelDistMin** [default: 1]: Minimum distance between the dune crest and dune heel.
15. **tHeelDistMax** [default: 20]: Maximum distance between the dune crest and dune heel.
16. **transect_direction** [default: W] Direction to extract landforms in. For example, "W" indicates that the program will start reading the raster from East to West in search of the shoreline, dune toe, dune crest, dune heel, and backbarrier shoreline (in that order). If 'rr' is specified in the `oProduct` field, then this option does not apply since no landform metrics are computed.

## Defaults

The default thresholds are as follows:
	
	- Shoreline:    0.02 masl
	- Dune Toe:     0.2
	- Dune Crest:   0.8
	- Dune Heel:    0.4
	- BB Shoreline: 1.0 masl

Any of the default values can be changed by altering the ```params_rr.ini``` file that accompanies this program.

## Program Function

This program functions by (1) importing an ENVI format DEM (.dat and .h file), (2) computing relative relief across 3 spatial scales, and (3) (OPTIONAL) extracting beach, dune, and island landscape features, and computing landform morphometrics (i.e. height, width, volume) from these landscape features.

Relative relief (RR) of every pixel in the input DEM is first computed \***using a 2D moving window**\*, where the window size is specified by the ```params_rr.ini``` file.

<img src='/images/Figure2.png' alt='Side profile (1D) profile of a transect through a 2D moving window' height=50% width=50%>

The 2D window size will significant affect the ability to extract different landform morphometrics and subsequent landforms.

<img src='/images/Figure3withDI.png' alt='Effect of 2D moving window size on relative relief values' height=50% width=50%>

Landform morphometrics (*i.e.* dune toe, dune crest, dune heel, and all additional derivatives) are extracted along transects of the 2D computed relative relief values.

<img src='/images/Figure5.png' alt='Planview map of where landform morphometrics from this paper' height=50% width=50%>

<img src='/images/Figure6.png' alt='Profile view of where landform morphometrics from this paper align with other approaches' height=50% width=50%>

## Questions and Feedback

This program was developed by Phil Wernette as part of his PhD Thesis from 2017 ("Assessing the Role of Framework Geology on Barrier Island Geomorphology") and *Geomorphology* paper from 2016 (Wernette, P., C. Houser, and M.P. Bishop. (2016) An automated approach for extracting barrier island morphology from digital elevation models. Geomorphology, 262(1), 1-7. http://doi.org/10.1016/j.geomorph.2016.02.024.)

For questions related to this program, please contact:

Phil Wernette [pwernette@usgs.gov]()

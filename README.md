# Relative Relief and Automated Barrier Island Landform Feature Extraction from DEMs

This program implements the approach detailed in:

> Wernette, P., C. Houser, and M.P. Bishop. (2016) An automated approach for extracting barrier island morphology from digital elevation models. Geomorphology, 262(1), 1-7. http://doi.org/10.1016/j.geomorph.2016.02.024.

![Graphical abstract from *Geomorphology* paper](/images/featureextractiongraphicalabstract.png)

## Compiling

This code can be compiled as follows:

```
g++ *.cpp -lm -O2 -o programname.exe
```
or
```
g++ *.cpp -lm -O2 -static -o programname.exe
```

The second compiling option will create a static stand-alone program which may be transferred from one device to another while maintaining function. **NOTE: Compiling with** ```relative_relief.cpp``` **will not work since data_structures.cpp is not also compiled in the process.**

## Purpose and Function

This program functions by (1) importing an ENVI format DEM (.dat and .h file), (2) computing relative relief across 3 spatial scales, and (3) (OPTIONAL) extracting beach, dune, and island landscape features, and computing landform morphometrics (i.e. height, width, volume) from these landscape features.

Relative relief (RR) of every pixel in the input DEM is first computed \***using a 2D moving window**\*, where the window size is specified by the ```params_rr.ini``` file.

![Side profile (1D) profile of a transect through a 2D moving window](/images/Figure2.png)

The 2D window size will significant affect the ability to extract different landform morphometrics and subsequent landforms.

![Effect of 2D moving window size on relative relief values](/images/Figure3withDI.png)

Landform morphometrics (*i.e.* dune toe, dune crest, dune heel, and all additional derivatives) are extracted along transects of the 2D computed relative relief values.

![Planview map of where landform morphometrics from this paper](/images/Figure5.png)

![Profile view of where landform morphometrics from this paper align with other approaches](/images/Figure6.png)

## Inputs

1. **Input filename** (excluding extension)
2. **Window Size**: This is the moving window diameter (*i.e.* kernel size) **in picxels** used to compute relative relief. Two additional scales of relative relief will also be computed by adding 2 to this user-specified window size.
3. **Desired Output Product**:
	- `rr` outputs relative relief rasters only
	- `shoreline` outputs shoreline
	- `dunetoe` outputs dune toe
	- `dunecrest` outputs dune crest
	- `duneheel` outputs dune heel
	- `backbarrier` outputs backbarrier
	- `landforms`: outputs all geomorphic feature parameters
	- `all`: outputs all products
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

## Example Usage
```program.exe sample_ENVI_raster_filename 25 all both```
Read in sample_ENVI_raster_filename as a raster (if available in ENVI format raster), compute relative relief at 25m, 27m, and 29m (assuming the raster has a 1m cell size), and write out all relative relief products as individual rasters, landform metrics rasters, and landform metrics table.

```program.exe sample_ENVI_raster_filename 11 rr envi```
Read in sample_ENVI_raster_filename as a raster (if available in ENVI format raster), compute relative relief at 11m, 13m, and 15m (assuming the raster has a 1m cell size), and write out all relative relief products as individual rasters.

```program.exe sample_ENVI_raster_filename 3 landforms both```
Read in sample_ENVI_raster_filename as a raster (if available in ENVI format raster), compute relative relief at 25m, 27m, and 29m (assuming the raster has a 1m cell size), and write out landform metrics rasters and landform metrics table.

## Questions and Feedback

This program was developed by Phil Wernette as part of his PhD Thesis from 2017 ("Assessing the Role of Framework Geology on Barrier Island Geomorphology") and *Geomorphology* paper from 2016 (Wernette, P., C. Houser, and M.P. Bishop. (2016) An automated approach for extracting barrier island morphology from digital elevation models. Geomorphology, 262(1), 1-7. http://doi.org/10.1016/j.geomorph.2016.02.024.)

For questions related to this program, please contact:

Phil Wernette [pwernette@usgs.gov]()

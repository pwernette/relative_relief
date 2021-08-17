# Relative Relief

This program implements the approach detailed in:

> Wernette, P., C. Houser, and M.P. Bishop. (2016) An automated approach for extracting barrier island morphology from digital elevation models. Geomorphology, 262(1), 1-7. http://doi.org/10.1016/j.geomorph.2016.02.024.

![Graphical abstract from *Geomorphology* paper](/images/featureextractiongraphicalabstract.png)

The purpose of this program is to:

	1. import a ENVI format DEM (.dat and .h file)
	2. extract beach, dune, and island landforms/features, and
	3. compute landform morphometrics (i.e. height, width, volume).

Relative relief (RR) of every pixel in the input DEM is first computed \***using a 2D moving window**\*, where the window size is specified by the ```params_rr.ini``` file.

![Side profile (1D) profile of a transect through a 2D moving window](/images/Figure2.png)

The 2D window size will significant affect the ability to extract different landform morphometrics and subsequent landforms.

![Effect of 2D moving window size on relative relief values](/images/Figure3withDI.png)

Landform morphometrics (*i.e.* dune toe, dune crest, dune heel, and all additional derivatives) are extracted along transects of the 2D computed relative relief values.

![Planview map of where landform morphometrics from this paper](/images/Figure5.png)

![Profile view of where landform morphometrics from this paper align with other approaches](/images/Figure6.png)

The program requires the following inputs:

	- Input filename (excluding extension)
	- window size to calculate statistics
	- desired product:
		rr --> outputs relative relief rasters only
		shoreline --> outputs shoreline
		dunetoe --> outputs dune toe
		dunecrest --> outputs dune crest
		duneheel --> outputs dune heel
		backbarrier --> outputs backbarrier
		landforms --> outputs all geomorphic feature parameters
		all --> outputs all products
	- output data types:
		ascii --> only outputs ascii file
		envi --> only outputs ENVI format raster
		both --> output both ascii and ENVI files


Defaults:
The default thresholds are as follows:
	
	- Shoreline:    0.02 masl
	- Dune Toe:     0.2
	- Dune Crest:   0.8
	- Dune Heel:    0.4
	- BB Shoreline: 1.0 masl

Any of the default values can be changed by altering the ```params_rr.ini``` file that accompanies this program.

Example Usage:
```
program sample_ENVI_raster_filename 25 all both

program.exe sample_ENVI_raster_filename 25 all both
```

This program was developed by Phil Wernette as part of his PhD Thesis from 2017 ("Assessing the Role of Framework Geology on Barrier Island Geomorphology") and *Geomorphology* paper from 2016 (Wernette, P., C. Houser, and M.P. Bishop. (2016) An automated approach for extracting barrier island morphology from digital elevation models. Geomorphology, 262(1), 1-7. http://doi.org/10.1016/j.geomorph.2016.02.024.)

For questions related to this program, please contact:

Phil Wernette [pwernette@usgs.gov]()

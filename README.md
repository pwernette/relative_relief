# Relative Relief

The purpose of this program is to

	- import a ENVI format DEM (.dat and .h file); extract beach, dune, and island topological features; and compute landform width and volume.

The program requires the following inputs:
		1) Input filename (excluding extension)
		
		2) window size to calculate statistics
		
		3) desired product:
			rr --> outputs relative relief rasters only
			
			shoreline --> outputs shoreline
			dunetoe --> outputs dune toe
			dunecrest --> outputs dune crest
			duneheel --> outputs dune heel
			backbarrier --> outputs backbarrier
			
			landforms --> outputs all geomorphic feature parameters
			
			all --> outputs all products
			
		4) output data types:
			ascii --> only outputs ascii file
			envi --> only outputs ENVI format raster
			both --> output both ascii and ENVI files

	Example Usage:
		program sample 11 all both
					OR
		program.exe npilidar 11 all both
		
NOTE: At North Padre Island, we found an ideal window size to be approximately 25 meters.

NOTE: The default thresholds are as follows:
	Shoreline:    0.02 masl
	Dune Toe:     0.2
	Dune Crest:   0.8
	Dune Heel:    0.4
	BB Shoreline: 1.0 masl

NOTE: This program assumes that the grid resolution is relatively small; therefore, a simple Pythagorean theorem distance is appropriate when calculating metrics.

NOTE: This assumption is not valid for distances larger than ~12 miles (~19 km), since the curvature of the Earth will affect the 3D distance.
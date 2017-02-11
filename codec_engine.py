#Author: Irfan Khan (khanirf1) 
#Student Number: 999207665
#Description: This script Upsamples a YUV420 Video Sequence to YUV444

import os
import sys

def main(argv):

	# Global Variables
	# ------------------------------
	infile 		= ""
	outfile		= ""
	width		= 0
	height 		= 0
	frames		= 0
	searchrange	= 0
	rounding	= 0

-width 352 
-height 288 
-curfile 'C:\Users\ikhan1\Documents\ECE1718\foreman_cif_Y_352x288.yuv' 
-recfile 'C:\Users\ikhan1\Documents\ECE1718\reconstructed.yuv' 
-mvfile '\C:\Users\ikhan1\Documents\ECE1718\mvfile.txt'
-resfile 'C:\Users\ikhan1\Documents\ECE1718\residual.yuv'
-frames 100 
-block 2 
-range 1 
-round 2 
-gmvx 'C:\Users\ikhan1\Documents\ECE1718\GMVX' 
-gmvy 'C:\Users\ikhan1\Documents\ECE1718\GMVY'

	# Parse Input Arguments 
	# ------------------------------
	for index in range(len(argv)):

		if (argv[index] == "-infile"):
			infile = argv[index+1]

		if (argv[index] == "-outfile"):
			outfile = argv[index+1]

		if (argv[index] == "-width"):
			width = int(argv[index+1])

		if (argv[index] == "-height"):
			height = int(argv[index+1])

		if (argv[index] == "-frames"):
			frames = int(argv[index+1])

		if (argv[index] == "-range"):
			searchrange = int(argv[index+1])

		if (argv[index] == "-round"):
			rounding = int(argv[index+1])

	# YUV Pre-processing
	# -------------------------------
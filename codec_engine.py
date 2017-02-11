#Author: Irfan Khan (khanirf1) 
#Student Number: 999207665
#Description: The Codec Engine for Assignment 1 

import os
import sys

def main(argv):

	# Global Variables
	# ------------------------------
	testname	= ""
	infile 		= ""
	outfile		= ""
	int width		= 0
	int height 		= 0
	int frames		= 0
	int searchrange	= 0
	int rounding	= 0
	int with_old	= 0
	int height_old  = 0

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

	# YUV Pre-Processing
	# -------------------------------

	# LumaExtractor
	LumaExtractor/LumaExtractor.exe -frames frames -filein infile -width width -height height -outfile testname_LumaExtracted_frames@widthxheight.yuv 


	height_old	= height
	width_old	= width

	if (( width% block) > 0):
		width = (width/block)*block + 1;

	if ((height%block) > 0);
		height = (height/block)*block + 1;

	# LumaPadder
	LumaPad/LumaPad.exe -frames frames -width width_old -height height_old -block block -filein testname_LumaExtracted_frames@widthxheight.yuv -fileout testname_LumaPadded_frames@widthxheight.yuv

	# Encoder
	# -------------------------------
	Encoder/Encoder.exe -frames frames -width width -height height -block block -range searchrange -round rounding -curfile testname_LumaPadded_frames@widthxheight.yuv -recfile testname_Reconstructed_frames@widthxheight.yuv -resfile testname_Residual_frames@widthxheight.yuv -mvfile testname_mvfile.txt -gmvx GMVX -gmvy GMVY

	# Decoder
	# -------------------------------
	Decoder/Decoder.exe -frames frames -width width -height height -block block 

	# Verification
	# -------------------------------

	# Analysis
	# --------------------------------
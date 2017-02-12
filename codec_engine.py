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
	command		= ""
	width		= 0
	height 		= 0
	frames		= 0
	searchrange	= 0
	rounding	= 0
	with_old	= 0
	height_old  = 0
	block 		= 0

	# Parse Input Arguments 
	# ------------------------------
	for index in range(len(argv)):

		if (argv[index] == "-infile"):
			infile = argv[index+1]

		if (argv[index] == "-outfile"):
			outfile = argv[index+1]

		if (argv[index] == "-test"):
			testname = argv[index+1]

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

		if (argv[index] == "-block"):
			block = int(argv[index+1])


	# LumaExtractor
	# -------------------------------
	command = "LumaExtractor\Debug\LumaExtractor.exe -frames %d -filein %s -width %d -height %d -fileout %s_LumaExtracted_%dx%d@%df.yuv" %(frames, infile, width, height, testname, width, height, frames)
	print ("\nLumaExtractor\n" + command)
	os.system (command)

	width_old	= width
	height_old	= height

	if ((height%block) > 0):
		height = int(height/block) * block + block;
	if (( width % block) > 0):
		width = int(width/block) * block + block;

	# LumaPadder
	# -------------------------------
	command = "LumaPad\Debug\LumaPad.exe -frames %d -width %d -height %d -block %d -filein %s_LumaExtracted_%dx%d@%df.yuv -fileout %s_LumaPadded_%dx%d@%df.yuv" %(frames,width_old,height_old,block,testname,width_old,height_old,frames,testname,width,height,frames)
	print ("\nLumaPadder:\n" + command)
	os.system (command)

	# Encoder
	# -------------------------------
	command = "Encoder\Debug\Encoder.exe -frames %d -width %d -height %d -block %d -range %d -round %d -curfile %s_LumaPadded_%dx%d@%df.yuv -recfile %s_Reconstructed_%dx%d@%df.yuv -resfile %s_Residual_%dx%d@%df.yuv -mvfile %s_mvfile.txt -gmvx %s_GMVX -gmvy %s_GMVY" %(frames,width,height,block,searchrange,rounding,testname,width,height,frames,testname,width,height,frames,testname,width,height,frames,testname,testname,testname)
	print ("\nEncoder:\n" + command)
	os.system(command)

	# Decoder
	# -------------------------------
	command = "Decoder\Debug\Decoder.exe -frames %d -width %d -height %d -block %d -decodedfile %s_Decoded_%dx%d@%df.yuv -resfile %s_Residual_%dx%d@%df.yuv -mvxfile %s_GMVX -mvyfile %s_GMVY" %(frames,width,height,block,testname,width,height,frames,testname,width,height,frames,testname,testname)
	print ("\nDecoder:\n" + command)
	os.system(command)

	# DIFF
	# --------------------------------
	command = "DIFF\Debug\DIFF.exe -frames %d -width %d -height %d -recfile %s_Reconstructed_%dx%d@%df.yuv -decfile %s_Decoded_%dx%d@%df.yuv" %(frames,width,height,testname,width,height,frames,testname,width,height,frames)
	print ("\nDIFF:\n" + command)
	os.system(command)

	# Analyze SAD
	# --------------------------------
	command = "AnalyzeSAD\Debug\AnalyzeSAD.exe -frames %d -width %d -height %d -reffile %s_LumaPadded_%dx%d@%df.yuv -decfile %s_Decoded_%dx%d@%df.yuv -SAD %s_SAD.txt" %(frames,width,height,testname,width,height,frames,testname,width,height,frames,testname)
	print ("\nAnalyze SAD:\n" + command)
	os.system(command)

main (sys.argv)
#Author: Irfan Khan (khanirf1) 
#Student Number: 999207665
#Description: The Codec Engine for Assignment 2 

import os
import sys

def main(argv):

	# Global Variables
	width			= 0
	height 			= 0
	frames			= 0
	searchrange		= 0
	with_old		= 0
	height_old  	= 0
	block 			= 0
	qp              = 0
	i_period	 	= 0
	nRefFrames		= 0
	VBSEnable		= 0

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

		if (argv[index] == "-block"):
			block = int(argv[index+1])

		if (argv[index] == "-qp"):
			qp = int(argv[index+1])

		if (argv[index] == "-i_period"):
			i_period = int(argv[index+1])

		if (argv[index] == "-nRefFrames"):
			nRefFrames = int(argv[index+1])

		if (argv[index] == "-VBSEnable"):
			VBSEnable = int(argv[index+1])

  	# LumaExtractor
	# ------------------------------
	luma_extract_command  = "LumaExtractor\Debug\LumaExtractor.exe"
	luma_extract_command += " -frames %d" 						                %(frames)
	luma_extract_command += " -filein %s" 						                %(infile)
	luma_extract_command += " -width %d"						                %(width)
	luma_extract_command += " -height %d"  						                %(height)
	luma_extract_command += " -fileout testdata\%s_LumaExtracted_%dx%d@%df.yuv"	%(testname, width, height, frames)
	
	print ("\nLumaExtractor:\n" + luma_extract_command)
	os.system (luma_extract_command)

	width_old	= width
	height_old	= height

	if ((height%block) > 0):
		height = int(height/block) * block + block;
	if (( width % block) > 0):
		width = int(width/block) * block + block;

	# LumaPadder
	# -------------------------------
	luma_pad_command  = "LumaPad\Debug\LumaPad.exe"
	luma_pad_command += " -frames %d"						                    %(frames)
	luma_pad_command += " -width %d"							                %(width_old)
	luma_pad_command += " -height %d"						                    %(height_old)
	luma_pad_command += " -block %d"							                %(block)
	luma_pad_command += " -filein testdata\%s_LumaExtracted_%dx%d@%df.yuv"		%(testname,width_old,height_old,frames)
	luma_pad_command += " -fileout testdata\%s_LumaPadded_%dx%d@%df.yuv" 		%(testname,width,height,frames)
	
	print ("\nLumaPadder:\n" + luma_pad_command)
	os.system (luma_pad_command)

	# Encoder
	# -------------------------------
	encode_command  = "Encoder\Debug\Encoder.exe"
	encode_command += " -width %d"							                    %(width)
	encode_command += " -height %d"						    	                %(height)
	encode_command += " -curfile testdata\%s_LumaPadded_%dx%d@%df.yuv"		    %(testname,width,height,frames)
	encode_command += " -recfile testdata\%s_Reconstructed_%dx%d@%df.yuv"		%(testname,width,height,frames)
	encode_command += " -mdiff_bitcount_name testdata\%s_MDIFF_BITCOUNT.txt"	%(testname)
	encode_command += " -frames %s" 						                    %(frames)
	encode_command += " -block %s"							                    %(block)
	encode_command += " -range %s"							                    %(searchrange)
	encode_command += " -coeff_bitcount_name testdata\%s_COEFF_BITCOUNT.txt"	%(testname)
	encode_command += " -qp %d" 							                    %(qp)
	encode_command += " -i_period %d"						                    %(i_period)
	encode_command += " -nRefFrames %d%" 						                %(nRefFrames)
	encode_command += " -VBSEnable %d"						                    %(VBSEnable)
	encode_command += " -frame_header testdata\%s_FRAME_HEADER.txt"             %(testname)

	print ("\nEncoder:\n" + encode_command)
	os.system(encode_command)

	# Decoder
	# -------------------------------
	decode_command  = "Decoder\Debug\Decoder.exe"
	decode_command += " -frames %d" 									        %(frames)
	decode_command += " -qp %d"  									            %(qp)
	decode_command += " -decfile testdata\%s_Decoded_%dx%d@%df.yuv"				%(testname,width,height,frames)
	decode_command += " -filepath Decoder\\"
	decode_command += " -frame_header testdata\%s_FRAME_HEADER.txt" 	        %(testname)

	print ("\nDecoder:\n" + decode_command)
	os.system(decode_command)

    # DIFF
	# --------------------------------
	diff_command  = "DIFF\Debug\DIFF.exe" 
	diff_command += " -frames %d"                                               %(frames)
	diff_command += " -width %d"                                                %(width)
	diff_command += " -height %d"                                               %(height)
	diff_command += " -recfile testdata\%s_Reconstructed_%dx%d@%df.yuv"         %(testname,width,height,frames)
	diff_command += " -decfile testdata\%s_Decoded_%dx%d@%df.yuv"               %(testname,width,height,frames)

	print ("\nDIFF:\n" + diff_command)
	os.system(diff_command)

	# Analyze Frame
	# --------------------------------
	analyze_command = "AnalyzeFrame\Debug\AnalyzeFrame.exe"
	analyze_command += " -frames %d"                                            %(frames)
	analyze_command += " -width %d"                                             %(width)
	analyze_command += " -height %d"                                            %(height)
	analyze_command += " -reffile testdata\%s_LumaPadded_%dx%d@%df.yuv"         %(testname,width,height,frames)
	analyze_command += " -decfile testdata\%s_Decoded_%dx%d@%df.yuv"            %(testname,width,height,frames)
	analyze_command += " -SAD testdata\%s_SAD.csv"                              %(testname)
	analyze_command += " -PSNR testdata\%s_PSNR.csv"                            %(testname)

	print ("\nAnalyze Frames:\n" + analyze_command)
	os.system(analyze_command)

main (sys.argv)

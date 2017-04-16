#Author: Irfan Khan (khanirf1) 
#Student Number: 999207665
#Description: The Codec Engine for Assignment 2 

import os
import sys

def main(argv):

	# Global Variables
	infile			= ""
	outfile			= ""
	width			= 0
	height 			= 0
	frames			= 0

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


    	# DIFF
	# --------------------------------
	diff_command  = "DIFF\Debug\DIFF.exe" 
	diff_command += " -frames %d"                                               %(frames)
	diff_command += " -width %d"                                                %(width)
	diff_command += " -height %d"                                               %(height)
	diff_command += " -recfile testdata\%s"						%(infile)
	diff_command += " -decfile testdata\%s"              			 %(outfile)

	print ("\nDIFF:\n" + diff_command)
	os.system(diff_command)

main (sys.argv)

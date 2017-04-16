import os
import sys

testcases = ["TestCIF_PMode_1_VBS_1_Reconstructed_352x288@21f.yuv","TestCIF_PMode_2_VBS_1_Reconstructed_352x288@21f.yuv","TestCIF_PMode_3_VBS_1_Reconstructed_352x288@21f.yuv"]

width = 352
height = 288
frames = 21

for testcase in testcases:
	command = "compare.py -infile TestCIF_PMode_0_VBS_1_Reconstructed_352x288@21f.yuv -outfile %s -width 352 -height 288 -frames 21" % (testcase)
	print ("\n" + testcase + "\n==================\n" + command + "\n")
	os.system(command)

import os
import sys
import csv

testcases = ["IP_1_Block_8_QP_0","IP_1_Block_8_QP_3","IP_1_Block_8_QP_6","IP_1_Block_8_QP_9"]
for testname in testcases:
	input = open ("testdata\\" + testname + "_COEFF_BITCOUNT.csv",'r')
	for line in input:
		testname.rstrip('\n')
		line.rstrip('\n')

		#print (line)
		print (testname + "\t" + line, end='')
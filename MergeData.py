import os
import sys
import csv

block			= 8
qp_list 		= [0,3,6,9]
i_period_list 		= [1,4,10]

for i_period in i_period_list:

	coeff_bits	= [[0]*4 for k in range(10)];
	diff_bits	= [[0]*4 for k in range(10)];
	psnr		= [[0]*4 for k in range(10)];
	runtime 	= [[0]*4 for k in range(10)];
	
	testgroup = "IP_%d_Block_%d" %(i_period,block)
	output = open ("%s.csv" %(testgroup),'w')
	output.write("%s\n" %testgroup)

	output.write("COEFF BITS\n")
	for qp in qp_list:
		output.write("QP = %d," %(qp))
	output.write("\n")

	file_iterator = 0
	for qp in qp_list:
		testname = "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)
		input = open ("testdata\\" + testname + "_COEFF_BITCOUNT.csv",'r')
		iterator = 0
		for line in input:
			frame,coeff_bits[iterator][file_iterator] = line.strip().split(",")
			iterator = iterator + 1
		file_iterator = file_iterator + 1
		
	for j in range(0,10):
		for i in range(0,4):
			output.write(coeff_bits[j][i] + ",")
		output.write("\n")
	output.write("\n")
		
	output.write("MDIFF BITS\n")
	for qp in qp_list:
		output.write("QP = %d," %(qp))
	output.write("\n")

	file_iterator = 0
	for qp in qp_list:
		testname = "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)
		input = open ("testdata\\" + testname + "_MDIFF_BITCOUNT.csv",'r')
		iterator = 0
		for line in input:
			frame,diff_bits[iterator][file_iterator] = line.strip().split(",")
			iterator = iterator + 1
		file_iterator = file_iterator + 1
	
	for j in range(0,10):
		for i in range(0,4):
			output.write(diff_bits[j][i] + ",")
		output.write("\n")
	output.write("\n")
	

	output.write ("PSNR\n")

	for qp in qp_list:
		output.write("QP = %d," %(qp))
	output.write("\n")

	file_iterator = 0
	for qp in qp_list:
		testname = "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)
		input = open ("testdata\\" + testname + "_PSNR.csv",'r')
		iterator = 0
		for line in input:
			frame,psnr[iterator][file_iterator] = line.strip().split(",")
			iterator = iterator + 1
		file_iterator = file_iterator + 1
	
	for j in range(0,10):
		for i in range(0,4):
			output.write(psnr[j][i] + ",")
		output.write("\n")
	output.write("\n")
	
	
	output.write("RUN_TIME\n")
	for qp in qp_list:
		output.write("QP = %d," %(qp))
	output.write("\n")

	for qp in qp_list:
		testname = "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)
		input = open ("testdata\\" + testname + "_RUNTIME.csv",'r')
		for line in input:
			runtime = line.strip()
			output.write(runtime + ",")



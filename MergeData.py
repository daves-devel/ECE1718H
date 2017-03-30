import os
import sys
import csv

qp_list 		= [0,1,2,3,4,5,6,7,8,9,10,11]
resource 		= ["TestCIF","TestQCIF"]
i_period		= [1,21]

for yuv in resource:

	for ip in i_period:

		testgroup = "%s_IP_%d" % (yuv,ip)
		output = open ("%s.csv" %(testgroup),'w')
		output.write("%s\n" %testgroup)

		bitsPerRow = [[0]*12 for k in range(21)]
		
		for qp in qp_list:
			
			testcase = "%s_QP_%d_Block_16_VBS_FME_BITCOUNT_ROW" % (testgroup,qp)
			input = open("testdata\\" + testcase + ".csv", 'r')
			iterator = 0
			for line in input:
				frametype,frame,bitsPerRow[iterator][qp] = line.strip().split(",") 
				#int (bitsPerRow[iterator][qp])
				#print ("%s,%s,%d" %(frametype,frame,bitsPerRow[iterator][qp]))
				iterator = iterator + 1
		
		for i in range (0,12):
			output.write("%d" %(i) + ",")
		output.write("\n")

		for j in range(0,21):
			for i in range(0,12):
				output.write("%i," %(int(bitsPerRow[j][i])))
			output.write("\n")



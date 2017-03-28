import os
import sys

# Available Parameters
testname		= ""
infile			= ""
width			= 0
height 			= 0
frames			= 0
searchrange		= 0
block 			= 0
qp              = 0
i_period	 	= 0
nRefFrames		= 0
VBSEnable		= 0
RDOEnable       = 0
FMEnable        = 0


# TRUE FOR ALL THESE TEST CASES
infile			= "resource\\foreman_cif.yuv"
width			= 352
height 			= 288
frames			= 10
searchrange		= 2
nRefFrames		= 1
VBSEnable		= 0
RDOEnable       	= 0
FMEnable        	= 0

# BLOCK = 8 TEST CASES RDO OFF
# ====================================================================================================================================
block 			= 8
qp_list 		= [0,1,2,4,5,6,7,8,9,10,11]
i_period_list 	= [1,4,10]

for i_period in i_period_list:
	for qp in qp_list:
		testname 		= "Block_%d_IP_%d_QP_%d_RDO_%d" %(block,i_period,qp,RDOEnable)
		command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
		print ("\n" + testname + "\n=======================\n" + command+"\n")
		os.system(command)

# BLOCK = 16 TEST CASES RDO OFF
# ====================================================================================================================================
block 			= 16
qp_list 		= [1,4,7,10]
i_period_list 	= [1,4,10]

for i_period in i_period_list:
	for qp in qp_list:
		testname 		= "Block_%d_IP_%d_QP_%d_RDO_%d" %(block,i_period,qp,RDOEnable)		
		command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
		print ("\n" + testname + "\n=======================\n" + command+"\n")
		os.system(command)

RDOEnable = 1

# BLOCK = 8 TEST CASES RDO ON
# ====================================================================================================================================
block 			= 8
qp_list 		= [0,3,6,9]
i_period_list 	= [1,4,10]

for i_period in i_period_list:
	for qp in qp_list:
		testname 		= "Block_%d_IP_%d_QP_%d_RDO_%d" %(block,i_period,qp,RDOEnable)
		command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
		print ("\n" + testname + "\n=======================\n" + command+"\n")
		os.system(command)

# BLOCK = 16 TEST CASES
# ====================================================================================================================================
block 			= 16
qp_list 		= [1,4,7,10]
i_period_list 	= [1,4,10]

for i_period in i_period_list:
	for qp in qp_list:
		testname 		= "Block_%d_IP_%d_QP_%d_RDO_%d" %(block,i_period,qp,RDOEnable)		
		command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
		print ("\n" + testname + "\n=======================\n" + command+"\n")
		os.system(command)



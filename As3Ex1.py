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
RCflag			= 0


# TRUE FOR ALL THESE TEST CASES
frames			= 21
searchrange		= 8
block 			= 16
nRefFrames		= 1
VBSEnable		= 1
RDOEnable       = 1
FMEnable        = 1
qp_list 		= [0,1,2,3,4,5,6,7,8,9,10,11]
i_period_list 	= [21]
RCflag			= 0
targetBr = 2400000

# CIF TEST CASES
# ====================================================================================================================================

infile = "resource\\TestCIF.yuv"
width  = 352
height = 288

for i_period in i_period_list:
	for qp in qp_list:
		testname 		= "TestCIF_IP_%d_QP_%d_Block_16_VBS_FME" %(i_period,qp)
		command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d -targetBr %d -RCflag %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period, targetBr, RCflag)
		print ("\n" + testname + "\n=======================\n" + command+"\n")
		os.system(command)

# QCIF TEST CASES
# ====================================================================================================================================

infile = "resource\\TestQCIF.yuv"
width  = 176
height = 144

i_period_list = [1,21]

for i_period in i_period_list:
	for qp in qp_list:
		testname 		= "TestQCIF_IP_%d_QP_%d_Block_16_VBS_FME" %(i_period,qp)
		command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d -targetBr %d -RCflag %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period, targetBr, RCflag)
		print ("\n" + testname + "\n=======================\n" + command+"\n")
		os.system(command)



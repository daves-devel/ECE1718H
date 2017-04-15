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
i_period 	= 21
RCflag			= 0


# CIF TEST CASES
# ====================================================================================================================================

infile = "resource\\TestCIF.yuv"
width  = 352
height = 288
targetBr = 2400000
RCflag	 = 0
qp_list = [3,6,9]
#testcase 1
for qp in qp_list:
	testname 		= "TestCIF_IP_%d_QP_%d_Block_16_VBS_FME_CONSTANT_QP_E2" %(i_period,qp)
	command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d -targetBr %d -RCflag %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period,targetBr,RCflag)
	print ("\n" + testname + "\n=======================\n" + command+"\n")
	os.system(command)
		
infile = "resource\\TestCIF.yuv"
width  = 352
height = 288
targetBr_list = [7000000,2400000,360000]


rc_list = [1,2, 3]
qp=4		
#qp=9
#testcase 2
for targetBr in targetBr_list:
	for RCflag in rc_list:
		testname 		= "TestCIF_IP_%d_BR_%d_RC_%d_Block_16_VBS_FME_E2" %(i_period,targetBr,RCflag)
		command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d -targetBr %d -RCflag %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period,targetBr,RCflag)
		print ("\n" + testname + "\n=======================\n" + command+"\n")
		os.system(command)


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
VBSEnable		= 0
RDOEnable       = 1
FMEnable        = 1
i_period 		= 4
RCflag			= 0
targetBr = 0


# CIF TEST CASES
# ====================================================================================================================================

infile = "resource\\TestCIF.yuv"
width  = 352
height = 288
#qp_list = [0, 1 , 2, 3,4,5,6,7,8,9]
qp_list = [3,6,9]
deblock_on = 0
#testcase 1
for qp in qp_list:
	testname 		= "TestCIF_Deblock_%d_QP_%d_Block_16_VBS_FME_CONSTANT_QP_E24" %(deblock_on,qp)
	command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d -targetBr %d -RCflag %d -deblock_on %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period,targetBr,RCflag, deblock_on)
	print ("\n" + testname + "\n=======================\n" + command+"\n")
	os.system(command)
infile = "resource\\TestCIF.yuv"
width  = 352
height = 288
#qp_list = [0,1,2,3,4,5,6,7,8,9]
qp_list = [3,6,9]
deblock_on = 1
for qp in qp_list:
	testname 		= "TestCIF_Deblock_%d_QP_%d_Block_16_VBS_FME_CONSTANT_QP_E24" %(deblock_on,qp)
	command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d -targetBr %d -RCflag %d -deblock_on %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period,targetBr,RCflag, deblock_on)
	print ("\n" + testname + "\n=======================\n" + command+"\n")
	os.system(command)
	
	



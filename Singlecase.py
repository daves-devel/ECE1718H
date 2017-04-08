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
qp              	= 0
i_period	 	= 0
nRefFrames		= 0
VBSEnable		= 0
RDOEnable       	= 0
FMEnable        	= 0


# TRUE FOR ALL THESE TEST CASES
infile			= "resource\\TestCIF.yuv"
width			= 352
height 			= 288
frames			= 10
searchrange		= 2
nRefFrames		= 1
VBSEnable		= 0
RDOEnable       	= 0
FMEnable        	= 1
ParallelMode		= 1
block 			= 8
qp 			= 1
i_period 		= 3

testname 		= "Block_%d_IP_%d_QP_%d_RDO_%d" %(block,i_period,qp,RDOEnable)
command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)




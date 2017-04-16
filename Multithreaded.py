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
frames			= 2
searchrange		= 8
nRefFrames		= 1
RDOEnable       	= 0
FMEnable        	= 1
block 			= 16
qp 			= 0
i_period 		= 10

# Different Test Cases
ParallelModes	=	[0,1,2,3]
VBSModes	= 	[0,1]

for ParallelMode in ParallelModes:
	for VBSEnable in VBSModes:
		testname 		= "TestCIF_PMode_%d_VBS_%d" %(ParallelMode,VBSEnable)
		command 		= "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnable %d -block %d -qp %d -i_period %d -ParallelMode %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period,ParallelMode)
		print ("\n" + testname + "\n=======================\n" + command+"\n")
		os.system(command)




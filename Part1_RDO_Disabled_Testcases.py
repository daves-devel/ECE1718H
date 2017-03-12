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
RDOEnable       = 0
FMEnable        = 0

# ====================================================================================================================================
#
# BLOCK = 8 TEST CASES
#
# ====================================================================================================================================

# IPERIOD = 1 BLOCK = 8 
# ==================================

block 			= 8
qp              = 0
i_period	 	= 1
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 8
qp              = 3
i_period	 	= 1
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 8
qp              = 6
i_period	 	= 1
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 8
qp              = 9
i_period	 	= 1
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

# IPERIOD = 4 BLOCK = 8 
# ==================================

block 			= 8
qp              = 0 
i_period	 	= 4
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 8
qp              = 3
i_period	 	= 4
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 8
qp              = 6
i_period	 	= 4
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 8
qp              = 9
i_period	 	= 4
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

# IPERIOD = 10 BLOCK = 8  
# ==================================

block 			= 8
qp              = 0
i_period	 	= 10
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 8
qp              = 3
i_period	 	= 10
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 8
qp              = 6
i_period	 	= 10
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 8
qp              = 9
i_period	 	= 10
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

# ====================================================================================================================================
#
# BLOCK = 16 TEST CASES
#
# ====================================================================================================================================

# IPERIOD = 1 BLOCK = 16
# ==================================

block 			= 16
qp              = 1
i_period	 	= 1
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 16
qp              = 4
i_period	 	= 1
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 16
qp              = 7
i_period	 	= 1
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 16
qp              = 10
i_period	 	= 1
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

# IPERIOD = 4 BLOCK = 16 
# ==================================

block 			= 16
qp              = 1
i_period	 	= 4
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 16
qp              = 4
i_period	 	= 4
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 16
qp              = 7
i_period	 	= 4
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 16
qp              = 10
i_period	 	= 4
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

# IPERIOD = 10 BLOCK = 16 
# ==================================

block 			= 16
qp              = 1
i_period	 	= 10
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 16
qp              = 4
i_period	 	= 10
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 16
qp              = 7
i_period	 	= 10
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

block 			= 16
qp              = 10
i_period	 	= 10
testname 		= "IP_%d_Block_%d_QP_%d" %(i_period,block,qp)

command = "codec_engine.py -infile %s -testname %s -width %d -height %d -frames %d -searchrange %d -nRefFrames %d -VBSEnable %d -RDOEnable %d -FMEnabled %d -block %d -qp %d -i_period %d" %(infile,testname,width,height,frames,searchrange,nRefFrames,VBSEnable,RDOEnable,FMEnable,block,qp,i_period)
print ("\n" + testname + "\n=======================\n" + command+"\n")
os.system(command)

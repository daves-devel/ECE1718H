import os
import sys

################################################################################################################################################################
# FOREMAN TEST CASES
################################################################################################################################################################

print ("\nForeman_Required\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 10 -range 4 -round 3 -block 8 -test Foreman_Required"
os.system(command)

print ("\nForeman_Varying_BlockSize_2\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 3 -block 2 -test Foreman_Varying_BlockSize_2"
os.system(command)

print ("\nForeman_Varying_BlockSize_8\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 3 -block 8 -test Foreman_Varying_BlockSize_8"
os.system(command)

print ("\nForeman_Varying_BlockSize_64\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 3 -block 64 -test Foreman_Varying_BlockSize_64"
os.system(command)

print ("\nForeman_Varying_SearchRange_1\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 100 -range 1 -round 3 -block 8 -test Foreman_Varying_SearchRange_1"
os.system(command)

print ("\nForeman_Varying_SearchRange_4\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 3 -block 8 -test Foreman_Varying_SearchRange_4"
os.system(command)

print ("\nForeman_Varying_SearchRange_8\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 100 -range 8 -round 3 -block 8 -test Foreman_Varying_SearchRange_8"
os.system(command)

print ("\nForeman_Varying_Rounding_1\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 1 -block 8 -test Foreman_Varying_Rounding_1"
os.system(command)

print ("\nForeman_Varying_Rounding_2\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 2 -block 8 -test Foreman_Varying_Rounding_2"
os.system(command)

print ("\nForeman_Varying_Rounding_3\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 3 -block 8 -test Foreman_Varying_Rounding_3"
os.system(command)

################################################################################################################################################################
# STEFAN TEST CASES
################################################################################################################################################################
print ("\nStefan_Varying_BlockSize_2\n=======================\n")
command = r"codec_engine.py -infile resource\stefan_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 3 -block 2 -test Stefan_Varying_BlockSize_2"
os.system(command)

print ("\nStefan_Varying_BlockSize_8\n=======================\n")
command = r"codec_engine.py -infile resource\stefan_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 3 -block 8 -test Stefan_Varying_BlockSize_8"
os.system(command)

print ("\nStefan_Varying_BlockSize_64\n=======================\n")
command = r"codec_engine.py -infile resource\stefan_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 3 -block 64 -test Stefan_Varying_BlockSize_64"
os.system(command)

print ("\nStefan_Varying_SearchRange_1\n=======================\n")
command = r"codec_engine.py -infile resource\stefan_cif.yuv -width 352 -height 288 -frames 100 -range 1 -round 3 -block 8 -test Stefan_Varying_SearchRange_1"
os.system(command)

print ("\nStefan_Varying_SearchRange_4\n=======================\n")
command = r"codec_engine.py -infile resource\stefan_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 3 -block 8 -test Stefan_Varying_SearchRange_4"
os.system(command)

print ("\nStefan_Varying_SearchRange_8\n=======================\n")
command = r"codec_engine.py -infile resource\stefan_cif.yuv -width 352 -height 288 -frames 100 -range 8 -round 3 -block 8 -test Stefan_Varying_SearchRange_8"
os.system(command)

print ("\nStefan_Varying_Rounding_1\n=======================\n")
command = r"codec_engine.py -infile resource\stefan_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 1 -block 8 -test Stefan_Varying_Rounding_1"
os.system(command)

print ("\nStefan_Varying_Rounding_2\n=======================\n")
command = r"codec_engine.py -infile resource\stefan_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 2 -block 8 -test Stefan_Varying_Rounding_2"
os.system(command)

print ("\nStefan_Varying_Rounding_3\n=======================\n")
command = r"codec_engine.py -infile resource\stefan_cif.yuv -width 352 -height 288 -frames 100 -range 4 -round 3 -block 8 -test Stefan_Varying_Rounding_3"
os.system(command)


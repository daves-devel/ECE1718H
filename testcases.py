import os
import sys

print ("\nSimpleTest\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 100 -range 1 -round 2 -block 64 -test SimpleTest"
os.system(command)

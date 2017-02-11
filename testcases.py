import os
import sys

print ("\nTestCase1\n=======================\n")
command = r"codec_engine.py -infile resource\foreman_cif.yuv -width 352 -height 288 -frames 10 -range 1 -round 2 -block 2 -test TestCase1"
os.system(command)
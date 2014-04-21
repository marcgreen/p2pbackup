#!/usr/bin/python

import os
import sys

def gen_file(numMs, numGs):
    oneMBString = '0' * 1024 * 1024
    f = open(fname, 'w')
    
    print("Generating " + fname + " (this might take a while)")
    
    if numGs == 0:
        for mb in range(0, numMs): # Number of MB
            f.write(oneMBString)
    else:
        for gb in range(0, numGs): # Number of GB
            for mb in range(0, numMs): # Number of MB
                f.write(oneMBString)
            print(str(gb + 1) + 'GB')

if len(sys.argv) < 3 or (sys.argv[1] != '1M'  and sys.argv[1] != '10M' and sys.argv[1] != '100M' and sys.argv[1] != '1G' and sys.argv[1] != '10G'):
    print("Usage: " + os.path.basename(__file__) + " [1M|10M|100M|1G|10G] output_file")
    sys.exit()

fsize = sys.argv[1]
fname = sys.argv[2]

if fsize == '1M':
    numMs = 1
    numGs = 0
elif fsize == '10M':
    numMs = 10
    numGs = 0
elif fsize == '100M':
    numMs = 100
    numGs = 0
elif fsize == '1G':
    numMs = 1024
    numGs = 1
else:
    numMs = 1024
    numGs = 10

gen_file(numMs, numGs)

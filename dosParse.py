#!/usr/bin/python3

"""
Writes the total DOS as a function of energy.  Accounts for spin pol if necessary.
The format is suitable for my text plotter
"""

import sys

inPATH = "DOSCAR"
outPATH = "doss"

OFFSET = 5 #number of lines before the first NEDOS is written to DOSCAR file

def IsSpinPol(filePath, off):
    ret = False
    with open(filePath, 'r') as infile:
        for i, line in enumerate(infile):
            if(i >= off):
                if(len(line.split()) > 3):
                    ret = True
                break
        
        infile.close()
    return ret

#Command Line
i = 0
while(i < len(sys.argv)):
    if(sys.argv[i][0] == '-' and (sys.argv[i][1] == 'i' or sys.argv[i][1] == 'I')):
        inPATH = str(sys.argv[i+1])
    if(sys.argv[i][0] == '-' and (sys.argv[i][1] == 'o' or sys.argv[i][1] == 'O')):
        outPATH = str(sys.argv[i+1])
    i = i + 1

#Spin pol determination
sp = False
if(IsSpinPol(inPATH, OFFSET + 1)):
    sp = True

nedos = float('inf')
energy, dosU, dosD = [], [], []
#Read file
with open(inPATH, 'r') as infile:
    for i, line in enumerate(infile):
        if(i == OFFSET):
            nedos = int(line.split()[2])
        if(i > OFFSET):
            energy.append(line.split()[0])
            dosU.append(line.split()[1])
            dosD.append(line.split()[2])            
        if(i - OFFSET >= nedos):
            break


    infile.close()

#Print Results
with open(outPATH, 'w') as outfile:
    #For Spin Pol
    if(sp):
        outfile.write(str(2*nedos))
        for i in range(0, 2):
            for e in energy:
                outfile.write(" " + str(e))              
        for du in dosU:
            outfile.write(" " + str(du))
        for dd in dosD:
            outfile.write(" " + str(dd))                  
    #For non Spin Pol
    else:
        outfile.write(str(nedos))    
        for e in energy:
            outfile.write(" " + str(e))              
        for du in dosU:
            outfile.write(" " + str(du))    

    outfile.close()












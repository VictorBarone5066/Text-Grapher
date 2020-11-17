#!/usr/bin/python3

"""
Writes the magnitude of the largest force on an atom as a function of step.
The format is suitable for my text plotter
"""

import sys
import re

inPATH = "OUTCAR"
outPATH = "forces"
ION = 0 #zero for the max of all ions

def magnitude(a, b, c):
    return (a*a + b*b + c*c)**(1/2)

def GetLinesContainingPhrase(filePath, key, lineNums = False):
    phrases = []
    lines = []
    count = 0
    with open (filePath, 'r') as infile: ##scan infile as read-only line-by-line
        for line in infile:
            count = count + 1
            if(re.search(key, line)):
                phrases.append(line)
                lines.append(count)
    infile.close()
    
    if(len(phrases) == 0):
        return False
    if(lineNums):
        return phrases, lines
    return phrases

#Command Line
i = 0
while(i < len(sys.argv)):
    if(sys.argv[i][0] == '-' and (sys.argv[i][1] == 'i' or sys.argv[i][1] == 'I')):
        inPATH = str(sys.argv[i+1])
    if(sys.argv[i][0] == '-' and (sys.argv[i][1] == 'o' or sys.argv[i][1] == 'O')):
        outPATH = str(sys.argv[i+1])
    if(sys.argv[i][0] == '-' and (sys.argv[i][1] == 'n' or sys.argv[i][1] == 'N')):
        ION = int(sys.argv[i+1])
    i = i + 1

#Get the number of ions
nIons = GetLinesContainingPhrase(inPATH, "NIONS", lineNums=False)[0].split()
for e in range(0, len(nIons)):    
    if(nIons[e] == "NIONS"):
        nIons = int(nIons[e + 2])
        break

#Get the line number of the force readout      
forceLineNums = GetLinesContainingPhrase(inPATH, "TOTAL-FORCE", lineNums=True)[1]
forceLineNums.append(0) #to avoid out of bounds index errors later

#If we want the maximum force out of all atoms plotted:
if(ION == 0):
    mags = []
    highestForceList = []
    ionItr = 0
    #Go to correct line numbers, read in forces for each ion
    with open(inPATH, 'r') as infile:
        for i, line in enumerate(infile):
            if(i == forceLineNums[0] + 1):
                forceLineNums = forceLineNums[1:] #reduce size of list for speed (does this even work in
                ionItr = nIons                    #python?  I cant control pointers here so who knows..)
            if(ionItr != 0):
                ionItr = ionItr - 1
                a, b, c = float(line.split()[3]), float(line.split()[4]), float(line.split()[5])
                mags.append(float(magnitude(a, b, c)))
                if(ionItr == 0):
                    highestForceList.append(max(mags))
                    mags = []
        infile.close()

#Plot forces on a specific ion:
else:
    mag = []
    #Go to correct line numbers, read in forces for each ion
    with open(inPATH, 'r') as infile:
        for i, line in enumerate(infile):
            if(i == forceLineNums[0] + ION):
                forceLineNums = forceLineNums[1:] 
                a, b, c = float(line.split()[3]), float(line.split()[4]), float(line.split()[5])
                mag.append(float(magnitude(a, b, c)))
        infile.close()
        highestForceList = mag #I'm lazy. Ignore the name, this will be printed out
    
#Print Results
with open(outPATH, 'w') as outfile:
    outfile.write(str(int(len(highestForceList))))
    for n in range(0, len(highestForceList)):
        outfile.write(" " + str(n))
    for dat in highestForceList:
        outfile.write(" " + str(dat))
    outfile.close()












#!/usr/bin/python3

"""
Writes the free energy as a function of step.
The format is suitable for my text plotter
"""

import sys
import re

inPATH = "OUTCAR"
outPATH = "energies"

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
    i = i + 1

#Get the line number of the force reado      
energies = GetLinesContainingPhrase(inPATH, "free  energy", lineNums=True)[0]
energies_ = []
for en in energies:
	energies_.append(float(en.split()[4]))

#Print Results
with open(outPATH, 'w') as outfile:
    outfile.write(str(int(len(energies_))))
    for n in range(0, len(energies_)):
        outfile.write(" " + str(n))
    for dat in energies_:
        outfile.write(" " + str(dat))
    outfile.close()












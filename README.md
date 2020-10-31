# Text-Grapher
Quick graph generation for use on the command line.

You'll need to compile this with he c++ 2017 std.  

DESCRIPTION:
This is for getting rough graphs of data printed to the command line instead of transferring files to a PC with a GUI to graph data.
It is meant for general 2D graphing and is meant to work with any reasonable number range.  It is not responsible for generating data
into a format for it to read (unless you make it do that, I guess).  It expects the data to be supplied in a file with the following 
format:
	n x1 x2 x3 x4 ... xn y1 y2 y3 y4 ... yn
Any whitespace seperator (should) work.  
The graph's accuracy is determined by how the range of requested values compares with the number of characters allowed on your screen.
Obviously, do not expect to differentiate between x=0.1 and x=0.2 if you are plotting an x range of -100 to +100.  This can be resolved
with command line arguments detailed below.

TO USE:
./<executable name> -infile data.txt -nx 5 -ny 6 -prec 2 -lx 0 25 -ly 5 10 -char Q
Will read data from data.txt and plot values between 0<=x<=25 and 5<=y<=10.  It will give 5 x labels and 6 y labels, to a decimal 
precision of 2.  Points will be represented as the letter Q on the screen.   
COMMAND LINE ARGUMENTS:
-i     Infile: Specifies the path to the input data file.  Give a string after it.
-lx    Lims X: Specifies the limits on the x axis to plot.  Give it two numbers or your input will be ignored (or the code will break)
-ly    Lims Y: Same as for Lims X
-nx    Num Keys X: Specifies the number of lables for the x axis.  Give an integer between/including 2 and 13 or it will switch to 13
-ny    Num Keys Y: Same as for Num Keys X
-p     Precision: Number of decimal places that the lables will be written to include.  Has no effect on the graph's accuracy
-c     Char: Specifies which character is used to represent data points on the graph
*Note that only the first letter (or two) of the CLAs matter.  The negative sign in front is necessary.  Capitalization does not matter.

POSSIBLE ERRORS:
I've done minimal error checking/correction for this.  Some basic things (file not found, inconsistent x vs y lengths, etc) are all.
Attempting to limit the graph to a size where there are no data points will likley fail.  Any characters in the input file that are not
numbers will cause unknown issues.  Calling the -lx or -ly CLAs without two numbers following it will cause unknown issues.  

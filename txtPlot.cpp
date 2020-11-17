/*----------------------------------------------------------------
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

POSSIBLE ERRORS:
I've done minimal error checking/correction for this.  Some basic things (file not found, inconsistent x vs y lengths, etc) are all.
Attempting to limit the graph to a size where there are no data points will likley fail.  Any characters in the input file that are not
numbers will cause unknown issues.  Calling the -lx or -ly CLAs without two numbers following it will cause unknown issues.  
-----------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <cmath>

//Default Params that you may change:
///Screen Size
const int WIDTH = 140;
const int HEIGHT = 30;
///Number of labels to print.  Choose between 2 and 13
int numXKeys = 9;
int numYKeys = 9;
///Precision for key readout (number of decimal places)
int prec = 5;
///Limits for x and y values to graph.  The value set {2147483646, 2147483646} activates the default limits
double DEFAULT = 2147483646;
double xLims[2] = {DEFAULT, DEFAULT};
double yLims[2] = {DEFAULT, DEFAULT};
///File to load.  The first number is the number n.  This is followed by n x-values and then n y-values.  All seperated by only spaces
std::string PATH = "data";
///Character to use for plotting
char defChar = '*';

//Params that you should not change
const int OFFSET = 2; 

//To avoid messing around with parallel arrays
struct graphPt
{
	int arrVal;
	double actVal;

	graphPt(){};
	graphPt(double d){
		actVal = d;
	}
};

//Declerations
void ReadFile(std::string, double &, double &, double &, double &, std::vector<graphPt> &, std::vector<graphPt> &);
void Snip(std::vector<graphPt> &, std::vector<graphPt> &, double *, double *);
void FindMinsMaxs(std::vector<graphPt>, std::vector<graphPt>, double &, double &, double &, double &);
int ArrayVal(double, double, double, double, int); //scales different values to their true values (for the keys) or array values (for printing)
void GetArrayVals(std::vector<graphPt> &, double, double, int, int);
void GetAlphabet(char *);
void GetKeys(graphPt *, int, double, double, int, int);
void InitArray(char [][WIDTH + OFFSET], int, int, int);
void InitLabs(char [][WIDTH + OFFSET], char *, graphPt *, graphPt *);
void InitPoints(char [][WIDTH + OFFSET], std::vector <graphPt>, std::vector <graphPt>, int, int, char);
void PrintArray(char [][WIDTH + OFFSET], int, int, int);
void PrintKeys(char *, graphPt *, graphPt *, int);

int main(int argc, char *argv[])
{
	//Command Line Arguments
	for (int i = 0; i < argc; i++) {
		if (argv[i][0] == '-' && (argv[i][1] == 'i' || argv[i][1] == 'I')) ///input file name
			PATH = (argv[i + 1]);
		if (argv[i][0] == '-' && ((argv[i][1] == 'l' || argv[i][1] == 'L') && (argv[i][2] == 'x' || argv[i][2] == 'X'))){ ///x lims
			xLims[0] = std::atof(argv[i + 1]);
			xLims[1] = std::atof(argv[i + 2]);
		}
		if (argv[i][0] == '-' && ((argv[i][1] == 'l' || argv[i][1] == 'L') && (argv[i][2] == 'y' || argv[i][2] == 'Y'))) { ///y lims
			yLims[0] = std::atof(argv[i + 1]);
			yLims[1] = std::atof(argv[i + 2]);
		}
		if (argv[i][0] == '-' && ((argv[i][1] == 'n' || argv[i][1] == 'N') && (argv[i][2] == 'x' || argv[i][2] == 'X'))) ///num x keys
			numXKeys = std::atoi(argv[i + 1]);
		if (argv[i][0] == '-' && ((argv[i][1] == 'n' || argv[i][1] == 'N') && (argv[i][2] == 'y' || argv[i][2] == 'Y'))) ///num y keys
			numYKeys = std::atoi(argv[i + 1]);
		if (argv[i][0] == '-' && (argv[i][1] == 'p' || argv[i][1] == 'P')) ///precision
			prec = std::atof(argv[i + 1]);
		if (argv[i][0] == '-' && (argv[i][1] == 'c' || argv[i][1] == 'C')) ///plotting char
			defChar = argv[i + 1][0];
	}

	//(Minimal) Error checking
	///Make sure we don't accidently write over important memory values.  Minimum of 2 to avoid dividing by zero
	if(numXKeys > 13 || numXKeys < 2)
		numXKeys = 13;
	if(numYKeys > 13 || numYKeys < 2)
		numYKeys = 13;

	//Read infile.  Find mins and maxes.  Snip vector to only show requeted value ranges 
	std::vector<graphPt> xVals;
	std::vector<graphPt> yVals;
	double xMin, xMax, yMin, yMax;
	ReadFile(PATH, xMin, xMax, yMin, yMax, xVals, yVals);
	if(xLims[0] == 2147483646 && xLims[1] == 2147483646){
		xLims[0] = xMin;
		xLims[1] = xMax;
	}
	if (yLims[0] == DEFAULT && yLims[1] == DEFAULT) {
		yLims[0] = yMin;
		yLims[1] = yMax;
	}
	Snip(xVals, yVals, xLims, yLims);
	FindMinsMaxs(xVals, yVals, xMin, xMax, yMin, yMax);

	//Get scaled array values
	GetArrayVals(xVals, xMin, xMax, WIDTH, OFFSET);
	GetArrayVals(yVals, yMin, yMax, HEIGHT, OFFSET);

	//Generate labels
	char alphabet[26];
	graphPt xKeys[14];
	graphPt yKeys[14];
	GetAlphabet(alphabet);
	GetKeys(xKeys, numXKeys, xMin, xMax, WIDTH, OFFSET);
	GetKeys(yKeys, numYKeys, yMin, yMax, HEIGHT, OFFSET);
	yKeys[0].arrVal ++; ///fixes an annoying bug related to floor arithmatic

	//Fill array
	char arr[HEIGHT + OFFSET][WIDTH + OFFSET];
	InitArray(arr, WIDTH, HEIGHT, OFFSET);
	InitLabs(arr, alphabet, xKeys, yKeys);
	InitPoints(arr, xVals, yVals, HEIGHT, OFFSET, defChar);
	
	//Print array
	PrintArray(arr, WIDTH, HEIGHT, OFFSET);
	PrintKeys(alphabet, xKeys, yKeys, prec);

	//End
	std::cout << "\n";
	return 0;
}

void ReadFile(std::string path, double &xMin, double &xMax, double &yMin, double &yMax, std::vector<graphPt> &xS, std::vector<graphPt> &yS)
{
	//Fill unscaled vector of x and y values
	std::ifstream infile(path);
	if(infile.fail()){
		std::cout << "Failed to find " << path << ".\n";
		std::exit(1);
	}
	int range;
	infile >> range;
	for (int i = 0; i < 2 * range; i++) {
		double val;
		infile >> val;

		//Min and Max init
		if (i == 0) {
			xMin = val;
			xMax = val;
		}
		if (i == range) {
			yMin = val;
			yMax = val;
		}

		//Read numbers into vectors.  Also determine min and max values
		if (i < range) { ///x values
			xS.push_back(val);
			if (val < xMin)
				xMin = val;
			if (val > xMax)
				xMax = val;
		}
		else { ///y values
			yS.push_back(val);
			if (val < yMin)
				yMin = val;
			if (val > yMax)
				yMax = val;
		}
	}
	infile.close();
}

void Snip(std::vector<graphPt> &xS, std::vector<graphPt> &yS, double *xLims, double *yLims)
{
	std::vector<graphPt> tmpx, tmpy;
	for (int i = 0; i < xS.size(); i++){
		if (xS[i].actVal <= xLims[1] && xS[i].actVal >= xLims[0] && yS[i].actVal <= yLims[1] && yS[i].actVal >= yLims[0]){
			tmpx.push_back(xS[i]);
			tmpy.push_back(yS[i]);
		}
	}
	xS = tmpx;
	yS = tmpy;

	if(tmpx.size() == 0 || tmpy.size() == 0 || tmpx.size() != tmpy.size()){
		std::cout << "WARNING:  Vector sizes are 0 or inconsistent.  This will cause errors at best, and crash the code at worst.\n";
		std::cout << "Make sure that there are values in the input file fitting in the limits supplied.\n";
		std::exit(2);
	}
}

void FindMinsMaxs(std::vector<graphPt> xS, std::vector<graphPt> yS, double &xmi, double &xma, double &ymi, double &yma)
{
	xmi = xS[0].actVal;
	xma = xS[0].actVal;
	ymi = yS[0].actVal;
	yma = yS[0].actVal;

	for (int i = 0; i < xS.size(); i++){
		if(xS[i].actVal < xmi)
			xmi = xS[i].actVal;
		if(xS[i].actVal > xma)
			xma = xS[i].actVal;
	}
	for (int i = 0; i < yS.size(); i++) {
		if (yS[i].actVal < ymi)
			ymi = yS[i].actVal;
		if (yS[i].actVal > yma)
			yma = yS[i].actVal;
	}
}

int ArrayVal(double actual, double min, double max, double scale, int offset)
{
	return (int)std::floor((actual - min) * (scale / (max - min))) + offset;
}

void GetArrayVals(std::vector<graphPt> &vals, double min, double max, int scale, int off)
{
	for (int i = 0; i < vals.size(); i++) {
		vals[i].arrVal = ArrayVal(vals[i].actVal, min, max, scale, off);
		if (vals[i].actVal == max)
			vals[i].arrVal--;
	}
}

void GetAlphabet(char *ar)
{
	for (char i = 'a'; i < 'z' + 1; i++)
		ar[i - 'a'] = i;
}

void GetKeys(graphPt *keys, int nKeys, double min, double max, int scale, int off)
{
	///goes a thru m for x, n thru z for y
	for (int i = 0; i < nKeys; i++) { 
		keys[i].actVal = (min + i*((max - min) / (nKeys - 1)));
		keys[i].arrVal = ArrayVal(keys[i].actVal, min, max, scale, off);
		if (keys[i].actVal == max)
			keys[i].arrVal--;
	}
	keys[nKeys].arrVal = -99; ///terminate flag.  do not touch
}

void InitArray(char a[][WIDTH + OFFSET], int wid, int hei, int off)
{
	//Fill array with emtpy spaces and label seperators
	for (int h = 0; h < hei + off; h++) {
		for (int w = 0; w < wid + off; w++) {
			if (h == hei) ///horizontal
				a[h][w] = '-';
			else if (w == off - 1) ///vertical
				a[h][w] = '|';
			else
				a[h][w] = ' ';
		}
		std::cout << "\n";
	}
}

void InitLabs(char a[][WIDTH + OFFSET], char *alph, graphPt *xK, graphPt *yK)
{
	//x labels
	for (int i = 0; i < 14; i++) {
		if (xK[i].arrVal == -99)
			break;
		a[HEIGHT + 1][xK[i].arrVal] = alph[i];
	}
	//y labels
	for (int i = 0; i < 14; i++) {
		if (yK[i].arrVal == -99)
			break;
		a[HEIGHT - yK[i].arrVal + OFFSET][0] = alph[i + 13];
	}
}

void InitPoints(char a[][WIDTH + OFFSET], std::vector<graphPt> xS, std::vector<graphPt> yS, int hei, int off, char fill)
{
	for (int i = 0; i < yS.size(); i++) {
		a[HEIGHT - yS[i].arrVal - 1 + OFFSET][xS[i].arrVal] = fill;
	}
}

void PrintArray(char a[][WIDTH + OFFSET], int wid, int hei, int off)
{
	for (int h = 0; h < hei + off; h++) {
		for (int w = 0; w < wid + off; w++) {
			std::cout << a[h][w];
		}
		std::cout << "\n";
	}
}

void PrintKeys(char *labels, graphPt *xK, graphPt *yK, int prec)
{
	std::cout << "\nX AXIS: ";
	for (int i = 0; i < 14; i++) {
		std::string buff = " "; ///for dealing with negative signs
		if (xK[i].arrVal == -99)
			break;
		if(xK[i].actVal < 0)
			buff = "";
		std::cout << std::setprecision(prec) << std::fixed <<  labels[i] << ":" + buff << xK[i].actVal << " || ";
	}
	std::cout << "\nY AXIS: ";
	for (int i = 0; i < 14; i++) {
		std::string buff = " "; ///for dealing with negative signs
		if (yK[i].arrVal == -99)
			break;
		if (yK[i].actVal < 0)
			buff = "";
		std::cout << std::setprecision(prec) << std::fixed << labels[i + 13]  << ":" + buff << yK[i].actVal << " || ";
	}
}

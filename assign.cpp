#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <iterator> //will be used during the evaluation of the variable in map
#include <algorithm>
#include "Expression.cpp"

using namespace std;

//--------------------------!!!!!! C A U T I O N !!!!!!!-------------------------------------
//
// Main part is copied to mylang2IR.cpp. Functions are copied to Expression.cpp.
//
//                      This file is not being used anymore
//
//-------------------------------------------------------------------------------------------

int main(int argc, char const *argv[])
{
    cout << "define i32 @main() {" << endl; // main starts
    vector<string> tokens;

    string inputFile = "input1.txt";   //argv[1];
    string outputFile = "output.txt"; //argv[2];

    ifstream infile;
    infile.open(inputFile);

    ofstream outfile;
    outfile.open(outputFile);

    vector<string> lines;
    string line;

    while (getline(infile, line)) // !! this may be working incorrectly in case of empty lines ?? check 
    { //take lines
        lines.push_back(line);
    }

    for (lineNumber = 0; lineNumber < lines.size(); lineNumber++)
    {
        string i = lines[lineNumber];
        i.erase(remove(i.begin(), i.end(), '\t'), i.end()); // removes all tab characters
        i.erase(remove(i.begin(), i.end(), ' '), i.end());  //removes all spaces
       
        if (i.find("=") < i.length())    //this part is just for assignment
        {
            int position = i.find("=");
            string variable = i.substr(0, position);
            string otherPart = i.substr(position + 1, i.length());

            if (isValidVariable(variable)) {            
                string rightPart = evaluate(otherPart); //this part is going to change we may need to call
                cout << "store i32 " << rightPart  << ", i32* %" << variable << endl;                          
            }
        }
    }
    cout << "ret i32 0" << endl; //return 0
    cout << "}" << endl; //end of main
    return 0;
}

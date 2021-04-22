#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <iterator> //will be used during the evaluation of the variable in map
#include <algorithm>
#include "Expression.cpp"

using namespace std;

//--------------------------!!!!!! C A U T I O N !!!!!!!-------------------------------------
//main part and functions will be copied to mylang2IR.cpp or will be adjusted for external usage if it is okey
//please read the comments

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


    string operators = "-+()/*";

    for (lineNumber = 0; lineNumber < lines.size(); lineNumber++)
    {
        string i = lines[lineNumber];
        i.erase(remove(i.begin(), i.end(), '\t'), i.end()); // removes all tab characters
        i.erase(remove(i.begin(), i.end(), ' '), i.end());  //removes all spaces
        //this part is just for assignment
        if (i.find("=") < i.length())
        {
            int position = i.find("=");
            string variable = i.substr(0, position);
            string otherPart = i.substr(position + 1, i.length());
            if (isValidVariable(variable))
            {
                int value; // -------------------------- 3a * 5 isValid in Expression ?? write the funcrtion for this
                if (otherPart.find("-") < otherPart.size() || otherPart.find("+") < otherPart.size() || otherPart.find("(") < otherPart.size() 
                    || otherPart.find(")") < otherPart.size() || otherPart.find("*") < otherPart.size() || otherPart.find("/") < otherPart.size())
                {
                    string rightPart = evaluate(postfix(otherPart)); //this part is going to change we may need to call
                    cout << "store i32 " << rightPart  << ", i32* %" << variable << endl;
                    //evaluate from inside postfix or sth.
                    //and also if there is a problem with the exp. we need to terminate the program
                }
                if (isValidNumber(otherPart, value))
                {
                    //?????? Do we need to check if the variable already exists ???????
                    //actually i am not sure adlkjsldk
                    //will be written here
                    //puts the pair in the map
                    declareVariable(variable,value);
                }
            }
        }
    }
    cout << "ret i32 0" << endl; //return 0
    cout << "}" << endl; //end of main
    return 0;
}

#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include "Expression.cpp"

using namespace std;

void assign(string i)
{
    int position = i.find("=");
    string variable = i.substr(0, position);
    string otherPart = i.substr(position + 1, i.length());

    if (isValidVariable(variable))
    {
        string rightPart = evaluate(otherPart); //this part is going to change we may need to call
        cout << "store i32 " << rightPart << ", i32* %" << variable << endl;
    }
}

int main(int argc, char const *argv[])
{
    cout << "define i32 @main() {" << endl; // main starts
    vector<string> tokens;

    string inputFile = "input1.txt";  //argv[1];
    string outputFile = "output.txt"; //argv[2];

    ifstream infile;
    infile.open(inputFile);

    ofstream outfile;
    outfile.open(outputFile);

    vector<string> lines;
    string line;

    while (getline(infile, line))
    { //take lines
        lines.push_back(line);
    }

    for (lineNumber = 0; lineNumber < lines.size(); lineNumber++)
    { //main for loop
        string i = lines[lineNumber];
        i.erase(remove(i.begin(), i.end(), '\t'), i.end()); // removes all tab characters
        i.erase(remove(i.begin(), i.end(), ' '), i.end());  //removes all spaces

        if (i.find("#") < i.length())
        {
            i = i.substr(i.begin(), i.find("#"));
        }

        if (i.find("=") < i.length()) //this part is just for assignment
        {
            assign(i);
        }
        else if (i.find("while("))
        {
            //while.cpp
        }
        else if (i.find("if("))
        {
        }
        else if (i.find("print("))
        {
            //print the given variables
        }
        else if (i == "")
        {
            //do nothing
            continue;
        }
        else
        {
            //syntax error
        }
    }
    cout << "ret i32 0" << endl; //return 0
    cout << "}" << endl;         //end of main
    return 0;
}

#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include "Expression.cpp"

using namespace std;

int whileNamer = 0;

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

void mainLoop(vector<string> lines, int &lineNumber)
{
    string i = lines[lineNumber];
    i.erase(remove(i.begin(), i.end(), '\t'), i.end()); // removes all tab characters
    i.erase(remove(i.begin(), i.end(), ' '), i.end());  //removes all spaces

    if (i.find("#") < i.length())
    {
        i = i.substr(0, i.find("#"));
    }

    if (i.find("=") < i.length()) //this part is just for assignment
    {
        assign(i);
    }
    else if (i.find("while(") == 0)
    {
        int openPosition = i.find("(");
        int closePosition = i.find(")");
        if (i.substr(0, openPosition) != "while" || i.substr(closePosition + 1, i.length()) != "{")
        {
            cout << "Syntax Error in line #" << lineNumber << endl;
            exit(0);
        }
        string whileConditionName = "whilecond" + to_string(whileNamer);
        string whileBodyName = "whilebody" + to_string(whileNamer);
        string whileEndName = "whileend" + to_string(whileNamer++);

        cout << "br label %" + whileConditionName << endl;
        cout << whileConditionName << ":" << endl;
        string condition = i.substr(openPosition + 1, closePosition - openPosition - 1);

        isValidVariable(condition);

        string namerCondition1 = "t" + to_string(namer++);
        string s1 = "%" + namerCondition1 + " = load i32* %" + condition;
        cout << s1 << endl;

        string namerCondition2 = "t" + to_string(namer++);
        string s2 = "%" + namerCondition2 + " = icmp ne i32 %" + namerCondition1 + ", 0";
        cout << s2 << endl;

        string s3 = "br i1 %" + namerCondition2 + ", label %" + whileBodyName + ", label %" + whileEndName;
        cout << s3 << endl;

        cout << whileBodyName << ":" << endl;

        while (!(i.find("}") < i.size()))
        {
            lineNumber++;
            mainLoop(lines, lineNumber);
            i = lines[lineNumber];
        }
        cout << "br label %" + whileConditionName << endl;
        cout << whileEndName << ":" << endl;
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
    }
    else
    {
        cout << "Syntax Error in line #" << lineNumber << endl;
        exit(0);
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

    //main for loop
    for (int lineNumber = 0; lineNumber < lines.size(); lineNumber++)
    {
        mainLoop(lines, lineNumber);
    }

    cout << "ret i32 0" << endl; //return 0
    cout << "}" << endl;         //end of main
    return 0;
}

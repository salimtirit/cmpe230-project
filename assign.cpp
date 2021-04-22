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
map<string, int> variables; //name may ned to be changed due to conflicts

//returns true if the variable is valid
//valid variable: first character from alphabet (upper or lowercase) preceeding with alphanumeric characters
bool isValidVariable(string s)
{
    int firstChr = s[0];
    if ((firstChr > 96 && firstChr < 123) || (firstChr > 64 && firstChr < 90)) // all ascii table values
    {
        for (int i = 1; i < s.length(); i++)
        {
            int character = s[i];
            if (!((character > 96 && character < 123) || (character > 64 && character < 90) || (character > 47 && character < 58)))
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

//cheks if the number is a valid number
//returns false otherwise
//returns true and the number as initialized to a
bool isValidNumber(string s, int &a)
{
    for (char c : s)
    {
        if (!(c > 47 && c < 58)) //47 , 58 are the ascii table values
        {
            return false;
        }
    }
    a = stoi(s); // parses string to an integer.
    return true;
}

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

    for (int a = 0; a < lines.size(); a++)
    {
        string i = lines[a];
        i.erase(remove(i.begin(), i.end(), '\t'), i.end()); // removes all tab characters
        i.erase(remove(i.begin(), i.end(), ' '), i.end());  //removes all spaces
        //this part is just for assignment
        if (i.find("=") < i.length())
        {
            int position = i.find("=");
            string variable = i.substr(0, position);
            string otherPart = i.substr(i.find("=") + 1, i.length());
            if (isValidVariable(variable))
            {
                int value;
                if (otherPart.find("-") < otherPart.size() || otherPart.find("+") < otherPart.size() || otherPart.find("(") < otherPart.size() 
                    || otherPart.find(")") < otherPart.size() || otherPart.find("*") < otherPart.size() || otherPart.find("/") < otherPart.size())
                {
                    value = evaluate(postfix(otherPart)); //this part is going to change we may need to call
                    //evaluate from inside postfix or sth.
                    //and also if there is a problem with the exp. we need to terminate the program
                }
                if (isValidNumber(otherPart, value))
                {
                    //?????? Do we need to check if the variable already exists ???????
                    //actually i am not sure adlkjsldk
                    //will be written here
                    variables.insert(pair<string, int>(variable, value)); //puts the pair in the map
                    cout << "%" << variable << " = alloca i32" << endl;
                    cout << "store i32 " << value << ", i32* %" << variable << endl;
                }
                else
                {
                    cout << "Invalid assignment in line #" << a << endl; // theese will be exceptions
                }
            }
            else
            {
                cout << "Invalid variable in line #" << a << endl; // theese will be exceptions
            }
        }
    }
    cout << "ret i32 0" << endl; //return 0
    cout << "}" << endl; //end of main
    return 0;
}

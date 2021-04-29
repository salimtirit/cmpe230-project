#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include "Expression.cpp"

using namespace std;

int whileNamer = 0;
int ifNamer = 0;
int nOfLoops = 0;

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

void mainLoop(vector<string> lines, int &lineNumber, int &nOfLoops)
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
    else if(i.find("while(") == 0 || i.find("if(") == 0){
        if(nOfLoops){
            cout << "Syntax error in line #" << lineNumber << endl;
            exit(0);
        }
        nOfLoops++;
        int openPosition = i.find("(");
        int closePosition = i.find(")");
        string ifORwhile[2] = {"if","while"};
        int choose;
        int smallNamer;
        if (i.find("if(") == 0){
            smallNamer = ifNamer;
            ifNamer++;
            choose = 0;
        }else{
            smallNamer = whileNamer;
            whileNamer++;            
            choose = 1;
        }
        if (i.substr(0, openPosition) != ifORwhile[choose] || i.substr(closePosition + 1, i.length()) != "{")
        {
            cout << "Syntax Error in line #" << lineNumber << endl;
            exit(0);
        }      

        string conditionName = ifORwhile[choose]+"cond" + to_string(smallNamer);
        string bodyName = ifORwhile[choose]+"body" + to_string(smallNamer);
        string endName = ifORwhile[choose]+"end" + to_string(smallNamer);  

        cout << "br label %" + conditionName << endl;
        cout << conditionName << ":" << endl;
        string condition = i.substr(openPosition + 1, closePosition - openPosition - 1);

        istring tempCond = evaluate(condition);

        string namerCondition1 = "t" + to_string(namer++);
        string s1 = "%" + namerCondition1 + " = load i32* " + tempCond;
        cout << s1 << endl;

        string namerCondition2 = "t" + to_string(namer++);
        string s2 = "%" + namerCondition2 + " = icmp ne i32 %" + namerCondition1 + ", 0";
        cout << s2 << endl;

        string s3 = "br i1 %" + namerCondition2 + ", label %" + bodyName + ", label %" + endName;
        cout << s3 << endl;

        cout << bodyName << ":" << endl;

        lineNumber++;
        while (!(i.find("}") < i.size()))
        {
            if(lineNumber < lines.size()){
                mainLoop(lines, lineNumber, nOfLoops);
                lineNumber++;
            }else {
                cout << "No curly bracets in line #"<< lines.size() << endl; // last line number
                exit(0);
            }
            i = lines[lineNumber];
        }

        nOfLoops--;

        if(choose){
            cout << "br label %" + conditionName << endl;
        }else{
            cout << "br label %" + endName << endl;
        }
        cout << endName << ":" << endl;
    }
    else if (i.find("print(") < i.size())
    {
        int openPosition = i.find("(");
        int closePosition = i.find(")"); 
        if(i.substr(0,openPosition)!="print" || i.substr(closePosition+1)!=""){
            cout << "syntax error in line: "<<lineNumber <<endl;
            exit(0);
        }
        string printExpr = i.substr(openPosition + 1, closePosition - openPosition - 1);
        string printVar = evaluate(printExpr);      
        cout << "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 " +printVar+" )" << endl;
        
    }
    else if (i == "")
    {
        //do nothing
    }
    else
    {
        cout << "Syntax Error in line # here" << lineNumber << endl;
        exit(0);
    }
}

int main(int argc, char const *argv[])
{
   cout << "; ModuleID = 'mylang2ir'" << endl;
   cout << "declare i32 @printf(i8*, ...)" << endl;
   cout << "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl;


    cout << "define i32 @main() {" << endl; // main starts
    vector<string> tokens;

    string inputFile = "./inputs/input.txt";  //argv[1];
    string outputFile = "./outputs/output.txt"; //argv[2];

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
    for (; lineNumber < lines.size(); lineNumber++)
    {
        mainLoop(lines, lineNumber, nOfLoops);
    }

    cout << "ret i32 0" << endl; //return 0
    cout << "}" << endl;         //end of main
    return 0;
}

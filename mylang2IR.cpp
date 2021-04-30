#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include "Expression.cpp"

using namespace std;

int whileNamer = 0;
int ifNamer = 0;
int nOfLoops = 0;

ifstream infile;
ofstream outfile;

void assign(string i)
{
    int position = i.find("=");
    string variable = i.substr(0, position);
    string otherPart = i.substr(position + 1, i.length());

    if (isValidVariable(variable))
    {
        string rightPart = evaluate(otherPart); //this part is going to change we may need to call
        otherStatements.push_back("store i32 " + rightPart + ", i32* %" + variable);
    }
}

void mainLoop(vector<string> lines, int &lineNumber, int &nOfLoops)
{
    string i = lines[lineNumber];

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
            errorHandling(lineNumber);
        }
        nOfLoops++;
        int openPosition = i.find("(");
        int closePosition = i.find("){");
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
            errorHandling(lineNumber);
        }      

        string conditionName = ifORwhile[choose]+"cond" + to_string(smallNamer);
        string bodyName = ifORwhile[choose]+"body" + to_string(smallNamer);
        string endName = ifORwhile[choose]+"end" + to_string(smallNamer);  

        otherStatements.push_back("br label %" + conditionName);
        otherStatements.push_back(conditionName + ":");
        string condition = i.substr(openPosition + 1, closePosition - openPosition - 1);
        
        string namerCondition1;
        if(isValidNumber(condition)){
            string tempVar = varNamer();
            declareStatements.push_back("%"+tempVar + " = alloca i32");
            declareStatements.push_back("store i32 "+condition+", i32* %"+tempVar);
            namerCondition1 ="%"+ varNamer();
            otherStatements.push_back(namerCondition1 + " = load i32* %" + tempVar);
        } else {
            namerCondition1 = evaluate(condition);
        }

        string namerCondition2 = varNamer();
        string s2 = "%" + namerCondition2 + " = icmp ne i32 " + namerCondition1 + ", 0";
        otherStatements.push_back(s2);

        string s3 = "br i1 %" + namerCondition2 + ", label %" + bodyName + ", label %" + endName;
        otherStatements.push_back(s3);

        otherStatements.push_back(bodyName + ":");

        lineNumber++;
        while (!(i.find("}") < i.size()))
        {
            if(lineNumber < lines.size()){
                mainLoop(lines, lineNumber, nOfLoops);
                lineNumber++;
                if (lineNumber == lines.size())
                {
                    errorHandling(lineNumber-1);
                }
            }else if(lineNumber==lines.size()&&lines[lineNumber].find("}")){
                //cout << "here i am" << endl;
            }
            else {
                errorHandling(lineNumber-1);
            }
            i = lines[lineNumber];
        }

        nOfLoops--;

        if(choose){
            otherStatements.push_back("br label %" + conditionName);
        }else{
            otherStatements.push_back("br label %" + endName);
        }
        otherStatements.push_back(endName + ":");
    }
    else if (i.find("print(") < i.size())
    {
        int openPosition = i.find("(");
        int closePosition = i.find(")"); 
        if(i.substr(0,openPosition)!="print" || i.substr(closePosition+1)!=""){
            errorHandling(lineNumber);
        }
        string printExpr = i.substr(openPosition + 1, closePosition - openPosition - 1);
        string printVar = evaluate(printExpr);      
        otherStatements.push_back("call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 " +printVar+" )" );        
    }
    else if (i == "")
    {
        //do nothing
    }
    else
    {   
        errorHandling(lineNumber);
    }
}

void errorHandling(int line){
    //olması gereken hemen alttaki aslında
    string s = "call i32 (i8*, ...)* @printf(i8* getelementptr ([23 x i8]* @print.str1, i32 0, i32 0), i32 " +to_string(line)+" )" ;
    //outfile << "Line "<< line << ": syntax error" << endl;
    outfile << s << endl;
    outfile << "ret i32 0" << endl; //return 0
    outfile << "}" << endl;         //end of main
    exit(0);
}

int main(int argc, char const *argv[])
{
    string inputFile = argv[1];  //"./inputs/testcase8.my";  //      
    string outputFile = inputFile.substr(0,inputFile.find("my"))+"ll";       

    
    infile.open(inputFile);

    
    outfile.open(outputFile);

    vector<string> lines;
    string line;

    while (getline(infile, line))
    { //take lines
        line.erase(remove(line.begin(), line.end(), '\t'), line.end()); // removes all tab characters
        line.erase(remove(line.begin(), line.end(), ' '), line.end());  //removes all spaces
        lines.push_back(line);
    }

    outfile << "; ModuleID = 'mylang2ir'" << endl;
    outfile << "declare i32 @printf(i8*, ...)" << endl;
    outfile << "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl;
    outfile << "@print.str1 = constant [23 x i8] c\"Line %d: syntax error\\0A\\00\"" << endl;
    outfile << "define i32 @main() {" << endl; // main starts

    //main for loop
    for (; lineNumber < lines.size(); lineNumber++)
    {
        mainLoop(lines, lineNumber, nOfLoops);
    }

    for(string str: declareStatements){
        outfile << str << endl;
    }
    for(string str: otherStatements){
        outfile << str << endl;
    }

    outfile << "ret i32 0" << endl; //return 0
    outfile << "}" << endl;         //end of main
    return 0;
}

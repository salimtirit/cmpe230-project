#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include "Expression.cpp"

using namespace std;

//@authors @salimtirit @Brunettow

int whileNamer = 0; //makes sure the names of while conditions/boddies/ends are different
int ifNamer = 0; //makes sure the names of if conditions/boddies/ends are different
int nOfLoops = 0; //checks if there is an inner loop to give error

ifstream infile; // input file decleration
ofstream outfile;// output file decleration

// Handles the variable assignment part:
// seperates the expression to two part namely before "=" and after it.
// For the first part see isValidVariable function in Expression.cpp.
// For second part see evaluate function in Expression.cpp.
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

//This is where magic happens.
//First handles the comments which should start with # and should be in one line
//Then finds "=" sign and sends to assign function above.
//Then if & while expressions are handled
//Lastly print function is handeled
//Theese are all valid lines in our language except empty lines ofcourse.
//If there is sth else in a line it gives error. For further info check RULES & QUESTIONS
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
    else if(i.find("while(") == 0 || i.find("if(") == 0){ //basically if is one time excecuted while loop 
        if(nOfLoops){
            errorHandling(lineNumber); // nested loops are not allowed
        }
        nOfLoops++;
        int openPosition = i.find("(");
        int closePosition = i.find("){"); 
        string ifORwhile[2] = {"if","while"}; // will be used in the naming
        int choose;
        int smallNamer;
        if (i.find("if(") == 0){
            smallNamer = ifNamer;
            ifNamer++;
            choose = 0;  // ifORwhile array 0 element
        }else{
            smallNamer = whileNamer;
            whileNamer++;            
            choose = 1; // ifORwhile array 1 element
        }
        if (i.substr(0, openPosition) != ifORwhile[choose] || i.substr(closePosition + 1, i.length()) != "{")
        {
            errorHandling(lineNumber); //if there is sth we dont want in while line gives error
        }      

        string conditionName = ifORwhile[choose]+"cond" + to_string(smallNamer);
        string bodyName = ifORwhile[choose]+"body" + to_string(smallNamer);
        string endName = ifORwhile[choose]+"end" + to_string(smallNamer);  

        otherStatements.push_back("br label %" + conditionName);
        otherStatements.push_back(conditionName + ":");
        string condition = i.substr(openPosition + 1, closePosition - openPosition - 1);
        
        string namerCondition1;
        if(isValidNumber(condition)){ //if the condition is a number (0 false others true) 
            string tempVar = varNamer();
            declareStatements.push_back("%"+tempVar + " = alloca i32");
            declareStatements.push_back("store i32 "+condition+", i32* %"+tempVar);
            namerCondition1 ="%"+ varNamer();
            otherStatements.push_back(namerCondition1 + " = load i32* %" + tempVar);
        } else { // if the condition is an expression which needs to be evaluated
            namerCondition1 = evaluate(condition);
        }

        string namerCondition2 = varNamer(); //compare with zero
        string s2 = "%" + namerCondition2 + " = icmp ne i32 " + namerCondition1 + ", 0";
        otherStatements.push_back(s2);

        string s3 = "br i1 %" + namerCondition2 + ", label %" + bodyName + ", label %" + endName;
        otherStatements.push_back(s3);

        otherStatements.push_back(bodyName + ":");

        lineNumber++;
        while (!(i.find("}") < i.size())) //calls main loop again and again to excecute expressions inside the loop
        {
            if(lineNumber < lines.size()){
                mainLoop(lines, lineNumber, nOfLoops);
                lineNumber++;
                if (lineNumber == lines.size())
                {
                    errorHandling(lineNumber-1); // could not find the closing bracket and file is over so exception
                }
            }else if(lineNumber==lines.size()&&lines[lineNumber].find("}")){
                //this is just the last line of file and there is a closing bracket should not give any error
            }
            else {
                errorHandling(lineNumber-1); // if we somehow exceeded the number of lines in the file
            }
            i = lines[lineNumber];
        }

        nOfLoops--;

        if(choose){
            otherStatements.push_back("br label %" + conditionName); //for while loop we need to check the condition again
        }else{
            otherStatements.push_back("br label %" + endName); // for if we just finish
        }
        otherStatements.push_back(endName + ":");
    }
    else if (i.find("print(") < i.size()) //print statement for llvm
    {
        int openPosition = i.find_first_of("(");
        int closePosition = i.find_last_of(")"); 
        if(i.substr(0,openPosition)!="print" || i.substr(closePosition+1)!=""){
            errorHandling(lineNumber); //there must be nothing before or after print(<var>)
        }
        string printExpr = i.substr(openPosition + 1, closePosition - openPosition - 1);
        string printVar = evaluate(printExpr); // if there is sth to be evaluated inside if      
        otherStatements.push_back("call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* @print.str, i32 0, i32 0), i32 " +printVar+" )" );        
    }
    else if (i == "")
    {
        //do nothing if there is a blank line
    }
    else
    {   
        errorHandling(lineNumber); // if there is sth else which is not stated above it is not allowed.
    }
}

// in anywhere of this code if there is an error this function will be called.
// this function will print a line for the llvm to print error in a specific line and will end the programme immediately
void errorHandling(int line){
    string s = "call i32 (i8*, ...)* @printf(i8* getelementptr ([23 x i8]* @print.str1, i32 0, i32 0), i32 " +to_string(line)+" )" ;
    outfile << s << endl;
    outfile << "ret i32 0" << endl; //return 0
    outfile << "}" << endl;         //end of main
    exit(0); // terminate code
}

int main(int argc, char const *argv[])
{
    string inputFile = argv[1];  // input files name will be the first argument      
    string outputFile = inputFile.substr(0,inputFile.find(".my"))+".ll";  // output file will have the same name with "ll" extension 

    infile.open(inputFile);
    outfile.open(outputFile);

    vector<string> lines; // all lines in the input file
    string line; // to fill lines vector

    while (getline(infile, line))
    { //take lines and push them to the back of lines vector
        line.erase(remove(line.begin(), line.end(), '\t'), line.end()); // removes all tab characters in one line
        line.erase(remove(line.begin(), line.end(), ' '), line.end());  //removes all spaces in one line
        lines.push_back(line);
    }

    outfile << "; ModuleID = 'mylang2ir'" << endl;
    outfile << "declare i32 @printf(i8*, ...)" << endl;
    outfile << "@print.str = constant [4 x i8] c\"%d\\0A\\00\"" << endl; // print function to print variables
    outfile << "@print.str1 = constant [23 x i8] c\"Line %d: syntax error\\0A\\00\"" << endl; // print function to print syntax errors
    outfile << "define i32 @main() {" << endl; // main starts

    //sends lines to mainLoop function one by one
    for (; lineNumber < lines.size(); lineNumber++)
    {
        mainLoop(lines, lineNumber, nOfLoops);
    }

    //stored declaration statements will be printed first because our languages variables are always global
    for(string str: declareStatements){
        outfile << str << endl;
    }
    // all statements accept declaration will be printed to ll file
    for(string str: otherStatements){
        outfile << str << endl;
    }

    outfile << "ret i32 0" << endl; //return 0
    outfile << "}" << endl;         //end of main
    return 0;
}
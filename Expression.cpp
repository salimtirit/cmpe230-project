#include <iostream>
#include <stack>
#include <cstring>
#include <map>

using namespace std;
int lineNumber = 0; //current line
map<string, int> variables; //a map that stores all valid variables
vector<string> declareStatements;  //stores statements that will be printed
vector<string> otherStatements;

string operators = "-+()/*";
string bigger = "*/";  //operators that has precedence over others
string smaller = "-+";

stack<char> oper; //stores operations during making postfix expression
int namer = 0; //for naming temproray variables
int chooseNamer = 0;  //names choose function labels
int condNamer = 0; //names choose functions condition labels
string evaluate(string expr); //method for evaluating statements
void errorHandling(int line); 

string varNamer(){  //names temprory variables
    return "_t" + to_string(namer++);
}

 //declares new valid variables
 //writes declaration statements and stores them in a map

void declareVariable(string name, int value = 0) 
{
    if (variables.find(name) == variables.end()) //if it's not declared before
    {
        if(name == "if"||name == "while"||name == "choose"||name == "print"){
            errorHandling(lineNumber);
        }
        string str1 =  "%" + name + " = alloca i32";
        declareStatements.push_back(str1);
    }
    variables[name] = value;           
    string str2 = "store i32 " + to_string(variables.find(name)->second) + ", i32* %" + name;
    if (value == 0)
    {
        declareStatements.push_back(str2);  
    }else{
        otherStatements.push_back(str2);
    }
    
}

//checks whether given string is a valid number
//returns true if it's a number, else returns false

bool isValidNumber(string s){ 
    
    for (int j = 0; j < s.length(); j++) {
        int character = s[j];
        if (!(character > 47 && character < 58)) //checks whether character is a number using ascii values
        {
            return false; 
        }
    }
    return true;
}

//returns true if the variable is valid, if not throws error
//valid variable: first character from alphabet (upper or lowercase) preceeding with alphanumeric characters
//if it's a temprorary variable, returns false

bool isValidVariable(string s, bool isTempVar = false)
{
    if(isTempVar){ //if it's a temprorary variable
        return false;
    }

    int firstChr = s[0];
    if ((firstChr > 96 && firstChr < 123) || (firstChr > 64 && firstChr < 90)) // checks whether first character is a valid character using ascii 
    {
        for (int i = 1; i < s.length(); i++)  {
            int character = s[i];
            if (!((character > 96 && character < 123) || (character > 64 && character < 90) || (character > 47 && character < 58))) {
                errorHandling(lineNumber); //if it's not a valid variable, throws error
            }
        }
        if (variables.find(s) == variables.end())  { //if it's a valid variable and not declared before, declares
            declareVariable(s);
        }
        return true;
    } else {
        errorHandling(lineNumber);
    }
}

//writes statements for an if loop, specialized for choose function
//takes four parameters, expression that will be evaluated, operator for bool function,
//a condition variable which stores the value of first expression in choose function
//and returnVar which stores value of current expression 

void chooseCondPrint(string expr, string oper, string condVar, string returnVar){
    condNamer++;
    string condName = "choose" + to_string(chooseNamer) + "cond" + to_string(condNamer); //choose function condition label name
    string body = "choose" + to_string(chooseNamer) + "body" + to_string(condNamer); //body label name
    string chooseEnd = "choose" + to_string(chooseNamer) + "end" + to_string(condNamer); //end label name

    otherStatements.push_back("br label %" + condName);
    otherStatements.push_back(condName + ":");

    string boolVar = varNamer(); //bool variable for if statement
    otherStatements.push_back("%"+ boolVar + " = icmp " + oper + " i32 " + condVar + ", 0");
    otherStatements.push_back("br i1 %" + boolVar + ", label %" + body + ", label %"+ chooseEnd);
    otherStatements.push_back(body + ":");

    if(isValidNumber(expr)){
        otherStatements.push_back("store i32 "+ expr +", i32* %" + returnVar);
    } else { //expr2 will be given as a temp var, I want to give its value to another temp var which is returnVar
        otherStatements.push_back("store i32 "+ evaluate(expr) +", i32* %" + returnVar);
    }

    otherStatements.push_back("br label %"+chooseEnd);
    otherStatements.push_back(chooseEnd +":");
}

//takes choose expression as a variable and writes statements for three consecutive if loops

string choose(string var){
    
    var = var.substr(var.find("(")+1);  // deleting first "choose(" and ")" part
    var = var.substr(0, var.size()-1);
    vector<string> exprs;
    int parantheses = 0;
    string expr = "";

    for(char ch : var){ //tries to seperate four expressions in the string
        if( ch == ',' && parantheses == 0){
            exprs.push_back(expr);
            expr = "";
        }else{   
            if(ch == '('){
                parantheses++;
            } else if(ch==')'){
                parantheses--;
            }
            expr = expr + ch;
        }
    }
    if(exprs.size()!=3){  //checks whether expression's constructions are valid
        errorHandling(lineNumber);
    }
    string expr4 = expr;
    string expr3 = exprs.back();
    exprs.pop_back();
    string expr2 = exprs.back();
    exprs.pop_back();   
    string expr1 = exprs.back();
    exprs.pop_back();
    string condVar;

    if(isValidNumber(expr1)){ //if expr1 is a number, makes the condition variable for if loops to store it's value
        string tempRealVar = "%"+varNamer();
        declareStatements.push_back(tempRealVar + " = alloca i32");
        declareStatements.push_back("store i32 "+expr1+", i32* "+tempRealVar);
        condVar = "%" + varNamer();
        otherStatements.push_back(condVar + " = load i32* " + tempRealVar);
    } else {
        condVar = evaluate(expr1); //takes temp var of the expression and makes it the condition variable
    }
    
    //returnVar is the value of the choose function, it equals to values of expr2, expr3 and expr4
    //one of the if statements will be true and that value will be valid during execution

    string returnVar = varNamer(); 
    declareStatements.push_back("%" + returnVar + " = alloca i32");
    declareStatements.push_back("store i32 "+to_string(0)+", i32* %"+ returnVar);

    chooseCondPrint(expr2, "eq", condVar, returnVar);
    chooseCondPrint(expr3, "sgt", condVar, returnVar);
    chooseCondPrint(expr4, "slt", condVar, returnVar);

    chooseNamer++; //update
    condNamer = 0;

    string tempReturnVar = varNamer(); //returning the returnVar's temprory variable
    otherStatements.push_back("%" + tempReturnVar + " = load i32* %" + returnVar);

    return "%"+tempReturnVar;
}

//takes an expression
//checks its validness
//turns it into a postfix expression and put it into a stack called postfixExp token by token

stack<string> postfix(string expr,stack<string> postfixExp){
    stack<string> revPostfix; //stores expression in a reverse order
    string var = "";
    bool takingChoose = false; //when a choose function comes, taking until choose finishes
    string prevOper = ""; //for checking existence of consequtive operators
    string nestedChoose; //if there are nested chosose functions, stores parts of the function to take the all function 
    int nested=0;//number of nested parantheses

    for (char ch : expr) { //checking character by character

        if(takingChoose){ //if there is a choose function
            
            if(ch != ')'){ //if closed parantheses has not yet come
                if (ch == expr.back() && expr.find(ch)==(expr.size()-1) ) //if reached the end while expecting ')' throws error
                {
                    errorHandling(lineNumber);
                }

                if(ch == '('){ 
                   nested++;
                }
                var = var + ch;
            } else { // character equals ')'
                if(nested>0){  //update for the warning
                    nested--;
                    var = var + ch;
                }else{
                    if(nested<0){
                        errorHandling(lineNumber);
                    } else { // choose function taking is finished
                        var = "choose(" + nestedChoose + var + ")";
                        revPostfix.push(var); //puts it into the stack
                        var = "";
                        prevOper = "";
                        takingChoose = false;
                    }
                }
            }

        } else if (operators.find(ch) < operators.length()) { // ch is an operator

            if(ch == expr.back() && ch != ')' ){  // The last character of the string can not be an operator except ")", a variable is expected
                errorHandling(lineNumber); 
            }

            if(prevOper != "" ){  // checks validness of consequtive elements
                if(ch == ')'){   //only +( and )+ and )) and (( is valid, others are invalid
                    if(prevOper != ")"){
                        errorHandling(lineNumber);
                    }
                } else if(prevOper == "("){
                    if(ch != '('){
                        errorHandling(lineNumber);
                    }
                }else if(prevOper != ")" && ch != '('){
                    errorHandling(lineNumber);  
                } 
            }
            string s(1,ch); //turn char to string
            prevOper = s;

            if(ch == '('){  // detecting choose
                if(var=="choose"){
                    var="";
                    takingChoose = true; //for taking choose
                    continue; 
                }
            }

            if (var != "") {
                revPostfix.push(var); //push prev var to postfix
                var = "";
            }

            if (oper.empty()) {  // if operator stack is empty
                if(ch == ')'){  // '(' was expected
                    errorHandling(lineNumber);
                }
                oper.push(ch); // or directly put operator to stack
            } else { // ,f stack is not empty
                if (ch == ')') {
                    while (oper.top() != '(') //found a closed parantheses pop all the elements until reaching '('
                    {
                        string opTop(1, oper.top());
                        revPostfix.push(opTop);
                        oper.pop();
                    
                        if(oper.empty()){ //'(' was expected, reached the end however
                        errorHandling(lineNumber); 
                        }
                    }                    
                    oper.pop(); 
                } else {  //if not equals '('
                    while (!oper.empty() && ch != '(' && oper.top() != '('){ //checks precedence until reaching '('
                        if (smaller.find(oper.top()) < 2 && bigger.find(ch) < 2){ // if char is a operator with bigger precedence
                            break;
                        }
                        string opTop(1, oper.top());
                        revPostfix.push(opTop);
                        oper.pop();
                    }
                    oper.push(ch);
                }
            }

        } else { //character is a variable
            var = var + ch;
            prevOper = "";
        }
    }

    if (var != "") {
        revPostfix.push(var); //push prev var to postfix
        var = "";
    }

    while(!oper.empty()){//push all operators left to stack
        string opTop(1, oper.top());
        revPostfix.push(opTop);
        oper.pop();
    }

    while (!revPostfix.empty())  //reverse the stack
    {
        postfixExp.push(revPostfix.top()); 
        revPostfix.pop();
    }
    return postfixExp;
}

//takes expression as a parameter and writes statements to evaluate the expression

string evaluate(string expr){
    if (expr == "") //checks whether given expression is null
    {
        errorHandling(lineNumber);
    }
    
    stack<string> postfixExp;

    postfixExp = postfix(expr,postfixExp); //making new postfix expression
    if (postfixExp.size() == 1){   //if there is only a number inside just return number

        if(isValidNumber(postfixExp.top())){ //checks before its a valid number
            return postfixExp.top(); 
        } else {
            string singleVar;
            string loadVar;
            if(postfixExp.top().find("choose")<postfixExp.top().size()){  //if the expression is a choose function, calls choose function
                return choose(postfixExp.top());  //returns the temprory variable that came from the function
            } else if(isValidVariable(postfixExp.top())){ //if it is a valid var
                singleVar = varNamer();
                otherStatements.push_back("%" + singleVar + " = load i32* %" + postfixExp.top());  //makes a temp var and sends it
                return "%" + singleVar;
            } else {
                errorHandling(lineNumber);
            }
        }
    }

    stack<pair<string,bool>> taken; //stores variables or evaluated expression parts
    string returnVar = varNamer();
    declareVariable(returnVar);  // This will be returned

    while (!postfixExp.empty())  { // until expression finishes

        string s_top = postfixExp.top(); //string on the top

        if (!(operators.find(s_top) < operators.length()) || (s_top.find("choose(")<s_top.size())){ //s_top is not an operator
            taken.push(make_pair(s_top,false)); // simply push it to stack, it's a variable or a choose function
            postfixExp.pop(); 

        } else { //s_top is an operator

            bool isTempVar1, isTempVar2; //to avoid declaration of temprory variables 

            string var2 = taken.top().first; //take variable
            if(var2.find("choose(")<var2.size()){  //check if it is a choose function
                var2 = choose(var2); //temp var returned 
                isTempVar2 = true;  
            } else {
                isTempVar2 = taken.top().second; 
            }
            taken.pop();
             
            string var1 = taken.top().first;  //making same thing for other variable
            if(var1.find("choose(")<var1.size()){
                var1 = choose(var1);
                isTempVar1 = true;
            } else {
                isTempVar1 = taken.top().second;
            }
            taken.pop();

            string operation;  //determine what is the operator
            if(s_top == "+"){
                operation = "add";
            }else if(s_top == "-"){
                operation = "sub";    
            }else if(s_top == "*"){
                operation = "mul";    
            }else{
                operation = "sdiv";
            }

            string operand1, operand2;  //determining operands for the expression
            
            if(!isValidNumber(var1)){   //if var1 iis not a number
                isValidVariable(var1,isTempVar1);  //if it is a valid variable or a temp variable
                 if(!isTempVar1){ //if a valid var
                    operand1 = "%"+varNamer();  //make the operand as a temp var
                    otherStatements.push_back( operand1 + " = load i32* %" + var1); //load its value to a temp var
                 } else {
                     operand1 = var1;
                 }
            } else {  //if var1 is a number
                otherStatements.push_back("store i32 "+ var1 +", i32* %" + returnVar);  //use the returnVar above for loading its value to a var
                operand1 = "%"+varNamer();
                otherStatements.push_back( operand1 + " = load i32* %" + returnVar);
            }

            if(!isValidNumber(var2)){  //making similar things for var2
                 isValidVariable(var2,isTempVar2);
                if(!isTempVar2){ 
                    operand2 = "%"+varNamer();
                    otherStatements.push_back( operand2 + " = load i32* %" + var2);
                } else {
                     operand2 = var2;
                }
            } else {
                operand2 = var2;
            }
      
            string pushVar = "%" + varNamer();  //temp variable to store the value of the operation
            otherStatements.push_back(pushVar+" = " + operation + " i32 " + operand1 + ", " + operand2);  //operation

            postfixExp.pop(); //pop the operator 
            taken.push(make_pair(pushVar,true));  //push the operation

        }
    }

    string topVar = taken.top().first;
    taken.pop();
    return topVar; //return the temp variable that stores the value of the expression
}

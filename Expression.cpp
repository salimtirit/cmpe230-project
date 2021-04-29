#include <iostream>
#include <stack>
#include <cstring>
#include <map>

using namespace std;
int lineNumber = 0;
map<string, int> variables; //name may ned to be changed due to conflicts

string operators = "-+()/*";
string bigger = "*/";
string smaller = "-+";
stack<string> postfixExp;
stack<string> expression;
stack<char> oper;
int namer = 0;
int chooseNamer = 0;
string evaluate(string expr);

string varNamer(){
    return "_t" + to_string(namer++);
}

void declareVariable(string name, int value = 0)
{
    if (variables.find(name) == variables.end())
    {
        cout << "%" << name << " = alloca i32" << endl;
    }
    variables[name] = value;                              //  variables.insert(pair<string,int>(name,value));
    cout << "store i32 " << variables.find(name)->second  << ", i32* %" << name << endl; //variables[name]->second
}

bool isValidNumber(string s){
    
    for (int j = 0; j < s.length(); j++) {
        int character = s[j];
        if (!(character > 47 && character < 58)) //47 , 58 are the ascii table values
        {
            //no need to throw error message
            return false; 
        }
    }
    return true;
}

//returns true if the variable is valid
//valid variable: first character from alphabet (upper or lowercase) preceeding with alphanumeric characters

bool isValidVariable(string s, bool isTempVar = false)
{
 //   if(variables.find(s) == variables.end() && (variables.size()>1)){
   //     return true;
    //}

    if(isTempVar){
        return false;
    }

    int firstChr = s[0];
    if ((firstChr > 96 && firstChr < 123) || (firstChr > 64 && firstChr < 90)) // all ascii table values 
    {
        for (int i = 1; i < s.length(); i++)  {
            int character = s[i];
            if (!((character > 96 && character < 123) || (character > 64 && character < 90) || (character > 47 && character < 58))) {
                cout << "Invalid variable in line #" << lineNumber << endl;
                exit(0);
                //return false;
            }
        }
        if (variables.find(s) == variables.end())  { //if it's not declared before
            declareVariable(s);
        }
        return true;
    } else {
        cout << "Invalid variable in line #" << lineNumber << endl;
        exit(0);
    }
}

string choose(string var){

    var = var.substr(var.find("(")+1);  // deleting first "choose(" and ")" part
    var = var.substr(0, var.size()-1);
    vector<string> exprs;
    for ( int i = 0; i < 3; i++) {

        if(var.find(",")<var.size()){
            string expr = var.substr(0, var.find(",")); 
            var=var.substr(var.find(",")+1);
            exprs.push_back(expr);
        }else {
            cout << "error in line: "<< lineNumber <<endl;
            exit(0);
        }
    }

    string expr4 = var;
    string expr3 = exprs.back();
    exprs.pop_back();
    string expr2 = exprs.back();
    exprs.pop_back();   
    string expr1 = exprs.back();
    exprs.pop_back();
    
    string condName = "choose" + to_string(chooseNamer) + "cond";
    string body0 = "choose" + to_string(chooseNamer) + "body0";
    string body1 = "choose" + to_string(chooseNamer) + "body1";
    string body1_1 = "choose" + to_string(chooseNamer) + "body1.1";
    string body1_2 = "choose" + to_string(chooseNamer) + "body1.2";
    string choseEnd = "choose" + to_string(chooseNamer) + "end";
    chooseNamer++;
    string returnVar = varNamer();

    //string temp_expr1 = evaluate(expr1);  //possibility of return val is a number
    //expr1 = varNamer();
    //cout << "%" + expr1 + " = alloca i32" << endl;
    //cout << "store i32 " + temp_expr1 + ", i32* %" + expr1 << endl;  // temp_expr1 can return a number so I did this

    cout << "br label %" << condName << endl; //br label %choose0cond
    cout << condName << ":" << endl; //choose0cond:

    if(isValidNumber(expr1)){
        string tempVar = varNamer();
        cout << "%"+tempVar + " = alloca i32"<<endl;  //making a new variable that stores the num
        cout << "store i32 "+ expr1 +", i32* %" + tempVar << endl;
        string firstEq = "%" + varNamer(); 
        cout << firstEq << " = load i32* %" << tempVar << endl;  //load condition to a variable
        expr1 = firstEq;
    } else {
        expr1 = evaluate(expr1);
    }
    string secondEq ="%" + varNamer();
    cout << secondEq << " = icmp eq i32 " << expr1 << ", 0" << endl; //compare with 0
    cout << "br i1 " << secondEq << ", label %" << body0 << ", label %"<< body1 << endl; 
    
    cout << body0 << ":" << endl;
    string tempVar2 = varNamer();
    cout << "%"+tempVar2 + " = alloca i32"<<endl;  //making a new variable that stores the num
     if(!(isValidNumber(expr2))){
         expr2 = evaluate(expr2);
    }
    cout << "store i32 "+ expr2 +", i32* %" + tempVar2 << endl;
    cout << "%"+ returnVar << " = load %" << tempVar2 << endl;
    cout << "br label " << choseEnd << endl;

    cout << body1 << ":" << endl;
    string thirdEq = "%_t" + to_string(namer++);
    cout << thirdEq << " = icmp sgt i32 " << expr1 << ", 0" << endl; // may not be third?
    cout << "br i1 " << thirdEq << ", label %"  << body1_1  << ", label %" << body1_2 << endl;

    cout << body1_1 << ":" << endl;
    string tempVar3 = varNamer();
    cout << "%"+tempVar3 + " = alloca i32"<<endl;  //making a new variable that stores the num
     if(!(isValidNumber(expr3))){
         expr3 = evaluate(expr3);
    }
    cout << "store i32 "+ expr3 +", i32* %" + tempVar3 << endl;
    cout << "%"+ returnVar << " = load %" << tempVar3 << endl;
    cout << "br label " << choseEnd << endl;

    cout << body1_2 << ":" << endl;
    string tempVar4 = varNamer();
    cout << "%"+tempVar4 + " = alloca i32"<<endl;  //making a new variable that stores the num
    if(!(isValidNumber(expr4))){
         expr4 = evaluate(expr4);
    }
    cout << "store i32 "+ expr4 +", i32* %" + tempVar4 << endl;
    cout << "%"+ returnVar << " = load %" << tempVar4 << endl;
    cout << "br label " << choseEnd << endl;
    cout << choseEnd << ":" << endl;

    return returnVar;
}

void postfix(string expr){
    stack<string> revPostfix;
    string var = "";
    bool takingChoose = false; 
    string prevOper = "";
    string nestedChoose;
    int nested=0;
    int nestedParantheses = 0;
    for (char ch : expr) {

        if(takingChoose){
            
            if(ch != ')'){ //kalan ifadeyi almayı deniyorum
                if (ch == expr.back()) // ')' was expected, reached the end however
                {
                    cout << "error in line: " << lineNumber << endl;
                    exit(0);
                }

                if(ch == '('){
                    if(var.find("choose")<var.size()){ //nested choose
                        nestedChoose = nestedChoose + var + ch;
                        var = "";

                        nested++;
                    } 
                } else {
                var = var + ch;
                }
            } else { // ch == ')'
                
                if(nested){
                    var = var + ch;
                    nested--;
                    continue;
                } else { // finished
                    var = "choose(" + nestedChoose + var + ")";
                    revPostfix.push(var);
                    var = "";
                    prevOper = "";
                    takingChoose = false;
                }
            }

        } else if (operators.find(ch) < operators.length()) { // ch bir operator

            if(ch == expr.back() && ch != ')' ){  // The last character of the string can not be an operator except ")", a variable is expected
                cout << "syntax error in line: " << lineNumber << endl;
                exit(0);  
            }

            if(prevOper != "" ){  // only +( and )+ and )) and (( is valid, others are invalid
                if(ch == ')'){   //
                    if(prevOper != ")"){
                        cout << "syntax error in line: " << lineNumber << endl;
                        exit(0);  
                    }
                } else if(prevOper == "("){
                    if(ch != '('){
                        cout << "syntax error in line: " << lineNumber << endl;
                        exit(0);
                    }
                }else if(prevOper != ")" && ch != '('){
                    cout << "syntax error in line: " << lineNumber << endl;
                    exit(0);  
                } 
            }
            string s(1,ch);
            prevOper = s;

            if(ch == '('){  // detecting choose
                if(var=="choose"){
                    var="";
                    takingChoose = true;
                    continue; //does this work?
                }
            }

            if (var != "") {
                revPostfix.push(var); //push prev var to postfix
                var = "";
            }

            if (oper.empty()) {    // if operator stack is empty
                if(ch == ')'){
                    cout << "'(' is expected, error in line: " << lineNumber << endl;
                    exit(0); 
                }
                oper.push(ch); // directly put ch to stack
            } else { // stack boş değilse
                if (ch == ')') {
                    while (oper.top() != '(')
                    {
                        string opTop(1, oper.top());
                        revPostfix.push(opTop);
                        oper.pop();
                    
                        if(oper.empty()){
                        cout << "'(' is expected, error in line: " << lineNumber << endl;
                        exit(0); 
                        }
                    }                    
                    oper.pop();
                } else {
                    while (!oper.empty() && ch != '(' && oper.top() != '('){ //precedence
                        if (smaller.find(oper.top()) < 2 && bigger.find(ch) < 2){
                            break;
                        }
                        string opTop(1, oper.top());
                        revPostfix.push(opTop);
                        oper.pop();
                    }
                    oper.push(ch);
                }
            }

        } else { //ch is a variable
            var = var + ch;
            prevOper = "";
        }
    }

    if (var != "") {
        revPostfix.push(var); //push prev var to postfix
        var = "";
    }

    while(!oper.empty()){
        
        string opTop(1, oper.top());
        revPostfix.push(opTop);
        oper.pop();
    }

    while (!revPostfix.empty())
    {
        postfixExp.push(revPostfix.top());
        revPostfix.pop();
    }
}

//string operators = "-+()/*";  yukarıda

string evaluate(string expr){

    while (!postfixExp.empty()){ //empty stack before any expression
        postfixExp.pop();
    }

    postfix(expr); //making new postfix expression
    if (postfixExp.size() == 1){   //if there is only a number inside just return number

        if(isValidNumber(postfixExp.top())){ //checks before its a valid number
            return postfixExp.top(); //->There should be something like declaereVariable ?
        } else {
             string singleVar;
             string loadVar;
            if(postfixExp.top().find("choose")<postfixExp.top().size()){
                return choose(postfixExp.top());
              //  loadVar = "_t" + to_string(namer++);
            } else if(isValidVariable(postfixExp.top())){
                singleVar = "_t" + to_string(namer++);
                loadVar = postfixExp.top();
                cout << "%" + singleVar + " = load i32* %" + loadVar << endl;
                return "%" + singleVar;
            } else {
                cout << "Invalid variable in line #" << lineNumber << endl;
                exit(0);
            }
        }
    }

    stack<pair<string,bool>> taken; 

    while (!postfixExp.empty())  { // stack boş olana dek

        string s_top = postfixExp.top();

        if(s_top.find("choose")<s_top.size()){
            string toTaken = choose(postfixExp.top());
            taken.push(make_pair(toTaken,false));

        }else if (!(operators.find(s_top) < operators.length())){ //s_top operator değil ise
            taken.push(make_pair(s_top,false)); // diğer stack'e at
            postfixExp.pop(); //I will check validity during operations since I need to know if it is a numb or a var

        } else { //s_top bir operator ise

            string var2 = taken.top().first;
            bool isTempVar2 = taken.top().second;
            taken.pop();

            string var1 = taken.top().first; 
            bool isTempVar1 = taken.top().second; 
            taken.pop();

            string operation;
            if(s_top == "+"){
                operation = "add";
            }else if(s_top == "-"){
                operation = "sub";    
            }else if(s_top == "*"){
                operation = "mul";    
            }else{
                operation = "udiv";
            }

            string pushVar, loadVar, opVar; //variable to be pushed, temp var for loading, temp var for operation
            
            if(!isValidNumber(var1)){ //var1 is not a number
                isValidVariable(var1,isTempVar1); 

                pushVar = var1;
                loadVar = "_t" + to_string(namer++);
                cout << "%" + loadVar + " = load i32* %" + pushVar << endl;

            } else { //var1 is a number
                pushVar = "_t" + to_string(namer++);
                cout << "%" + pushVar + " = alloca i32" << endl;  //alloca new var
                cout << "store i32 " + var1 + ", i32* %" + pushVar << endl;
        
                loadVar = "_t" + to_string(namer++);
                cout << "%" + loadVar + " = load i32* %" + pushVar << endl;
               
            } 

            if(!isValidNumber(var2)){  // var2 is variable
                isValidVariable(var2,isTempVar2);
                opVar = "%t" + to_string(namer++); // % kısmı önemli
                cout << opVar + " = load i32* %" + var2 << endl;  //load

            } else { //var2 is number
               opVar = var2;

            }

            string tempVar = "_t" + to_string(namer++);
            cout << "%"+tempVar+" = " + operation + " i32 %" + loadVar + ", " + opVar << endl;
            cout <<"store i32 %"+tempVar+", i32* %"+pushVar << endl;

            postfixExp.pop();
            taken.push(make_pair(pushVar,true));

        }
    }

    string sendVar = "_t" + to_string(namer++);
    cout << "%" + sendVar + " = load i32* %" + taken.top().first << endl;
    return "%" + sendVar;
}

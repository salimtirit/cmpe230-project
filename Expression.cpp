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
    if(variables.find(s) == variables.end()){
        return true;
    }

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

    expr1 = evaluate(expr1);  //until no choose left
    
    cout << "br label %" << condName << endl;
    cout << condName << ":" << endl;
    string firstEq = "%_t" + to_string(namer++);
    cout << firstEq <<"= load %32* %" << expr1 << endl;
    string secondEq = "%_t" + to_string(namer++);
    cout << secondEq << " icmp eq i32 " << firstEq << ", 0" << endl;
    cout << "br i1 " << secondEq << ", label %" << body0 << ", label %"<< body1 << endl; 
    
    string thirdEq = "%_t" + to_string(namer++);
    cout << body0 << ":" << endl;
    if(expr2.find("choose(")< expr2.length()){
        expr2 = choose(expr2);
    }
    cout << "%_t" << to_string(namer+1) << "= load %" << expr2;
    cout << "br label" << choseEnd << endl;

    cout << body1 << ":" << endl;
    
    cout << thirdEq << "icmp sgt i32 %" << expr1 << ", 0" << endl; // may not be third?
    cout << "br i1 " << thirdEq << ", label %"  << body1_1  << ", label %" << body1_2 << endl;

    cout << body1_1 << ":" << endl;
    if(expr3.find("choose(")< expr3.length()){
        expr3 = choose(expr3);
    }
    string fourthEq = "%_t" + to_string(namer++);
    cout << fourthEq << "= load %" << expr3;
    cout << "br label" << choseEnd << endl;

    cout << body1_2 << ":" << endl;
    if(expr4.find("choose(")< expr4.length()){
        expr4 = choose(expr4);
    }
    cout << fourthEq << "= load %" << expr4;
    cout << "br label" << choseEnd << endl;

    cout << choseEnd << ":" << endl;


    //string _valExpr1 = evaluate(expr1);
    //exit(0);
    //_valExpr1 i bir variable olarak define edeceğim çünkü temp var olması biraz zor olabilir?
    //_valExpr1 = evaluate(expr1);
    //for(i=0; i<3 ; i++){
    // llvm için ardışık 3 if kodu?
    //}

    return "";
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
    string singleVar;
    if (postfixExp.size() == 1){   //if there is only a number inside just return number

        if(isValidNumber(postfixExp.top())){ //checks before its a valid number
            return postfixExp.top(); //->There should be something like declaereVariable ?
        } else {
            if(postfixExp.top().find("choose")<postfixExp.top().size()){
                singleVar = choose(postfixExp.top());
            } else if(isValidVariable(postfixExp.top())){
                singleVar = "_t" + to_string(namer++);
            } else {
                cout << "Invalid variable in line #" << lineNumber << endl;
                exit(0);
            }
            cout << "%" + singleVar + " = load i32* %" + postfixExp.top() << endl;
            return "%" + singleVar;
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

#include <iostream>
#include <stack>
#include <cstring>
#include <map>

using namespace std;
int lineNumber;
map<string, int> variables; //name may ned to be changed due to conflicts

string operators = "-+()/*";
string bigger = "*/";
string smaller = "-+";
stack<string> postfixExp;
stack<string> expression;
stack<char> oper;
int namer = 0;

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

bool isValidVariable(string s)
{
    bool isTempVar = false;
        if(s[0]=='t'){
            isTempVar = true;
            for(int i = 1; i<s.size(); i++){
                int character = s[i];
                if (!(character > 47 && character < 58)){
                    isTempVar = false;
                }
            }
        }
        if(isTempVar){
            return false;;
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

void postfix(string expr)
{
    stack<string> revPostfix;
    string var = "";
    for (char ch : expr)
    {
        if (operators.find(ch) < operators.length())
        { // ch bir operator

            if (var != "")
            {
                revPostfix.push(var); //push prev var to postfix
                var = "";
            }

            if (oper.empty())
            {                  // stack boş ise
                oper.push(ch); // ch yi direkt stack e koy
            }
            else
            { // stack boş değilse

                if (ch == ')')
                {

                    while (oper.top() != '(')
                    {

                        string opTop(1, oper.top());
                        revPostfix.push(opTop);
                        oper.pop();
                    }

                    oper.pop();
                }
                else
                {
                    while (!oper.empty() && ch != '(' && oper.top() != '(')
                    {
                        if (smaller.find(oper.top()) < 2 && bigger.find(ch) < 2)
                        {
                            break;
                        }
                        string opTop(1, oper.top());
                        revPostfix.push(opTop);
                        oper.pop();
                    }
                    oper.push(ch);
                }
            }
        }
        else
        { //ch bir variable
            var = var + ch;
        }
    }

    if (var != "")
    {
        revPostfix.push(var); //push prev var to postfix
        var = "";
    }

    if (!oper.empty())
    {
        string opTop(1, oper.top());
        revPostfix.push(opTop);
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
        }else {
            cout << "Invalid variable in line #" << lineNumber << endl;
            exit(0);
        }
    }

    stack<string> taken; 

    while (!postfixExp.empty())  { // stack boş olana dek

        string s_top = postfixExp.top();
        if (!(operators.find(s_top) < operators.length())){ //s_top operator değil ise
            taken.push(s_top); // diğer stack'e at
            postfixExp.pop(); //I will check validity during operations since I need to know if it is a numb or a var

        } else { //s_top bir operator ise

            string var2 = taken.top();
            taken.pop();

            string var1 = taken.top();  
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
                isValidVariable(var1); 

                pushVar = var1;
                loadVar = "t" + to_string(namer++);
                cout << "%" + loadVar + " = load i32* %" + pushVar << endl;

            } else { //var1 is a number
                pushVar = "t" + to_string(namer++);
                cout << "%" + pushVar + " = alloca i32" << endl;  //alloca new var
                cout << "store i32 " + var1 + ", i32* %" + pushVar << endl;
        
                loadVar = "t" + to_string(namer++);
                cout << "%" + loadVar + " = load i32* %" + pushVar << endl;
               
            } 

            if(!isValidNumber(var2)){  // var2 is variable
                isValidVariable(var2);
                opVar = "%t" + to_string(namer++); // % kısmı önemli
                cout << opVar + " = load i32* %" + var2 << endl;  //load

            } else { //var2 is number
               opVar = var2;

            }

            string tempVar = "t" + to_string(namer++);
            cout << "%"+tempVar+" = " + operation + " i32 %" + loadVar + ", " + opVar << endl;
            cout <<"store i32 %"+tempVar+", i32* %"+pushVar << endl;

            postfixExp.pop();
            taken.push(pushVar);
        }
    }

    string sendVar = "t" + to_string(namer++);
    cout << "%" + sendVar + " = load i32* %" + taken.top() << endl;
    return sendVar;
}

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
//----------------------TODO-------------------------------------------
//variable values must be checked
//can be a function findVariable which returns true/false and also changes a value with alias operator
//if the variables are not defined before they need to be equal to zero
//postfix and evaluate should work together

void declareVariable(string name, int value = 0)
{
    if (variables.find(name) == variables.end())
    {
        cout << "%" << name << " = alloca i32" << endl;
    }
    variables[name] = value;                              //  variables.insert(pair<string,int>(name,value));
    cout << "store i32 " << variables.find(name)->second  << ", i32* %" << name << endl; //variables[name]->second
}

//returns true if the variable is valid
//valid variable: first character from alphabet (upper or lowercase) preceeding with alphanumeric characters
bool isValidVariable(string s) //variable
{
    int firstChr = s[0];
    if ((firstChr > 96 && firstChr < 123) || (firstChr > 64 && firstChr < 90)) // all ascii table values
    {
        for (int i = 1; i < s.length(); i++)
        {
            int character = s[i];
            if (!((character > 96 && character < 123) || (character > 64 && character < 90) || (character > 47 && character < 58)))
            {
                cout << "Invalid variable in line #" << lineNumber << endl;
                exit(0);
                //return false;
            }
        }
        if (variables.find(s) == variables.end())
        {
            declareVariable(s);
        }
        return true;
    }
    else if (firstChr > 47 && firstChr < 58)
    { //number

        for (int j = 1; j < s.length(); j++)
        {
            int character = s[j];
            if (!(character > 47 && character < 58)) //47 , 58 are the ascii table values
            {
                cout << "Invalid number in line #" << lineNumber << endl;
                exit(0);
                //return false;
            }
        }
        return true;
    }
    else
    {
        cout << "Invalid variable in line #" << lineNumber << endl;
        exit(0);
    }
    //return false;
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
                isValidVariable(var); //yeni operator geldiğine göre prev variable bitti, checks whether var is valid
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
        isValidVariable(var); //operator en son gelmemiş olacağı için
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

string evaluate(string expr)
{

    while (!postfixExp.empty())
    { //empty stack before any expression
        postfixExp.pop();
    }

    postfix(expr); //making new postfix expression

    if (postfixExp.size() == 1)
    {                            //if there is only a number inside just return number
        return postfixExp.top(); //->There should be something like declaereVariable ?
    }

    /* while(!postfixExp.empty()){  //empty stack before any expression
    cout << postfixExp.top() + " ";
    postfixExp.pop();
    }*/

    stack<string> taken;
    // int namer = 0 yukarıda
    string namer1 = "t" + to_string(namer++);
    string s1 = "%" + namer1 + " = load i32* %" + postfixExp.top();
    cout << s1 << endl;

    taken.push(namer1);
    postfixExp.pop();
    while (!postfixExp.empty())
    { // stack boş olana dek

        string s_top = postfixExp.top();
        if (!(operators.find(s_top) < operators.length()))
        {                      //s_top operator değil ise
            taken.push(s_top); // diğer stack'e at
            postfixExp.pop();
        }
        else
        { //s_top bir operator ise

            //before evaluating we need to check if the variable is defined before. findVariable function.
            //If not defined we need to make it zero but before initializing
            //to zero we need to check if the variable conditions are satisfied.
            string var1 = taken.top();
            taken.pop();
            string var2 = taken.top();
            taken.pop();
            string namer2 = "t" + to_string(namer++);
            string s2 = "%" + namer2 + " = load i32* %" + var1;
            string namer3 = "t" + to_string(namer++);
            cout << s2 << endl;
            if (s_top == "+" || s_top == "-")
            {
                if (s_top == "-")
                {
                    string s3 = "%" + namer3 + " = sub i32 %" + var2 + ", %" + namer2;
                    cout << s3 << endl;
                }
                else
                {
                    string s3 = "%" + namer3 + " = add i32 %" + var2 + ", %" + namer2;
                    cout << s3 << endl;
                }
            }
            else
            {
                if (s_top == "*")
                {
                    string s3 = "%" + namer3 + " = mul i32 %" + var2 + ", %" + namer2;
                    cout << s3 << endl;
                }
                else
                {

                    //This is going to be (?) sdiv instead of udiv check piazza
                    string s3 = "%" + namer3 + " = udiv i32 %" + var2 + ", %" + namer2;
                    cout << s3 << endl;
                }
            }
            postfixExp.pop();
            taken.push("%" + namer3);
        }
    }
    return taken.top();
}

#include <iostream>
#include <stack>
#include <cstring>

using namespace std;

string operators = "-+()/*";
string bigger = "*/";
string smaller = "-+";
stack<char> oper;

string postfix(string expr)
{

    string postfixExp = "";
    for (char ch : expr)
    {

        if (operators.find(ch) < operators.length())
        { // ch bir operator  // değiştir burayı

            if (oper.empty())
            {                  // stack boş
                oper.push(ch); // ch yi stack a koy
            }
            else
            { // stack boş değil

                if (ch == ')')
                {

                    while (oper.top() != '(')
                    {

                        postfixExp = postfixExp + oper.top();
                        oper.pop();
                    }

                    oper.pop();
                }
                else
                {
                    // cout << "yeni gelen = " <<operators.find(ch) << "içeride olan = "<< operators.find(oper.top()) << endl;

                    // cout << operators.find(ch)-operators.find(oper.top()) << endl;

                    // cout << !(operators.find(ch)-operators.find(oper.top())>1) << endl;

                    while (!oper.empty() && ch != '(' && oper.top() != '(')
                    {
                        if (smaller.find(oper.top()) < 2 && bigger.find(ch) < 2)
                        {
                            break;
                        }

                        postfixExp = postfixExp + oper.top();
                        oper.pop();
                    }

                    oper.push(ch);
                }
            }
        }
        else
        { //ch bir variable
            postfixExp = postfixExp + ch;
        }
    }


    return postfixExp + oper.top();
}

int evaluate(string str){
    return 0;
}
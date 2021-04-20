#include <iostream>
#include <stack>
#include <cstring>

using namespace std;

string operators = "-+()/*";
string bigger = "*/";
string smaller = "-+";
stack<string> postfixExp;
stack<string> expression;
stack<char> oper;

stack<string> postfix(string expr) {
    string var = "";
    for (char ch : expr) {

        if (operators.find(ch) < operators.length()) { // ch bir operator  // değiştir burayı

            postfixExp.push(var);
            var = "";

            if (oper.empty()) {                  // stack boş
                oper.push(ch); // ch yi stack a koy
            } else { // stack boş değil

                if (ch == ')') {

                    while (oper.top() != '(') {

                           string opTop(1, oper.top());
                        postfixExp.push(opTop);
                        oper.pop();
                    }

                    oper.pop();
                } else {
                    while (!oper.empty() && ch != '(' && oper.top() != '(') {
                        if (smaller.find(oper.top()) < 2 && bigger.find(ch) < 2) {
                            break;
                        }
                         string opTop(1, oper.top());
                        postfixExp.push(opTop);
                        oper.pop();
                    }
                    oper.push(ch);
                }
            }
        } else { //ch bir variable
            var = var + ch;
        }
    }

    string opTop(1, oper.top());
    postfixExp.push(opTop);

    while(!postfixExp.empty()){
        expression.push(postfixExp.top());
        postfixExp.pop();
    }
    
    return expression;
}


//string operators = "-+()/*";  yukarıda

int evaluate(stack<string> postfixExp){
    stack<string> taken;
    int namer = 0;
    string namer1 = to_string(namer++);
    string s1 = "%t"+namer1+" = load i32* %"+postfixExp.top();
    cout << s1 << endl;
    taken.push(namer1);
    postfixExp.pop();
    while(!postfixExp.empty()){  // stack boş olana dek

        string s_top = postfixExp.top();
        if(!(operators.find(s_top)<operators.length())){  //s_top operator değil ise
            taken.push(s_top); // diğer stack'e at
            postfixExp.pop();

        }else{  //s_top bir operator ise

            string var1 = taken.top();
            taken.pop();
            string var2 = taken.top();
            taken.pop();
            string namer2 = to_string(namer++);
            string s2 = "%t"+namer2+" = load i32* %"+var1;
            string namer3 = to_string(namer++);
            cout << s2 << endl;
            if(s_top == "+"||s_top == "-"){
                if (s_top == "-")
                {
                    string s3 = "%t"+namer3+" = sub i32 %t"+var2+", %t"+namer2;
                    cout << s3 << endl;
                }
                else{
                    string s3 = "%t"+namer3+" = add i32 %t"+var2+", %t"+namer2;
                    cout << s3 << endl;
                }
                //cout << "store i32 %t"+namer3+", i32* %"+var2 << endl;
            //%t1 = load i32* %var1
            //%t2 = load i32* %var2
            //%t3 = add i32* %t1, %t2
            //%t4 = sub i32* %t3, %var3
            //store i32 %t3, i32* %var2   // var2=var1+var2
            //taken.push(var2);

            }else{
                if(s_top=="*"){
                    string s3 = "%t"+namer3+" = mul i32 %t"+var2+", %t"+namer2;
                    cout << s3 << endl;
                }else{
                    string s3 = "%t"+namer3+" = udiv i32 %t"+var2+", %t"+namer2;
                    cout << s3 << endl;
                }
            }
            postfixExp.pop();
                
            taken.push(namer3);
        }



    }

    return 0;
}
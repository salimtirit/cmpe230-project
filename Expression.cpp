#include <iostream>
#include <stack>
#include <cstring>

using namespace std;

string operators = "-+()/*";
stack<char> oper;

string postfix(string expr){
  
  string postfixExp = "";
  for(char ch : expr){

      if(operators.find(ch) < operators.length()){  // ch bir operator  // değiştir burayı  

        if(oper.empty()){    // stack boş
            oper.push(ch);  // ch yi stack a koy
        } else {  // stack boş değil

            if(ch==')'){

                while(oper.top()!='('){

                    postfixExp = postfixExp + oper.top();
                    oper.pop();
                }

                oper.pop();

            } else {

                while( ch!='(' || oper.top()!='(' || !oper.empty() || !(operators.find(ch)-operators.find(oper.top())>1) ){

                    postfixExp = postfixExp + oper.top();
                    oper.pop();

                }

                oper.push(ch);
            }
            
        }

      } else {  //ch bir variable
          postfixExp = postfixExp + ch;
      }





  }



}

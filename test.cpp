#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <stack>
#include "Expression.cpp"
using namespace std;


int main(int argc, char const *argv[]){
    
    string expr = "KwwA+L-dM*N+(11O*P)*2W/U/V*(T+Q)";
 
    stack<string> postfixExp  = postfix(expr);
    evaluate(postfixExp);
    
    //KL+MN*-OP*W*U/V/T*+Q+
    
    return 0;
}

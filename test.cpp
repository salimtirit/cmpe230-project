#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <stack>
#include "Expression.cpp"
using namespace std;


int main(int argc, char const *argv[]){
    
    string expr = "K+L-M*N+(O*P)*W/U/V*(T+Q)";
    //string a = "+*-/*-//*/+-";
    string q  = postfix(expr);
    cout << q << endl;  
    
    //KL+MN*-OP*W*U/V/T*+Q+
    
    return 0;
}
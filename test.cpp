#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <stack>
#include "Expression.cpp"
using namespace std;


int main(int argc, char const *argv[]){
    
    string expr = "K+L-M*(N+W)/U/V*T+Q";
    string q  = postfix(expr);
    cout << q << endl;  
    

    
    return 0;
}
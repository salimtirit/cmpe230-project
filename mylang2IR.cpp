#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <stack>

using namespace std;
//there can be a map for variables
//whenever we see a string type in "factor" part, after checking whether it was parentezli expression
// we can add it to the map

stack<string> postfix;
int varNamer = 0;

string handleParnt(string expr){ // burası sırf parantezli kısımlara isim verip saklamak için  

    string name = "par_" + to_string(varNamer);

    string parntz = expr.substr(expr.find_last_of("(")+1);
    parntz = parntz.substr(0,parntz.find_first_of(")")); // parentezli kısmın içi 

    //parentezli kısmı map e falan koy
    //içinden parentezli kısmı çıkar, isim koy returnle

}

void expres(string expr){ 

    if(expr.find("(")){

        expr = handleParnt(expr);       //parentezli kısmı çıkardım diyelim
        
        if (expr.find("+") != string::npos) { //+ içeriyorsa
        postfix.push("+");
        string right = expr.substr(expr.find_last_of("+")+1);  // + lı kısmın sağı solu
        string left = expr.substr(0,expr.find_last_of("+"));
        expres(right); //içinde + yok - olabilir
        expres(left);

        } else if (expr.find("-") != string::npos) { //+ içeriyorsa
        postfix.push("-");
        string right = expr.substr(expr.find_last_of("-")+1);
        string left = expr.substr(0,expr.find_last_of("-")); 
        terms(right); //içinde +, - yok direkt sonrakine gönderebilirsin
        expres(left);

        } else{  //içinde + - yok
        terms(expr);
        }
    }
}

void terms(string term){

        if (term.find("*") != string::npos) { //+ içeriyorsa
        postfix.push("*");
        string right = term.substr(term.find_last_of("*")+1);
        string left = term.substr(0,term.find_last_of("*")); 
        terms(right); //içinde * yok / olabilir
        terms(left);

        } else if (term.find("-") != string::npos) { //+ içeriyorsa
        postfix.push("/");
        string right = term.substr(term.find_last_of("/")+1);
        string left = term.substr(0,term.find_last_of("/")); 
        factors(right); //içinde *, / yok direkt sonrakine gönderebilirsin
        terms(left);

        }else{  //içinde * / yok
        factors(term);
        }
}

void factors(string factor){

    if(factor.find("par_")!= string::npos){
    //string expr = map(factor);
    //expres(expr);

    }else{ //variable name ise veya number ise
    postfix.push(factor);
    }
}

int main(int argc, char const *argv[])
{
    vector<string> tokens;

    string inputFile = "input.txt";   //argv[1];
    string outputFile = "output.txt"; //argv[2];

    ifstream infile;
    infile.open(inputFile);

    ofstream outfile;
    outfile.open(outputFile);

    vector<string> lines;
    string line;

    while (getline(infile, line)){  //take lines
        lines.push_back(line);  
    }

    for (string i : lines){         //main for loop

        if(i.find("while")<i.length()) { //this may not work check RULES & QUESTIONS file line 37

            vector<string> tokens;
            char charrr[i.length()+1];  //array of chars
            strcpy(charrr, i.c_str());  //string copy
            char* token = strtok(charrr,"()");
            while (token != NULL)
            {
                tokens.push_back(token);
                token = strtok(NULL,"()");  //split strings into tokens
            }
            int truthValue = findInVariables(tokens[1]);
            if(truthValue){

            }else{
                
            }

        } else if(i.find("=")!= string::npos){  

            char charrr[i.length()+1];  //array of chars, line ın character uzunluğu kadar
            strcpy(charrr, i.c_str());  //string copy
            char* token = strtok(charrr,"=");

            vector<string> tokens;

            while (token != NULL) {
                tokens.push_back(token);
                token = strtok(NULL,"=");  //split strings into tokens
            }

            // = kısmı gitti şimdi elimizde expression var.
            // variable olan tokens[0]'ı map e gönder?

            expres(tokens[1]); //expression kısmı için postfix notation oluşturacak stack'te

         } else if(i.find("print")) {
            
         } else {

         }
        
    }

    vector<string> variables;
    // for (string s : variables)
    // {
    //     if (s == "x")
    //     {
    //         cout << "ckjldk" << endl;
    //     }
    // }
    // string a = "a=12+22";

    // cout << a.find("=");

    return 0;
}

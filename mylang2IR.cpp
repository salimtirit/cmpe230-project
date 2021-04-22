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

    return 0;
}

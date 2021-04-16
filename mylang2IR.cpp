#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

using namespace std;

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
    while (getline(infile, line))
    {
        lines.push_back(line);
    }


    for (string i : lines)
    {
        if(i.find("while")<i.length()){
            vector<string> tokens;
            char charrr[i.length()+1];
            strcpy(charrr,i.c_str());
            char* token = strtok(charrr,"()");
            while (token != NULL)
            {
                tokens.push_back(token);
                token = strtok(NULL,"()");
            }
            int truthValue = findInVariables(tokens[1]);
            if(truthValue){

            }else{
                
            }
        }/*else if(i.find("if")){

        }else if(i.find("=")){

        }else{
            cout << 
        }*/
        
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

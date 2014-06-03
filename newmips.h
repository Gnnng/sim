#ifndef __mips__newmips__
#define __mips__newmips__

#include <iostream>
#include <string>
#include <vector>
using namespace std;
struct ins{
    string ope;
    string format;
};

typedef vector<ins> INSSET;
void init(string filename,vector <ins> &insset);
int single(INSSET insset,string sins,string &reterror,string &result,int &ins);
#endif /* defined(__mips__newmips__) */

#define _USE_MATH_DEFINES
#include<bits/stdc++.h>
#include"Jtol.h"
using namespace Jtol;
using namespace std;
int main(int argc,char** argv){
    Net net=nc("google.com.tw",80,1);
    auto &str=nc(net);
    while(!nc_is_closed(net)){
        string s;
        getline(cin,s);
        s+="\n";
        NetSend(net,s.c_str());
        }
    return 0;
    }

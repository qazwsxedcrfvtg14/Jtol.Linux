#define _USE_MATH_DEFINES
#include<bits/stdc++.h>
#include"Jtol.h"
using namespace Jtol;
using namespace std;
int main(int argc,char** argv){
    mutex_set<int>st;
    //for(Net n:st){}
    //auto i=st.begin();
    st.insert(87);
    st.insert(98);
    for(int x:st){
        cout<<x<<endl;
        }
    Net net=nc("google.com.tw",80,1);
    //auto &str=nc(net);
    while(!nc_is_closed(net)){
        string s;
        getline(cin,s);
        s+="\n";
        NetSend(net,s.c_str());
        }
    mutex_set<Net>::iter it;
    //++it;
    return 0;
    }

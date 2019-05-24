#define _USE_MATH_DEFINES
#include<bits/stdc++.h>
#include"Jtol.h"
using namespace Jtol;
using namespace std;

int main(){
    json js = json::parse(FileToStr("config.json"));
    vector<string>IPs;
    for(auto j_ip:js["ip"])
        IPs.push_back(j_ip);
    string data=js["data"];
    SNetCreat(IPs,js["port"],0,[&](Net sock,string IP){
        fprintf(stderr,"connect: %s\n",IP.c_str());
        auto [is,os] = sock2stream(sock,sock);
        string s;
        while(true){
            is->clear();
            *is>>s;
            if(s.length()){
                string fil=data;
                string str;
                str+="HTTP/1.1 200 OK\r\n";
                str+="Content-Length: "+IntToStr(fil.length())+"\r\n";
                str+="Content-Type: text/html\r\n";
                str+="Server: Jtol/2.0.3 (Linux) (Windows10/WindowsNT)\r\n";
                str+="Last-Modified: "+UTCTime()+"\r\n";
                str+="Date: "+UTCTime()+"\r\n";
                str+="\r\n";
                str+=fil;
                *os<<str;
                break;
            }
        }
    });
    return 0;
}

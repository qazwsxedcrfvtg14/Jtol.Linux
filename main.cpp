#define _USE_MATH_DEFINES
#include<bits/stdc++.h>
#include"Jtol.h"
using namespace Jtol;
using namespace std;
int main(int argc,char** argv){
    HostIP.push_back("10.5.5.103");
    HostIP.push_back("192.168.38.1");
    HostIP.push_back("127.0.1.1");
    HostIP.push_back("127.0.0.1");
    auto snet=SNetCreat(8787);
    if(snet==nullptr)return 0;
    auto &snet_c=*snet;
    puts("SNet Started!");
    string fil=FileToStr("a.html");
    int len=fil.length();
    while(1){
        vector<Net> need_erase;
        for(auto sock:snet_c){
            auto &str=nc(sock,0);
            //NetClose(sock);
            //need_erase.push_back(sock);
            //continue;
            string s;//=NetGet(sock);
            str>>s;
            if(s.length()){
                //printf("%s",buf);
                string str;
                str+="HTTP/1.1 200 OK\n";
                str+="Content-Length: "+IntToStr(len)+"\n";
                str+="Content-Type: text/html\n";
                str+="Server: Jtol/1.7.3.4 (Win) (Windows10/WindowsNT)\n";
                str+="Last-Modified: "+UTCTime()+"\n";
                str+="Accept-Ranges: bytes\n";
                str+="Date: "+UTCTime()+"\n";
                str+="\n";
                str+=fil;
                //cout<<str;
                NetSend(sock,&str[0]);
                need_erase.push_back(sock);
                }
            else if(nc_is_closed(sock)){
                need_erase.push_back(sock);
                }
            }
        for(auto sock:need_erase){
            snet_c.erase(sock);
            nc_close(sock);
            }
        //if(Key('E'))break;
        Sleep(15);
        }
    //for(Net n:st){}
    //auto i=st.begin();
    mutex_set<int>st;
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

#define _USE_MATH_DEFINES
#include<bits/stdc++.h>
#include"Jtol.h"
using namespace Jtol;
using namespace std;
/*
int test(){
    string input=FileToStr("cxnvhaekljlkjxxqkq");
    unsigned int z=pack('<','h','t','m');
    unsigned int x=pack(input[0],input[1],input[2],input[3]);
    z^=x;
    //printf("%u\n",z);
    //for(int j=0;j<100-1;j++){
    for(int j=0;j<input.length()/4;j++){
        unsigned int y=pack(input[j*4],input[j*4+1],input[j*4+2],input[j*4+3]);
        unsigned int w = y^z;
        printf("%c%c%c%c",(char)((w>>0)&255),(char)((w>>8)&255),(char)((w>>16)&255),(char)((w>>24)&255));
        z=(z<<8)|(z>>24);
    }
    return 0;
}
*/
/*
unsigned int pack(unsigned char a,unsigned char b,unsigned char c,unsigned char d){
    return ((unsigned int)(d)<<24)|((unsigned int)(c)<<16)|((unsigned int)(b)<<8)|((unsigned int)(a));
}
void print(unsigned w){
    printf("%c%c%c%c",(char)((w>>0)&255),(char)((w>>8)&255),(char)((w>>16)&255),(char)((w>>24)&255));
}
int main(){
    unsigned int flags[10];
    string flag;
    flag.resize(40);
    string half=" => pHd_1w_e4rL13r;)";
    for(int i=0;i<5;i++)
        flags[i+5]=pack(half[i*4+0],half[i*4+1],half[i*4+2],half[i*4+3]);
    flags[4]=flags[9]^1565666646;
    flags[3]=flags[7]^322115650;
    flags[2]=flags[5]^flags[8]^1311204206;
    flags[1]=flags[3]^flags[5]^flags[7]^flags[9]^1615013692;
    flags[0]=flags[1]^flags[2]^flags[3]^flags[4]^flags[5]^flags[6]^flags[7]^flags[8]^flags[9]^53412119;
    printf("OOO{");
    for(int i=0;i<10;i++)
        print(flags[i]);
    printf("}\n");
}
*/

int main(){
    SNetCreat({},8787,0,[&](Net sock){
        auto &str=nc(sock,0);
        //NetClose(sock);
        //need_erase.push_back(sock);
        //continue;
        string s;
        while(!nc_is_closed(sock)){
            str>>s;
            if(s.length()){
                string fil=FileToStr("a.html");
                string str;
                str+="HTTP/1.1 200 OK\r\n";
                str+="Content-Length: "+IntToStr(fil.length())+"\r\n";
                str+="Content-Type: text/html\r\n";
                str+="Server: Jtol/2.0.2 (Linux) (Windows10/WindowsNT)\r\n";
                str+="Last-Modified: "+UTCTime()+"\r\n";
                str+="Date: "+UTCTime()+"\r\n";
                str+="\r\n";
                str+=fil;
                NetSend(sock,&str[0]);
                break;
            }
        }
        nc_close(sock);
    });
    return 0;
}

/*
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
*/

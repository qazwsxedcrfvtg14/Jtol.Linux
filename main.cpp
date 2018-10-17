#define _USE_MATH_DEFINES
#include<bits/stdc++.h>
#include"Jtol.h"
using namespace Jtol;
using namespace std;
int mp[105][105];
int cnt[105][105];
int bom[105][105];
char s[1000];
string ss;
int n;
void add9(int x,int y){
    bom[x][y]=1;
    for(int i=max(x-1,0);i<=min(x+1,n-1);i++)
        for(int j=max(y-1,0);j<=min(y+1,n-1);j++)
            if(i!=x||j!=y)
                cnt[i][j]++;
    }
void sub9(int x,int y){
    bom[x][y]=0;
    for(int i=max(x-1,0);i<=min(x+1,n-1);i++)
        for(int j=max(y-1,0);j<=min(y+1,n-1);j++)
            if(i!=x||j!=y)
                cnt[i][j]--;
    }
inline bool chk(int x,int y){
    if(!~mp[x][y]){
        return bom[x][y]||cnt[x][y]==0;
        }
    return mp[x][y]==cnt[x][y];
    }
inline bool chk_low(int x,int y){
    if(!~mp[x][y]){
        return bom[x][y]||cnt[x][y]==0;
        }
    return mp[x][y]>=cnt[x][y];
    }
bool F(int x,int y){
    if(y==n+1)return F(x+1,0);
    if(x==n+1)return true;
    if(x==n||y==n){
        if(x&&y&&!chk(x-1,y-1))return false;
        return F(x,y+1);
        }
    if(!(x&&y&&!chk(x-1,y-1)))
        if(!(x&&!chk_low(x-1,y)))
            if(!(y&&!chk_low(x,y-1)))
                if(F(x,y+1))
                    return true;
    if(~mp[x][y])
        return false;
    //printf("%d %d\n",x,y);
    add9(x,y);
    if(!(x&&y&&!chk(x-1,y-1)))
        if(!(x&&!chk_low(x-1,y)))
            if(!(y&&!chk_low(x,y-1)))
                if(F(x,y+1))return true;
    sub9(x,y);
    return false;
    }
int main(){
    auto &input = nc("140.113.209.24",10003,1);
    for(int k=0;k<9;k++){
        Sleep(2000*(k+1));
        fprintf(stderr,"~~%d~~\n",k);
        input.mut.lock();
        getline(input.stri,ss);
        input.mut.unlock();
        //gets(s);
        stringstream str(ss);
        n=0;
        while(str>>s)n++;
        for(int i=0;i<n;i++)
            for(int j=-1;j<n;j++){
                input>>s;
                //scanf("%s",s);
                if(j==-1)continue;
                if(s[0]=='-')
                    mp[i][j]=-1;
                else
                    mp[i][j]=s[0]-'0';
                }
        input.mut.lock();
        getline(input.stri,ss);
        input.mut.unlock();
        memset(cnt,0,sizeof(cnt));
        memset(bom,0,sizeof(bom));
        if(!F(0,0))fprintf(stderr,"~~GG~~\n");
        //add9(1,1);
        //add9(2,0);
        /*
        for(int i=0;i<n;i++,puts(""))
            for(int j=0;j<n;j++)
                printf("%d ",cnt[i][j]);
        for(int i=0;i<n;i++,puts(""))
            for(int j=0;j<n;j++)
                printf("%d ",chk(i,j));
        */
        for(int i=0;i<n;i++,puts(""),NetSend(net,"\n"))
            for(int j=0;j<n;j++)
                if(bom[i][j])
                    printf("-1 "),NetSend(net,"-1 ");
                else if(~mp[i][j])
                    printf("%2d ",mp[i][j]),NetSend(net," "+ToStr(mp[i][j])+" ");
                else
                    printf(" 0 "),NetSend(net," 0 ");
        }
    while(true)Sleep(100);
    //while(gets(s))fprintf(stderr,"%s\n",s);
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

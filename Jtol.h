//Jtol.Linux.h v1.7.3.3
#ifndef JTOL_H_
#define JTOL_H_
#include<sys/types.h>
#include <sys/socket.h>
#include<netinet/in.h>
#include<bits/stdc++.h>
#include<ext/rope>
#include"md5.h"
#include"lodepng.h"
#undef UNICODE
#define UNICODE
#define f first
#define s second
//#pragma comment(linker, "/subsystem:console /entry:WinMainCRTStartup")
namespace Jtol{
    using namespace __gnu_cxx;
    //using namespace Gdiplus;
    using namespace std;
    typedef pair<int,int> par;
    typedef unsigned long long int Time;
    typedef par Pos;
    typedef int Net;
    typedef thread* Thread;
    typedef basic_string<unsigned char> ustring;
    void Sleep(int t);
    struct Color{
        unsigned char R,G,B,A;
        Color(){}
        Color(unsigned char r,unsigned char g,unsigned char b){R=r,G=g,B=b,A=255;}
        Color(unsigned char r,unsigned char g,unsigned char b,unsigned char a){R=r,G=g,B=b,A=a;}
        unsigned char L(){return max(R,max(G,B));}
        };
    class rwlock {
        private:
            mutex _lock;
            condition_variable _wcon, _rcon;
            unsigned _writer, _reader;
            int _active;
        public:
            void read_lock() {
                unique_lock<mutex> lock(_lock);
                ++_reader;
                while(_active < 0 || _writer > 0)
                    _rcon.wait(lock);
                --_reader;
                ++_active;
                }
            void write_lock() {
                unique_lock<mutex> lock(_lock);
                ++_writer;
                while(_active != 0)
                    _wcon.wait(lock);
                --_writer;
                _active = -1;
                }
            void unlock() {
                unique_lock<mutex> lock(_lock);
                if(_active > 0) {
                    --_active;
                    if(_active == 0) _wcon.notify_one();
                    }
                else{
                    _active = 0;
                    if(_writer > 0) _wcon.notify_one();
                    else if(_reader > 0) _rcon.notify_all();
                    }
                }
            rwlock():_writer(0),_reader(0),_active(0){}
        };
    unsigned long long int GetTime();
    void Delay(Time x,Time feq=1);
    Pos GetPos();
    Color GetCol(int x,int y);
    Color GetCol(Pos p);
    void Click(int t=50);
    void Press(int r=0);
    void Release(int r=0);
    void Mov(int x,int y);
    void Move(int x,int y,int t=0);
    void MoveEX(int x,int y,int t=0);
    void Clear();
    bool ChkKey(int key);
    bool Key(int key);
    void WaitKey(int key);
    void PressKey(int key);
    void ReleaseKey(int key);
    void ClickKey(int key,int t=50);
    Net NetCreat(const char ip[],int port=23,int mode=1);//mode: 1->NoWait 0->Wait
    extern map<Net,string > NetBuf;
    extern rwlock NetBuf_lock;
    void NetClose(Net sock);
    string NetGet(Net sock);
    string NetGet(Net sock,int &result);
    void NetSend(Net sock,string s);
    extern vector<string> HostIP;
    void SetHostIP();
    string FileToStr(const char *fil);
    void StrToFile(string s,const char fil[]);
    string UTCTime();
    string IntToStr(int x);
    int StrToInt(string x);
    template<typename T>
    string ToStr(T x){
        stringstream str;
        string s;
        str<<x;
        str>>s;
        return s;
        }
    struct Node{
        string name;
        list<Node*> Klis;
        Node* Fath;
        multimap<string,list<Node*>::iterator> Kmap;
        map<string,string>Type;
        Node();
        Node(string s,Node* f=NULL);
        ~Node();
        void AddType(string s,string ss);
        void AddNode(string s);
        void Print(int l=0);
        };
    Node* HtmlToNode(string s);
    Node HttpDecode(string s);
    template<typename T,typename... Args>
    Thread ThreadCreate(T will_run, Args... args){
        Thread td=new thread(will_run,args...);
        return td;
        }
    void Wait(Thread thr);
    void HideConsole();
    void Setup();
    void Alert(string content="",string titile="");
    typedef vector<vector<Color>> Pic;
    Pic ReadBMP(string in);
    int WriteBMP(string out,Pic pic);
    Pic ReadPNG(string in);
    void WritePNG(string out,Pic pic);
    vector<string> Dir(string s);
    int FileExists(const char * file);
    string FileFullName(string s);
    std::string EncodeBase64(std::string const& decoded_string);
    std::string DecodeBase64(std::string const& encoded_string);
    std::string EncodeUrl(const std::string& src);
    std::string DecodeUrl(const std::string& src);
    std::string EncodeUtf8(const std::wstring &wstr);
    std::wstring DecodeUtf8(const std::string &str);
    struct Json_Node {
        int type;
        wstring data;
        map<wstring, Json_Node>child;
        vector<Json_Node>ary;
        };
    std::string trim(std::string s);
    std::wstring trim(std::wstring s);
    wstring erase_quote(wstring s);
    void ReadJson(wstring s, Json_Node& now);
    Json_Node ReadJson(wstring s);
    void PraseJson(const Json_Node& now, int l,wstring &out);
    wstring ReadableJson(const Json_Node&now);
    void TelnetPrint(string s);
    vector<string>split(string s,string cut);
    string exec(string cmd);
    struct stream{
        private:
            mutex mut;
            stringstream stri;
        public:
        explicit operator bool(){
            mut.lock();
            bool ret=(bool)stri;
            mut.unlock();
            return ret;
            }
        template <typename T>
        stream & operator<<(const T & data){
            mut.lock();
            stri<<data;
            mut.unlock();
            return *this;
            }
        template <typename T>
        stream & operator>>(T & data){
            if(!*this)Sleep(1);
            mut.lock();
            stri>>data;
            mut.unlock();
            return *this;
            }
        void clear(){
            mut.lock();
            stri.clear();
            mut.unlock();
            }
        string str(){
            mut.lock();
            string s=stri.str();
            mut.unlock();
            return s;
            }
        };
    Net nc(const string& ip,int port=23,int output=1);
    void nc_close(Net net);
    bool nc_is_closed(Net net);
    extern map<Net,mutex>nc_mutex;
    stream &nc(Net net);
    template <typename T, int N>
    string chars(T (&ca)[N]){
        return string(ca,N-1);
        }
    bool is_hex(char c);
    int hex(char c);
    string phrase_string(string s);
    }
#endif

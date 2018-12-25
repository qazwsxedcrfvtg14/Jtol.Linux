//Jtol.Linux.h v1.9.3
#ifndef JTOL_H_
#define JTOL_H_
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<bits/stdc++.h>
#include<ext/rope>
#include"md5.h"
#include"lodepng.h"
#include<curl/curl.h>
#include<dlfcn.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<unistd.h>
#include <ext/stdio_filebuf.h>

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
    typedef void* SO;
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
    Net NetCreat(const string& ip,int port=23,int udp=0,int udp_bind_port=0);//mode: 1->NoWait 0->Wait
    extern map<Net,string > NetBuf;
    extern rwlock NetBuf_lock;
    void NetClose(Net sock);
    string NetGet(Net sock);
    string NetGet(Net sock,int buffer_size,int &result);
    void NetSend(Net sock,const string &s);
    template<typename T>
    class mutex_set{
        private:
            mutex mut;
            set<T> st;
        public:
            friend class iter;
            class iter{
                private:
                    typename set<T>::iterator it;
                    mutex *mu;
                public:
                    iter(){}
                    iter(const typename set<T>::iterator &_it,mutex * _mu){it=_it;mu=_mu;}
                    const T& operator*(){
                        mu->lock();
                        const T &ret=*it;
                        mu->unlock();
                        return ret;
                    }
                    const T* operator->(){
                        mu->lock();
                        const T &ret=*it;
                        mu->unlock();
                        return &ret;
                    }
                    iter& operator++(){
                        mu->lock();
                        ++it;
                        mu->unlock();
                        return *this;
                    }
                    bool operator==(iter &b){
                        mu->lock();
                        bool ret=(it==b.it);
                        mu->unlock();
                        return ret;
                    }
                    bool operator!=(iter &b){
                        mu->lock();
                        bool ret=(it!=b.it);
                        mu->unlock();
                        return ret;
                    }
            };
            iter begin(){
                mut.lock();
                iter it(st.begin(),&mut);
                mut.unlock();
                return it;
            }
            iter end(){
                mut.lock();
                iter it(st.end(),&mut);
                mut.unlock();
                return it;
            }
            auto size(){
                mut.lock();
                auto ret=st.size();
                mut.unlock();
                return ret;
            }
            auto empty(){
                mut.lock();
                auto ret=st.empty();
                mut.unlock();
                return ret;
            }
            auto insert(const T &val){
                mut.lock();
                auto res=st.insert(val);
                pair<iter,bool> ret(iter(res.f,&mut),res.s);
                mut.unlock();
                return ret;
            }
            auto erase(const T &val){
                mut.lock();
                auto ret=st.erase(val);
                mut.unlock();
                return ret;
            }
            void clear(){
                mut.lock();
                st.clear();
                mut.unlock();
            }
    };
    extern vector<string> HostIP;
    void SetHostIP();
    shared_ptr<mutex_set<Net>> SNetCreat(int port=23,int non_block_mode=1);
    string FileToStr(const string &fil);
    void StrToFile(string s,const string& fil);
    string UTCTime();
    string IntToStr(int x);
    int StrToInt(string x);
    template<typename T>
    string ToStr(T x){
        stringstream str;
        string s;
        str<<x;
        return str.str();
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
    Thread ThreadCreate(T will_run, Args&&... args){
        Thread td=new thread(will_run,forward<Args>(args)...);
        return td;
    }
    extern unordered_map<int,__gnu_cxx::stdio_filebuf<char>>filebuf;
    template<typename... Args>
    auto exec_pipe(string cmd,Args... args){
        int fd_1[2],fd_2[2];
        pipe(fd_1);
        pipe(fd_2);
        int in=fd_1[0];
        int out=fd_2[1];
        if(fork()){
            close(fd_1[1]);
            close(fd_2[0]);
        }
        else{
            close(fd_1[0]);
            close(fd_2[1]);
            dup2(fd_1[1],STDOUT_FILENO);
            dup2(fd_2[0],STDIN_FILENO);
            close(fd_1[1]);
            close(fd_2[0]);
            execl(cmd.c_str(),cmd.c_str(),args...,NULL);
            fprintf(stderr,"exec filed!\n");
        }
        filebuf[in]=__gnu_cxx::stdio_filebuf<char>(in, std::ios::in);
        filebuf[out]=__gnu_cxx::stdio_filebuf<char>(out, std::ios::out);
        shared_ptr<istream> is(new istream(&filebuf[in]),[in](istream *p){
            delete p;
            filebuf.erase(in);
            close(in);
    });
        shared_ptr<ostream> os(new ostream(&filebuf[out]),[out](ostream *p){
            delete p;
            filebuf.erase(out);
            close(out);
    });
        return tuple(is,os);
    }
    void Wait(Thread thr);
    void HideConsole();
    void Setup();
    void Alert(string content="",string titile="");
    SO LoadSO(const string &path);
    void FreeSO(SO handle);
    template <typename T>
    struct TypeParser{};
    template <typename Ret, typename... Args>
    struct TypeParser<Ret(Args...)> {
        static std::function<Ret(Args...)> createFunction(void* lpfnGetProcessID) {
            return std::function<Ret(Args...)>(reinterpret_cast<Ret(*)(Args...)>(lpfnGetProcessID));
        }
    };
    template<typename Signature>
    function<Signature>GetSOFunc(SO handle,string s){
        auto func=dlsym(handle,s.c_str());
        if(!func){
            fprintf (stderr, "ERROR: unable to find SO function");
            FreeSO(handle);
            return 0;
        }
        return TypeParser<Signature>::createFunction(func);
    }
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
    vector<string>split(string s,string cut,int num=0);
    string join(vector<string>ve,string s);
    string exec(string cmd);
    struct stream{
        private:
        public:
        mutex mut;
        stringstream stri;
        Net net;
        explicit operator bool(){
            mut.lock();
            bool ret=(bool)stri;
            mut.unlock();
            return ret;
        }
        template <typename T>
        void append(const T & data){
            mut.lock();
            stri<<data;
            mut.unlock();
        }
        template <typename T>
        stream & operator<<(const T & data){
            mut.lock();
            NetSend(net,ToStr(data));
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
        string getline(){
            mut.lock();
            string s;
            std::getline(stri,s);
            mut.unlock();
            return s;
        }
        string until(string s){
            
            return s;
        }
    };
    stream &nc(const string& ip,int port=23,int output=1,int udp=0,int udp_bind_port=0);
    void nc_close(Net net);
    bool nc_is_closed(Net net);
    extern map<Net,mutex>nc_mutex;
    stream &nc(Net net,int output=1);
    template <typename T, int N>
    string chars(T (&ca)[N]){
        return string(ca,N-1);
    }
    bool is_hex(char c);
    int hex(char c);
    string phrase_string(string s);
    string request(string url);
}
#endif

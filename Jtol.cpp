//Jtol.Linux.cpp v1.9.0
#include<bits/stdc++.h>
#include<ext/rope>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include"md5.h"
#include"lodepng.h"
#include"Jtol.h"
#undef UNICODE
#define UNICODE
#define f first
#define s second
namespace Jtol{
    using namespace __gnu_cxx;
    using namespace std;
    void Sleep(int t){
        usleep((useconds_t)t*100);
        }
    Time GetTime() {
        struct timeb t;
        ftime(&t);
        return 1000 * t.time + t.millitm;
        }
    void Delay(Time x,Time feq){
        Time start=GetTime();
        while(1){
            if(GetTime()-start>=x)return;
            //Sleep(1);
            std::this_thread::sleep_for(std::chrono::milliseconds(feq));
            }
        }
    Net NetCreat(const char ip[],int port,int mode){
        int sockfd;
        struct sockaddr_in servaddr;
        if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
            return -1;
            }
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);

        servaddr.sin_addr.s_addr = inet_addr(ip);
        if( servaddr.sin_addr.s_addr == INADDR_NONE ){
            hostent *host = gethostbyname(ip);
            servaddr.sin_addr.s_addr = *reinterpret_cast<unsigned long*>(host->h_addr_list[0]);
            if( servaddr.sin_addr.s_addr == INADDR_NONE ){
                puts("DNS error");
                return -1;
                }
            }
        //ioctlsocket(sockfd,FIONBIO, (u_long FAR*) &mode);//NoWait
        int flags = fcntl(sockfd, F_GETFL, 0);
        if(mode)
            fcntl(sockfd, F_SETFL, flags|O_NONBLOCK);
        else
            fcntl(sockfd, F_SETFL, flags&(~O_NONBLOCK));

        if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
            printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
            return -1;
            }
        return sockfd;
        }
    rwlock NetBuf_lock;
    void NetClose(Net sock){
        close(sock);
        }
    string NetGet(Net sock,int &result){
        string s(1000,0);
        char *buf=&(s[0]);
        result=recv(sock,buf,s.size(),0);
        if(result>0){
            s.resize(result);
            return s;
            }
        else if(result==0){
            result=-1;
            return "";
            }
        else{
            //int error=WSAGetLastError();
            if(errno == EINTR)
                result=0;
            else if(errno == 11)
                result=0;
            else
                printf("recv failed: %s(errno: %d) %d",strerror(errno),errno,result);
            //printf("recv failed: %d %d\n", error,result),result=0;
            return "";
            }
        return s;
        }
    string NetGet(Net sock){
        int result;
        return NetGet(sock,result);
        }
    void NetSend(Net sock,string s){
        send(sock,s.c_str(),s.length(),0);
        }
    vector<string> HostIP;
    void SetHostIP(){
        HostIP.clear();
        char host_name[256];
        if (gethostname(host_name, sizeof(host_name)) == -1) {
            printf("gethostname failed: %s(errno: %d)",strerror(errno),errno);
            exit(-1);
            }
        struct hostent *phe = gethostbyname(host_name);
        if(phe==0){
            printf("Bad host lookup.");
            }
        for(int i=0;phe->h_addr_list[i]!=0;i++){
            struct in_addr addr;
            memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
            HostIP.push_back(inet_ntoa(addr));
            }
        HostIP.push_back("127.0.0.1");
        }
    void SNetCreatFnc(vector<Net> server_sockfd,shared_ptr<mutex_set<Net>> client_sockfd_list,int mode){
        vector<sockaddr_in> client_address;
        client_address.push_back(sockaddr_in());
        Net client_tmp;
        unsigned int client_len=sizeof(client_address.back());
        int sz=server_sockfd.size();
        while(1){
            for(int i=0;i<sz;i++){
                client_tmp=accept(server_sockfd.at(i),(struct sockaddr *)&client_address.back(), &client_len);
                if((int)client_tmp != -1){
                    printf("[%s] Connect!\n",inet_ntoa(client_address.back().sin_addr));
                    //CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)Fnc,(void*)(SNCF->client_sockfd_list)->back(),0,NULL);
                    client_address.push_back(sockaddr_in());
                    if(mode){
                        int flags = fcntl(client_tmp, F_GETFL, 0);
                        fcntl(client_tmp, F_SETFL, flags|O_NONBLOCK);
                        }
                    client_sockfd_list->insert(client_tmp);
                    }
                Sleep(15);
                }
			}
        }
    shared_ptr<mutex_set<Net>> SNetCreat(int port,int mode){
        vector<Net> server_sockfd;
        sockaddr_in server_address[100];
        int server_len[100];
        if(HostIP.size()==0)SetHostIP();
        for(auto x:HostIP)
            puts(x.c_str());
        int Err=0,sz=HostIP.size();
        for(int i=0;i<sz;i++){
            int err=0;
            server_sockfd.push_back(socket(AF_INET, SOCK_STREAM, 0));
            if((int)server_sockfd.at(i) == -1) {
                printf("Socket %d Error\n",i);
                err=1;
                }
            server_address[i].sin_family = AF_INET;
            server_address[i].sin_addr.s_addr = inet_addr(HostIP[i].c_str());
            server_address[i].sin_port = htons(port);
            server_len[i] = sizeof(server_address);
            if(bind(server_sockfd.at(i), (struct sockaddr *)&server_address[i], server_len[i]) < 0) {
                printf("Bind %d Error\n",i);
                err=1;
                }
            if(listen(server_sockfd.at(i), 5) < 0) {
                printf("Listen %d Error\n",i);
                err=1;
                }
            if(err){
                Err++;
                }
            else if(mode){
                int flags = fcntl(server_sockfd.at(i), F_GETFL, 0);
                fcntl(server_sockfd.at(i), F_SETFL, flags|O_NONBLOCK);
                //ioctlsocket(server_sockfd->at(i),FIONBIO, (u_long FAR*) &mode);
                }
            }
        shared_ptr<mutex_set<Net>>client_sockfd_list(new mutex_set<Net>);
        if(Err==sz){
            printf("Error %d\n",sz);
            client_sockfd_list->insert(-1);
            return nullptr;
            }
        ThreadCreate(SNetCreatFnc,server_sockfd,client_sockfd_list,mode);
        return client_sockfd_list;
        }

    string FileToStr(const char *fil){
        fstream fin;
        fin.open(fil,ios::in|ios::binary);
        char c;
        string ss;
        while(fin.get(c))
            ss+=c;
        fin.close();
        return ss;
        }
    void StrToFile(string s,const char fil[]){
        fstream fout;
        fout.open(fil,ios::out|ios::binary);
        fout<<s;
        fout.close();
        }
    string UTCTime(){
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];
        time (&rawtime);
        timeinfo = localtime (&rawtime);
        strftime (buffer,80,"%a, %d %b %y %H:%M:%S GMT",timeinfo);
        return buffer;
        }
    string IntToStr(int x){
        stringstream str;
        string s;
        str<<x;
        str>>s;
        return s;
        }
    int StrToInt(string x){
        stringstream str;
        int s;
        str<<x;
        str>>s;
        return s;
        }
    Node::Node(){Fath=NULL;}
    Node::Node(string s,Node* f){name=s;Fath=f;}
    Node::~Node(){
        for(list<Node*>::iterator it=Klis.begin(),now,ed=Klis.end();it!=ed;){
            now=it++;
            delete *now;
            }
        }
    void Node::AddType(string s,string ss){
        if(Type[s]!="")Type[s]+="::";
        Type[s]+=ss;
        }
    void Node::AddNode(string s){
        Klis.push_back(new Node(s,this));
        Kmap.insert(make_pair(s,--Klis.end()));
        }
    void Node::Print(int l){
        for(int i=0;i<l*2;i++)
            cout<<" ";
        cout<<"("<<name<<"):";
        for(auto it:Type){
            if(name!="style"||it.f!="innerHTML")
                if(name!="script"||it.f!="innerHTML")
                    cout<<it.f<<"->\""<<it.s<<"\" ";
            }
        cout<<endl;
        for(auto it:Klis)
            it->Print(l+1);
        for(int i=0;i<l*2;i++)
            cout<<" ";
        cout<<"(/"<<name<<")"<<endl;
        }
    Node* HtmlToNode(string s){
        string sigtag[]={"br","meta","link","img","input","!DOCTYPE","hr"};
        int sigtagtop=8;
        Node *now=new Node(),*root=now;
        int sz=s.length();
        bool intag=0,ontag=0,instr1=0,instr2=0,detag=0,typove=0,sbreak=0,onword=0,skip=0;
        string tnam,snam,bnam,onam,str,emp,scc;
        for(int i=0;i<sz;i++){
            if(i+3<sz&&s[i]=='<'&&s[i+1]=='!'&&s[i+2]=='-'&&s[i+3]=='-'){
                skip=1;
                }
            else if(skip){
                if(s[i-2]=='-'&&s[i-1]=='-'&&s[i]=='>')
                    skip=0;
                }
            else if(!intag){
                if(now->name=="script"){
                    if(s[i]=='<'&&s[i+1]=='/'&&s[i+2]=='s'&&s[i+3]=='c'&&s[i+4]=='r'&&s[i+5]=='i'&&s[i+6]=='p'&&s[i+7]=='t'){
                        now->AddType("innerHTML",scc);
                        now=now->Fath;
                        for(;i<sz;i++)
                            if(s[i]=='>'){i++;break;}
                        scc="";
                        }
                    else{
                        scc+=s[i];
                        }
                    }
                else if(s[i]=='<'){
                    if(onam!="")
                        now->AddType("innerHTML",onam);
                    //cout<<now->name<<"<"<<onam<<endl;
                    //puts("~~~");
                    onam="";
                    intag=1;
                    ontag=1;
                    onword=0;
                    emp="";
                    if(s[i+1]=='/')detag=1,i++;
                    }
                else if(s[i]==' '||s[i]=='\n'||s[i]=='\r'||s[i]==' '||(s[i]<32&&s[i]>=0))
                    emp+=s[i];
                else{
                    //printf("%d %c %d\n",onword,s[i],s[i]);
                    if(onword)
                        onam+=emp;
                    emp="";
                    onam+=s[i];
                    onword=1;
                    }
                }
            else if(ontag){
                //if(s[i]=='/')detag=1;
                if(s[i]==' '){
                    ontag=0;
                    if(!detag){
                        now->AddNode(tnam);
                        now=now->Klis.back();
                        }
                    else{
                        if(now->Fath!=NULL){
                            if(now->name==tnam){
                                now=now->Fath;
                                }
                            }
                        }
                    tnam="";
                    }
                else if(s[i]=='>'){
                    ontag=0;
                    intag=0;
                    if(!detag){
                        now->AddNode(tnam);
                        now=now->Klis.back();
                        }
                    else{
                        if(now->Fath!=NULL){
                            if(now->name==tnam){
                                now=now->Fath;
                                //cout<<now->name<<endl;
                                }
                            }
                        }
                    bool cksig=0;
                    for(int i=0;i<sigtagtop;i++)
                        if(now->name==sigtag[i])
                            cksig=1;
                    if(!detag)
                    if(s[i-1]=='/'||cksig)
                        if(now->Fath!=NULL)
                                now=now->Fath;
                    detag=0;
                    tnam="";
                    //puts("OVE");
                    }
                else
                    tnam+=s[i];
                }
            else if(instr1){
                if(s[i]=='"'){
                    instr1=0;
                    }
                else if(s[i]=='\\')
                    i++;
                else
                    bnam+=s[i];
                }
            else if(instr2){
                if(s[i]=='\'')
                    instr2=0;
                else if(s[i]=='\\')
                    i++;
                else
                    bnam+=s[i];
                }
            else if(typove){
                if(s[i]==' '||s[i]=='\n'||s[i]=='\r'||s[i]==' '||(s[i]<32&&s[i]>=0)){
                    if(snam!=""&&bnam!=""){
                        now->AddType(snam,bnam);
                        typove=0;snam="";bnam="";
                        }
                    }
                else if(s[i]=='/'){}
                else if(s[i]=='>'){
                    if(snam!=""&&bnam!=""){
                        now->AddType(snam,bnam);
                        typove=0;
                        snam="";bnam="";
                        }
                    ontag=0;
                    intag=0;
                    bool cksig=0;
                    for(int i=0;i<sigtagtop;i++)
                        if(now->name==sigtag[i])
                            cksig=1;
                    if(detag){
                        if(now->Fath!=NULL){
                            if(now->name==tnam){
                                now=now->Fath;
                                }
                            }
                        }
                    else if(s[i-1]=='/'||cksig)
                        if(now->Fath!=NULL)
                                now=now->Fath;
                    detag=0;
                    tnam="";
                    }
                else if(s[i]=='"'){
                    instr1=1;
                    }
                }
            else if(intag){
                if(s[i]==' '||s[i]=='\n'||s[i]=='\r'||s[i]==' '||(s[i]<32&&s[i]>=0)){
                    if(snam!="")
                        sbreak=1;
                    }
                else if(s[i]=='='){
                    sbreak=0;
                    typove=1;
                    }
                else if(s[i]=='/'){}
                else if(s[i]=='>'){
                    if(snam!=""){
                        now->AddType(snam,bnam);
                        typove=0;snam="";bnam="";
                        }
                    ontag=0;
                    intag=0;
                    bool cksig=0;
                    for(int i=0;i<sigtagtop;i++)
                        if(now->name==sigtag[i])
                            cksig=1;
                    if(detag){
                        if(now->Fath!=NULL){
                            if(now->name==tnam){
                                now=now->Fath;
                                }
                            }
                        }
                    else if(s[i-1]=='/'||cksig)
                        if(now->Fath!=NULL)
                                now=now->Fath;
                    detag=0;
                    tnam="";
                    }
                else{
                    if(sbreak)now->AddType(snam,""),snam="",sbreak=0;
                    snam+=s[i];
                    }
                }
            }
        return root;
        }
    Node HttpDecode(string s){
        Node nod;
        int sz=s.length();
        bool mo=0,html=0;
        string a,b;
        int i;
        for(i=1;i<sz;i++)
            if(s[i-1]=='\r'&&s[i]=='\n')
                break;
        for(;i<sz;i++){
            if(html){
                a+=s[i];
                }
            else if(i+3<sz&&s[i]=='\r'&&s[i+1]=='\n'&&s[i+2]=='\r'&&s[i+3]=='\n'){
                html=1;
                nod.AddType(a,b);
                a="";b="";
                i+=3;
                }
            else if(s[i]==':'&&!mo){
                mo=1;
                if(i+1<sz&&s[i+1]==' ')i++;
                }
            else if(i&&s[i-1]=='\r'&&s[i]=='\n'){
                nod.AddType(a,b);
                a="";b="";
                mo=0;
                }
            else if(s[i]!='\r'){
                if(mo)b+=s[i];
                else a+=s[i];
                }
            }
        nod.AddType("HTML",a);
        return nod;
        }
    void Wait(Thread thr){
        thr->join();
        delete thr;
        }
    void Setup(){
        srand(time(0));
        //memset(NetBuf,0,sizeof(NetBuf));
        }
    unordered_map<int,__gnu_cxx::stdio_filebuf<char>>filebuf;
    SO LoadSO(const string &path){
        SO handle;
        handle = dlopen (path.c_str(), RTLD_LAZY);
        if (!handle) {
            fprintf (stderr, "%s\n", dlerror());
            return 0;
        }
        dlerror();
        return handle;
    }
    void FreeSO(SO handle){
        dlclose(handle);
        }
    int bmp_read(const char *filein_name,int *xsize,int *ysize,int *bsize,unsigned char **rarray,unsigned char **garray,unsigned char **barray);
    int bmp_read_data(FILE *filein,int xsize,int ysize,int bsize,unsigned char *rarray,unsigned char *garray,unsigned char *barray);
    int bmp_read_header(FILE *filein,int *xsize,int *ysize,int *bsize,int *psize);
    int bmp_read_palette(FILE *filein,int psize);
    int bmp_read_test(char *filein_name);
    int bmp_write(const char *fileout_name,int xsize,int ysize,int bsize,unsigned char *rarray,unsigned char *garray,unsigned char *barray);
    int bmp_write_data(FILE *fileout,int xsize,int ysize,int bsize,unsigned char *rarray,unsigned char *garray,unsigned char *barray);
    int bmp_write_header(FILE *fileout,int xsize,int ysize,int bsize);
    int bmp_write_palette(FILE *fileout);
    int bmp_write_test(char *fileout_name);
    int read_u_long_int(unsigned long int*u_long_int_val,FILE *filein);
    int read_u_short_int(unsigned short int *u_short_int_val,FILE *filein);
    int write_u_long_int(unsigned long int u_long_int_val,FILE *fileout);
    int write_u_short_int(unsigned short int u_short_int_val,FILE *fileout);
    int byte_swap=1;
    int bmp_read(const char *filein_name,int *xsize,int *ysize,int* bsize,unsigned char **rarray,unsigned char **garray,unsigned char **barray){
        FILE *filein;
        int numbytes;
        int psize;
        int result;
        filein=fopen(filein_name,"rb");
        if(!filein){
            printf("\n");
            printf("BMP_READ - Fatal error!\n");
            printf("  Could not open the input file.\n");
            return 1;
            }
        result=bmp_read_header(filein,xsize,ysize,bsize,&psize);
        if(result==1){
            printf("\n");
            printf("BMP_READ: Fatal error!\n");
            printf("  BMP_READ_HEADER failed.\n");
            return 1;
            }
        if((*bsize)!=8&&(*bsize)!=24){
            printf("\n");
            printf("BMP_READ: bit size=%d is not supported\n",*bsize);
            printf("  BMP_READ failed.\n");
            return 1;
            }
        if(*bsize==8)
            psize=256;
        result=bmp_read_palette(filein,psize);
        if(result==1){
            printf("\n");
            printf("BMP_READ: Fatal error!\n");
            printf("  BMP_READ_PALETTE failed.\n");
            return 1;
            }
        numbytes=(*xsize)*(*ysize)* sizeof(unsigned char);
        *rarray=(unsigned char *)malloc(numbytes);
        if(rarray==NULL){
            printf("\n");
            printf("BMP_READ: Fatal error!\n");
            printf("  Could not allocate data storage.\n");
            return 1;
            }
        if(*bsize==24){
            *garray=(unsigned char *)malloc(numbytes);
            if(garray==NULL){
                printf("\n");
                printf("BMP_READ: Fatal error!\n");
                printf("  Could not allocate data storage.\n");
                return 1;
                }
            *barray=(unsigned char *)malloc(numbytes);
            if(barray==NULL){
                printf("\n");
                printf("BMP_READ: Fatal error!\n");
                printf("  Could not allocate data storage.\n");
                return 1;
                }
            }
        result=bmp_read_data(filein,*xsize,*ysize,*bsize,*rarray,*garray,*barray);
        if(result==1){
            printf("\n");
            printf("BMP_READ: Fatal error!\n");
            printf("  BMP_READ_DATA failed.\n");
            return 1;
            }
        fclose(filein);
        return 0;
        }
    int bmp_read_data(FILE *filein,int xsize,int ysize,int bsize,unsigned char *rarray,unsigned char *garray,unsigned char *barray){
        int i,j;
        unsigned char *indexb;
        unsigned char *indexg;
        unsigned char *indexr;
        int temp;
        int numbyte;
        numbyte=0;
        for(j=ysize-1;j>0;j--){
            indexr=rarray+xsize*j*sizeof(unsigned char);
            indexg=garray+xsize*j*sizeof(unsigned char);
            indexb=barray+xsize*j*sizeof(unsigned char);
            for(i=0;i<xsize;i++){
                if(bsize==24){
                    temp=fgetc(filein);
                    if(temp==EOF){
                        printf("BMP_READ_DATA: Failed reading data byte %d.\n",numbyte);
                        return 1;
                        }
                    *indexb=(unsigned char)temp;
                    numbyte=numbyte+1;
                    indexb=indexb+1;
                    temp=fgetc(filein);
                    if(temp==EOF){
                        printf("BMP_READ_DATA: Failed reading data byte %d.\n",numbyte);
                        return 1;
                        }
                    *indexg=(unsigned char)temp;
                    numbyte=numbyte+1;
                    indexg=indexg+1;
                    temp=fgetc(filein);
                    if(temp==EOF){
                        printf("BMP_READ_DATA: Failed reading data byte %d.\n",numbyte);
                        return 1;
                        }
                    *indexr=(unsigned char)temp;
                    numbyte=numbyte+1;
                    indexr=indexr+1;
                    }
                else if(bsize==8){
                    temp=fgetc(filein);
                    if(temp==EOF){
                        printf("BMP_READ_DATA: Failed reading data byte %d.\n",numbyte);
                        return 1;
                        }
                    *indexr=(unsigned char)temp;
                    numbyte=numbyte+1;
                    indexr=indexr+1;
                    }
                }
            while(numbyte % 4){
                fgetc(filein);
                numbyte++;
                }
            }
        return 0;
        }
    int bmp_read_header(FILE *filein,int *xsize,int *ysize,int *bsize,int *psize){
        int c1,c2,retval;
        unsigned long int u_long_int_val;
        unsigned short int u_short_int_val;
        c1=fgetc(filein);
        if(c1==EOF)
            return 1;
        c2=fgetc(filein);
        if(c2==EOF)
            return 1;
        if(c1!='B'||c2!='M')
            return 1;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        retval=read_u_short_int(&u_short_int_val,filein);
        if(retval==1)
            return 1;
        retval=read_u_short_int(&u_short_int_val,filein);
        if(retval==1)
            return 1;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        *xsize=(int)u_long_int_val;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        *ysize=(int)u_long_int_val;
        retval=read_u_short_int(&u_short_int_val,filein);
        if(retval==1)
            return 1;
        retval=read_u_short_int(&u_short_int_val,filein);
        if(retval==1)
            return 1;
        *bsize=(int)u_short_int_val;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        *psize=(int)u_long_int_val;
        retval=read_u_long_int(&u_long_int_val,filein);
        if(retval==1)
            return 1;
        return 0;
        }
    int bmp_read_palette(FILE *filein,int psize){
        int c,i,j;
        for(i=0;i<psize;i++){
            for(j=0;j<4;j++){
                c=fgetc(filein);
                if(c==EOF){
                    return 1;
                    }
                }
            }
        return 0;
        }
    int bmp_write(const char *fileout_name,int xsize,int ysize,int bsize,unsigned char *rarray,unsigned char *garray,unsigned char *barray){
        FILE *fileout;
        int   result;
        fileout=fopen(fileout_name,"wb");
        if(fileout==NULL){
            printf("\n");
            printf("BMP_WRITE - Fatal error!\n");
            printf("  Could not open the output file.\n");
            return 1;
            }
        result=bmp_write_header(fileout,xsize,ysize,bsize);
        if(result==1){
            printf("\n");
            printf("BMP_WRITE: Fatal error!\n");
            printf("  BMP_WRITE_HEADER failed.\n");
            return 1;
            }
        if(bsize==8){
            result=bmp_write_palette(fileout);
            if(result==1){
                printf("\n");
                printf("BMP_WRITE: Fatal error!\n");
                printf("  BMP_WRITE_PALETTE failed.\n");
                return 1;
                }
            }
        result=bmp_write_data(fileout,xsize,ysize,bsize,rarray,garray,barray);
        if(result==1){
            printf("\n");
            printf("BMP_WRITE: Fatal error!\n");
            printf("  BMP_WRITE_DATA failed.\n");
            return 1;
            }
        fclose(fileout);
        return 0;
        }
    int bmp_write_data(FILE *fileout,int xsize,int ysize,int bsize,unsigned char *rarray,unsigned char *garray,unsigned char *barray){
        int i;
        unsigned char *indexb;
        unsigned char *indexg;
        unsigned char *indexr;
        int j;
        int numbyte;
        numbyte=0;
        for(j=ysize-1;j>=0;j--){
            indexr=rarray+xsize*j*sizeof(unsigned char);
            indexg=garray+xsize*j*sizeof(unsigned char);
            indexb=barray+xsize*j*sizeof(unsigned char);
            for(i=0;i<xsize;i++){
                if(bsize==24){
                    fputc(*indexb,fileout);
                    fputc(*indexg,fileout);
                    fputc(*indexr,fileout);
                    indexb=indexb+1;
                    indexg=indexg+1;
                    indexr=indexr+1;
                    numbyte+=3;
                    }
                else{
                    fputc(*indexr,fileout);
                    indexr=indexr+1;
                    numbyte++;
                    }
                }
            while(numbyte %4){
                fputc(0,fileout);
                numbyte++;
                }
            }
        return 0;
        }
    int bmp_write_palette(FILE *fileout){
        int i,j;
        unsigned char c;
        for(i=0;i<256;i++){
            c=(unsigned char)i;
            for(j=0;j<3;j++)
                fputc(c,fileout);
            fputc(0,fileout);
            }
        return 0;
        }
    int bmp_write_header(FILE *fileout,int xsize,int ysize,int bsize){
        int i;
        unsigned long int u_long_int_val;
        unsigned short int u_short_int_val;
        int xsize_aligned;
        fputc('B',fileout);
        fputc('M',fileout);
        if(bsize==8){
            xsize_aligned=xsize;
            while(xsize_aligned %4)
                xsize_aligned++;
            u_long_int_val=xsize_aligned*ysize+54+256*4;
            }
        else{
            xsize_aligned=xsize;
            while(xsize_aligned %4)
                xsize_aligned++;
            u_long_int_val=xsize_aligned*ysize+54;
            }
        write_u_long_int(u_long_int_val,fileout);
        u_short_int_val=0;
        write_u_short_int(u_short_int_val,fileout);
        u_short_int_val=0;
        write_u_short_int(u_short_int_val,fileout);
        if(bsize==8){
            u_long_int_val=1078;
            }
        else{
            u_long_int_val=54;
            }
        write_u_long_int(u_long_int_val,fileout);
        u_long_int_val=40;
        write_u_long_int(u_long_int_val,fileout);
        write_u_long_int(xsize,fileout);
        write_u_long_int(ysize,fileout);
        u_short_int_val=1;
        write_u_short_int(u_short_int_val,fileout);
        u_short_int_val=bsize;
        write_u_short_int(u_short_int_val,fileout);
        u_long_int_val=0;
        write_u_long_int(u_long_int_val,fileout);   //compression
        u_long_int_val=(bsize/8)*xsize*ysize;
        write_u_long_int(u_long_int_val,fileout);
        for(i=2;i<4;i++){
            u_long_int_val=0;
            write_u_long_int(u_long_int_val,fileout);
            }
        if(bsize==8)
            u_long_int_val=256;		//Number of palette colors
        else
            u_long_int_val=0;
        write_u_long_int(u_long_int_val,fileout);
        u_long_int_val=0;
        write_u_long_int(u_long_int_val,fileout);
        return 0;
        }
    int bmp_write_test(char *fileout_name){
        unsigned char *barray;
        unsigned char *garray;
        unsigned char *rarray;
        int i;
        unsigned char *indexb;
        unsigned char *indexg;
        unsigned char *indexr;
        int j;
        int numbytes;
        int result;
        int xsize;
        int ysize;
        int sz=120;
        xsize=sz;
        ysize=sz;
        rarray=NULL;
        garray=NULL;
        barray=NULL;
        numbytes=xsize*ysize*sizeof(unsigned char);
        rarray=(unsigned char*)malloc(numbytes);
        if(rarray==NULL){
            printf("\n");
            printf("BMP_WRITE_TEST: Fatal error!\n");
            printf("  Unable to allocate memory for data.\n");
            return 1;
            }
        garray=(unsigned char*)malloc(numbytes);
        if(garray==NULL){
            printf("\n");
            printf("BMP_WRITE_TEST: Fatal error!\n");
            printf("  Unable to allocate memory for data.\n");
            return 1;
            }
        barray=(unsigned char *)malloc(numbytes);
        if(barray==NULL){
            printf("\n");
            printf("BMP_WRITE_TEST: Fatal error!\n");
            printf("  Unable to allocate memory for data.\n");
            return 1;
            }
        indexr=rarray;
        indexg=garray;
        indexb=barray;
        for(j=0;j<ysize;j++){
            for(i=0;i<xsize;i++){
                if(j<sz/3){
                    *indexr=255;
                    *indexg=0;
                    *indexb=0;
                    }
                else if(j<sz/3*2){
                    *indexr=0;
                    *indexg=255;
                    *indexb=0;
                    }
                else{
                    *indexr=0;
                    *indexg=0;
                    *indexb=255;
                    }
                indexr=indexr+1;
                indexg=indexg+1;
                indexb=indexb+1;
                }
            }
        result=bmp_write(fileout_name,xsize,ysize,24,rarray,garray,barray);
        if(rarray!=NULL){
            free(rarray);
            }
        if(garray!=NULL){
            free(garray);
            }
        if(barray!=NULL){
            free(barray);
            }
        if(result==1){
            printf("\n");
            printf("BMP_WRITE_TEST: Fatal error!\n");
            printf("  BMP_WRITE failed.\n");
            return 1;
            }
        return 0;
        }
    int read_u_long_int(unsigned long int *u_long_int_val,FILE *filein){
        int retval;
        unsigned short int u_short_int_val_hi;
        unsigned short int u_short_int_val_lo;
        if(byte_swap==1){
            retval=read_u_short_int(&u_short_int_val_lo,filein);
            if(retval==1){
                return 1;
                }
            retval=read_u_short_int(&u_short_int_val_hi,filein);
            if(retval==1){
                return 1;
                }
            }
        else{
            retval=read_u_short_int(&u_short_int_val_hi,filein);
            if(retval==1){
                return 1;
                }
            retval=read_u_short_int(&u_short_int_val_lo,filein);
            if(retval==1){
                return 1;
                }
            }
        *u_long_int_val=(u_short_int_val_hi << 16)| u_short_int_val_lo;
        return 0;
        }
    int read_u_short_int(unsigned short int *u_short_int_val,FILE *filein){
        int chi;
        int clo;
        if(byte_swap==1){
            clo=fgetc(filein);
            if(clo==EOF){
                return 1;
                }
            chi=fgetc(filein);
            if(chi==EOF){
                return 1;
                }
            }
        else{
            chi=fgetc(filein);
            if(chi==EOF){
                return 1;
                }
            clo=fgetc(filein);
            if(clo==EOF){
                return 1;
                }
            }
        *u_short_int_val=(chi << 8)| clo;
        return 0;
        }
    int write_u_long_int(unsigned long int u_long_int_val,FILE *fileout){
        unsigned short int  u_short_int_val_hi;
        unsigned short int  u_short_int_val_lo;
        u_short_int_val_hi=(unsigned short)(u_long_int_val / 65536);
        u_short_int_val_lo=(unsigned short)(u_long_int_val % 65536);
        if(byte_swap==1){
            write_u_short_int(u_short_int_val_lo,fileout);
            write_u_short_int(u_short_int_val_hi,fileout);
            }
        else{
            write_u_short_int(u_short_int_val_hi,fileout);
            write_u_short_int(u_short_int_val_lo,fileout);
            }
        return 4;
        }
    int write_u_short_int(unsigned short int u_short_int_val,FILE *fileout){
        unsigned char chi;
        unsigned char clo;
        chi=(unsigned char)(u_short_int_val / 256);
        clo=(unsigned char)(u_short_int_val % 256);
        if(byte_swap==1){
            fputc(clo,fileout);
            fputc(chi,fileout);
            }
        else{
            fputc(chi,fileout);
            fputc(clo,fileout);
            }
        return 2;
        }
    Pic ReadBMP(string in){
        unsigned char* rarray;
        unsigned char* garray;
        unsigned char* barray;
        int xsize,ysize,bsize;
        rarray=garray=barray=NULL;
        if(bmp_read(in.c_str(),&xsize,&ysize,&bsize,&rarray,&garray,&barray)==1){
            puts("1 BMP_READ failed.");
            }
        //printf("%d %d\n",ysize,xsize);
        Pic pic;
        for(int i=0;i<ysize;i++){
            pic.push_back(vector<Color>());
            for(int j=0;j<xsize;j++){
                //printf("%d %d %d\n",i,j,i*xsize+j);
                //printf("%d %d %d\n",rarray[i*xsize+j],rarray[i*xsize+j],rarray[i*xsize+j]);
                pic[i].push_back(Color(rarray[i*xsize+j],garray[i*xsize+j],barray[i*xsize+j]));
                }
            }
        if(rarray)free(rarray);
        if(garray)free(garray);
        if(barray)free(barray);
        return pic;
        }
    int WriteBMP(string out,Pic pic){
        unsigned char* rarray;
        unsigned char* garray;
        unsigned char* barray;
        int xsize=pic.front().size(),ysize=pic.size();
        rarray=new unsigned char[ysize*xsize];
        garray=new unsigned char[ysize*xsize];
        barray=new unsigned char[ysize*xsize];
        for(int i=0;i<ysize;i++){
            for(int j=0;j<xsize;j++){
                rarray[i*xsize+j]=pic[i][j].R;
                garray[i*xsize+j]=pic[i][j].G;
                barray[i*xsize+j]=pic[i][j].B;
                }
            }
        if(bmp_write(out.c_str(),xsize,ysize,24,rarray,garray,barray)==1){
            printf("\n");
            printf("BMP_WRITE failed.\n");
            return 1;
            }
        delete []rarray;
        delete []garray;
        delete []barray;
        return 0;
        }
    Pic ReadPNG(string in){
        std::vector<unsigned char> png;
        std::vector<unsigned char> image; //the raw pixels
        unsigned width, height;
        lodepng::State state; //optionally customize this one
        unsigned error = lodepng::load_file(png, in.c_str()); //load the image file with given filename
        if(!error) error = lodepng::decode(image, width, height, state, png);
        //State state contains extra information about the PNG such as text chunks, ...
        //if there's an error, display it
        if(error) std::cout << "decoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
        //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
        //printf("%d %d\n",width,height);
        Pic pic;
        pic.resize(height);
        for(unsigned int i=0;i<height;i++){
            pic[i].resize(width);
            for(unsigned int j=0;j<width;j++)
                pic[i][j]=Color(image[(i*width+j)*4],image[(i*width+j)*4+1],image[(i*width+j)*4+2],image[(i*width+j)*4+3]);
            }
        return pic;
        }
    void WritePNG(string out,Pic pic){
        std::vector<unsigned char> image;
        unsigned width=pic[0].size();
        unsigned height=pic.size();
        for(unsigned int i=0;i<height;i++)
            for(unsigned int j=0;j<width;j++)
                image.push_back(pic[i][j].R),
                image.push_back(pic[i][j].G),
                image.push_back(pic[i][j].B),
                image.push_back(pic[i][j].A);
        std::vector<unsigned char> png;
        lodepng::State state; //optionally customize this one
        unsigned error = lodepng::encode(png, image, width, height, state);
        if(!error) lodepng::save_file(png,out.c_str());
        //if there's an error, display it
        if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
        }
    static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";
    static inline bool is_base64(unsigned char c){
        return (isalnum(c)||(c=='+')||(c=='/'));
        }
    std::string EncodeBase64(std::string const& decoded_string){
        unsigned char const* bytes_to_encode=(unsigned char const*)decoded_string.c_str();
        unsigned int in_len=decoded_string.length();
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];
        while(in_len--){
            char_array_3[i++] = *(bytes_to_encode++);
            if(i == 3){
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for(i = 0; (i <4) ; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
                }
            }
        if(i){
            for(j = i; j < 3; j++)
                char_array_3[j] = '\0';
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (j = 0; (j < i + 1); j++)
                ret += base64_chars[char_array_4[j]];
            while((i++ < 3))
                ret += '=';
            }
        return ret;
        }
    std::string DecodeBase64(std::string const& encoded_string){
        int in_len = encoded_string.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        unsigned char char_array_4[4], char_array_3[3];
        std::string ret;
        while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])){
            char_array_4[i++] = encoded_string[in_];
            in_++;
            if(i ==4){
                for (i = 0; i <4; i++)
                    char_array_4[i] = base64_chars.find(char_array_4[i]);

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (i = 0; (i < 3); i++)
                    ret += char_array_3[i];
                i = 0;
                }
            }
        if(i){
            for (j = i; j <4; j++)
                char_array_4[j] = 0;
            for (j = 0; j <4; j++)
                char_array_4[j] = base64_chars.find(char_array_4[j]);
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
            }
        return ret;
        }
    std::string EncodeUrl(const std::string& src){
        static char hex[] = "0123456789ABCDEF";
        std::string dst;

        for (size_t i = 0; i < (size_t)src.size(); i++){
            uint8_t ch = src[i];
            if (isalnum(ch)){
                dst += ch;
                }
            else if (src[i] == ' '){
                dst += '+';
                }
            else{
                uint8_t c = static_cast<uint8_t>(src[i]);
                dst += '%';
                dst += hex[c / 16];
                dst += hex[c % 16];
                }
            }
        return dst;
        }
    std::string DecodeUrl(const std::string& src){
        std::string dst, dsturl;
        int srclen = src.size();
        for (int i = 0; i < srclen; i++){
            if (src[i] == '%'){
                if(isxdigit(src[i + 1]) && isxdigit(src[i + 2])){
                    char c1 = src[++i];
                    char c2 = src[++i];
                    c1 = c1 - 48 - ((c1 >= 'A') ? 7 : 0) - ((c1 >= 'a') ? 32 : 0);
                    c2 = c2 - 48 - ((c2 >= 'A') ? 7 : 0) - ((c2 >= 'a') ? 32 : 0);
                    dst += (uint8_t)(c1 * 16 + c2);
                    }
                }
            else if (src[i] == '+'){
                dst += ' ';
                }
            else{
                dst += src[i];
                }
            }
        /*
        int len = dst.size();
        for(uint32_t pos = 0; (int)pos < len;){
            uint32_t nvalue = utf8_decode((char *)dst.c_str(), &pos);
            dsturl += (uint8_t)nvalue;
            }
        return dsturl;
        */
        return dst;
        }
    std::string trim(std::string s){
        if(s.empty())
            return s;
        s.erase(0,s.find_first_not_of(" \t\n\r\0\x0B"));
        s.erase(s.find_last_not_of(" \t\n\r\0\x0B")+1);
        return s;
        }
    std::wstring trim(std::wstring s){
        if(s.empty())
            return s;
        s.erase(0,s.find_first_not_of(L" \t\n\r\0\x0B"));
        s.erase(s.find_last_not_of(L" \t\n\r\0\x0B")+1);
        return s;
        }
    wstring erase_quote(wstring s) {
        s = trim(s);
        if (s.front() == '"'&&s.back() == '"') {
            if (s.length() > 2)s = s.substr(1, s.length() - 2);
            else s = L"";
            }
        return s;
        }
    void ReadJson(wstring s, Json_Node& now) {
        s = trim(s);
        if (s.front() == '{'&&s.back() == '}') {
            if (s.length()>2)s = s.substr(1, s.length() - 2);
            else s = L"";
            now.type = 1;
            wstring nw;
            int ntp = 0;
            stack<wchar_t>st;
            int beg=0;
            int len = (int)(s.length());
            for (int i = 0;i <= len;i++) {
                if (ntp == 0) {
                    if (s[i] == ':')ntp = 1, beg = i + 1;
                    else nw += s[i];
                    }
                else if (ntp == 1) {
                    if (s[i] == '{')
                        st.push('{');
                    else if (s[i] == '[')
                        st.push('[');
                    else if (s[i] == '}') {
                        if (st.top() == '{')
                            st.pop();
                        }
                    else if (s[i] == ']') {
                        if (st.top() == '[')
                            st.pop();
                        }
                    else if (s[i] == '"') {
                        if (!st.empty() && st.top() == '"')
                            st.pop();
                        else
                            st.push('"');
                        }
                    else if (s[i] == '\\') {
                        i++;
                        }
                    else if (s[i] == ',' || s[i] == 0) {
                        if (st.empty())
                            ReadJson(s.substr(beg, i - beg), now.child[erase_quote(nw)]), ntp = 0, nw = L"";
                        }
                    }
                }
            }
        else if (s.front() == '['&&s.back() == ']') {
            if (s.length()>2)s = s.substr(1, s.length() - 2);
            else s = L"";
            now.type = 2;
            wstring nw;
            stack<wchar_t>st;
            int beg = 0;
            int len = (int)(s.length());
            for (int i = 0;i <= len;i++) {
                if (s[i] == '{')
                    st.push('{');
                else if (s[i] == '[')
                    st.push('[');
                else if (s[i] == '}') {
                    if (st.top() == '{')
                        st.pop();
                    }
                else if (s[i] == ']') {
                    if (st.top() == '[')
                        st.pop();
                    }
                else if (s[i] == '"') {
                    if (!st.empty() && st.top() == '"')
                        st.pop();
                    else
                        st.push('"');
                    }
                else if (s[i] == '\\') {
                    i++;
                    }
                else if (s[i] == ',' || s[i] == 0) {
                    if (st.empty()) {
                        now.ary.push_back(Json_Node()), ReadJson(s.substr(beg, i - beg), now.ary.back()), beg = i + 1;
                        }
                    }
                }
            }
        else {
            now.type = 0;
            now.data = erase_quote(s);
            }
        }

    Json_Node ReadJson(wstring s) {
        s = trim(s);
        Json_Node now;
        if (s.front() == '{'&&s.back() == '}') {
            now.type = 1;
            if (s.length()>2)s = s.substr(1, s.length() - 2);
            else s = L"";
            wstring nw;
            int ntp = 0;
            stack<wchar_t>st;
            int beg=0;
            int len = (int)(s.length());
            for (int i = 0;i <= len;i++) {
                if (ntp == 0) {
                    if (s[i] == ':')ntp = 1, beg = i + 1;
                    else nw += s[i];
                    }
                else if (ntp == 1) {
                    if (s[i] == '{')
                        st.push('{');
                    else if (s[i] == '[')
                        st.push('[');
                    else if (s[i] == '}') {
                        if (st.top() == '{')
                            st.pop();
                        }
                    else if (s[i] == ']') {
                        if (st.top() == '[')
                            st.pop();
                        }
                    else if (s[i] == '"') {
                        if (!st.empty() && st.top() == '"')
                            st.pop();
                        else
                            st.push('"');
                        }
                    else if (s[i] == '\\') {
                        i++;
                        }
                    else if (s[i] == ',' || s[i] == 0) {
                        if (st.empty())
                            ReadJson(s.substr(beg, i - beg), now.child[erase_quote(nw)]), ntp = 0, nw = L"";
                        }
                    }
                }
            }
        else if (s.front() == '['&&s.back() == ']') {
            if (s.length()>2)s = s.substr(1, s.length() - 2);
            else s = L"";
            now.type = 2;
            wstring nw;
            stack<wchar_t>st;
            int beg = 0;
            int len = (int)(s.length());
            for (int i = 0;i <= len;i++) {
                if (s[i] == '{')
                    st.push('{');
                else if (s[i] == '[')
                    st.push('[');
                else if (s[i] == '}') {
                    if (st.top() == '{')
                        st.pop();
                    }
                else if (s[i] == ']') {
                    if (st.top() == '[')
                        st.pop();
                    }
                else if (s[i] == '"') {
                    if (!st.empty() && st.top() == '"')
                        st.pop();
                    else
                        st.push('"');
                    }
                else if (s[i] == '\\') {
                    i++;
                    }
                else if (s[i] == ',' || s[i] == 0) {
                    if (st.empty())
                        now.ary.push_back(Json_Node()), ReadJson(s.substr(beg, i - beg), now.ary.back()), beg = i + 1;
                    }
                }
            }
        else {
            now.type = 0;
            now.data = erase_quote(s);
            }
        return now;
        }
    void PraseJson(const Json_Node& now, int l,wstring &out){
        if (now.type == 0)out += now.data;
        else if (now.type == 1) {
            out += L"{\n";
            for (auto &x : now.child) {
                for (int i = 0;i <= l;i++)
                    out += L"  ";
                out += x.f + L": ";
                PraseJson(x.s, l + 1,out);
                out += L"\n";
                }
            for (int i = 0;i < l;i++)
                out += L"  ";
            out += L"}\n";
            }
        else if (now.type == 2) {
            out += L"[\n";
            for (auto &x : now.ary) {
                for (int i = 0;i <= l;i++)
                    out += L"  ";
                PraseJson(x,l + 1,out);
                out += L"\n";
                }
            for (int i = 0;i < l;i++)
                out += L"  ";
            out += L"]\n";
            }
        }
    wstring ReadableJson(const Json_Node&now){
        wstring res;
        PraseJson(now,0,res);
        return res;
        }
    vector<string>split(string s,string cut,int num){
        vector<string>ve;
        auto clen=cut.length();
        while(true){
            if(num==1){
                ve.push_back(s);
                break;
                }
            auto pos=s.find(cut);
            if(pos==string::npos){
                ve.push_back(s);
                break;
                }
            else{
                if(pos)
                    ve.push_back(s.substr(0,pos));
                else
                    ve.push_back("");
                if(pos+clen!=s.length())
                    s=s.substr(pos+clen);
                else
                    s="";
                }
            num--;
            }
        return ve;
        }
    string join(vector<string>ve,string s){
        size_t sz=ve.size();
        if(sz==0)return "";
        string result=ve[0];
        for(size_t i=1;i<sz;i++)
            result+=s+ve[i];
        return result;
    }
    mutex exec_mut;
    string exec(string cmd){
        exec_mut.lock();
        //cout<<cmd<<endl;
        char buffer[1005];
        string result = "";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) throw std::runtime_error("popen() failed!");
        try {
            while (!feof(pipe)) {
                size_t len=fread(buffer,sizeof(char),1000,pipe);
                if (len > 0)
                    result += string(buffer,len);
                }
            }
        catch(...){
            pclose(pipe);
            throw;
            }
        pclose(pipe);
        exec_mut.unlock();
        return result;
        }
    map<Net,Thread>nc_map;
    map<Net,stream>nc_stream;
    rwlock nc_lock;
    void nc_background(Net net,int output){
        while(true){
            nc_lock.read_lock();
            bool brk=nc_map.find(net)==nc_map.end();
            auto &str=nc_stream[net];
            nc_lock.unlock();
            //printf("^%d\n",brk);
            if(brk)break;
            int result;
            string s=NetGet(net,result);
            if(result<0)break;
            str.clear();
            str.append(s);
            if(output==1)
                printf("%s",s.c_str());
            Sleep(15);
            }
        nc_lock.write_lock();
        if(nc_map.find(net)!=nc_map.end())nc_map.erase(net);
        nc_lock.unlock();
        NetClose(net);
        }
    stream &nc(const string& ip,int port,int output){
        Net net=NetCreat(ip.c_str(),port,0);
        nc_lock.write_lock();
        nc_stream[net].net=net;
        nc_map[net]=ThreadCreate(nc_background,net,output);
        auto &st=nc_stream[net];
        nc_lock.unlock();
        return st;
        }
    void nc_close(Net net){
        Thread th=0;
        nc_lock.write_lock();
        if(nc_map.find(net)!=nc_map.end())th=nc_map[net],nc_map.erase(net);
        nc_lock.unlock();
        if(th)Wait(th);
        nc_lock.write_lock();
        if(nc_stream.find(net)!=nc_stream.end())nc_stream.erase(net);
        nc_lock.unlock();
        }
    bool nc_is_closed(Net net){
        nc_lock.read_lock();
        auto it=nc_map.find(net),ed=nc_map.end();
        bool res=it==ed;
        nc_lock.unlock();
        return res;
        }
    stream &nc(Net net,int output){
        nc_lock.read_lock();
        auto str=nc_stream.find(net);
        auto end=nc_stream.end();
        nc_lock.unlock();
        if(str!=end)
            return str->s;
        else{
            nc_lock.write_lock();
            nc_map[net]=ThreadCreate(nc_background,net,output);
            auto &st=nc_stream[net];
            nc_lock.unlock();
            return st;
            }
        }
    bool is_hex(char c){
        if('0'<=c&&c<='9')return true;
        else if('a'<=c&&c<='f')return true;
        else if('A'<=c&&c<='F')return true;
        return false;
        }
    int hex(char c){
        if('0'<=c&&c<='9')return c-'0';
        else if('a'<=c&&c<='f')return c-'a'+10;
        else if('A'<=c&&c<='F')return c-'A'+10;
        return 0;
        }
    string phrase_string(string s){
        string ret;
        int sz=s.length();
        for(int i=0;i<sz;i++){
            if(s[i]=='\\'){
                if(i<sz-1&&s[i+1]=='n')
                    ret+='\n',i++;
                else if(i<sz-1&&s[i+1]=='t')
                    ret+='\t',i++;
                else if(i<sz-1&&s[i+1]=='v')
                    ret+='\v',i++;
                else if(i<sz-1&&s[i+1]=='b')
                    ret+='\b',i++;
                else if(i<sz-1&&s[i+1]=='r')
                    ret+='\r',i++;
                else if(i<sz-1&&s[i+1]=='f')
                    ret+='\f',i++;
                else if(i<sz-1&&s[i+1]=='a')
                    ret+='\a',i++;
                else if(i<sz-1&&s[i+1]=='\\')
                    ret+='\\',i++;
                else if(i<sz-1&&s[i+1]=='?')
                    ret+='\?',i++;
                else if(i<sz-1&&s[i+1]=='\'')
                    ret+='\'',i++;
                else if(i<sz-1&&s[i+1]=='\"')
                    ret+='\"',i++;
                else if(i<sz-1&&s[i+1]=='0')
                    ret+='\0',i++;
                else if(i<sz-3&&s[i+1]=='x'&&is_hex(s[i+2])&&is_hex(s[i+3]))
                    ret+=hex(s[i+2])*16+hex(s[i+3]),i+=3;
                else if(i<sz-2&&s[i+1]=='x'&&is_hex(s[i+2]))
                    ret+=hex(s[i+2])*16+hex(s[i+3]),i+=2;
                else ret+=s[i];
                }
            else
                ret+=s[i];
            }
        return ret;
        }
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp){
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
    string request(string url){
        CURL *curl;
        CURLcode res;
        string s;
        curl = curl_easy_init();
        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&s);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            }
        if(res)return s;
        return s;
        }
    }

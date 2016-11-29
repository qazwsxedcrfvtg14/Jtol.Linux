#include<stdio.h>
#include<stdlib.h>
#include<Winsock2.h>
#include<process.h>
#include<winsock2.h>
#include<stdio.h>
#include<windows.h>
#include <process.h>
#include <conio.h>
#include <iostream>
#include <string>
using namespace std;
const int RAND=5491;
char ip_list[10][1000]={
	"ptt.cc",
	"infor.org",
	"csisc.twbbs.org",
	"standaway.twbbs.org",
	"fgisc.org",
	};
SOCKET socket1;
struct sockaddr_in server;
int len,T=0;
HANDLE hIn, hOut;
void gotoxy (int x, int y) {
    static COORD c; c.X = y; c.Y = x;
    SetConsoleCursorPosition(hOut, c);
	}
void getxy(int &x, int &y){
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hOut, &csbi);
	x = csbi.dwCursorPosition.X;
	y = csbi.dwCursorPosition.Y;
	}
HANDLE hConsole;
void Clr(){
        COORD coordScreen = { 0, 0 };
        DWORD cCharsWritten;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD dwConSize;
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
        FillConsoleOutputCharacter(hConsole, TEXT(' '),
                dwConSize,
                coordScreen,
                &cCharsWritten);
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        FillConsoleOutputAttribute(hConsole,
                csbi.wAttributes,
                dwConSize,
                coordScreen,
                &cCharsWritten);
        SetConsoleCursorPosition(hConsole, coordScreen);
	}
void pri(string s){
    string bufer;
	int len=s.length();
	//cerr<<s;
	for(int i=0;i<len;i++){
		if(s[i]==27&&i+1<len&&s[i+1]=='['){
			int k=i;
			for(i+=2;i<len;i++){
				if(s[i]=='J'){
					printf("%s",&bufer[0]);
					bufer="";
					Clr();
					break;
					}
				if(s[i]=='K'){
					printf("%s",&bufer[0]);
					bufer="";
					int x,y;
					getxy(x,y);
					for(int i=0;i<(79-x);i++)
						putchar(' ');
					break;
					}
				if(s[i]=='m'){
					break;
					}
				if(s[i]=='H'){
					int a[3]={0,0},type=0;
					for(int j=k+2;j<i;j++){
						if(s[j]>='0'&&s[j]<='9'){
							a[type]=a[type]*10+s[j]-'0';
							}
						else{
							type++;
							}
						}
					if(a[1])a[1]--;
					if(a[0])a[0]--;

						printf("%s",&bufer[0]);
						bufer="";
						gotoxy(a[0],a[1]);
					//gotoxy(0,0);
					break;
					}
				if(s[i]>='a'&&s[i]<='z'){
					//printf("!!!!!!!%c!!!!!!",s[i]);
					break;
					}
				if(s[i]>='A'&&s[i]<='Z'){
					//printf("!!!!!!!%c!!!!!!",s[i]);
					break;
					}
				}
			//s=s.erase(i,j-i+1);
			}
        else if(s[i]==0x0d&&i+1<len&&s[i+1]==0x0a){

            i++;
        /*
            printf("%s",&bufer[0]);
            bufer="";
            int x,y;
            getxy(x,y);
            //cerr<<x<<" "<<y<<endl;
            //putchar(0x0a);
            gotoxy(x,y+1);
        */
            bufer+=0x0d;
            bufer+=0x0a;
            }
		else{
            //if(s[i]==8)bufer+=8,bufer+=' ';
			bufer+=s[i];
			}
		}
    printf("%s",&bufer[0]);
	return ;//s
	}
char buffer[10000000];
string str;
int brk;
void chk(){
	//puts("waiting...");
	while(1){ ////[
		if(brk)break;
		memset(buffer,0,sizeof(buffer));
        if(recv(socket1,buffer,sizeof(buffer),0) != SOCKET_ERROR){
        	//if(strcmp(buffer,"$Testcon")==0)
				//T=1;
        	//else if(strcmp(buffer,"$OK")!=0){
				str=buffer;
				//homed=0;
				pri(str);
				//if(kx!=-1&&ky!=-1)
					//gotoxy(kx,ky);
				//printf("%s",buffer);
                //send sth to the server
				//send(socket1,"$OK",sizeof("$OK"),0);
				//}
			}
		}
	}


int main (int agrc,char*argv[]){
    //cout<<"12349487"<<char(8)<<char(13)<<"567"<<char(10)<<"789";
    //const char* ip="csisc.twbbs.org";//"203.68.236.13";
    char ip[100];//="csisc.twbbs.org";//"203.68.236.13";
    printf("telnet//:");
    scanf("%s",ip);
    if(strlen(ip)==1){
		strcpy(ip,ip_list[ip[0]-'0']);
		}
	//memset(end_line,0,sizeof(end_line));
	hOut = GetStdHandle (STD_OUTPUT_HANDLE);
    hIn  = GetStdHandle (STD_INPUT_HANDLE);
    HANDLE err = INVALID_HANDLE_VALUE;
	//printf("%c\n",33);
	HANDLE thd;
	DWORD tid;
	WSADATA wsaData;
    int ErrorCode;
    if(WSAStartup(MAKEWORD(2,1),&wsaData)){
		printf("Winsock initiate failed!!\n");
        WSACleanup();
        return 0;
		}
    printf("Winsock start...\n");
    len = sizeof(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(23);  //the port need listened

    server.sin_addr.s_addr = inet_addr(ip);
    if( server.sin_addr.s_addr == INADDR_NONE ){
    	hostent *host = gethostbyname(ip);
        server.sin_addr.s_addr = *reinterpret_cast<unsigned long*>(host->h_addr_list[0]);
		if( server.sin_addr.s_addr == INADDR_NONE ){
			puts("DNS error");
			exit (1);
			}
		}


    //socket1 = socket(AF_INET,SOCK_DGRAM,0);
    socket1 = socket(AF_INET,SOCK_STREAM,0);
    int r=connect(socket1, (struct sockaddr *)&server, len);
    if(r == -1) {
		printf("Connetc Error");
		exit(1);
	}
	thd = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)chk,(void*)0,0,&tid);
	printf("Thread %d is start \n",tid);
	puts("Loading~");
    /*while(1){
		char buffer[1024] = "$Testcon";
		send(socket1,buffer,sizeof(buffer),0);
    	Sleep(1000);
    	if(T){puts("Done!");break;}
		puts("Loading~");
		}*/
	int type=0,type2=0;
	string buf="",rec,rec2,rec3;
    while(1){
        //scanf("%s",buffer);
        char ch=getch();
        //buffer[0]='$';buffer[1]='T';buffer[2]='E';buffer[3]='S';buffer[4]='T';
        //buffer[5]='A';
        //buffer[6]='B';
        //buffer[7]=0;
        //printf("%s",buffer);
        /*
        if(strcmp(buffer,"bye")==0){
			printf("exit,bye!!\n");
			Sleep(100);
			closesocket(socket1);
			break;
			}
		int len=strlen(buffer);
		buffer[len]='\n';
		buffer[len+1]='\0';
		*/
		//F1-59 F2-60 ...
		if(type==1&&ch==27){
			if(rec=="")rec=buf;
			else if(rec2=="")rec2=buf;
			else if(rec3=="")rec3=buf;
			type=0;
			}
		else if(type==1){
			buf+=ch;
			}
		else{
			if(ch!=-32){buf+=ch;}//µL
			else {
				char kk=getch();
				//18 26¨ê·s­¶­±
				//10 enter
				//13 enter
				//1 home
				//21 ?
				//127 del
				if(kk==72){
					buf+='\33';buf+="[A";
					//if(type2){rec+='\33';rec+="[A";}
					}//¤W
				else if(kk==80){
					buf+='\33';buf+="[B";
					//if(type2){rec+='\33';rec+="[B";}
					}//¤U
				else if(kk==77){
					buf+='\33';buf+="[C";
					//if(type2){rec+='\33';rec+="[C";}
					}//¥k
				else if(kk==75){
					buf+='\33';buf+="[D";
					//if(type2){rec+='\33';rec+="[D";}
					}//¥ª
				else{
					buf+=ch;buf+=kk;
					//if(type2){rec+=ch;rec+=kk;}
					}
				}
			}
		if(!type){
			if(send(socket1,&buf[0],buf.length(),0)==SOCKET_ERROR){
				brk=0;
				break;
				}
			buf="";
			}
		}
    closesocket(socket1);
    return 0;
	}



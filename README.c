請記得在連結器內加入 "-lwsock32","-lgdi32","-luser32","-lkernel32","-lcomctl32"
如果要執行檔可以跨電腦執行請再加入 "-static"
然後使用了Jtol的namespace
結構:
    Time 存時間
    Pos 存座標(f,s)
    Net 網路連線用指針
    Color 存顏色(R,G,B,A)
    Node Html_DOM節點
    Thread 執行緒指針
    rwlock 讀寫鎖, read_lock(), write_lock() ,unlock()
    Pic 圖片結構=vector<vector<Color>>
    Json_Node {type,data,child,ary}
函式:
    Setup()
        初始化
    GetTime():Time
        取得時間
    NetCreat(char ip[],int port=23,int mode=1):Net
        建立網路連線
    NetClose(Net sock)
        關閉網路連線
    NetGet(Net sock):char*
        取得網路訊息
    NetSend(NET sock,char *s)
        送出網路訊息
    FileToStr(const char *file_name):string
        將字串寫入檔案
    StrToFile(string s,const char *file_name)
        把檔案中全部字元放入字串
    UTCTime()
        印出現在時間之標準格式
    IntToStr(int x):string
        int->string
    StrToInt(string x):int
        string->int
    HtmlToNode(string s):Node*
        把一個html字串轉換為DOM節點
    HttpDecode(string s):Node
        解析http header
    ThreadCreate(function,args...):Thread
        建立新執行緒
    Wait(Thread thr)
        等待thr結束
    ReadBMP(string file_name):Pic
        讀取bmp檔案
    WriteBMP(string out,Pic pic)
        寫入bmp檔案
    ReadPNG(string file_name):Pic
        讀取png檔案
    WritePNG(string out,Pic pic)
        寫入png檔案
    md5(string const& encoded_string):string
        編碼成md5
    EncodeUrl(string const& decoded_string):string
        編碼成Url字串
    DecodeUrl(string const& encoded_string):string
        以Url字串解碼
    EncodeUtf8(wstring const& encoded_string):string
        編碼成Utf8
    DecodeUtf8(string const& encoded_string):wstring
        以Utf8解碼
    ReadJson(wstring s):Json_Node
        解析Json字串
    ReadableJson(const Json_Node&now):wstring
        將Json物件變成可讀字串

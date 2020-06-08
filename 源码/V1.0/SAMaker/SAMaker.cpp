
#include "include.h"


static  char configpath[200];
static   char sourcenamepath[200];


static float dest[1024 * 4][3];
size_t dest_num = 0;

bool is_equ(char A[], size_t Astart, size_t Anum, char B[], size_t Bstart, size_t Bnum);
float TransB(char sou[], size_t start, size_t num);
float TransB_16(char sou[], size_t start, size_t num);
int Init(void);
int ReadSource(void);
int Start();
void initshuzu(char* A, size_t len);
inline float PosMaker(char A[], size_t start, size_t num);
void initbms(OneBMS& bms);
inline float JPMaker(OneBMS onebms, size_t pos);
void JieXi(OneBMS& bms, char A[], size_t start, size_t num);
void AAA(char* A, const char B[]);
void output(ofstream& out);


static class MAP
{
private:
    char _BPM[3];
    char A[30][3];  //映射
    size_t BPMNum;
    size_t MapNum;
    int Aflag[30];//0代表普通  1代表左绿 2代表右绿 3代表左绿尾 4代表右绿尾
    float B[30];
    void init();
public:
    char sour[1024 * 50];
    char music_name[30];
    float BPM;
    size_t length, point;
    int is_green(char sou[], size_t start, size_t num);   //0不是绿，1是左绿，2是右绿  3代表左绿尾 4代表右绿尾  -1代表未找到;
    float TransA(char sou[], size_t start, size_t num, uchar flag);    //按键映射  0普通，1绿头，2绿中,5代表未找到
    MAP(void) {
        init();
    };
    void open(ifstream& in);
    ~MAP()
    {

    }
}map;
static class BuFFER {
private:
    void init();
    void exchange(size_t A, size_t B);
    int LeftOrRight(size_t pos);
    size_t findend(size_t pos);

public:
    float BUFFER1[1024 * 4];     //存放节拍
    char BUFFER2[1024 * 4][3];   //存放位置及控制码
    char BUFFER3[1024 * 4][3];   //存放码
    size_t NUM;

    BuFFER() {
        init();
    };
    void open(MAP& map);


    ~BuFFER() {};
}BUFFER;
int leftorright(size_t A, size_t B, size_t C);
size_t findnext(size_t P);
float TranC(const char A[], size_t a);
int IsSingleOrDouble(size_t pos);
int getnewdata(char* buffer, size_t len, MAP& map);
int Working(ifstream& in, ofstream& out);




int BeginMakeFromBMS( char config[] ,  char source[])
{
    ifstream in;
    ofstream out;
    bool flag;
    char buffer[100];

    strcpy_s(configpath, config);
    strcpy_s(sourcenamepath, source);


    if (!Start())return 0;

    for (size_t i = 0; i < Works.Num; i++)
    {
        flag = true;
        for (size_t j = 0; j < 100; j++) buffer[j] = 0;
        strcpy_s(buffer, SourePath);
        strcat_s(buffer, Works.Name[i]);
        in.open(buffer, ios::in | ios::binary);
        for (size_t j = 0; j < 100; j++) buffer[j] = 0;
        strcpy_s(buffer, DestinationPath);
        strcat_s(buffer, Works.Name[i]);
        out.open(buffer, ios::out | ios::binary);

        if (!in.is_open())flag = false;
        if (!out.is_open())flag = false;



        cout << '\t' << '\t';
        cout.fill('0');
        cout.width(3);
        cout << i + 1 << ": " << Works.Name[i] << '\t' << "working……" << '\r';
        if (flag && Working(in, out)) {
            cout << "                                                                                    " << '\r';
            cout << '\t' << '\t';
            cout.fill('0');
            cout.width(3);
            cout << i + 1 << ": " << Works.Name[i] << '\t' << "success!" << '\r';
        }
        else
        {
            cout << "                                                                                    " << '\r';
            cout << '\t' << '\t';
            cout.fill('0');
            cout.width(3);
            cout << i + 1 << ": " << Works.Name[i] << '\t' << "fail~" << '\r';
        }
        cout << endl;
        in.close();
        out.close();
    }


    cout << endl << '\t' << '\t' << '\t' << '\t' << "〖弄好了~~~〗" << endl;
    cout << endl << endl;

    map.~MAP();
    BUFFER.~BUFFER();
    return 0;
}

////*----------------------HEADER FIELD
//static struct
//{
//    char FileName[30][50];
//    char Key[30][3];
//    size_t Num;
//
//}FileAndKey;
//
//static struct
//{
//    char BMSTrack[7][6];
//    char ScoreTrack[7][3];
//
//}BMSAndScore;
//
//static char SourePath[100];
//static char DestinationPath[100];
//
//
////*---------------------- INDEX FIELD
//
//static struct
//{
//    char Number[400][4];
//    char MusicName[400][50];
//    size_t Num;
//}Index;
//
////*---------------------- WORKSTATION FIELD
//static struct
//{
//    char Name[400][100];
//    size_t Num;
//}Works;





bool is_equ(char A[], size_t Astart, size_t Anum, char B[], size_t Bstart, size_t Bnum) {
    if (Anum != Bnum)return false;
    for (size_t i = 0; i < Anum; i++)
    {
        if (A[Astart + i] != B[Bstart + i])return false;
    }

    return true;
}
float TransB(char sou[], size_t start, size_t num) //把字符串转换成浮点数
{
    float temp = 0;
    for (size_t i = 0; i < num; i++)
    {
        temp = temp * 10 + (float)(sou[start + i] - '0');
    }

    return temp;
}
float TransB_16(char sou[], size_t start, size_t num) //把字符串转换成浮点数
{
    float temp = 0;
    for (size_t i = 0; i < num; i++)
    {
        if (sou[i] <= '9')
        {
            temp = temp * 16 + sou[start + i] - '0';
        }
        else
        {
            temp = temp * 16 + sou[start + i] - 'A' + 10;
        }
    }

    return temp;
}
int Init(void) {
    ifstream config(configpath, ios::in | ios::binary);
    char buffer[100];
    char buffer2[30];
    size_t i, j;

    if (!config.is_open()) {
        return 0;
    }
    //开始初始化Config
    //FileAndKey
    config.getline(buffer, 100, '#');
    config.getline(buffer, 100, '#');
    for (i = 0; i < 100; i++)
    {
        if (buffer[i] == '\n' || buffer[i] == '\t' || buffer[i] == '\r' || buffer[i] == ' ') {
            buffer[i] = '\0';
            break;
        }
    }
    if (strcmp(buffer, "note") != 0) {
        return 0;
    }
    for (i = 0; i < 30; i++)
    {
        config.getline(buffer, 100, '#');//读取;
        for (j = 0; j < strlen("nodeend"); j++)
        {
            buffer2[j] = buffer[j];
        }
        buffer2[j] = '\0';
        if (strcmp(buffer2, "nodeend") == 0)
        {
            break;
        }

        for (j = 0; buffer[j] != '%' && buffer[j] != '\n' && buffer[j] != '\t' && buffer[i] != '\r' && buffer[i] != ' '; j++)//初始化
        {
            FileAndKey.FileName[i][j] = buffer[j];
        }
        FileAndKey.FileName[i][j] = '\0';
        if (buffer[j] != '%')for (; buffer[j] != '%'; j++);
        size_t jj = 0;
        for (j++; buffer[j] != '$' && buffer[j] != '\n' && buffer[j] != '\t' && buffer[i] != '\r' && buffer[i] != ' '; j++)//初始化
        {
            FileAndKey.Key[i][jj++] = buffer[j];
        }
        FileAndKey.Key[i][jj] = '\0';
        FileAndKey.Num = i + 1;
    }


    //BMSAndScore
    config.getline(buffer, 100, '#');
    for (i = 0; i < 100; i++)
    {
        if (buffer[i] == '\n' || buffer[i] == '\t' || buffer[i] == '\r' || buffer[i] == ' ') {
            buffer[i] = '\0';
            break;
        }
    }
    if (strcmp(buffer, "track") != 0) {
        return 0;
    }
    for (i = 0; i < 7; i++)
    {
        config.getline(buffer, 100, '#');//读取;
        for (j = 0; buffer[j] != '%' && buffer[j] != '\n' && buffer[j] != '\t' && buffer[i] != '\r' && buffer[i] != ' '; j++)//初始化
        {
            BMSAndScore.BMSTrack[i][j] = buffer[j];
        }
        BMSAndScore.BMSTrack[i][j] = '\0';
        if (buffer[j] != '%')for (; buffer[j] != '%'; j++);
        size_t jj = 0;
        for (j++; buffer[j] != '$' && buffer[j] != '\n' && buffer[j] != '\t' && buffer[i] != '\r' && buffer[i] != ' '; j++)//初始化
        {
            BMSAndScore.ScoreTrack[i][jj++] = buffer[j];
        }
        BMSAndScore.ScoreTrack[i][jj] = '\0';

    }

    //SourePath
    config.getline(buffer, 100, '#');
    config.getline(buffer, 100, '#');
    for (i = 0; i < 100; i++)
    {
        if (buffer[i] == '\n' || buffer[i] == '\t' || buffer[i] == '\r' || buffer[i] == ' ') {
            buffer[i] = '\0';
            break;
        }
    }
    if (strcmp(buffer, "sourcepath") != 0) {
        return 0;
    }
    config.getline(buffer, 100, '%');
    config.getline(buffer, 100, '#');
    for (i = 0; i < 100 && buffer[i] != '$'; i++)
    {
        SourePath[i] = buffer[i];
    }
    SourePath[i] = '\0';
    config.getline(buffer, 100, '#');

    //DestinationPath
    config.getline(buffer, 100, '#');
    for (i = 0; i < 100; i++)
    {
        if (buffer[i] == '\n' || buffer[i] == '\t' || buffer[i] == '\r' || buffer[i] == ' ') {
            buffer[i] = '\0';
            break;
        }
    }
    if (strcmp(buffer, "destinationpath") != 0) {
        return 0;
    }
    config.getline(buffer, 100, '%');
    config.getline(buffer, 100, '#');
    for (i = 0; i < 100 && buffer[i] != '$'; i++)
    {
        DestinationPath[i] = buffer[i];
    }
    DestinationPath[i] = '\0';
    config.getline(buffer, 100, '#');



    //Index
    config.getline(buffer, 100, '#');
    for (i = 0; i < 100; i++)
    {
        if (buffer[i] == '\n' || buffer[i] == '\t' || buffer[i] == '\r' || buffer[i] == ' ') {
            buffer[i] = '\0';
            break;
        }
    }
    if (strcmp(buffer, "index") != 0) {
        return 0;
    }
    for (i = 0; i < 400; i++)
    {
        config.getline(buffer, 100, '#');//读取;
        for (j = 0; j < strlen("indexend"); j++)
        {
            buffer2[j] = buffer[j];
        }
        buffer2[j] = '\0';
        if (strcmp(buffer2, "indexend") == 0)
        {
            break;
        }

        for (j = 0; buffer[j] != '%' && buffer[j] != '\t'; j++)//初始化
        {
            Index.Number[i][j] = buffer[j];
        }
        Index.Number[i][j] = '\0';
        if (buffer[j] != '%')for (; buffer[j] != '%'; j++);
        size_t jj = 0;
        for (j++; buffer[j] != '$'; j++)//初始化
        {
            Index.MusicName[i][jj++] = buffer[j];
        }
        Index.MusicName[i][jj] = '\0';
        Index.Num = i + 1;
    }


    config.close();
    return -1;
}
int ReadSource(void) {
    ifstream in(sourcenamepath, ios::in | ios::binary);
    char buffer[100];
    char buffer2[30];
    size_t i, j;

    if (!in.is_open()) {
        return 0;
    }
    in.getline(buffer, 100, '#');
    in.getline(buffer, 100, '#');

    for (i = 0; i < 400; i++) {
        in.getline(buffer, 100, '#');//读取;
        for (j = 0; j < strlen("sourceend"); j++)
        {
            buffer2[j] = buffer[j];
        }
        buffer2[j] = '\0';
        if (strcmp(buffer2, "sourceend") == 0)
        {
            break;
        }

        for (j = 0; j < 100 && buffer[j] != '\t' && buffer[j] != '\r'; j++)
        {
            Works.Name[i][j] = buffer[j];
        }
        Works.Name[i][j] = '\0';
        Works.Num = i + 1;
    }

    in.close();
    return -1;
}
int Start() {

    if (!Init()) {
        cout << "初始化失败" << endl;
        return 0;
    }
    if (!ReadSource()) {
        cout << "获取文件名失败" << endl;
        return 0;
    }
    cout << '\t' << "  ===============================================================" << endl;
    cout << '\t' << "||" << endl;
    cout << '\t' << "||" << '\t' << "(Ⅰ) 目标文件路径:  " << DestinationPath << endl << '\t' << "||" << endl;
    cout << '\t' << "||" << '\t' << "(Ⅱ) 源文件路径:    " << SourePath << endl << '\t' << "||" << endl;
    cout << '\t' << "||" << '\t' << "(Ⅲ) 源文件名称:" << endl << '\t' << "||" << endl;
    for (size_t i = 0; i < Works.Num; i++)
    {
        cout << '\t' << "||" << '\t' << '\t';
        cout.fill('0');
        cout.width(3);
        cout << i + 1 << ": " << Works.Name[i] << endl;
    }
    cout << '\t' << "||" << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << '\t' << "Totle: " << Works.Num << endl << '\t' << "||" << endl;
    cout << '\t' << "  ===============================================================" << endl;
    cout << '\t' << '\t' << '\t' << '\t' << "〖回车键开始♂〗" << endl;
    getchar();
    return -1;
}
void initbms(OneBMS& bms) {
    for (size_t i = 0; i < 33; i++)
    {
        bms.data[i] = '\0';
    }
    bms.Num = 0;
    bms.con[0] = '\0';
    bms.con[1] = '\0';
    bms.con[2] = '\0';
    bms.XiaoJie[0] = '\0';
    bms.XiaoJie[1] = '\0';
    bms.XiaoJie[2] = '\0';
    bms.XiaoJie[3] = '\0';

}
void JieXi(OneBMS& bms, char A[], size_t start, size_t num) {

    initbms(bms);
    bms.XiaoJie[0] = A[start + 0];
    bms.XiaoJie[1] = A[start + 1];
    bms.XiaoJie[2] = A[start + 2];
    bms.XiaoJie[3] = '\0';

    bms.con[0] = A[start + 3];
    bms.con[1] = A[start + 4];
    bms.con[2] = '\0';

    bms.Num = num - 6;
    for (size_t i = 6; i < num; i++)
    {
        bms.data[i - 6] = A[i];
    }
}
void initshuzu(char* A, size_t len) {
    for (size_t i = 0; i < len; i++)
    {
        A[i] = '\0';
    }
}
int getnewdata(char* buffer, size_t len, MAP& map) {    //返回0代表已结束
    size_t i;
    initshuzu(buffer, len);
    i = 0;
    while (map.point < map.length && map.sour[map.point] != '#')
    {
        buffer[i] = map.sour[map.point];
        i++;
        map.point++;
    }
    map.point++;//指向下一数据的头部

    if (map.point > map.length&& i == 0)
    {
        return 0;
    }
    return -1;
}
inline float JPMaker(OneBMS onebms, size_t pos) {

    return (float)(TransB(onebms.XiaoJie, 0, 3) * 4.0 + ((double)pos - 1) * 8.0 / (double)onebms.Num);

}
inline float PosMaker(char A[], size_t start, size_t num) {
    if (num == 1)
    {
        for (size_t i = 0; i < 7; i++)
        {

            if (is_equ(A, start, num, BMSAndScore.BMSTrack[i], 4, 1))
            {
                return TransB(BMSAndScore.ScoreTrack[i], 0, 2);
            }
        }

    }
    return 0;
}
void output(ofstream& out) {


    char a[256];



    sprintf_s(a, "/*\nMusic name:\t%s\n*/\n\n\n\0", map.music_name);
    out.write(a, strlen(a));

    sprintf_s(a, "#ifndef __musicscore_h__\n#define __musicscore_h__\n\n\n#define musiclength %d\n\nfloat musicscore[] = {\n\0", dest_num * 3);
    out.write(a, strlen(a));

    for (size_t i = 0; i < dest_num - 1; i++)
    {
        sprintf_s(a, "   %4.3f,  %4.3f,  %4.3f,\n\0", dest[i][0], dest[i][1], dest[i][2]);
        out.write(a, strlen(a));
    }

    sprintf_s(a, "   %4.3f,  %4.3f,  %4.3f\n\0", dest[dest_num-1][0], dest[dest_num - 1][1], dest[dest_num - 1][2]);
    out.write(a, strlen(a));

    sprintf_s(a, "};\n\n\n#endif\n\0");
    out.write(a, strlen(a));

}

int Working(ifstream& in, ofstream& out) {

    static OneBMS onebms;
    static char buffer1[1024];
    float a, b,c,d,e,f,g,h;
    size_t pp, qq;
    bool left_green_flag = false, right_green_flag = false;
    int ff;
    map.open(in);
    BUFFER.open(map);

    //初始化
    for (size_t i = 0; i < 1024 * 4; i++)
    {
        dest[i][0] = 0;
        dest[i][1] = 0;
        dest[i][2] = 0;
    }
    initshuzu(buffer1, 1024);
    dest_num = 0;

    dest[dest_num][0] = 0;
    dest[dest_num][1] = 5;
    dest[dest_num][2] = map.BPM;
    dest_num++;

    for (size_t i = 0; i < BUFFER.NUM; i++)
    {

        if (BUFFER.BUFFER2[i][0] == '1')
        {
       
            ff = map.is_green(BUFFER.BUFFER3[i], 0, 2);

            switch (ff)
            {
            case 0://普
                dest[dest_num][0] = BUFFER.BUFFER1[i];
                dest[dest_num][2] = PosMaker(BUFFER.BUFFER2[i], 1, 1);
                dest[dest_num][1] = map.TransA(BUFFER.BUFFER3[i], 0, 2, 0);
                dest_num++;
                break;
            case 10://fen
                dest[dest_num][0] = BUFFER.BUFFER1[i];
                dest[dest_num][2] = PosMaker(BUFFER.BUFFER2[i], 1, 1);
                dest[dest_num][1] = map.TransA(BUFFER.BUFFER3[i], 0, 2, 0);
                dest_num++;
                break;
            case 1://左
                if (!left_green_flag)
                {
                    left_green_flag = true;
                    dest[dest_num][0] = BUFFER.BUFFER1[i];
                    dest[dest_num][2] = PosMaker(BUFFER.BUFFER2[i], 1, 1);
                    dest[dest_num][1] = map.TransA(BUFFER.BUFFER3[i], 0, 2, 1);
                    dest_num++;
                }
                else
                {
                    dest[dest_num][0] = BUFFER.BUFFER1[i];
                    dest[dest_num][2] = PosMaker(BUFFER.BUFFER2[i], 1, 1);
                    dest[dest_num][1] = map.TransA(BUFFER.BUFFER3[i], 0, 2, 2);
                    dest_num++;
                }

                break;
            case 2://右
                if (!right_green_flag)
                {
                    right_green_flag = true;
                    dest[dest_num][0] = BUFFER.BUFFER1[i];
                    dest[dest_num][2] = PosMaker(BUFFER.BUFFER2[i], 1, 1);
                    dest[dest_num][1] = map.TransA(BUFFER.BUFFER3[i], 0, 2, 1);
                    dest_num++;
                }
                else
                {
                    dest[dest_num][0] = BUFFER.BUFFER1[i];
                    dest[dest_num][2] = PosMaker(BUFFER.BUFFER2[i], 1, 1);
                    dest[dest_num][1] = map.TransA(BUFFER.BUFFER3[i], 0, 2, 2);
                    dest_num++;
                }
                break; 
            case 3://左尾
                left_green_flag = false;
                dest[dest_num][0] = BUFFER.BUFFER1[i];
                dest[dest_num][2] = PosMaker(BUFFER.BUFFER2[i], 1, 1);
                dest[dest_num][1] = map.TransA(BUFFER.BUFFER3[i], 0, 2, 0);
                dest_num++;

                break;
            case 4://右尾
                right_green_flag = false;
                dest[dest_num][0] = BUFFER.BUFFER1[i];
                dest[dest_num][2] = PosMaker(BUFFER.BUFFER2[i], 1, 1);
                dest[dest_num][1] = map.TransA(BUFFER.BUFFER3[i], 0, 2, 0);
                dest_num++;
                break;
            default:
                break;
            }

            continue;
        }

        if (BUFFER.BUFFER2[i][0]=='5')
        {
            dest[dest_num][0] = BUFFER.BUFFER1[i];
            dest[dest_num][2] = PosMaker(BUFFER.BUFFER2[i], 1, 1);
            dest[dest_num][1] = TransB(BUFFER.BUFFER3[i], 0, 2);
            dest_num++;
            continue;
        }

        if (BUFFER.BUFFER2[i][0] == '0'&& BUFFER.BUFFER2[i][1] == '3')
        {
            dest[dest_num][0] = BUFFER.BUFFER1[i];
            dest[dest_num][1] = 5;
            dest[dest_num][2] = TransB_16(BUFFER.BUFFER3[i], 0, 2);
            dest_num++;
            continue;
        }

    }

    a = TranC("slide_a.wav", 0);
    b = TranC("slide_a.wav", 1);
    c = TranC("slide_end_a.wav", 0);
    d = TranC("slide_end_flick_a.wav", 0);

    e = TranC("slide_b.wav", 0);
    f = TranC("slide_b.wav", 1);
    g = TranC("slide_end_b.wav", 0);
    h = TranC("slide_end_flick_b.wav", 0);

    for (size_t i = 0; i < dest_num; i++)
    {
        if (dest[i][1] == a)
        {
            if (IsSingleOrDouble(i) == 11) {

                pp = i;
                qq = pp;
                pp = findnext(qq);
                dest[qq][1] = e;
                while (dest[pp][1] != c && dest[pp][1] != d)
                {
                    qq = pp;
                    pp = findnext(qq);
                    dest[qq][1] = f;
                }

                if (dest[pp][1] == c)
                {
                    dest[pp][1] = g;
                }
                else
                {
                    dest[pp][1] = h;
                }
            }
        }
        if (dest[i][1] == e)
        {
            if (IsSingleOrDouble(i) == 12) {

                pp = i;
                qq = pp;
                pp = findnext(qq);
                dest[qq][1] = a;
                while (dest[pp][1] != g && dest[pp][1] != h)
                {
                    qq = pp;
                    pp = findnext(qq);
                    dest[qq][1] = b;
                }

                if (dest[pp][1] == g)
                {
                    dest[pp][1] = c;
                }
                else
                {
                    dest[pp][1] = d;
                }
            }
        }
    }
    output(out);

    return -1;
}

int leftorright(size_t A, size_t B, size_t C) {
    float a;
    if (dest[C][1] != 5)
    {
        if (dest[C][0] == dest[A][0])
        {
            if (dest[C][2] > dest[A][2])
            {
                return 2;
            }
            else if (dest[C][2] < dest[A][2])
            {
                return 1;
            }
         
        }


        if (dest[C][0] == dest[B][0])
        {
            if (dest[C][2] > dest[B][2])
            {
                return 2;
            }
            else if (dest[C][2] < dest[B][2])
            {
                return 1;
            }
        }

        if (dest[C][0] > dest[A][0]&& dest[C][0] < dest[B][0])
        {
            if (dest[B][2] == dest[A][2]) {
                if (dest[C][2]>dest[A][2])
                {
                    return 2;
                }
                if (dest[C][2] < dest[A][2])
                {
                    return 1;
                }
            }

            if (dest[B][2] < dest[A][2])
            {
                a = (dest[A][2] - dest[B][2]) * (dest[B][0] - dest[C][0]) / (dest[B][0] - dest[A][0]);

                if (a>dest[C][2]- dest[B][2])
                {
                    return 1;
                }
                else
                {
                    return 2;
                }
            }
            if (dest[B][2] > dest[A][2])
            {
                a = (dest[B][2] - dest[A][2]) * (dest[B][0] - dest[C][0]) / (dest[B][0] - dest[A][0]);
                if (dest[B][2]-a+dest[A][2]>dest[C][2])
                {
                    return 1;
                }

                else
                {
                    return 2;
                }
                
            }

        }
    }

    return 0;

}
size_t findnext(size_t P) {
    float a, b, c, d, e, f, g, h;
    size_t i;
    h = TranC("slide_a.wav", 0);
    f = TranC("slide_a.wav", 1);
    a = TranC("slide_end_a.wav", 0);
    b = TranC("slide_end_flick_a.wav", 0);

    c = TranC("slide_b.wav", 0);
    g = TranC("slide_b.wav", 1);
    d = TranC("slide_end_b.wav", 0);
    e = TranC("slide_end_flick_b.wav", 0);


    if (dest[P][1] == h|| dest[P][1] == f)
    {
        i = P+1;
        while (dest[i][1] != f && dest[i][1] != a && dest[i][1] != b)i++;
        return i;
    }

    if (dest[P][1] == c || dest[P][1] == g)
    {
        i = P + 1;
        while (dest[i][1] != g && dest[i][1] != d && dest[i][1] != e)i++;
        return i;
    }
    return 0;
}
float TranC(const char A[], size_t a) {
    if (a==0)
    {
        for (size_t i = 0; i < FileAndKey.Num; i++)
        {
            if (strcmp(A, FileAndKey.FileName[i]) == 0)
            {
                return TransB(FileAndKey.Key[i], 0, 2);
            }

        }
    }
    else
    {
        for (size_t i = 0; i < FileAndKey.Num; i++)
        {
            if (strcmp(A, FileAndKey.FileName[i]) == 0)
            {
                return TransB(FileAndKey.Key[i], 3, 2);
            }

        }
    }
    return 0;

}
int IsSingleOrDouble(size_t pos) {
    //0不是绿，1是左绿，2是右绿  3代表左绿尾 4代表右绿尾 10fen  -1代表未找到

    bool leftgreen = false, rightgreen = false;
    size_t i, j, k;
    float a, b, c, d, e, f, g;
    a = 0;
    if (dest[pos][1]==TranC("slide_a.wav",0))
    {
        a = 1;
    }
    else if (dest[pos][1] == TranC("slide_b.wav",0))
    {
        a = 2;
    }
  
    if (a == 1)
    {
        f = TranC("slide_a.wav", 1);
        a = TranC("slide_end_a.wav", 0);   
        b = TranC("slide_end_flick_a.wav", 0);

        c = TranC("slide_b.wav", 0);
        g = TranC("slide_b.wav", 1);
        d = TranC("slide_end_b.wav", 0);
        e = TranC("slide_end_flick_b.wav", 0);

        for (i = 0; i < dest_num; i++)
        {
            if (dest[pos][0] == dest[i][0])
            {
                break;
            }
        }
        for (j = pos + 1; j < dest_num; j++)
        {
            if (dest[j][1] == a || dest[j][1] == b)
            {
                break;
            }
        }
        k = j;
        while (dest[j][0] <= dest[k][0])
        {
            j++;
        }
       
   
        k = pos;
        for (; i < j; i++)
        {
            if (dest[i][1] == c || dest[i][1] == g || dest[i][1] == d || dest[i][1] == e)
            {
                return 0;
            }
            if (k == i)
            {
                continue;
            }
            if (dest[i][1] == 5)
            {
                continue;
            }
            if (dest[i][1]==f)
            {
                k = i;
            }
            if (leftorright(k, findnext(k), i) == 1)
            {
                return 12;//右单绿
            }
            if (leftorright(k, findnext(k), i) == 2)
            {
                return 11;//左单绿
            }
        }
        

    }
    else if (a == 2)
    {

        f = TranC("slide_b.wav", 1);
        a = TranC("slide_end_b.wav", 0);
        b = TranC("slide_end_flick_b.wav", 0);

        c = TranC("slide_a.wav", 0);
        g = TranC("slide_a.wav", 1);
        d = TranC("slide_end_a.wav", 0);
        e = TranC("slide_end_flick_a.wav", 0);

        for (i = 0; i < dest_num; i++)
        {
            if (dest[pos][0] == dest[i][0])
            {
                break;
            }
        }
        for (j = pos + 1; j < dest_num; j++)
        {
            if (dest[j][1] == a || dest[j][1] == b)
            {
                break;
            }
        }
        k = j;
        while (dest[j][0] <= dest[k][0])
        {
            j++;
        }


        k = pos;
        for (; i < j; i++)
        {
            if (dest[i][1]== c|| dest[i][1] == g || dest[i][1] == d || dest[i][1] == e)
            {
                return 0;
            }
            if (k==i)
            {
                continue;
            }
            if (dest[i][1] == 5)
            {
                continue;
            }
            if (dest[i][1] == f)
            {
                k = i;
            }
            if (leftorright(k, findnext(k), i) == 1)
            {
                return 12;//右单绿
            }
            if (leftorright(k, findnext(k), i) == 2)
            {
                return 11;//左单绿
            }
        }




    }
    


    return 0;

}


void BuFFER::init() {
    for (size_t i = 0; i < 1024 * 4; i++)
    {
        BUFFER1[i] = 0;
        for (size_t j = 0; j < 3; j++)
        {
            BUFFER2[i][j] = '\0';
            BUFFER3[i][j] = '\0';
        }
    }
    NUM = 0;
}
int BuFFER::LeftOrRight(size_t pos) {   //1左2右

    size_t c;
    float b, d;
    b = PosMaker(BUFFER2[pos], 1, 1);
    for (c = 0; c < BUFFER.NUM; c++)
    {
        if (BUFFER1[c] == BUFFER1[pos])
        {
            break;
        }
    }
    
    for (size_t i = c; i < BUFFER.NUM; i++)
    {
        if (i == pos)
        {
            continue;
        }
        if (BUFFER2[i][0]=='0'&& BUFFER2[i][1] == '3')
        {
            continue;
        }
        d = PosMaker(BUFFER2[i], 1, 1);
        if (d==b)
        {
            if (BUFFER1[i]==BUFFER1[i+1])
            {
                d= PosMaker(BUFFER2[i+1], 1, 1);
                if (b > d)
                {
                    return 2;
                }
                else
                {
                    return 1;
                }
            }
            else if (BUFFER1[i] == BUFFER1[i + 2])
            {
                d = PosMaker(BUFFER2[i + 2], 1, 1);
                if (b > d)
                {
                    return 2;
                }
                else
                {
                    return 1;
                }
            }
            return 1;
        }
        if (b>d)
        {
            return 2;
        }
        else
        {
            return 1;
        }     
    }

    return 0;

}
size_t BuFFER::findend(size_t pos) {
    size_t c;
    float b, d;
    b = PosMaker(BUFFER2[pos], 1, 1);
    for (c = 0; c < BUFFER.NUM; c++)
    {
        if (BUFFER1[c] == BUFFER1[pos])
        {
            break;
        }
    }


    for (size_t i = c; i < BUFFER.NUM; i++)
    {
        if (i == pos)
        {
            continue;
        }
        if (BUFFER2[i][0] == '0' && BUFFER2[i][1] == '3')
        {
            continue;
        }
        d = PosMaker(BUFFER2[i], 1, 1);

        if (b==d)
        {
            return i;
        }

    }
    return 0;

}
void BuFFER::open(MAP& map) {
    static char buffer1[1024];
    static uchar flag[1024 * 4];
    char lgs[3],lge[3];
    char rgs[3], rge[3];
    char lfli[3], rfli[3];
    size_t bb, j;
    AAA(lgs, "slide_b.wav");
    AAA(lge, "slide_end_b.wav");
    AAA(rgs, "slide_a.wav");
    AAA(rge, "slide_end_a.wav");
    AAA(lfli, "slide_end_flick_b.wav");
    AAA(rfli, "slide_end_flick_a.wav");
    OneBMS bms;
    init();
    initbms(bms);

    while (getnewdata(buffer1, 1024, map)) {
        for (size_t i = 0; i < strlen(buffer1); i++)
        {
            if (buffer1[i] == ' ' || buffer1[i] == '\r' || buffer1[i] == '\t' || buffer1[i] == '\n') {
                buffer1[i] = '\0';
                break;
            }
        }
        JieXi(bms, buffer1, 0, strlen(buffer1));

        for (size_t i = 0; i < bms.Num / 2; i++)
        {
            if (bms.con[0] == '0' && bms.con[1] == '3')
            {
                BUFFER1[NUM] = JPMaker(bms, i + 1);
                strcpy_s(BUFFER2[NUM], bms.con);
                BUFFER3[NUM][0] = bms.data[2 * i];
                BUFFER3[NUM][1] = bms.data[2 * i + 1];
                BUFFER3[NUM][2] = '\0';
                NUM++;


                continue;
            }
            if (bms.data[2 * i] == '0' && bms.data[2 * i + 1] == '0')
            {
                continue;
            }
            if (map.is_green(bms.data, 2 * i, 2) == -1)
            {
                continue;
            }



            BUFFER1[NUM] = JPMaker(bms, i + 1);
            strcpy_s(BUFFER2[NUM], bms.con);
            BUFFER3[NUM][0] = bms.data[2 * i];
            BUFFER3[NUM][1] = bms.data[2 * i + 1];
            BUFFER3[NUM][2] = '\0';
            NUM++;

     


        }

    }

    //排序
    for (size_t i = 0; i < NUM; i++)
    {
        for (size_t j = 0; j < NUM-1-i; j++)
        {
            if (BUFFER1[j]>BUFFER1[j+1])
            {
                exchange(j, j + 1);
            }
        }
    }
 
    
    //优化变速键位置
    for (size_t i = 0; i < NUM; i++)
    {
        if (BUFFER2[i][0] == '0' && BUFFER2[i][1] == '3') 
        {
            for ( j = 0; j < NUM; j++)
            {
                if (BUFFER1[j]==BUFFER1[i])
                {
                    break;
                }
            }
            if (i!=j)
            {
                exchange(i, j);
            }
        
        }


    }

    //替换长直绿条
    for (size_t i = 0; i < 1024*4; i++)
    {
        flag[i] = 0;
    }
    for (size_t i = 0; i < NUM; i++)
    {
        if (BUFFER2[i][0] == '5')
        {
            if (LeftOrRight(i) == 1 && flag[i] == 0)
            {
  
                BUFFER3[i][0] = lgs[0];
                BUFFER3[i][1] = lgs[1];
                BUFFER3[i][2] = '\0';
                bb = findend(i);

                if (map.is_green(BUFFER3[bb], 0, 2) == 10)
                {

                    BUFFER3[bb][0] = lfli[0];
                    BUFFER3[bb][1] = lfli[1];
                    BUFFER3[bb][2] = '\0';

                }
                else
                {
                    BUFFER3[bb][0] = lge[0];
                    BUFFER3[bb][1] = lge[1];
                    BUFFER3[bb][2] = '\0';
                }
                flag[i] = 1;
                flag[bb] = 1;

            }

            else if (LeftOrRight(i) == 2 && flag[i] == 0)
            {

                BUFFER3[i][0] = rgs[0];
                BUFFER3[i][1] = rgs[1];
                BUFFER3[i][2] = '\0';
                bb = findend(i);


                if (map.is_green(BUFFER3[bb], 0, 2) == 10)
                {

                    BUFFER3[bb][0] = rfli[0];
                    BUFFER3[bb][1] = rfli[1];
                    BUFFER3[bb][2] = '\0';

                }
                else
                {
                    BUFFER3[bb][0] = rge[0];
                    BUFFER3[bb][1] = rge[1];
                    BUFFER3[bb][2] = '\0';
                }

                flag[i] = 1;
                flag[bb] = 1;

            }

        }
    }

}
void BuFFER::exchange(size_t A, size_t B) {
    char a;
    float b;

    b = BUFFER1[B];
    BUFFER1[B] = BUFFER1[A];
    BUFFER1[A] = b;

    for (size_t i = 0; i < 3; i++)
    {
        a = BUFFER2[B][i];
        BUFFER2[B][i] = BUFFER2[A][i];
        BUFFER2[A][i] = a;
    }

    for (size_t i = 0; i < 3; i++)
    {
        a = BUFFER3[B][i];
        BUFFER3[B][i] = BUFFER3[A][i];
        BUFFER3[A][i] = a;
    }

}

void AAA(char* A, const char B[]) {

    for (size_t i = 0; i < FileAndKey.Num; i++)
    {
        if (strcmp(B, FileAndKey.FileName[i]) == 0)
        {
            A[0] = FileAndKey.Key[i][0];
            A[1] = FileAndKey.Key[i][1];
            A[2] = '\0';
        }
    }

}
int MAP::is_green(char sou[], size_t start, size_t num) {
    for (size_t i = 0; i < MapNum; i++)
    {
        if (is_equ(sou, start, num, A[i], 0, 2))
        {
            return Aflag[i];
        }
    }
    return -1;
} //0不是绿，1是左绿，2是右绿  3代表左绿尾 4代表右绿尾 10fen  -1代表未找到;
float MAP::TransA(char sou[], size_t start, size_t num, uchar flag)    //按键映射  0普通，1绿头，2绿中,5代表未找到
{
    if (flag != 2) {
        for (size_t i = 0; i < MapNum; i++)
        {
            if (is_equ(sou, start, num, A[i], 0, 2))
            {
                return B[i];
            }
        }
    }
    else
    {
        for (size_t i = 0; i < MapNum; i++)
        {
            if (is_equ(sou, start, num, A[i], 0, 2))
            {
                return B[i + 1];
            }
        }
    }
    return 5.0;
}
void MAP::open(ifstream& in)
{
    size_t L = 0, P = 0;
    char buffer[100];
    float a, b;
    bool flag;
    init();
    //读
    in.seekg(0, in.end);
    length = (size_t)in.tellg();
    in.seekg(0, in.beg);
    in.read(sour, length);
    sour[length] = '\0';
    in.seekg(0, in.beg);
    //BPM
    flag = true;
    while (flag)
    {
        in.getline(buffer, 100, '#');
        if (buffer[0] == 'B' && buffer[1] == 'P' && buffer[2] == 'M') {
            flag = false;
            _BPM[0] = buffer[4];
            _BPM[1] = buffer[5];
            if (buffer[4] > '1') {
                BPMNum = 2;
            }
            else
            {
                BPMNum = 3;
                _BPM[2] = buffer[6];
            }
        }
    }
    BPM = TransB(_BPM, 0, BPMNum);

    //name
    flag = true;
    while (flag)
    {
        in.getline(buffer, 100, '#');
        if (buffer[6] == 'b' && buffer[7] == 'g' && buffer[8] == 'm') {
            flag = false;
            for (size_t i = 0; i < Index.Num; i++)
            {
                a = TransB(buffer, 9, 3);
                b = TransB(Index.Number[i], 0, 3);
                if (a == b) {
                    strcpy_s(music_name, Index.MusicName[i]);
                    music_name[strlen(Index.MusicName[i])] = '\0';
                    break;
                }
            }

        }

    }

    //map
    flag = true;
    P = 0;
    while (flag)
    {
        in.getline(buffer, 100, '#');
        for (size_t i = 6; buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\t' && buffer[i] != '\r'; i++) {
            L = i + 1;
        }
        buffer[L] = '\0';

        if (buffer[0] == 'W') {
            for (size_t i = 0; i < FileAndKey.Num; i++)
            {

                if (L - 6 == strlen(FileAndKey.FileName[i]))
                {
                    if (is_equ(FileAndKey.FileName[i], 0, L - 6, buffer, 6, L - 6)) //文件名相等
                    {
                        A[P][0] = buffer[3];
                        A[P][1] = buffer[4];
                        A[P][2] = '\0';

                        if (strcmp(FileAndKey.FileName[i], "slide_a.wav") == 0 || strcmp(FileAndKey.FileName[i], "fever_slide_a.wav") == 0) Aflag[P] = 2;
                        else if (strcmp(FileAndKey.FileName[i], "slide_b.wav") == 0 || strcmp(FileAndKey.FileName[i], "fever_slide_b.wav") == 0) Aflag[P] = 1;
                        else if (strcmp(FileAndKey.FileName[i], "slide_end_a.wav") == 0 || strcmp(FileAndKey.FileName[i], "fever_slide_end_a.wav") == 0) Aflag[P] = 4;
                        else if (strcmp(FileAndKey.FileName[i], "slide_end_flick_a.wav") == 0 || strcmp(FileAndKey.FileName[i], "fever_slide_end_flick_a.wav") == 0) Aflag[P] = 4;
                        else if (strcmp(FileAndKey.FileName[i], "slide_end_b.wav") == 0 || strcmp(FileAndKey.FileName[i], "slide_end_flick_b.wav") == 0) Aflag[P] = 3;
                        else if (strcmp(FileAndKey.FileName[i], "fever_slide_end_b.wav") == 0 || strcmp(FileAndKey.FileName[i], "fever_slide_end_flick_b.wav") == 0) Aflag[P] = 3;
                        else if (strcmp(FileAndKey.FileName[i], "flick.wav") == 0 || strcmp(FileAndKey.FileName[i], "fever_note_flick.wav") == 0) Aflag[P] = 10;
                        else Aflag[P] = 0;

                        B[P] = TransB(FileAndKey.Key[i], 0, 2);
                        MapNum = ++P;

                        if (strlen(FileAndKey.Key[i]) > 3) {//11%12这种情况就存两个，到时候再根据上下文选
                            A[P][0] = buffer[3];
                            A[P][1] = buffer[4];
                            A[P][2] = '\0';
                            B[P] = TransB(FileAndKey.Key[i], 3, 2);
                            Aflag[P] = 0;
                            MapNum = ++P;
                        }


                    }
                }

            }
        }
        else {
            in.seekg(0, in.beg);
            flag = false;
        }
    }


    //point ,指向数据段第一个#的下一个
    for (size_t i = 0; i < length; i++)
    {
        if (sour[i] == '#' && sour[i + 1] == '0' && sour[i + 2] == '0') {
            point = i + 1;
            break;
        }
    }

}
void MAP::init() {
    for (size_t i = 0; i < 30; i++)
    {
        A[i][0] = '0';
        A[i][1] = '0';
        A[i][2] = '0';
        Aflag[i] = '0';
        B[i] = 0;
        music_name[i] = '\0';
    }
    for (size_t i = 0; i < 1024 * 50; i++)
    {
        sour[i] = '\0';
    }
    BPMNum = 0;
    MapNum = 0;
    BPM = 100;
    length = 0;
    point = 0;

}

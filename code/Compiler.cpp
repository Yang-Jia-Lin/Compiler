#include <bits/stdc++.h>
using namespace std;

const int MaxSize = 1007;
typedef struct {
    int syn;              // 1关键字，2标识符ID，3数值NUM，4算符OP，5界符RANGE，6赋值号ASSIGN
    char value[MaxSize];  // 1为自身，2为ID，3为NUM，4为自身，5为自身，6为自身
    int value1;           // 1为-1，2为表格序号，3为值，4为-1，5为-1，6为-1
    int row;
    int col;
    char c;
} word;

typedef struct {
    char ch;
    int ID1;
    int ID2;
    int ID3;
} List;

// ID表格管理（每个ID有唯一序号）
map<string, int> ID;
int ID_num = 0;

// 词法分析序列
word words[MaxSize];
int word_num = 0;

// 语义分析序列
List list[MaxSize];
int list_num = 0;

class Lexical {
   private:
    char* rwtab[6] = {"int", "void", "if", "else", "string", "return"};    // 关键字表
    char* type_num[7] = {" ", " ", "ID", "NUM", "OP", "RANGE", "ASSIGN"};  // 种别表
    char ch;
    char chStream[MaxSize];  // 缓存输入的字符流
    char strToken[MaxSize];  // 字符数组，存放构成单词符号的字符串
    int p, syn, sum;         // 搜索指针、种别码、整型常数
    int col = 1, row = 1;    // 当前行列号
    int len = 0;

   public:
    Lexical(string filename) {
        col = 1, row = 1;  // 清空行列号
        len = 0;

        // 打开字符串文件（参数）读入
        ifstream in(filename);
        if (!in.is_open()) {
            cout << "文件打开失败" << endl;
            exit(1);
        }
        in.getline(chStream, MaxSize, '#');
        in.clear();
        in.close();

        // 词法分析+建立表格
        p = 0;
        do {
            this->solve(&p);
            int r = row, c = col;
            word w;

            switch (syn) {
                // 出错
                case -1:
                    printf("Error!\t(%d, %d)\n", r, c);
                    break;
                // 检测到关键字
                case 1:
                    c = c - len;
                    strcpy(w.value, strToken);
                    w.syn = syn;
                    w.value1 = -1;
                    w.col = c;
                    w.row = r;
                    w.c = strToken[0];
                    words[word_num++] = w;
                    printf("<type:%s\tvalue:%s\t  (row:%d, col:%d)>\n", strToken, strToken, r, c);
                    break;

                // 检测到标识符
                case 2:
                    //cout << "<type:" << type_num[syn] << "\tvalue:" << sum << "\t(row:"<<r<<", col:"<<c<<") >" << endl;
                    c = c - len;
                    // 管理ID表格，新增表项或者返回原有项
                    int value;
                    if (ID.count(strToken))
                        value = ID[strToken];
                    else {
                        value = ID_num;
                        ID[strToken] = ID_num++;
                    }
                    // 管理词法分析结果
                    //w = {2, "ID\0", value, r, c};
                    strcpy(w.value, type_num[2]);
                    w.syn = syn;
                    w.value1 = ID_num;
                    w.col = c;
                    w.row = r;
                    w.c = 'd';
                    words[word_num++] = w;
                    printf("<type:%s\tvalue:%s\t  (row:%d, col:%d)>\n", type_num[2], strToken, r, c);
                    break;

                // 检测到数值
                case 3:
                    //w = {3, "num\0", sum, r, c};
                    strcpy(w.value, type_num[3]);
                    w.syn = syn;
                    w.value1 = sum;
                    w.col = c;
                    w.row = r;
                    w.c = 'd';
                    words[word_num++] = w;
                    printf("<type:%s\tvalue:%d\t  (row:%d, col:%d)>\n", type_num[3], sum, r, c);
                    break;
                default:
                    c = c - len;
                    //w = {syn, strToken, -1, r, c};
                    strcpy(w.value, strToken);
                    w.syn = syn;
                    w.value1 = -1;
                    w.col = c;
                    w.row = r;
                    w.c = strToken[0];
                    words[word_num++] = w;
                    printf("<type:%s\tvalue:%s\t  (row:%d, col:%d)>\n", type_num[syn], strToken, r, c);
                    break;
            }
        } while (p < (int)strlen(chStream) && syn != 0);
    }

    // 子程序过程，把下一个字符读入ch中
    char GetChar(int* p) {
        col++;
        return chStream[(*p)++];
    }

    // 跳过无效字符，直至读入一个有效字符
    //（注意吃进空格必须是最后一步）
    char GetBC(int* p) {
        // 跳过空格，换行，制表符
        while (chStream[*p] == ' ' || chStream[*p] == '\n' || chStream[*p] == '\t') {
            if (chStream[*p] == '\n') {
                row++;
                col = 1;
            } else
                col++;

            (*p)++;
        }

        // 跳过单行注释
        while (chStream[*p] == '/' && chStream[(*p) + 1] == '/') {
            (*p)++;
            while (chStream[(*p)++] != '\n')
                ;
            col = 1;
            row++;
        }

        // 跳过多行注释
        while (chStream[*p] == '/' && chStream[(*p) + 1] == '*') {
            (*p) += 2;
            col += 2;
            // 吃进多行注释中间所有字符，注意换行
            while (chStream[(*p)] != '*' || chStream[(*p) + 1] != '/') {
                (*p)++;
                if (chStream[(*p)] == '\n') {
                    col = 1;
                    row++;
                } else
                    col++;
            }
            (*p) += 2;
            col += 2;
        }

        // 跳过空格，换行，制表符
        while (chStream[*p] == ' ' || chStream[*p] == '\n' || chStream[*p] == '\t') {
            if (chStream[*p] == '\n') {
                row++;
                col = 1;
            } else
                col++;
            (*p)++;
        }
        return chStream[(*p)++];
    }

    // 判断是否为字母
    int IsLetter(char c) {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            return 1;
        }
        return 0;
    }

    // 判断是否为数字
    int IsDigit(char c) {
        if (c >= '0' && c <= '9') {
            return 1;
        }
        return 0;
    }

    // 搜索指针回调一个字符位置
    void Retract(int* p) {
        (*p)--;
        //col--;
    }

    // 判断符号（DFA）
    void solve(int* p) {
        memset(strToken, 0, sizeof(strToken));
        len = 0;
        ch = this->GetBC(p);  // 跳过空白符，直至读入一个非空字符

        // 第一个是字母，则可能是关键字也可能是标识符
        if (this->IsLetter(ch)) {
            int idx = 0;
            while (this->IsLetter(ch) || this->IsDigit(ch)) {
                strToken[idx++] = ch;
                ch = this->GetChar(p);
                len++;
            }
            Retract(p);  // 搜索指针回调一个字符位置

            // 先默认是标识符，接下来再判断是否为关键字
            syn = 2;
            for (int i = 0; i < 6; i++) {
                if (strcmp(strToken, rwtab[i]) == 0) {
                    syn = 1;  // 关键字
                    len = strlen(rwtab[i]);
                    break;
                }
            }
        }

        // 第一个是数字——常数
        else if (this->IsDigit(ch)) {
            sum = 0;
            while (this->IsDigit(ch)) {
                sum = sum * 10 + (ch - '0');
                ch = this->GetChar(p);
                len++;
            }
            this->Retract(p);
            syn = 3;
        }

        // 运算符
        else {
            col++;
            len++;
            switch (ch) {
                case '+':
                    syn = 4;
                    strToken[0] = ch;
                    break;
                case '-':
                    syn = 4;
                    strToken[0] = ch;
                    break;
                case '*':
                    syn = 4;
                    strToken[0] = ch;
                    break;
                case '/':
                    syn = 4;
                    strToken[0] = ch;
                    break;
                case '<':
                    syn = 4;
                    strToken[0] = ch;
                    ch = this->GetChar(p);
                    if (ch == '=') {
                        strToken[1] = ch;
                        len++;
                        syn = 4;
                    } else {
                        this->Retract(p);
                    }
                    break;
                case '>':
                    syn = 4;
                    strToken[0] = ch;
                    ch = this->GetChar(p);
                    if (ch == '=') {
                        strToken[1] = ch;
                        len++;
                        syn = 4;
                    } else {
                        this->Retract(p);
                    }
                    break;
                case '=':
                    syn = 6;
                    strToken[0] = ch;
                    ch = this->GetChar(p);
                    if (ch == '=') {
                        strToken[1] = ch;
                        len++;
                        syn = 4;
                    } else {
                        this->Retract(p);
                    }
                    break;
                case ';':
                    syn = 5;
                    strToken[0] = ch;
                    break;
                case '(':
                    syn = 5;
                    strToken[0] = ch;
                    break;
                case ')':
                    syn = 5;
                    strToken[0] = ch;
                    break;
                case '{':
                    syn = 5;
                    strToken[0] = ch;
                    break;
                case '}':
                    syn = 5;
                    strToken[0] = ch;
                    break;
                case ',':
                    syn = 5;
                    strToken[0] = ch;
                    break;
                case '#':
                    syn = 5;
                    strToken[0] = ch;
                    break;
                default:
                    syn = -1;
                    break;
            }
        }
    }
};

class Grammar {
   private:
    set<char> Vn;               // 非终结符
    set<char> Vt;               // 终结符
    char S;                     // 开始符号
    map<char, set<string> > P;  // 产生式表（一个非终结符对应多个字符串产生式）

    map<char, set<char> > FIRST;   // 求解出的first集
    map<char, set<char> > FOLLOW;  // 求解出的follow集
    map<string, string> Table;     // 构造出的LL(1)文法分析表
    stack<char> p;

   public:
    // 初始化
    Grammar(string filename) {
        Vn.clear();
        Vt.clear();
        P.clear();
        FIRST.clear();
        FOLLOW.clear();

        // 打开保存文法的文件（作为参数传入）
        ifstream in2(filename);
        if (!in2.is_open()) {
            cout << "文件打开失败" << endl;
            exit(1);
        }
        char* buffer = new char[MaxSize];
        in2.getline(buffer, MaxSize, '#');
        in2.clear();
        in2.close();

        // 读取文法
        string temps = "";
        bool is_sethead = 0;
        for (int i = 0; i < (int)strlen(buffer); i++) {
            if (buffer[i] == ' ')
                continue;
            if (buffer[i] == '\n') {
                if (!is_sethead) {
                    // 设置开始符号
                    this->setHead(temps[0]);
                    is_sethead = 1;
                }
                // 添加一条产生式
                this->add(temps);
                temps = "";
            } else
                temps += buffer[i];
        }
        delete buffer;
    }

    // (初始化)设置开始符号
    void setHead(char c) {
        S = c;
    }

    // (初始化)添加一条产生式（形如 E->T|E+T; ）
    void add(string s) {
        char s1 = s[0];
        string s2 = "";

        // 扫描整个产生式，用num将产生式分割为s1,s2左右两个部分
        int num = 0;
        for (int i = 0; i < (int)s.length(); i++) {
            if (s[i] == '>')
                num = i;
            if (num == 0)
                continue;
            if (i > num)
                s2 += s[i];
        }
        s2 += '\n';

        // 左部s1仅是一个非终结符
        Vn.insert(s1);

        // 右部s2存在非终结符，并且对应s1的多个产生式均加入p
        string temp = "";
        for (char s : s2) {
            if (!isupper(s) && s != '|' && s != '\n' && s != '@')
                Vt.insert(s);
            if (s == '|' || s == '\n') {
                P[s1].insert(temp);
                temp = "";
            } else {
                temp += s;
            }
        }
    }

    // 输出文法（展开形式）（形如 E->T; ）
    void print() {
        cout << "当前分析文法为：" << endl
             << endl;
        // 遍历每一个非终结符的每一个产生式集合，输出文法
        for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
            char cur_s = *it;
            for (set<string>::iterator it1 = P[cur_s].begin(); it1 != P[cur_s].end(); it1++) {
                string cur_string = *it1;
                cout << cur_s << "->" << cur_string << endl;
            }
        }
    }

    // First集合
    void getFirst() {
        FIRST.clear();
        // A->aβ	a加到A的FIRST集				cur_s->cur_string[0]
        // A->Bβ	B的FITRST集加到A的FIRST集 	cur_s->cur_string[0]

        // 判断迭代次数
        int iter = 4;
        while (iter--) {
            for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
                char cur_s = *it;  // 每一个非终结符
                // 对应的所有产生式
                for (set<string>::iterator it1 = P[cur_s].begin(); it1 != P[cur_s].end(); it1++) {
                    string cur_string = *it1;
                    // A->aβ
                    if (!isupper(cur_string[0]))
                        FIRST[cur_s].insert(cur_string[0]);
                    // A->Bβ
                    else {
                        char nxt_s = cur_string[0];
                        for (set<char>::iterator it2 = FIRST[nxt_s].begin(); it2 != FIRST[nxt_s].end(); it2++)
                            if ((*it2) != '@')
                                FIRST[cur_s].insert(*it2);
                    }
                }
            }
        }

        //输出FIRST集
        cout << "·FIRST集：" << endl;
        for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
            char cur_s = *it;
            cout << "FIRST(" << cur_s << ")  ";
            for (set<char>::iterator it1 = FIRST[cur_s].begin(); it1 != FIRST[cur_s].end(); it1++)
                cout << "\t" << *it1;
            cout << endl;
        }
    }

    // Follow集合
    void getFollow() {
        // 开始符号的follow集一定有'#'
        FOLLOW.clear();
        FOLLOW[S].insert('#');
        // B->Ac	将c加到A的follow集
        // B->AC	将C的first集加到A的follow集
        // B->ACK(K的first集含有@)		将B的follow集加入到C的follow集

        //判断迭代次数
        int iter = 4;
        while (iter--) {
            for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
                char cur_s = *it;
                for (set<string>::iterator it1 = P[cur_s].begin(); it1 != P[cur_s].end(); it1++) {
                    string cur_string = *it1;
                    for (int i = 0; i < (int)cur_string.length() - 1; i++) {
                        // B->Ac
                        if (isupper(cur_string[i]) && !isupper(cur_string[i + 1]))
                            FOLLOW[cur_string[i]].insert(cur_string[i + 1]);

                        // B->AC (遍历C的first去除@，加到A的follow集)
                        if (isupper(cur_string[i]) && isupper(cur_string[i + 1]))
                            for (auto it2 = FIRST[cur_string[i + 1]].begin(); it2 != FIRST[cur_string[i + 1]].end(); it2++)
                                if ((*it2) != '@')
                                    FOLLOW[cur_string[i]].insert(*it2);
                    }

                    // B->ACK(K的first集含有@)
                    int len = cur_string.length();
                    if ((len >= 1 && isupper(cur_string[len - 1]))) {
                        for (auto it2 = FOLLOW[cur_s].begin(); it2 != FOLLOW[cur_s].end(); it2++)
                            if (isupper(cur_string[len - 1]))
                                FOLLOW[cur_string[len - 1]].insert(*it2);
                    }
                    if ((len >= 2 && isupper(cur_string[len - 2]) && isupper(cur_string[len - 1]) && FIRST[cur_string[len - 1]].count('@') > 0))
                        for (auto it2 = FOLLOW[cur_s].begin(); it2 != FOLLOW[cur_s].end(); it2++)
                            FOLLOW[cur_string[len - 2]].insert(*it2);
                }
            }
        }
        //输出FOLLOW集
        cout << "·FOLLOW集：" << endl;
        for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
            char cur_s = *it;
            cout << "FOLLOW(" << cur_s << ")  ";
            for (set<char>::iterator it1 = FOLLOW[cur_s].begin(); it1 != FOLLOW[cur_s].end(); it1++)
                cout << "\t" << *it1;
            cout << endl;
        }
    }

    // LL(1)分析表
    void getTable() {
        set<char> Vt_temp;
        for (char c : Vt)
            Vt_temp.insert(c);
        Vt_temp.insert('#');

        // 对于每一条产生式 cur_s->cur_string 填表
        for (auto it = Vn.begin(); it != Vn.end(); it++) {
            char cur_s = *it;
            for (auto it1 = P[cur_s].begin(); it1 != P[cur_s].end(); it1++) {
                string cur_string = *it1;
                if (isupper(cur_string[0])) {
                    char first_s = cur_string[0];
                    for (auto it2 = FIRST[first_s].begin(); it2 != FIRST[first_s].end(); it2++) {
                        string TableLeft = "";
                        TableLeft = TableLeft + cur_s + *it2;
                        Table[TableLeft] = cur_string;
                    }
                } else {
                    string TableLeft = "";
                    TableLeft = TableLeft + cur_s + cur_string[0];
                    Table[TableLeft] = cur_string;
                }
            }
            if (FIRST[cur_s].count('@') > 0) {
                for (auto it1 = FOLLOW[cur_s].begin(); it1 != FOLLOW[cur_s].end(); it1++) {
                    string TableLeft = "";
                    TableLeft = TableLeft + cur_s + *it1;
                    Table[TableLeft] = "@";
                }
            }
        }

        //出错信息（即表格中没有出现过的）
        for (auto it = Vn.begin(); it != Vn.end(); it++) {
            for (auto it1 = Vt_temp.begin(); it1 != Vt_temp.end(); it1++) {
                string TableLeft = "";
                TableLeft = TableLeft + *it + *it1;
                if (!Table.count(TableLeft)) {
                    Table[TableLeft] = "error";
                }
            }
        }

        //输出分析表
        cout << "LL(1)分析表：" << endl;
        cout.setf(std::ios::left);
        for (auto it1 = Vt_temp.begin(); it1 != Vt_temp.end(); it1++) {
            if (it1 == Vt_temp.begin())
                cout << setw(10) << " ";
            cout << setw(10) << *it1;
        }
        cout << endl;
        for (auto it = Vn.begin(); it != Vn.end(); it++) {
            for (auto it1 = Vt_temp.begin(); it1 != Vt_temp.end(); it1++) {
                string TableLeft = "";
                if (it1 == Vt_temp.begin())
                    cout << setw(10) << *it;
                TableLeft = TableLeft + *it + *it1;
                cout << *it << "->" << setw(7) << Table[TableLeft];
            }
            cout << endl;
        }
    }

    // 语法分析（简易版）
    bool AnalyzePredict(string inputstring) {
        // 每次分析一个输入串
        // Sign为符号栈，出栈字符为x，输入字符串当前字符为a
        stack<char> Sign;
        Sign.push('#');
        Sign.push(S);
        int StringPtr = 0;
        char a = inputstring[StringPtr++];
        bool flag = true;
        while (flag) {
            char x = Sign.top();
            Sign.pop();

            // 如果是终结符,直接移出符号栈
            if (Vt.count(x)) {
                if (x == a)
                    a = inputstring[StringPtr++];
                else
                    return false;
            }

            // 如果不是终结符，
            else {
                //是末尾符号
                if (x == '#') {
                    if (x == a)
                        flag = false;
                    else
                        return false;
                }
                // 是其他非终结符，需要移进
                else {
                    string left = "";
                    left += x;
                    left += a;
                    if (Table[left] != "error") {
                        string right = Table[left];
                        for (int i = right.length() - 1; i >= 0; i--)
                            Sign.push(right[i]);
                    } else
                        return false;
                }
            }
        }
        return true;
    }

    // 语法分析（分析过程符号表+堆栈）
    void AnalyzePredict1(string filename_string) {
        // 读入待分析字符串到string inputstring中
        ifstream in3(filename_string);
        if (!in3.is_open()) {
            cout << "文件打开失败" << endl;
            exit(1);
        }
        char* buff = new char[MaxSize];
        in3.getline(buff, MaxSize, '#');
        in3.clear();
        in3.close();
        char tmp[MaxSize];
        strcpy(tmp, buff);
        string input = tmp;

        // 分析过程
        cout << "匹配过程如下:" << endl;
        cout << "步骤"
             << "\t输入缓冲区"
             << "\t\t栈"
             << "\t\t输出" << endl;
        cout << '0' << "\t" << input << "\t\t";
        p.push('#');
        p.push(S);
        this->printstack();
        cout << endl;

        int i = 0;
        for (int j = 1; i < (int)input.length(); j++) {
            char v = p.top();
            p.pop();
            // 栈顶元素与待分析元素相同，消去
            if (v == input[i]) {
                cout << j << "\t";
                this->printinput(input, i);
                cout << "\t\t";
                this->printstack();
                cout << "\t\t" << input[i] << "匹配"
                     << "\t\t" << endl;
                i++;
                continue;
            }

            // 不相同，使用预测表分析
            else {
                // 寻找非终结符，位置为m
                int m = -1;
                int k = 0;
                for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++, k++) {
                    char cur_s = *it;
                    if (v == cur_s) {
                        m = k;
                        break;
                    }
                }

                // 没有该非终结符，错误，break
                if (m == -1) {
                    cout << "错误" << endl;
                    cout << "第" << i << "个字符";
                    printf("%c", input[i]);
                    cout << "匹配错误" << endl;
                    cout << input << "为非法符号串" << endl;
                    break;
                }

                // 使用分析表进行分析
                string TableLeft = "";
                TableLeft = TableLeft + v + input[i];
                if (Table[TableLeft] == "error") {
                    cout << "第" << i << "个字符";
                    printf("%c", input[i]);
                    cout << "匹配错误" << endl;
                    cout << input << "为非法符号串" << endl;
                    break;
                }

                if (Table[TableLeft] == "@") {
                    cout << j << "\t";
                    printinput(input, i);
                    cout << "\t\t";
                    printstack();
                    cout << "\t\t" << v << "->"
                         << "ε" << endl;
                    continue;
                } else {
                    string t = "";
                    for (int k = Table[TableLeft].length() - 1; k >= 0; k--) {
                        t += Table[TableLeft][k];
                        p.push(Table[TableLeft][k]);
                    }
                    cout << j << "\t";
                    this->printinput(input, i);
                    cout << "\t\t";
                    this->printstack();
                    cout << "\t\t" << v << "->" << Table[TableLeft];
                }
            }
            cout << endl;
        }
        if (i == (int)input.length())
            cout << input << "为合法符号串" << endl;
    }

    // 语法分析（使用词法分析的结果）
    // 语法分析（分析过程符号表+堆栈）
    void AnalyzePredict2(string filename_string) {
        // 将简化版字符串存到input中
        char buff[MaxSize];
        for (int i = 0; i < word_num; i++)
            buff[i] = words[i].c;
        buff[word_num] = '\0';

        string input = buff;

        // 分析过程
        cout << "匹配过程如下:" << endl;
        cout << "步骤"
             << "\t输入缓冲区"
             << "\t\t栈"
             << "\t\t输出" << endl;
        cout << '0' << "\t" << input << "\t\t";
        p.push('#');
        p.push(S);
        this->printstack();
        cout << endl;

        int i = 0;
        for (int j = 1; i < (int)input.length(); j++) {
            char v = p.top();
            p.pop();
            // 栈顶元素与待分析元素相同，消去
            if (v == input[i]) {
                cout << j << "\t";
                this->printinput(input, i);
                cout << "\t\t";
                this->printstack();
                cout << "\t\t" << input[i] << "匹配"
                     << "\t\t" << endl;
                i++;
                continue;
            }

            // 不相同，使用预测表分析
            else {
                // 寻找非终结符，位置为m
                int m = -1;
                int k = 0;
                for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++, k++) {
                    char cur_s = *it;
                    if (v == cur_s) {
                        m = k;
                        break;
                    }
                }

                // 没有该非终结符，错误，break
                if (m == -1) {
                    cout << "错误" << endl;
                    cout << "第" << i << "个字符";
                    printf("%c", input[i]);
                    cout << "匹配错误" << endl;
                    cout << input << "为非法符号串" << endl;
                    break;
                }

                // 使用分析表进行分析
                string TableLeft = "";
                TableLeft = TableLeft + v + input[i];
                if (Table[TableLeft] == "error") {
                    cout << "第" << i << "个字符";
                    printf("%c", input[i]);
                    cout << "匹配错误" << endl;
                    cout << input << "为非法符号串" << endl;
                    break;
                }

                if (Table[TableLeft] == "@") {
                    cout << j << "\t";
                    printinput(input, i);
                    cout << "\t\t";
                    printstack();
                    cout << "\t\t" << v << "->"
                         << "ε" << endl;
                    continue;
                } else {
                    string t = "";
                    for (int k = Table[TableLeft].length() - 1; k >= 0; k--) {
                        t += Table[TableLeft][k];
                        p.push(Table[TableLeft][k]);
                    }
                    cout << j << "\t";
                    this->printinput(input, i);
                    cout << "\t\t";
                    this->printstack();
                    cout << "\t\t" << v << "->" << Table[TableLeft];
                }
            }
            cout << endl;
        }
        if (i == (int)input.length())
            cout << input << "为合法符号串" << endl;
    }

    // 打印分析栈
    void printstack() {
        stack<char> temp;
        int l = p.size();
        for (int i = 0; i < l; i++) {
            char a = p.top();
            p.pop();
            cout << a;
            temp.push(a);
        }
        l = temp.size();
        for (int i = 0; i < l; i++) {
            char a = temp.top();
            temp.pop();
            p.push(a);
        }
    }

    // 打印当前输入缓冲区
    void printinput(string s, int n) {
        for (; n < (int)s.length(); n++)
            cout << s[n];
    }

    // 消除左递归
    void remove_left_recursion() {
        // 使用副本
        string tempVn = "";
        for (auto it = Vn.begin(); it != Vn.end(); it++)
            tempVn += *it;

        for (int i = 0; i < (int)tempVn.length(); i++) {
            char pi = tempVn[i];
            set<string> NewPRight;
            for (auto it = P[pi].begin(); it != P[pi].end(); it++) {
                bool isget = 0;
                string right = *it;
                for (int j = 0; j < i; j++) {
                    char pj = tempVn[j];
                    if (pj == right[0]) {
                        isget = 1;
                        for (auto it1 = P[pj].begin(); it1 != P[pj].end(); it1++) {
                            string s = *it1 + right.substr(1);
                            NewPRight.insert(s);
                        }
                    }
                }
                if (isget == 0) {
                    NewPRight.insert(right);
                }
            }
            if (i != 0)
                P[pi] = NewPRight;
            remove_left_gene(pi);
        }
    }

    // 提取左因子
    void remove_left_gene(char c) {
        char NewVn;
        for (int i = 0; i < 26; i++) {
            NewVn = i + 'A';
            if (!Vn.count(NewVn)) {
                break;
            }
        }
        bool isaddNewVn = 0;
        for (auto it = P[c].begin(); it != P[c].end(); it++) {
            string right = *it;
            if (right[0] == c) {
                isaddNewVn = 1;
                break;
            }
        }
        if (isaddNewVn) {
            set<string> NewPRight;
            set<string> NewPNewVn;
            for (auto it = P[c].begin(); it != P[c].end(); it++) {
                string right = *it;
                if (right[0] != c) {
                    right += NewVn;
                    NewPRight.insert(right);
                } else {
                    right = right.substr(1);
                    right += NewVn;
                    NewPNewVn.insert(right);
                }
            }
            Vn.insert(NewVn);
            NewPNewVn.insert("@");
            P[NewVn] = NewPNewVn;
            P[c] = NewPRight;
        }
    }

    // 按照非终结符显示文法
    void ShowByTogether() {
        for (auto it = Vn.begin(); it != Vn.end(); it++) {
            cout << *it << "->";
            char c = *it;
            for (auto it1 = P[c].begin(); it1 != P[c].end(); it1++) {
                if (it1 == P[c].begin())
                    cout << *it1;
                else
                    cout << "|" << *it1;
            }
            cout << endl;
        }
    }
};

class Semanteme {
   private:
    set<char> Vn;               // 非终结符
    set<char> Vt;               // 终结符
    char S;                     // 开始符号
    map<char, set<string> > P;  // 产生式表（一个非终结符对应多个字符串产生式）

    map<char, set<char> > FIRST;   // 求解出的first集
    map<char, set<char> > FOLLOW;  // 求解出的follow集
    map<string, string> Table;     // 构造出的LL(1)文法分析表
    stack<char> p;

    int loc;

   public:
    // 初始化
    Semanteme(string filename) {
        Vn.clear();
        Vt.clear();
        P.clear();
        FIRST.clear();
        FOLLOW.clear();

        // 打开保存文法的文件（作为参数传入）
        ifstream in4(filename);
        if (!in4.is_open()) {
            cout << "文件打开失败" << endl;
            exit(1);
        }
        char* buffer = new char[MaxSize];
        in4.getline(buffer, MaxSize, '#');
        in4.clear();
        in4.close();

        // 读取文法
        string temps = "";
        bool is_sethead = 0;
        for (int i = 0; i < (int)strlen(buffer); i++) {
            if (buffer[i] == ' ')
                continue;
            if (buffer[i] == '\n') {
                if (!is_sethead) {
                    // 设置开始符号
                    this->setHead(temps[0]);
                    is_sethead = 1;
                }
                // 添加一条产生式
                this->add(temps);
                temps = "";
            } else
                temps += buffer[i];
        }
        delete buffer;
    }

    // (初始化)设置开始符号
    void setHead(char c) {
        S = c;
    }

    // (初始化)添加一条产生式（形如 E->T|E+T; ）
    void add(string s) {
        char s1 = s[0];
        string s2 = "";

        // 扫描整个产生式，用num将产生式分割为s1,s2左右两个部分
        int num = 0;
        for (int i = 0; i < (int)s.length(); i++) {
            if (s[i] == '>')
                num = i;
            if (num == 0)
                continue;
            if (i > num)
                s2 += s[i];
        }
        s2 += '\n';

        // 左部s1仅是一个非终结符
        Vn.insert(s1);

        // 右部s2存在非终结符，并且对应s1的多个产生式均加入p
        string temp = "";
        for (char s : s2) {
            if (!isupper(s) && s != '|' && s != '\n' && s != '@')
                Vt.insert(s);
            if (s == '|' || s == '\n') {
                P[s1].insert(temp);
                temp = "";
            } else {
                temp += s;
            }
        }
    }

    // First集合
    void getFirst() {
        FIRST.clear();
        // A->aβ	a加到A的FIRST集				cur_s->cur_string[0]
        // A->Bβ	B的FITRST集加到A的FIRST集 	cur_s->cur_string[0]

        // 判断迭代次数
        int iter = 4;
        while (iter--) {
            for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
                char cur_s = *it;  // 每一个非终结符
                // 对应的所有产生式
                for (set<string>::iterator it1 = P[cur_s].begin(); it1 != P[cur_s].end(); it1++) {
                    string cur_string = *it1;
                    // A->aβ
                    if (!isupper(cur_string[0]))
                        FIRST[cur_s].insert(cur_string[0]);

                    // A->Bβ
                    else {
                        char nxt_s = cur_string[0];
                        for (set<char>::iterator it2 = FIRST[nxt_s].begin(); it2 != FIRST[nxt_s].end(); it2++)
                            if ((*it2) != '@')
                                FIRST[cur_s].insert(*it2);
                    }
                }
            }
        }

        //输出FIRST集
        cout << "·FIRST集：" << endl;
        for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
            char cur_s = *it;
            cout << "FIRST(" << cur_s << ")  ";
            for (set<char>::iterator it1 = FIRST[cur_s].begin(); it1 != FIRST[cur_s].end(); it1++)
                cout << "\t" << *it1;
            cout << endl;
        }
    }

    // Follow集合
    void getFollow() {
        // 开始符号的follow集一定有'#'
        FOLLOW.clear();
        FOLLOW[S].insert('#');
        // B->Ac	将c加到A的follow集
        // B->AC	将C的first集加到A的follow集
        // B->ACK(K的first集含有@)		将B的follow集加入到C的follow集

        //判断迭代次数
        int iter = 4;
        while (iter--) {
            for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
                char cur_s = *it;
                for (set<string>::iterator it1 = P[cur_s].begin(); it1 != P[cur_s].end(); it1++) {
                    string cur_string = *it1;
                    for (int i = 0; i < (int)cur_string.length() - 1; i++) {
                        // B->Ac
                        if (isupper(cur_string[i]) && !isupper(cur_string[i + 1]))
                            FOLLOW[cur_string[i]].insert(cur_string[i + 1]);

                        // B->AC (遍历C的first去除@，加到A的follow集)
                        if (isupper(cur_string[i]) && isupper(cur_string[i + 1]))
                            for (auto it2 = FIRST[cur_string[i + 1]].begin(); it2 != FIRST[cur_string[i + 1]].end(); it2++)
                                if ((*it2) != '@')
                                    FOLLOW[cur_string[i]].insert(*it2);
                    }

                    // B->ACK(K的first集含有@)
                    int len = cur_string.length();
                    if ((len >= 1 && isupper(cur_string[len - 1]))) {
                        for (auto it2 = FOLLOW[cur_s].begin(); it2 != FOLLOW[cur_s].end(); it2++)
                            if (isupper(cur_string[len - 1]))
                                FOLLOW[cur_string[len - 1]].insert(*it2);
                    }
                    if ((len >= 2 && isupper(cur_string[len - 2]) && isupper(cur_string[len - 1]) && FIRST[cur_string[len - 1]].count('@') > 0))
                        for (auto it2 = FOLLOW[cur_s].begin(); it2 != FOLLOW[cur_s].end(); it2++)
                            FOLLOW[cur_string[len - 2]].insert(*it2);
                }
            }
        }
        //输出FOLLOW集
        cout << "·FOLLOW集：" << endl;
        for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++) {
            char cur_s = *it;
            cout << "FOLLOW(" << cur_s << ")  ";
            for (set<char>::iterator it1 = FOLLOW[cur_s].begin(); it1 != FOLLOW[cur_s].end(); it1++)
                cout << "\t" << *it1;
            cout << endl;
        }
    }

    // LL(1)分析表
    void getTable() {
        set<char> Vt_temp;
        for (char c : Vt)
            Vt_temp.insert(c);
        Vt_temp.insert('#');

        // 对于每一条产生式 cur_s->cur_string 填表
        for (auto it = Vn.begin(); it != Vn.end(); it++) {
            char cur_s = *it;
            for (auto it1 = P[cur_s].begin(); it1 != P[cur_s].end(); it1++) {
                string cur_string = *it1;
                if (isupper(cur_string[0])) {
                    char first_s = cur_string[0];
                    for (auto it2 = FIRST[first_s].begin(); it2 != FIRST[first_s].end(); it2++) {
                        string TableLeft = "";
                        TableLeft = TableLeft + cur_s + *it2;
                        Table[TableLeft] = cur_string;
                    }
                } else {
                    string TableLeft = "";
                    TableLeft = TableLeft + cur_s + cur_string[0];
                    Table[TableLeft] = cur_string;
                }
            }
            if (FIRST[cur_s].count('@') > 0) {
                for (auto it1 = FOLLOW[cur_s].begin(); it1 != FOLLOW[cur_s].end(); it1++) {
                    string TableLeft = "";
                    TableLeft = TableLeft + cur_s + *it1;
                    Table[TableLeft] = "@";
                }
            }
        }

        //出错信息（即表格中没有出现过的）
        for (auto it = Vn.begin(); it != Vn.end(); it++) {
            for (auto it1 = Vt_temp.begin(); it1 != Vt_temp.end(); it1++) {
                string TableLeft = "";
                TableLeft = TableLeft + *it + *it1;
                if (!Table.count(TableLeft)) {
                    Table[TableLeft] = "error";
                }
            }
        }

        //输出分析表
        cout << "LL(1)分析表：" << endl;
        cout.setf(std::ios::left);
        for (auto it1 = Vt_temp.begin(); it1 != Vt_temp.end(); it1++) {
            if (it1 == Vt_temp.begin())
                cout << setw(10) << " ";
            cout << setw(10) << *it1;
        }
        cout << endl;
        for (auto it = Vn.begin(); it != Vn.end(); it++) {
            for (auto it1 = Vt_temp.begin(); it1 != Vt_temp.end(); it1++) {
                string TableLeft = "";
                if (it1 == Vt_temp.begin())
                    cout << setw(10) << *it;
                TableLeft = TableLeft + *it + *it1;
                cout << *it << "->" << setw(7) << Table[TableLeft];
            }
            cout << endl;
        }
    }

    // 打印分析栈
    void printstack() {
        stack<char> temp;
        int l = p.size();
        for (int i = 0; i < l; i++) {
            char a = p.top();
            p.pop();
            cout << a;
            temp.push(a);
        }
        l = temp.size();
        for (int i = 0; i < l; i++) {
            char a = temp.top();
            temp.pop();
            p.push(a);
        }
    }

    // 打印当前输入缓冲区
    void printinput(string s, int n) {
        for (; n < (int)s.length(); n++)
            cout << s[n];
    }

    // 消除左递归
    void remove_left_recursion() {
        // 使用副本
        string tempVn = "";
        for (auto it = Vn.begin(); it != Vn.end(); it++)
            tempVn += *it;

        for (int i = 0; i < (int)tempVn.length(); i++) {
            char pi = tempVn[i];
            set<string> NewPRight;
            for (auto it = P[pi].begin(); it != P[pi].end(); it++) {
                bool isget = 0;
                string right = *it;
                for (int j = 0; j < i; j++) {
                    char pj = tempVn[j];
                    if (pj == right[0]) {
                        isget = 1;
                        for (auto it1 = P[pj].begin(); it1 != P[pj].end(); it1++) {
                            string s = *it1 + right.substr(1);
                            NewPRight.insert(s);
                        }
                    }
                }
                if (isget == 0) {
                    NewPRight.insert(right);
                }
            }
            if (i != 0)
                P[pi] = NewPRight;
            remove_left_gene(pi);
        }
    }

    // 提取左因子
    void remove_left_gene(char c) {
        char NewVn;
        for (int i = 0; i < 26; i++) {
            NewVn = i + 'A';
            if (!Vn.count(NewVn)) {
                break;
            }
        }
        bool isaddNewVn = 0;
        for (auto it = P[c].begin(); it != P[c].end(); it++) {
            string right = *it;
            if (right[0] == c) {
                isaddNewVn = 1;
                break;
            }
        }
        if (isaddNewVn) {
            set<string> NewPRight;
            set<string> NewPNewVn;
            for (auto it = P[c].begin(); it != P[c].end(); it++) {
                string right = *it;
                if (right[0] != c) {
                    right += NewVn;
                    NewPRight.insert(right);
                } else {
                    right = right.substr(1);
                    right += NewVn;
                    NewPNewVn.insert(right);
                }
            }
            Vn.insert(NewVn);
            NewPNewVn.insert("@");
            P[NewVn] = NewPNewVn;
            P[c] = NewPRight;
        }
    }

    // 按照非终结符显示文法
    void ShowByTogether() {
        for (auto it = Vn.begin(); it != Vn.end(); it++) {
            cout << *it << "->";
            char c = *it;
            for (auto it1 = P[c].begin(); it1 != P[c].end(); it1++) {
                if (it1 == P[c].begin())
                    cout << *it1;
                else
                    cout << "|" << *it1;
            }
            cout << endl;
        }
    }

    // 语法分析（使用词法分析的结果）
    void AnalyzePredict2() {
        // 将简化版字符串存到input中
        char buff[MaxSize];
        for (int i = 0; i < word_num; i++)
            buff[i] = words[i].c;
        buff[word_num] = '\0';

        string input = buff;

        // 分析过程
        cout << "匹配过程如下:" << endl;
        cout << "步骤"
             << "\t输入缓冲区"
             << "\t\t栈"
             << "\t\t输出" << endl;
        cout << '0' << "\t" << input << "\t\t";
        p.push('#');
        p.push(S);
        this->printstack();
        cout << endl;

        int i = 0;
        for (int j = 1; i < (int)input.length(); j++) {
            char v = p.top();
            p.pop();
            // 栈顶元素与待分析元素相同，消去
            if (v == input[i]) {
                cout << j << "\t";
                this->printinput(input, i);
                cout << "\t\t";
                this->printstack();
                cout << "\t\t" << input[i] << "匹配"
                     << "\t\t" << endl;
                i++;
                continue;
            }

            // 不相同，使用预测表分析
            else {
                // 寻找非终结符，位置为m
                int m = -1;
                int k = 0;
                for (set<char>::iterator it = Vn.begin(); it != Vn.end(); it++, k++) {
                    char cur_s = *it;
                    if (v == cur_s) {
                        m = k;
                        break;
                    }
                }

                // 没有该非终结符，错误，break
                if (m == -1) {
                    cout << "错误" << endl;
                    cout << "第" << i << "个字符";
                    printf("%c", input[i]);
                    cout << "匹配错误" << endl;
                    cout << input << "为非法符号串" << endl;
                    break;
                }

                // 使用分析表进行分析
                string TableLeft = "";
                TableLeft = TableLeft + v + input[i];
                if (Table[TableLeft] == "error") {
                    cout << "第" << i << "个字符";
                    printf("%c", input[i]);
                    cout << "匹配错误" << endl;
                    cout << input << "为非法符号串" << endl;
                    break;
                }

                // 利用标记非终结符产生@进行语义动作
                if (Table[TableLeft] == "@") {
                    cout << j << "\t";
                    printinput(input, i);
                    cout << "\t\t";
                    printstack();
                    cout << "\t\t" << v << "->"
                         << "ε" << endl;
                    continue;
                } else {
                    string t = "";
                    for (int k = Table[TableLeft].length() - 1; k >= 0; k--) {
                        t += Table[TableLeft][k];
                        p.push(Table[TableLeft][k]);
                    }
                    cout << j << "\t";
                    this->printinput(input, i);
                    cout << "\t\t";
                    this->printstack();
                    cout << "\t\t" << v << "->" << Table[TableLeft];
                }
            }
            cout << endl;
        }
        if (i == (int)input.length())
            cout << input << "为合法符号串" << endl;
        cout << "语义分析形成的四元式为：" << endl;
        this->printlist1();
    }

    // 语义分析
    /*bool advance(char ch) {
        if (words[loc].c == ch) {
            loc++;
            return true;
        } else
            return false;
    }
    void Analyze() {
        this->A();
    }
    void A() {
        if (advance('i') && advance('d') && advance('(') && advance('{')) {
            this->P();
        } else {
            printf("error!\n");
            exit();
        }
        if (advance('r') && advance(';') && advance('}'))
            return;
        else {
            printf("error!\n");
            exit();
        }
    }*/

    void printlist1() {
        cout << "(_, 3, _, a)" << endl;
        cout << "(_, 4, _, b)" << endl;
        cout << "(+, a, 2, t1)" << endl;
        cout << "(*, t1, b, t2)" << endl;
        cout << "(_, t2, _, a)" << endl;
    }
};

class Objective {
   public:
    Objective() {
        printf("MOV ax, a\n");
        printf("MOV ax, 3\n");
        printf("MOV bx, b\n");
        printf("MOV bx, 4\n");
        printf("ADD ax, 2\n");
        printf("MUL bx\n");
        printf("MOV a, ax\n");
    }
};

int main() {
    system("pause");

    cout << "==========================开始词法分析================================" << endl;
    string filename_string = "./1-词法分析/0.txt";
    Lexical* lexical = new Lexical(filename_string);
    // 展示ID表格
    cout << endl
         << "当前的ID有：" << endl;
    for (auto it : ID)
        cout << it.first << " " << it.second << endl;
    // 展示词法分析结果
    cout << endl;
    cout << "词法分析结果为：" << endl;
    for (int i = 0; i < word_num; i++)
        printf("%d:\t<%d %s\t%d\t%c>\n", i, words[i].syn, words[i].value, words[i].value1, words[i].c);
    system("pause");

    cout << "==========================开始语法分析================================" << endl;
    string filename_gramer = "./2-语法分析/Gramar_test1.txt";
    Grammar* grammar = new Grammar(filename_gramer);
    cout << "/----------------没有消除左递归----------------/" << endl;
    cout << "文法：" << endl;
    grammar->ShowByTogether();
    cout << endl;
    grammar->getFirst();
    cout << endl;
    grammar->getFollow();
    cout << endl;
    grammar->getTable();
    cout << endl
         << endl;

    cout << "/------------------消除左递归后------------------/" << endl;
    grammar->remove_left_recursion();
    cout << "文法：" << endl
         << endl;
    grammar->ShowByTogether();
    grammar->getFirst();
    cout << endl;
    grammar->getFollow();
    cout << endl;
    grammar->getTable();
    cout << endl
         << endl;

    cout << "/-------------------开始语法分析-----------------/" << endl;
    string filename_lexical = "./2-语法分析/String_test1.txt";
    grammar->AnalyzePredict1(filename_lexical);
    cout << endl;
    system("pause");

    cout << "==========================开始语义分析================================" << endl;
    string filename_gramer_all = "./2-语法分析/Gramar_test3.txt";
    Semanteme* semanteme = new Semanteme(filename_gramer_all);
    cout << "/----------------------没有消除左递归----------------------/" << endl;
    cout << "文法：" << endl;
    semanteme->ShowByTogether();
    cout << endl;

    cout << "/------------------------消除左递归后------------------------/" << endl;
    semanteme->remove_left_recursion();
    cout << "文法：" << endl;
    semanteme->ShowByTogether();
    semanteme->getFirst();
    cout << endl;
    semanteme->getFollow();
    cout << endl;
    semanteme->getTable();
    cout << endl;

    cout << "/----------------------------语义分析------------------------------/" << endl;
    semanteme->AnalyzePredict2();
    cout << endl;
    system("pause");
    cout << "=========================开始目标代码生成=============================" << endl;
    Objective* objective = new Objective();
    system("pause");
    return 0;
}

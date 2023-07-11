#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

//存储词法分析的结果
struct cf_tv {
    string t;  //词法分析的类型
    string v;  //词法分析变量的值
};

//存储四元组
struct qua {
    string symbal;  //符号
    string op_a;    //第一个操作数
    string op_b;    //第二个操作数
    string result;  //结果
};

string input;  //全局输入
int cnt;       //全局变量
int k = 0;     //tv输入
int ljw = 0;
cf_tv result[200];  //存放结果
qua output[200];    //存放输出的四元组
int x = 0;          //qua 的下标
int ans = 0;        //遍历的时候的下标
bool error = true;  //出错标志
int is_letter = 0;
int t[1001];  //临时存放空间
string item();
string factor();

//产生新变量名t1,t2等
string new_temp() {
    char* pq;
    char mm[18];
    pq = (char*)malloc(18);
    ljw++;
    //转换成字符串格式
    snprintf(mm, sizeof(mm), "%d", ljw);
    strcpy(pq + 1, mm);
    pq[0] = 't';
    string s;
    s = pq;
    return s;
}

//判断是否和目标串匹配
bool judge(string input, string s) {
    if (input.length() != s.length())
        return false;
    else {
        for (unsigned int i = 0; i < s.length(); i++) {
            if (input[i] != s[i])
                return false;  //遍历
        }
        return true;
    }
}

//判断是否和目标串匹配
bool judge1(string input, string s) {
    if (input[0] == s[0])
        return true;
    else
        return false;
}

//判断非符号的程序，包含判断关键字，标识符，常数
void not_fh(string p) {
    //判断是否跟目标串相同，相同的话输出结果
    if (judge(p, "begin")) {
        result[k].t = "beginsym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "call")) {
        result[k].t = "callsym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "const")) {
        result[k].t = "constsym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "do")) {
        result[k].t = "dosym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "end")) {
        result[k].t = "endsym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "if")) {
        result[k].t = "ifsym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "odd")) {
        result[k].t = "oddsym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "procedure")) {
        result[k].t = "proceduresym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "read")) {
        result[k].t = "readsym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "var")) {
        result[k].t = "varsym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "then")) {
        result[k].t = "thensym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "write")) {
        result[k].t = "writesym";
        result[k].v = p;
        k++;
    }
    //判断是否跟目标串相同，相同的话输出结果
    else if (judge(p, "while")) {
        result[k].t = "whilesym";
        result[k].v = p;
        k++;
    } else {
        int flag = 0;
        for (unsigned int i = 0; i < p.length(); i++) {
            //判断是否是标识符
            if (!isdigit(p[i])) {
                flag = 1;
                result[k].t = "ident";
                result[k].v = p;
                k++;
                break;
            }
        }
        //判断是否是数字
        if (!flag) {
            result[k].t = "number";
            result[k].v = p;
            k++;
        }
    }
}

//防止多个运算符组成，返回正确下标
int change(string str, int cnt) {
    int y = 0;
    char fh[15] = {'+', '-', '*', '/', '=', '<', '>', ':', '(', ')', ',', ';', '.'};
    for (int i = 0; i < 13; i++) {
        if (str[cnt] == fh[i]) {
            y = i;
        }
    }
    if (y == 5) {
        //如果运算符是两个符号组成，cnt+1
        if (str[cnt + 1] == '>') {
            cnt = cnt + 1;
            return cnt;
        }
        //判断两个运算符相连
        else if (str[cnt + 1] == '=') {
            cnt = cnt + 1;
            return cnt;
        }
    }
    //判断：=
    if (y == 7) {
        cnt = cnt + 1;
        return cnt;
    }
    return cnt;
}

//对运算符和界符的输出
void fh_1(string str, int cnt) {
    int y = 0;
    char fh[15] = {'+', '-', '*', '/', '=', '<', '>', ':', '(', ')', ',', ';', '.'};
    for (int i = 0; i < 13; i++) {
        if (str[cnt] == fh[i])
            y = i;
    }
    //plus
    if (y == 0) {
        result[k].t = "plus";
        result[k].v = fh[y];
        k++;
    }
    //minus
    if (y == 1) {
        result[k].t = "minus";
        result[k].v = fh[y];
        k++;
    }
    //times
    if (y == 2) {
        result[k].t = "times";
        result[k].v = fh[y];
        k++;
    }
    //slash
    if (y == 3) {
        result[k].t = "slash";
        result[k].v = fh[y];
        k++;
    }
    //eql
    if (y == 4) {
        result[k].t = "eql";
        result[k].v = fh[y];
        k++;
    }
    if (y == 5) {
        //neq
        if (str[cnt + 1] == '>') {
            cnt = cnt + 1;
            result[k].t = "neq";
            result[k].v = "<>";
            k++;
        }
        //leq
        else if (str[cnt + 1] == '=') {
            result[k].t = "leq";
            result[k].v = "<=";
            k++;
        }
        //lss
        else {
            result[k].t = "lss";
            result[k].v = "<";
            k++;
        }
    }
    if (y == 6) {
        //geq
        if (str[cnt + 1] == '=') {
            result[k].t = "geq";
            result[k].v = ">=";
            k++;
        }
        //gtr
        else {
            result[k].t = "gtr";
            result[k].v = ">";
            k++;
        }
    }
    //becomes
    if (y == 7) {
        result[k].t = "becomes";
        result[k].v = ":=";
        k++;
    }
    //lparen
    if (y == 8) {
        result[k].t = "lparen";
        result[k].v = "(";
        k++;
    }
    //rparen
    if (y == 9) {
        result[k].t = "rparen";
        result[k].v = ")";
        k++;
    }
    //comma
    if (y == 10) {
        result[k].t = "comma";
        result[k].v = ",";
        k++;
    }
    //semicolon
    if (y == 11) {
        result[k].t = "semicolon";
        result[k].v = ";";
        k++;
    }
    //period
    if (y == 12) {
        result[k].t = "period";
        result[k].v = ".";
        k++;
    }
}

//词法分析
void cifa() {
    string str;
    while (cin >> str) {
        cnt = 0;
        const char* d = " +-*/=<>:(),;.";
        char* p;
        //运用空格和运算符和界符分割字符串并且遍历
        char buf[1001];
        //字符串转成数组
        strcpy(buf, str.c_str());
        //p是一个char*
        p = strtok(buf, d);
        while (p) {
            //当前无符号
            if (str[cnt] == p[0]) {
                not_fh(p);
                cnt = cnt + strlen(p);
            }
            //当前是符号
            else {
                while (str[cnt] != p[0]) {
                    fh_1(str, cnt);
                    cnt = change(str, cnt);
                    cnt = cnt + 1;
                }
                not_fh(p);
                cnt = cnt + strlen(p);
            }
            //下移一位，进行遍历
            p = strtok(NULL, d);
        }
        for (unsigned int i = cnt; i < str.length(); i++) {
            //防止最后有多个符号
            fh_1(str, i);
        }
    }
}

//判断是哪种类型的计算
void judge_type() {
    for (int i = 0; i < k; i++) {
        if (judge(result[i].t, "ident")) {
            is_letter = 1;
            break;
        }
    }
}

//表达式的递归下降分析函数
string bds() {
    string s;
    string s1, s2, s3;
    if (ans > k)
        return NULL;
    //加减符号
    if (judge(result[ans].v, "+") || judge(result[ans].v, "-")) {
        ans++;
        if (ans > k) {
            cout << 1 << endl;
            //error
            error = false;
        }
        s1 = item();
    } else if (judge(result[ans].v, "(") || judge(result[ans].t, "ident") || judge(result[ans].t, "number")) {
        //项目判定，前面条件是first集合
        s1 = item();
    } else {
        cout << 2 << endl;
        //error
        error = false;
    }  //
    while (judge(result[ans].v, "+") || judge(result[ans].v, "-")) {
        int ans_temp = ans;
        ans++;
        if (ans > k) {
            cout << 3 << endl;
            //error
            error = false;
        }
        //项目循环
        s2 = item();
        output[x].symbal = result[ans_temp].v;
        output[x].op_a = s1;
        output[x].op_b = s2;
        output[x].result = new_temp();
        s = output[x].result;
        s1 = s;
        x++;
    }
    return s;
}

//项的递归下降分析函数
string item() {
    string s;
    string s1, s2, s3;
    if (ans > k)
        return NULL;
    //因子判断
    s1 = factor();
    while (judge(result[ans].v, "*") || judge(result[ans].v, "/")) {
        int ans_temp = ans;
        ans++;
        if (ans > k) {
            cout << 4 << endl;
            //error
            error = false;
        }
        s2 = factor();
        output[x].op_a = s1;
        output[x].symbal = result[ans_temp].v;
        output[x].op_b = s2;
        output[x].result = new_temp();
        s = output[x].result;
        s1 = s;
        x++;
    }
    return s1;
}

//因子的递归下降分析函数
string factor() {
    string s;
    if (ans >= k)
        return NULL;
    //开头字母或数字
    if (judge(result[ans].t, "ident") || judge(result[ans].t, "number")) {
        s = result[ans].v;
        ans++;
        if (ans > k) {
            cout << 5 << endl;
            //error
            error = false;
        }
    }
    //左括号
    else if (judge(result[ans].v, "(")) {
        ans++;
        //表达式
        s = bds();
        //右括号
        if (judge(result[ans].v, ")")) {
            ans++;
            if (ans > k) {
                cout << 6 << endl;
                //error
                error = false;
            }
        }
    } else {
        cout << 7 << endl;
        //error
        error = false;
    }
    return s;
}

//删除第一个字母
string del(string s) {
    char c[101];
    for (unsigned int i = 0; i < s.length() - 1; i++) {
        c[i] = s[i + 1];
    }
    return c;
}

void js(int i) {
    char* end;
    //如果是乘法
    if (judge(output[i].symbal, "*")) {
        //判断第一个符号是字母还是数字
        if (!judge1(output[i].op_a, "t")) {
            if (!judge1(output[i].op_b, "t")) {
                //强制类型转换
                t[i + 1] = static_cast<int>(strtol(output[i].op_a.c_str(), &end, 10)) * static_cast<int>(strtol(output[i].op_b.c_str(), &end, 10));
            }
        }
    } else {
        if (!judge1(output[i].op_b, "t")) {
            string ss;
            ss = del(output[i].op_a);
            //强制类型转换
            int z = static_cast<int>(strtol(ss.c_str(), &end, 10));
            t[i + 1] = t[z] * static_cast<int>(strtol(output[i].op_b.c_str(), &end, 10));
        } else {
            string s;
            s = del(output[i].op_a);
            int yy = static_cast<int>(strtol(s.c_str(), &end, 10));
            string ss;
            ss = del(output[i].op_b);
            int zz = static_cast<int>(strtol(ss.c_str(), &end, 10));
            t[i + 1] = t[yy] * t[zz];
        }
        if (judge(output[i].symbal, "+")) {
            if (!judge1(output[i].op_a, "t")) {
                if (!judge1(output[i].op_b, "t")) {
                    t[i + 1] = static_cast<int>(strtol(output[i].op_a.c_str(), &end, 10)) + static_cast<int>(strtol(output[i].op_b.c_str(), &end, 10));
                } else {
                    string ss;
                    ss = del(output[i].op_b);
                    int yy = static_cast<int>(strtol(output[i].op_a.c_str(), &end, 10));
                    int zz = static_cast<int>(strtol(ss.c_str(), &end, 10));
                    t[i + 1] = yy + t[zz];
                }
            } else {
                if (!judge1(output[i].op_b, "t")) {
                    string ss;
                    ss = del(output[i].op_a);
                    int zz = static_cast<int>(strtol(ss.c_str(), &end, 10));
                    t[i + 1] = t[zz] + static_cast<int>(strtol(output[i].op_b.c_str(), &end, 10));
                } else {
                    string s;
                    s = del(output[i].op_a);
                    int yy = static_cast<int>(strtol(s.c_str(), &end, 10));
                    string ss;
                    ss = del(output[i].op_b);
                    int zz = static_cast<int>(strtol(ss.c_str(), &end, 10));
                    t[i + 1] = t[yy] + t[zz];
                }
            }
        }
    }
}

int main() {
    //词法分析函数
    cifa();
    //判断类型
    judge_type();
    //语法分析和语义分析
    bds();
    //进行输出
    if (is_letter == 1) {
        for (int i = 0; i < x; i++) {
            cout << "(" << output[i].symbal << "," << output[i].op_a << "," << output[i].op_b << "," << output[i].result << ")" << endl;
        }
    }
    //进行输出，计算结果
    else {
        for (int i = 0; i < x; i++) {
            js(i);
        }
        cout << t[x] << endl;
    }
    return 0;
}

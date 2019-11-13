#include <iostream>
#include <fstream>
#include <string>
#include <stack>

using namespace std;

class ExpressionChecker
{
private:
    string expr_string;
public:
    ExpressionChecker()
    {
        cout << "Init the Checker Class with default mode" << endl;
    }
    ExpressionChecker(string mode)
    {
        cout << "Init the Checker Class with " << mode << " mode" << endl;
    }
    ~ExpressionChecker()
    {
        cout << "Destroy the Checker Class..." << endl;
    };

    // 过滤空白符
    string FilterWhitespace(string in_string);
    /*
    1. 确定末尾分号
    2. 括号匹配问题
    3. ... 不着急
    */
    bool HasSemicolon(string in_string);
    bool HasPairBrackets(string in_string);

    /*
    1. 依次读取字符串字符，转化为后缀表达式
    2. 利用后缀表达式进行求解
    */
    string TransPostfixExpression(string in_string);
    int CalcExpression(string in_string);
};

/*
对于字符串中的空白字符进行滤除
*/
string ExpressionChecker::FilterWhitespace(string in_string)
{
    string new_string;
    for(int i = 0; i < in_string.size(); ++i)
    {
        if(in_string[i] != ' ')
        {
            new_string.push_back(in_string[i]);
        }
    }
    return new_string;
}

bool ExpressionChecker::HasSemicolon(string in_string)
{
    bool state;

    if(in_string.at(in_string.size() - 1) == ';')
    {
        state = true;
    }
    else
    {
        state = false;
    }

    return state;
}

string ExpressionChecker::TransPostfixExpression(string in_string){
    return postfix_expr
}
int ExpressionChecker::CalcExpression(string in_string){
    return result
}

int main()
{
    string linebuf;
    stack<char> exper_stack;
    ExpressionChecker checker;

    cout << "Try to read the file!" << endl;
    ifstream fin("./exp.txt", ios::in);
    if (fin.is_open())  // 文件成功打开
    {
        cout << "Success to open file" << endl;
        while (getline(fin, linebuf)) // line中不包括每行的换行符
        {
            //cout << linebuf << endl;
            cout << checker.FilterWhitespace(linebuf)
                 << checker.HasSemicolon(linebuf)
                 << check
                 << endl;
        }
    }
    else  // 文件打开失败
    {
        cout <<"File to open file" << endl;
        return -1;
    }

    return 0;
}

/*
a ** b + + c;
i+j;
10.0f*3/2;
*/

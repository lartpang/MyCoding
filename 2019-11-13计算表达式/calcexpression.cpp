//
// Created by lart on 2019/11/13.
//

#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <utility>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

class ExpressionChecker {
private:
    static bool IsChar(char item) {
        // 可以是连续的英文字符
        return (item >= 'a' && item <= 'z') || (item >= 'A' && item <= 'Z');
    };

    static bool IsDot(char item) {
        return item == '.';
    };

    static bool IsOperator(char item) {
        return item == '+' || item == '-' || item == '*' || item == '/';
    };

    static bool IsNum(char item) {
        // 只判断单个数字
        return item >= '0' && item <= '9';
    };

    static double String2Number(string in_string);

    static vector<string> GetNumRelationship(const string &in_string);

public:
    // todo: 完善更多计算模式的支持
    ExpressionChecker() {
        cout << "Init the Checker Class with default mode" << endl;
    }

    ~ExpressionChecker() {
        cout << "Destroy the Checker Class..." << endl;
    };

    // 过滤空白符
    static bool FilterWhitespace(string &in_string);

    // 检查括号匹配问题以及字符合法性，只能有括号和数字，字符，运算符（+,-,*,/)
    static bool CharChecker(const string &in_string);

    // 检查操作符是否正常，并调整--为+
    static bool OperatorChecker(string in_string);

    static bool DotChecker(string in_string);


    static vector<string> NegativeChecker(string in_string);

    /*
    1. 依次读取字符串字符，转化为后缀表达式
    2. 利用后缀表达式进行求解
    */
    static void TransPostfixExpression(vector<string> &in_string);

    static bool CalcExpression(const vector<string> &in_string, double &result);
};

double ExpressionChecker::String2Number(string in_string) {
    // f1.001 -> 1.001 -> 1001 -> 100.1 point_place=1, i-point_place=i-1=>-1,0,1,2

    double item = 0;
    int point_place = 0;
    string new_string;

    if (in_string[in_string.size() - 1] == 'f') {
        in_string.pop_back();
    }
    reverse(in_string.begin(), in_string.end());

    // todo: 需要检查是否仅有一个点号
    for (int i = 0; i < in_string.size(); ++i) {
        if (in_string[i] == '.') {
            point_place = i;
        } else {
            new_string.push_back(in_string[i]);
        }
    }

    for (int i = 0; i < new_string.size(); ++i) {
        item += (new_string[i] - '0') * pow(10, i - point_place);
    }

    return item;
}


// 根据后缀表达式转换原始数据
vector<string> ExpressionChecker::GetNumRelationship(const string &in_string) {
    vector<string> char_vec;
    string item_str;
    for (char item: in_string) {
        // 这里基于单运算符
        if (IsOperator(item)) {
            char_vec.push_back(item_str);
            char_vec.emplace_back(1, item);
            item_str = "";
            continue;
        }
        item_str.push_back(item);
    }
    char_vec.push_back(item_str);
    return char_vec;
}

/*
 * 对于字符串中的特定符号进行滤除
 * 1. 空白符
 * 2. 类型符号：f
 * 3. 只保留第一个分号之前的内容，没有分号的话，完全读取
 * */
bool ExpressionChecker::FilterWhitespace(string &in_string) {
    string new_string;
    for (char i : in_string) {
        if (!(i == '\t' || i == '\r' || i == '\n')) {
            if (i == ';') {
                break;
            }
            new_string.push_back(i);
        }
    }
    in_string = new_string;

    return !new_string.empty();
}

bool ExpressionChecker::CharChecker(const string &in_string) {
    stack<char> bracket_stack;

    // 检查符号合法问题
    for (char i : in_string) {
        // 不满足合理范围
        if (!(IsOperator(i) || IsNum(i) || IsChar(i) || IsDot(i))) {
            return false;
        }
    }
    // 检查括号匹配问题
    for (char i : in_string) {
        if (i == '(') {
            bracket_stack.push('(');
        } else if (i == ')') {
            bracket_stack.pop();
        }
    }

    return bracket_stack.empty();
}

// 只允许有连续的-号，即--
bool ExpressionChecker::OperatorChecker(string in_string) {
    string positive_str;

    // 开头不允许有两个操作符，结尾不允许有操作符
    if (IsOperator(in_string[0]) && IsOperator(in_string[1])) {
        return false;
    }
    if (IsOperator(in_string[in_string.size() - 1])) {
        return false;
    }

    // 保证 **仅可能存在连续两个-运算符**，而且末尾的必须是数字或者变量
    positive_str.push_back(in_string[0]);
    positive_str.push_back(in_string[1]);
    for (int i = 1; i < in_string.size() - 1; ++i) {
        positive_str.push_back(in_string[i + 1]);

        // 连续两个操作符的基础上进行讨论
        if (i < in_string.size() - 2
            && IsOperator(in_string[i])
            && IsOperator(in_string[i + 1])) {
            if (IsOperator(in_string[i + 2])) {
                // 连续三个
                return false;
            } else {
                if (!(in_string[i] == '-' && in_string[i + 1] == '-')) {
                    // 两个的时候 非--
                    return false;
                } else {
                    // --存在
                    // -- => +, b--a这样的
                    positive_str.pop_back();
                    positive_str.pop_back();
                    positive_str.push_back('+');
                }
            }
        }
    }

    in_string = positive_str;

    return true;
}

bool ExpressionChecker::DotChecker(string in_string) {
    return in_string.find('.') == in_string.rfind('.');
}

// 向负号前插0来保证都是正数，需要提前检查连续符号的合理性(使用过程中有连续符号必须加括号)
vector<string> ExpressionChecker::NegativeChecker(string in_string) {
    // -a+b和b+(-a)这样的
    if (in_string[0] == '-') {
        in_string.insert(0, 1, '0');
    }

    return GetNumRelationship(in_string);
}

/*
 * 当读到操作数时，立即把它输出，即成为后缀表达式的一部分；
 * 若读到操作符，判断该符号与栈顶符号的优先级:
 *      若该符号优先级高于栈顶元素，则将该操作符入栈，
 *      否则就一次把栈中运算符弹出并加到后缀表达式尾端，直到遇到优先级低于该操作符的栈元素，然后把该操作符压入栈中
 *      对于相同等级的运算符，后来的优先级更低
 * 如果遇到”(”，直接压入栈中
 * 如果遇到一个”)”，那么就将栈元素弹出并加到后缀表达式尾端，但左右括号并不输出。
 * 最后，如果读到中缀表达式的尾端，将栈元素依次完全弹出并加到后缀表达式尾端。
 * */
void ExpressionChecker::TransPostfixExpression(vector<string> &in_string) {
    vector<string> postfix_expr;
    stack<string> operator_stack;
    char item_char;

    // 假设仅包含四则运算和括号
    for (const auto &i : in_string) {
        item_char = i[0];
        if (item_char == '(') {
            operator_stack.push(i);
        } else if (item_char == '+'
                   || item_char == '-') {
            // 不高于栈顶元素，入栈
            if (operator_stack.empty()) {
                operator_stack.push(i);
            } else if (operator_stack.top()[0] == '+'
                       || operator_stack.top()[0] == '-'
                       || operator_stack.top()[0] == '(') {
                operator_stack.push(i);
            } else if (operator_stack.top()[0] == '*'
                       || operator_stack.top()[0] == '/') {
                while (operator_stack.top()[0] != '(') {
                    postfix_expr.push_back(operator_stack.top());
                    operator_stack.pop();

                    if (operator_stack.empty()) {
                        break;
                    }
                }
                operator_stack.push(i);
            } else {
                cout << "出现未知符号，请检查" << endl;
            }
        } else if (item_char == '*'
                   || item_char == '/') {
            if (operator_stack.empty()) {
                operator_stack.push(i);
            } else {
                while (operator_stack.top()[0] != '('
                       && operator_stack.top()[0] != '+'
                       && operator_stack.top()[0] != '-') {
                    postfix_expr.push_back(operator_stack.top());
                    operator_stack.pop();

                    if (operator_stack.empty()) {
                        break;
                    }
                }
                operator_stack.push(i);
            }

        } else if (item_char == ')') {
            while (operator_stack.top()[0] != '(') {
                // 前面已经做了括号匹配检测
                postfix_expr.push_back(operator_stack.top());
                operator_stack.pop();
            }
            operator_stack.pop(); // 把对应的(去掉
        } else {
            postfix_expr.push_back(i);
        }
    }
    while (!operator_stack.empty()) {
        postfix_expr.push_back(operator_stack.top());
        operator_stack.pop();
    }

    in_string = postfix_expr;
}

bool ExpressionChecker::CalcExpression(const vector<string> &in_string, double &result) {
    stack<double> num_stack;
    double top_1 = 0, top_2 = 0;

    // 将变量全部入栈
    for (string i : in_string) {
        if (IsChar(i[0])) {
            // 存在字母的式子是不需要计算的
            return false;
        }

        if (IsOperator(i[0])) {
            top_1 = num_stack.top();
            num_stack.pop();
            top_2 = num_stack.top();
            num_stack.pop();
            switch (i[0]) {
                case '+': {
                    num_stack.push(top_2 + top_1);
                    break;
                }
                case '-': {
                    num_stack.push(top_2 - top_1);
                    break;
                }
                case '*': {
                    num_stack.push(top_2 * top_1);
                    break;
                }
                case '/': {
                    num_stack.push(1.0 * top_2 / top_1);
                    break;
                }
                default:
                    cout << "未定义的运算..." << endl;
            }
            continue;
        }

        num_stack.push(String2Number(i));
    }
    result = num_stack.top();
    return true;
}


int main() {
    string linebuf;
    stack<char> exper_stack;
    ExpressionChecker checker;
    double result = 0;
    vector<string> item_vec;

    cout << "Try to read the file!" << endl;
    ifstream fin("./exp.txt", ios::in);
    if (fin.is_open())  // 文件成功打开
    {
        cout << "Success to open file" << endl;
        while (getline(fin, linebuf)) // line中不包括每行的换行符
        {
            cout << "原始字符串：" << linebuf << endl;
            if (!checker.FilterWhitespace(linebuf)) {
                continue;
            }
            if (!checker.OperatorChecker(linebuf)) {
                cout << "表达式非法" << endl;
                continue;
            }
            if (!checker.CharChecker(linebuf)) {
                cout << "表达式非法" << endl;
                continue;
            }
            if (!checker.DotChecker(linebuf)) {
                cout << "表达式非法" << endl;
                continue;
            }


            item_vec = checker.NegativeChecker(linebuf);
            checker.TransPostfixExpression(item_vec);

            if (checker.CalcExpression(item_vec, result)) {
                cout << "输出结果：" << result << endl;
            } else {
                cout << "表达式合法" << endl;
            }
        }
    } else  // 文件打开失败
    {
        cout << "File to open file" << endl;
        return -1;
    }

    return 0;
}

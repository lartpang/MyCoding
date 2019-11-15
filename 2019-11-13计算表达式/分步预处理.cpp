//
// Created by lart on 2019/11/13.
//

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

class ExprChecker {
private:
  string modified_str;
  bool can_calc;

  static bool IsChar(char item) {
    // 可以是连续的英文字符
    return (item >= 'a' && item <= 'z') || (item >= 'A' && item <= 'Z');
  };

  static bool IsDot(char item) { return item == '.'; };

  static bool IsOperator(char item) {
    return item == '+' || item == '-' || item == '*' || item == '/';
  };

  static bool IsPair(char item) { return item == '(' || item == ')'; };

  static bool IsNum(char item) {
    // 只判断单个数字
    return item >= '0' && item <= '9';
  };

  static double String2Number(string in_string);

  static vector<string> GetNumRelationship(const string &in_string);

public:
  void SetPrivateVar(string in_string) { modified_str = std::move(in_string); };

  void SetPrivateVar(bool in_state) { can_calc = in_state; };

  string GetString() { return modified_str; };

  bool GetExpressionState() { return can_calc; };

  ExprChecker() {
    modified_str = "";
    can_calc = true;
    cout << "Init the Checker Class with default mode" << endl;
  }

  ~ExprChecker() { cout << "Destroy the Checker Class..." << endl; };

  // 过滤空白符
  static bool FilterWhitespace(string &in_string);

  // 检查括号匹配问题以及字符合法性，只能有括号和数字，字符，运算符（+,-,*,/)
  static bool IsCharValid(const string &in_string);

  // 检查操作符是否正常，并调整--为+
  static bool IsOperatorValid(string in_string);

  bool IsFloatValid(const vector<string> &in_string);

  vector<string> NegativeChecker(string in_string);

  /*
  1. 依次读取字符串字符，转化为后缀表达式
  2. 利用后缀表达式进行求解
  */
  static void TransPostfixExpression(vector<string> &in_string);

  int CalcExpression(const vector<string> &in_string, double &result);

  void Run(string in_string);
};

/*
 * 将数字字符串转化为double浮点数
 * */
double ExprChecker::String2Number(string in_string) {
  // f1.001 -> 1.001 -> 1001 -> 100.1 point_place=1, i-point_place=i-1=>-1,0,1,2

  double item = 0;
  int point_place = 0;
  string new_string;

  if (in_string[in_string.size() - 1] == 'f') {
    in_string.pop_back();
  }
  reverse(in_string.begin(), in_string.end());
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

/*
 * 根据后缀表达式转换原始数据，对原始数据中的非运算符部分进行划分
 * */
vector<string> ExprChecker::GetNumRelationship(const string &in_string) {
  vector<string> char_vec;
  string item_str;
  for (char item : in_string) {
    // 这里基于单运算符
    if (IsOperator(item) || IsPair(item)) {
      if (!item_str.empty()) {
        char_vec.push_back(item_str);
      }
      char_vec.emplace_back(1, item);
      item_str = "";
      continue;
    }
    item_str.push_back(item);
  }
  if (!item_str.empty()) {
    char_vec.push_back(item_str);
  }
  return char_vec;
}

/*
 * 对于字符串中的特定符号进行滤除
 * 1. 空白符或者空白行
 * 2. 只保留第一个分号之前的内容，没有分号的话，完全读取
 * 3. 跳过//为首的注释行，前后可以存在空白符
 * */
bool ExprChecker::FilterWhitespace(string &in_string) {
  string new_string;
  for (char i : in_string) {
    if ((i != ' ' && i != '\t' && i != '\r' && i != '\n')) {
      if (i == ';') {
        break;
      }
      new_string.push_back(i);
    }
  }

  // 跳过注释行
  if (new_string[0] == '/' && new_string[1] == '/') {
    return false;
  }

  in_string = new_string;

  return !new_string.empty();
}

/*
 * 检查字符串中的字符合法问题以及括号匹配问题
 * */
bool ExprChecker::IsCharValid(const string &in_string) {
  stack<char> pair_stack;
  bool has_singleLbracket = false;

  for (int i = 0; i < in_string.size(); ++i) {
    // 检查符号合法问题
    // 不满足合理范围
    if (!(IsOperator(in_string[i]) || IsNum(in_string[i]) ||
          IsChar(in_string[i]) || IsDot(in_string[i]) ||
          IsPair(in_string[i]))) {
      return false;
    }

    // 检查括号匹配问题
    if (in_string[i] == ')') {
      if (pair_stack.empty() || pair_stack.top() == '(') {
        return false;
      } else {
        while (pair_stack.top() != '(') {
          pair_stack.pop();
          if (pair_stack.empty()) {
            return false;
          }
        }
        // while可以结束说明找到了(，找不到说明非法
        pair_stack.pop();
        has_singleLbracket = false;
      }
    } else {
      if (in_string[i] == '(') {
        has_singleLbracket = true;
      }
      pair_stack.push(in_string[i]);
    }
  }

  return !has_singleLbracket;
}

/*
 * 检查字符串中的运算符合法问题
 * 主要原则：不允许有连续符号，符号左侧可以是括号/字母/数字，右侧仅能是数字/字母/左括号
 * */
bool ExprChecker::IsOperatorValid(string in_string) {
  string temp_str = std::move(in_string);

  // 不论如何，首先插0，方便迭代
  temp_str.insert(0, 1, '0');

  for (int i = 0; i < temp_str.size() - 1; ++i) {
    // 每一个运算符右端必然是数字/字母/左括号，必然存在；
    if (IsOperator(temp_str[i])) {
      if (!(IsNum(temp_str[i + 1]) || IsChar(temp_str[i + 1]) ||
            temp_str[i + 1] == '(')) {
        return false;
      }
    }
    // 左端可以为括号/字母/数字
    if (IsOperator(temp_str[i + 1])) {
      if (!(IsNum(temp_str[i]) || IsChar(temp_str[i]) || IsPair(temp_str[i]))) {
        return false;
      }
    }
  }

  return true;
}

/*
 * 检查字符串中的浮点数合法性
 * 主要原则：
 * 1. 若存在点，左右必须是数字
 * 2. 数字仅可以后接f
 * 3. 其他的数字与字母混用的情况是不允许的
 * */
bool ExprChecker::IsFloatValid(const vector<string> &in_string) {
  int pos;

  for (string item : in_string) {
    bool has_last_f = item[item.size() - 1] == 'f';
    int num_num = 0, num_char = 0;

    // 检查计算项不允许数字与字母混用，仅能在数字后使用f
    for (char i_char : item) {
      // 此处仅剩数字，字符，点号，运算符
      if (IsOperator(i_char) || IsDot(i_char) || IsPair(i_char)) {
        continue;
      }
      // 数字，字符数量统计
      if (IsNum(i_char)) {
        num_num++;
      } else {
        num_char++;
      }
    }

    // 仅当只有数字或者只有字符的时候，以及数字和一个结尾的f的时候，才需要进一步检测，否则就是错误的
    if (!((num_num > 0 && num_char == 1 && has_last_f) || num_num == 0 ||
          num_char == 0)) {
      return false;
    }
    // 在上一个限定之后，判定该表达式是否为纯数字表达式或者仅包含f后缀，只有这样才可以计算
    if (GetExpressionState()) {
      // 因为can_calc是一个特例，只要出现=false，就不会改变
      SetPrivateVar((num_num > 0 && num_char == 1 && has_last_f) ||
                    num_char == 0);
    }

    // 开始检查小数情况下的合法性：对每项中的点号检查
    if (item.find('.') == item.rfind('.')) {
      // 只有一个.或者没有点
      if (item.find('.') != string::npos) {
        // 找到一个.
        pos = item.find('.');
        if (pos == 0 || pos == item.size() - 1) {
          return false;
        } else {
          return IsNum(item[pos - 1]) && IsNum(item[pos + 1]);
        }
      }
      // 没有点
    } else {
      return false;
    }
  }

  return true;
}

/*
 * 负数检查
 * 主要原则：向开头的负号，以及左括号后的负号前插0来保证数字不带负号
 * */
vector<string> ExprChecker::NegativeChecker(string in_string) {
  // -a+b和b+(-a)这样的
  for (int i = 0; i < in_string.size() - 1; ++i) {
    if (in_string[i] == '-') {
      if (i == 0 || in_string[i - 1] == '(') {
        in_string.insert(i, 1, '0');
      }
    }
  }

  SetPrivateVar(in_string);
  return GetNumRelationship(in_string);
}

/* 中缀转换后缀表达式
 * 主要原则：
 * 1. 当读到操作数时，立即把它输出，即成为后缀表达式的一部分；
 * 2. 若读到操作符，判断该符号与栈顶符号的优先级:
 *    * 若该符号优先级高于栈顶元素，则将该操作符入栈，
 *    *
 * 否则就一次把栈中运算符弹出并加到后缀表达式尾端，直到遇到优先级低于该操作符的栈元素，然后把该操作符压入栈中
 *    * 对于相同等级的运算符，后来的优先级更低
 * 3. 如果遇到”(”，直接压入栈中
 * 4.
 * 如果遇到一个”)”，那么就将栈元素弹出并加到后缀表达式尾端，但左右括号并不输出。
 * 5. 最后，如果读到中缀表达式的尾端，将栈元素依次完全弹出并加到后缀表达式尾端
 * */
void ExprChecker::TransPostfixExpression(vector<string> &in_string) {
  vector<string> postfix_expr;
  stack<string> operator_stack;
  char item_char;

  // 假设仅包含四则运算和括号
  for (const auto &i : in_string) {
    item_char = i[0];
    if (item_char == '(') {
      operator_stack.push(i);
    } else if (item_char == '+' || item_char == '-') {
      // 不高于栈顶元素，入栈
      if (operator_stack.empty()) {
        operator_stack.push(i);
      } else if (operator_stack.top()[0] == '+' ||
                 operator_stack.top()[0] == '-' ||
                 operator_stack.top()[0] == '(') {
        operator_stack.push(i);
      } else if (operator_stack.top()[0] == '*' ||
                 operator_stack.top()[0] == '/') {
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
    } else if (item_char == '*' || item_char == '/') {
      if (operator_stack.empty()) {
        operator_stack.push(i);
      } else {
        while (operator_stack.top()[0] != '(' &&
               operator_stack.top()[0] != '+' &&
               operator_stack.top()[0] != '-') {
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

/*
 * 计算表达式
 * 主要原则：
 * 仅计算纯数字运算，或者有后缀f的数字项
 * 返回值0 字母式子，1 数字式子，-1 非法式子（除零）
 * */
int ExprChecker::CalcExpression(const vector<string> &in_string,
                                double &result) {
  stack<double> num_stack;
  double top_1 = 0, top_2 = 0;

  if (!GetExpressionState()) {
    // 存在字母的式子是不需要计算的
    // 根据状态can_calc来判定是否需要进一步计算
    return 0;
  }

  // 将变量全部入栈
  for (string i : in_string) {
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
        if (top_1 == 0) {
          return -1;
        }
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
  return 1;
}

/*
 * 最终的运算入口
 * */
void ExprChecker::Run(string in_string) {
  vector<string> item_vec;
  double result = 0;
  int state;
  SetPrivateVar(true);

  if (!FilterWhitespace(in_string)) {
    return;
  }
  if (!IsCharValid(in_string)) {
    cout << "表达式非法" << endl;
    return;
  }
  if (!IsOperatorValid(in_string)) {
    cout << "表达式非法" << endl;
    return;
  }

  item_vec = NegativeChecker(in_string);
  if (!IsFloatValid(item_vec)) {
    cout << "表达式非法" << endl;
    return;
  }

  TransPostfixExpression(item_vec);
  state = CalcExpression(item_vec, result);
  if (state == 1) {
    cout << result << endl;
  } else if (state == 0) {
    cout << "表达式合法" << endl;
  } else if (state == -1) {
    cout << "表达式非法" << endl;
  }
}

int main() {
  string linebuf;
  ExprChecker checker;

  cout << "Try to read the file!" << endl;
  ifstream fin("./exp.txt", ios::in);
  if (fin.is_open()) // 文件成功打开
  {
    while (getline(fin, linebuf)) // line中不包括每行的换行符
    {
      checker.Run(linebuf);
    }
  } else // 文件打开失败
  {
    cout << "File to open file" << endl;
    return -1;
  }

  return 0;
}

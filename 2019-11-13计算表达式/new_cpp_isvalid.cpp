//
// Created by lart on 2019/11/14.
//

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

class StackExprChecker {
private:
  string modified_str;
  bool can_calc;
  string calc_mode;

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

  static bool IsWhiteSpace(char item) {
    return (item == ' ' || item == '\t' || item == '\r' || item == '\n');
  };

  static bool IsSpaceOrTab(char item) { return item == ' ' || item == '\t'; }

  static double String2Number(string in_string);

  static vector<string> GetNumRelationship(const string &in_string);

public:
  void SetPrivateVar(string in_string) { modified_str = std::move(in_string); };

  void SetPrivateVar(bool in_state) { can_calc = in_state; };

  string GetString() { return modified_str; };

  bool GetExpressionState() { return can_calc; };
  string GetCalcMode() { return calc_mode; };

  // todo: 完善更多计算模式的支持
  StackExprChecker(string mode) {
    modified_str = "";
    can_calc = true;
    calc_mode = mode;
    cout << "Init the Checker Class with " << mode << "mode" << endl;
  }

  ~StackExprChecker() { cout << "Destroy the Checker Class..." << endl; };

  // 过滤空白符
  static bool FilterWhitespace(string &in_string);

  bool IsValid(string in_string, vector<string> &item_vec);

  /*
  1. 依次读取字符串字符，转化为后缀表达式
  2. 利用后缀表达式进行求解
  */
  static void TransPostfixExpression(vector<string> &in_string);

  int CalcExpression(const vector<string> &in_string, double &result);

  bool CalcSExpr(string in_string, double &result);

  void Run(string in_string);
};

/*
 * 将数字字符串转化为double浮点数
 * */
double StackExprChecker::String2Number(string in_string) {
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
vector<string> StackExprChecker::GetNumRelationship(const string &in_string) {
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
bool StackExprChecker::FilterWhitespace(string &in_string) {
  string new_string;
  for (char i : in_string) {
    if (!IsWhiteSpace(i)) {
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
void StackExprChecker::TransPostfixExpression(vector<string> &in_string) {
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
 * in_string仅是被FilterWhitespace处理过的字符串
 * */
bool StackExprChecker::IsValid(string in_string, vector<string> &item_vec) {
  stack<char> c_stack;
  bool onlynum = true;
  string new_string;
  int num_lbracket = 0;

  for (int i = 0; i < in_string.size(); ++i) {
    // 保证合法字符
    // if (!(IsOperator(in_string[i]) || IsNum(in_string[i]) ||
    //    IsChar(in_string[i]) || IsDot(in_string[i]) ||
    //    IsPair(in_string[i]))) {
    //    return false;
    //}

    if (i == 0) {
      // 1. 首字符必须是负号配数字或者字母开头或者数字开头
      if (IsChar(in_string[0])) {
        // 开头为字母
        c_stack.push(in_string[0]);
        onlynum = false;
      } else if (in_string[0] == '-') {
        // 开头为-
        // 后必须接数字
        if (!IsNum(in_string[1])) {
          return false;
        } else {
          // 开头负号前插0，负号后为数字
          c_stack.push('0');
          c_stack.push(in_string[0]);
        }
      } else if (IsNum(in_string[0])) {
        // 开头为数字
        c_stack.push(in_string[0]);
      } else if (in_string[0] == '(') {
        // 左括号开头
        c_stack.push(in_string[0]);
        num_lbracket++;
      } else {
        return false;
      }
    } else {
      // i>0的时候：
      // 1. 数字与字母在运算符同侧不可混搭
      // 2. 式子中出现字母（数字之后的f可忽略，或者作为输出的格式限制），
      //    则onlynum=false，不要置为true的语句
      // 3. 括号不可单独存在或者反向存在或者中间没有内容
      // 4. 不能有连续的运算符和点号
      // 5. 运算符左侧必须为数字，字母，括号，右侧必须为数字，字母和右括号
      // 6. 浮点数的点号左侧只能是数字，右侧只能是数字或者f标记
      // 7. (- => (0-
      if (IsOperator(in_string[i])) {
        if (IsOperator(c_stack.top()) || IsDot(c_stack.top())) {
          // 连续两个操作符或者前为点号，非法
          return false;
        } else if (c_stack.top() == '(' && in_string[i] == '-') {
          // (- => (0-
          c_stack.push('0');
          c_stack.push(in_string[i]);
        } else {
          // 单独的操作符
          c_stack.push(in_string[i]);
        }
      } else if (IsDot(in_string[i])) {
        if (!IsNum(c_stack.top())) {
          // 点号前面是字母，括号，点号，运算符，非法
          return false;
        } else {
          // 点号之前是数字
          c_stack.push(in_string[i]);
        }
      } else if (IsNum(in_string[i])) {
        if (IsChar(c_stack.top())) {
          // 数字之前不可以是字母，非法
          return false;
        } else {
          // 数字之前不是字母
          c_stack.push(in_string[i]);
        }
      } else if (IsChar(in_string[i])) {
        if (in_string[i] == 'f') {
          if (IsDot(c_stack.top()) || IsNum(c_stack.top())) {
            // f之前是点号或者数字的时候表示小数
            c_stack.push(in_string[i]);
          } else {
            // f之前不是点号
            c_stack.push(in_string[i]);
            onlynum = false;
          }
        } else {
          // 非f字母前只可以为运算符，字母，括号
          if (IsNum(c_stack.top()) || IsDot(c_stack.top())) {
            // 字母之前不可以是数字或者点号，非法
            return false;
          } else {
            // 可以为运算符，字母，括号
            c_stack.push(in_string[i]);
            onlynum = false;
          }
        }
      } else if (in_string[i] == '(') {
        if (!(IsOperator(c_stack.top()))) {
          // (前面只能是操作符
          return false;
        } else {
          c_stack.push(in_string[i]);
          num_lbracket++;
        }
      } else if (in_string[i] == ')') {
        if (c_stack.top() == '(' || IsOperator(c_stack.top()) ||
            IsDot(c_stack.top())) {
          // (前为(或运算符或点号，非法
          return false;
        } else {
          c_stack.push(in_string[i]);
          num_lbracket--;
        }
      } else {
        return false;
      }
    }
  }
  if (num_lbracket != 0 || IsOperator(c_stack.top()) || IsDot(c_stack.top())) {
    return false;
  }
  SetPrivateVar(onlynum);
  while (!c_stack.empty()) {
    new_string.push_back(c_stack.top());
    c_stack.pop();
  }
  reverse(new_string.begin(), new_string.end());
  item_vec = GetNumRelationship(new_string);

  return true;
}

/*
 * 计算表达式
 * 主要原则：
 * 仅计算纯数字运算，或者有后缀f的数字项
 * 返回值0 字母式子，1 数字式子，-1 非法式子（除零）
 * */
int StackExprChecker::CalcExpression(const vector<string> &in_string,
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
 * S表达式的检查与计算
 * 主要原则：
 * 1. 首为( 尾是)
 * 2. 每个运算符之前都是(，若存在空格可以删除至(
 * 3. 括号都需要匹配
 * 4. 只存在正负数，0，小数，运算符，括号
 * 5. 数字与运算符之间有空格
 * 6. 存在括号的时候必须存在运算符
 * */
bool StackExprChecker::CalcSExpr(string in_string, double &result) {
  string new_string;
  stack<string> calc_stack;
  stack<string> aux_stack;
  int num_lbracket = 0;
  bool cleaned_beginbracket = false;

  // 检查字符，删减空格
  for (int i = 0; i < in_string.size(); ++i) {
    if (!(IsSpaceOrTab(in_string[i]) || IsNum(in_string[i]) ||
          IsDot(in_string[i]) || IsPair(in_string[i]) ||
          IsOperator(in_string[i]))) {
      return false;
    }

    if (i > 0 && IsSpaceOrTab(in_string[i]) && IsSpaceOrTab(in_string[i - 1])) {
      continue;
    }

    // 跳过开头的空格
    if (!cleaned_beginbracket) {
      if (IsSpaceOrTab(in_string[i])) {
        continue;
      } else {
        cleaned_beginbracket = true;
      }
    }

    // 都是合法字符
    // ( -  2 -3 ) => (- 2 (- 0 3))
    if (i > 0 && (in_string[i - 1] == '+' || in_string[i - 1] == '-') &&
        IsNum(in_string[i])) {
      return false;
    } else {
      new_string.push_back(in_string[i]);
    }

    // 检查括号的合法性
    if (in_string[i] == '(') {
      num_lbracket++;
    } else if (in_string[i] == ')') {
      num_lbracket--;
    }
    if (num_lbracket < 0) {
      return false;
    }
  }
  if (num_lbracket != 0) {
    return false;
  }
  if (IsSpaceOrTab(new_string[new_string.size() - 1])) {
    new_string.pop_back();
  }

  cout << new_string << endl;

  string temp_str;
  for (char item : new_string) {
    if (IsSpaceOrTab(item)) {
      calc_stack.push(temp_str);
      temp_str = " ";
      temp_str.pop_back();
      continue;
    }
    temp_str.push_back(item);
  }
  calc_stack.push(temp_str);

  while (!calc_stack.empty()) {
    if (IsOperator(calc_stack.top()[0])) {
      // todo
    } else if () {
    } else {
      aux_stack.push(calc_stack.top());
    }
  }

  result = 0;
  return true;
}

/*
 * 最终的运算入口
 * */
void StackExprChecker::Run(string in_string) {
  vector<string> item_vec;
  double result = 0;
  int state;

  if (GetCalcMode() == "default") {
    SetPrivateVar(true);
    if (!FilterWhitespace(in_string)) {
      return;
    }
    if (!IsValid(in_string, item_vec)) {
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
  } else if (GetCalcMode() == "Sexpr") {
    cout << "正在使用Sepr模式" << endl;
    if (!CalcSExpr(in_string, result)) {
      cout << "S表达式非法" << endl;
    } else {
      cout << "S表达式 " << in_string << " 的结果为：" << result << endl;
    }
  } else {
    cout << "未实现的运算模式" << endl;
    return;
  }
}

int main() {
  string linebuf;
  StackExprChecker checker("Sexpr");

  cout << "尝试读取文件..." << endl;
  ifstream fin("./exp.txt", ios::in);
  if (fin.is_open()) // 文件成功打开
  {
    cout << "成功打开文件，开始处理..." << endl;
    while (getline(fin, linebuf)) // line中不包括每行的换行符
    {
      cout << "原始字符串：" << linebuf << endl;
      checker.Run(linebuf);
    }
  } else // 文件打开失败
  {
    cout << "文件打开失败，请检查文件是否存在..." << endl;
    return -1;
  }

  return 0;
}

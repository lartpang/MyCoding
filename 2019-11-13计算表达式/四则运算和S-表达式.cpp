/*
 * @Author: Lart Pang
 * @Date: 2019/11/14
 * @Discribe:
 * 1. 四则运算（Default模式）：利用后缀表达式配合堆栈结构来进行计算
 * 1.1 整数、小数(可以尾随f)，以及字符常量(a~z、A~Z)的加减乘除形式
 * 1.2 可以使用括号调整计算顺序
 * 1.3 对于各种不合法的输入形式进行了判定，不合法的输出位“表达式非法”
 * 1.4 对于表达式中包含非小数标识符f的字母的时候，判定为不可运算表达式，
 * 会输出“表达式合法”的字样
 * 1.5 对于数字表达式会输出对应的运算结果
 * 1.6 在exp.txt中，当行内存在;号的时候，仅计算;之前的部分
 * 1.7 在exp.txt中，当以//开头的行，会认为是注释，会被跳过
 * 2. S-表达式（Sexpr模式）：通过将S-表达式转化为通常使用的表达式，
 * 之后再利用1中的流程进行处理
 * 2.1 对于基本的两个数字的S表达式运算给予了支持，S-表达式例如(Operator num1
 * num2)、(Operator num1)的形式
 * 2.2 仅支持数字之间的运算，不能使用任何字母，可以有小数点
 * 2.3 对于非法输入进行了判定与报错，提示“表达式非法”，合法计算会输出结果
 * 2.4 不支持注释，但是可以随便写一点非S-表达式作为注释，会对应输出表达式非法
 * @Usage: 请使用我提供的相同文件夹下的exp.txt测试：
 *      Default模式，请删除文件中“// S-表达式测试”之后内容
 *      Sexpr模式请删除该行及之前的内容
 */

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
  explicit StackExprChecker(string mode) {
    modified_str = "";
    can_calc = true;
    calc_mode = mode;
    cout << "Init the Checker Class with " << mode << " mode" << endl;
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

  static bool CalcSExpr(string in_string, double &result);

  static int SExpr2MExpr(string &in_string);

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
      // 5.
      // 运算符左侧必须为数字，字母，括号，或者点号，右侧必须为数字，字母和右括号
      // 6. 浮点数的点号左侧只能是数字，右侧只能是数字或者f标记
      // 7. (- => (0-
      if (IsOperator(in_string[i])) {
        if (IsOperator(c_stack.top())) {
          // 连续两个操作符，非法
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
        if (!(IsOperator(c_stack.top()) || c_stack.top() == '(')) {
          // (前面只能是操作符或者左括号
          return false;
        } else {
          c_stack.push(in_string[i]);
          num_lbracket++;
        }
      } else if (in_string[i] == ')') {
        if (c_stack.top() == '(' || IsOperator(c_stack.top())) {
          // )前为(或运算符，非法
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
  if (num_lbracket != 0 || IsOperator(c_stack.top())) {
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
 * S表达式转化为中缀表达式，之后可以借用TransPostfixExpression来计算
 **/
int StackExprChecker::SExpr2MExpr(string &in_string) {
  // 检查字符，检查括号匹配，插入必要的空格
  int num_lbracket = 0;
  bool has_cleaned = false;
  string valid_char_str;
  for (int i = 0; i < in_string.size(); ++i) {
    if (!(IsSpaceOrTab(in_string[i]) || IsNum(in_string[i]) ||
          IsDot(in_string[i]) || IsPair(in_string[i]) ||
          IsOperator(in_string[i]))) {
      return -1;
    }
    valid_char_str.push_back(in_string[i]);
    // 检查括号的合法性
    if (in_string[i] == '(') {
      if (valid_char_str.size() > 1 &&
          !IsSpaceOrTab(valid_char_str[valid_char_str.size() - 2])) {
        valid_char_str.pop_back();
        valid_char_str.push_back(' ');
        valid_char_str.push_back(in_string[i]);
        valid_char_str.push_back(' ');
      } else if (i < in_string.size() - 1 && !IsSpaceOrTab(in_string[i + 1])) {
        valid_char_str.push_back(' ');
      }
      num_lbracket++;
    } else if (in_string[i] == ')') {
      if (!IsSpaceOrTab(valid_char_str[valid_char_str.size() - 2])) {
        valid_char_str.pop_back();
        valid_char_str.push_back(' ');
        valid_char_str.push_back(in_string[i]);
        valid_char_str.push_back(' ');
      }
      num_lbracket--;
    }
    if (num_lbracket < 0) {
      // 先有了右括号
      return -1;
    }
  }
  if (num_lbracket != 0) {
    return -1;
  }

  // 删除多余的空格
  string cleaned_space_str;
  for (int i = 0; i < valid_char_str.size(); ++i) {
    // 连续的空格仅保留一个
    if (i > 0 && IsSpaceOrTab(valid_char_str[i]) &&
        IsSpaceOrTab(valid_char_str[i - 1])) {
      continue;
    }
    // 跳过开头的空格
    if (!has_cleaned) {
      if (IsSpaceOrTab(valid_char_str[i])) {
        continue;
      } else {
        has_cleaned = true;
      }
    }
    cleaned_space_str.push_back(valid_char_str[i]);
  }
  if (IsSpaceOrTab(cleaned_space_str[cleaned_space_str.size() - 1])) {
    cleaned_space_str.pop_back();
  }

  // 跳过空行和注释行
  bool all_whitesapce = true;
  for (char item : cleaned_space_str) {
    if (all_whitesapce && !IsSpaceOrTab(item)) {
      all_whitesapce = false;
    }
  }
  if (all_whitesapce) {
    return 0;
  }

  // 检查单项运算，将所有的单项转化为括号形式
  bool non_end = false;
  bool need_more = false;
  bool need_more_if = false;
  string non_single_str;

  non_single_str.push_back(cleaned_space_str[0]);
  for (int i = 1; i < cleaned_space_str.size(); ++i) {
    if (cleaned_space_str.size() - i >= 3 && cleaned_space_str[i - 1] == '(' &&
        IsSpaceOrTab(cleaned_space_str[i]) &&
        (cleaned_space_str[i + 1] == '+' || cleaned_space_str[i + 1] == '-') &&
        IsNum(cleaned_space_str[i + 2])) {
      // 特定错误模板检查
      return -1;
    }

    if (cleaned_space_str.size() - i >= 4 && cleaned_space_str[i - 1] == '(' &&
        IsSpaceOrTab(cleaned_space_str[i]) &&
        IsOperator(cleaned_space_str[i + 1]) &&
        IsSpaceOrTab(cleaned_space_str[i + 2]) &&
        IsNum(cleaned_space_str[i + 3])) {
      need_more = true;
    } else if (need_more) {
      // 前面是数字
      if (cleaned_space_str.size() - i >= 3 &&
          (IsNum(cleaned_space_str[i]) || IsDot(cleaned_space_str[i]))) {
        if (IsSpaceOrTab(cleaned_space_str[i + 1]) &&
            cleaned_space_str[i + 2] == ')') {
          need_more = false;
          need_more_if = true;
        } else if (IsSpaceOrTab(cleaned_space_str[i + 1]) &&
                   (IsOperator(cleaned_space_str[i + 2]) ||
                    IsNum(cleaned_space_str[i + 2]))) {
          need_more = false;
        } else if (!(IsDot(cleaned_space_str[i]) ||
                     IsNum(cleaned_space_str[i]))) {
          return -1;
        }
      }
    }

    if ((cleaned_space_str[i - 1] == '+' || cleaned_space_str[i - 1] == '-') &&
        IsNum(cleaned_space_str[i])) {
      non_single_str.pop_back();
      non_single_str.push_back('(');
      non_single_str.push_back(' ');
      non_single_str.push_back(cleaned_space_str[i - 1]);
      non_single_str.push_back(' ');
      non_single_str.push_back('0');
      non_single_str.push_back(' ');
      non_single_str.push_back(cleaned_space_str[i]);
      non_end = true;
    } else if (non_end) {
      if (IsNum(cleaned_space_str[i]) || IsDot(cleaned_space_str[i])) {
        non_single_str.push_back(cleaned_space_str[i]);
      } else {
        non_single_str.push_back(' ');
        non_single_str.push_back(')');
        non_single_str.push_back(cleaned_space_str[i]);
        non_end = false;
      }
    } else {
      non_single_str.push_back(cleaned_space_str[i]);
    }
  }
  // 前面的操作可能会缺一个末尾的括号
  if (non_single_str[non_single_str.size() - 1] != ')') {
    non_single_str.push_back(' ');
    non_single_str.push_back(')');
  }

  // 主要针对 (+ 1) (- 1) (* 1) (/ 1) 这样的形式
  if (need_more_if) {
    for (int i = 0; i < non_single_str.size(); ++i) {
      if (IsOperator(non_single_str[i])) {
        if (non_single_str[i] == '-' || non_single_str[i] == '+') {
          non_single_str.insert(i + 1, 1, ' ');
          non_single_str.insert(i + 2, 1, '0');
        } else if (non_single_str[i] == '*' || non_single_str[i] == '/') {
          non_single_str.insert(i + 1, 1, ' ');
          non_single_str.insert(i + 2, 1, '1');
        }
      }
    }
  }

  // 检查S表达式中运算符的合法性
  string over_str;
  over_str.push_back(non_single_str[0]);
  for (int i = 1; i < non_single_str.size(); ++i) {
    if (IsOperator(non_single_str[i - 1]) && !IsSpaceOrTab(non_single_str[i])) {
      // 运算符右侧必须是空格
      return -1;
    } else {
      over_str.push_back(non_single_str[i]);
    }
  }

  // S表达式转化为中缀表达式，前面需要保证:
  vector<string> aux_vec;
  string temp_str;
  for (char item_c : over_str) {
    temp_str.push_back(item_c);
    if (IsSpaceOrTab(item_c)) {
      temp_str.pop_back();
      aux_vec.push_back(temp_str);
      temp_str = " ";
      temp_str.pop_back();
      continue;
    }
  }
  aux_vec.push_back(temp_str);

  for (int i = 0; i < aux_vec.size() - 1; ++i) {
    if (IsOperator(aux_vec[i][0]) && IsNum(aux_vec[i + 1][0])) {
      temp_str = aux_vec[i];
      aux_vec[i] = aux_vec[i + 1];
      aux_vec[i + 1] = temp_str;
      i++;
    }
  }

  // 将vector中的各项string拼接在一起
  string out_str;
  for (const string &item_str : aux_vec) {
    out_str += item_str;
  }

  in_string = out_str;
  return 1;
}

/*
 * 最终的运算入口
 * */
void StackExprChecker::Run(string in_string) {
  vector<string> item_vec;
  double result = 0;
  int state;
  if (GetCalcMode() == "Default") {
    SetPrivateVar(true);
    if (!FilterWhitespace(in_string)) {
      return;
    }
  } else if (GetCalcMode() == "Sexpr") {
    state = SExpr2MExpr(in_string);
    if (state == -1) {
      cout << "表达式非法" << endl;
      return;
    } else if (state == 0) {
      return;
    }
    // state == 1的时候继续
  } else {
    cout << "未实现的运算模式" << endl;
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
}

/*
 * @param: mode，
 *      计算模式选择：
 *      1. "Default": 默认四则运算
 *      2. "Sexpr": S-表达式
 * */
int main() {
  string linebuf;
  StackExprChecker checker("Default");

  cout << "尝试读取文件..." << endl;
  ifstream fin("./exp.txt", ios::in);
  if (fin.is_open()) {
    cout << "成功打开文件，开始处理..." << endl;
    while (getline(fin, linebuf)) // line中不包括每行的换行符
    {
      cout << "原始字符串：" << linebuf << endl;
      checker.Run(linebuf);
    }
  } else {
    cout << "文件打开失败，请检查文件是否存在..." << endl;
    return -1;
  }

  return 0;
}

#include "toylang/regex.h"

#include <map>
#include <tuple>
#include <variant>
#include <vector>

// #include "toylang/hex.h"

namespace toylang::regex {

namespace {

/** 字符集 */
std::set<char> char_range_digit = {'0', '1', '2', '3', '4',
                                   '5', '6', '7', '8', '9'};
std::set<char> char_range_lower = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                                   'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                                   's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
std::set<char> char_range_upper = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
                                   'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                                   'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
std::set<char> char_range_punct = {'!', '"', '#', '$', '%', '&', '\'', '(',
                                   ')', '*', '+', ',', '-', '.', '/',  ':',
                                   ';', '<', '=', '>', '?', '@', '[',  '\\',
                                   ']', '^', '_', '`', '{', '|', '}',  '~'};
std::set<char> char_range_space = {'\t', '\n', '\v', '\f', '\r', ' '};
std::set<char> char_range_word = {
    '_', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
    'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
    'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
    'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

/** 转义字符映射表 */
std::map<char, std::string> str_escape_table = {
    {'\a', "\\a"},  {'\b', "\\b"},  {'\f', "\\f"}, {'\n', "\\n"},
    {'\r', "\\r"},  {'\t', "\\t"},  {'\v', "\\v"}, {'\\', "\\\\"},
    {'\'', "\\\'"}, {'\"', "\\\""}, {'?', "\\?"},  {'*', "\\*"},
    {'+', "\\+"},   {'|', "\\|"},   {'(', "\\("},  {')', "\\)"},
    {'[', "\\["},   {']', "\\]"},   {'\0', "\\0"}};

/** 转义字符映射表 */
std::map<char, char> char_escape_table = {{'a', '\a'}, {'b', '\b'},
                                          {'f', '\f'}, {'n', '\n'},
                                          {'r', '\r'}, {'t', '\t'}};

/** 字符类映射表 */
std::map<char, std::tuple<RangeNode::Direction, std::set<char>>>
    char_class_table = {
        {'D', {RangeNode::kNegative, char_range_digit}},
        {'d', {RangeNode::kPositive, char_range_digit}},
        {'L', {RangeNode::kNegative, char_range_lower}},
        {'l', {RangeNode::kPositive, char_range_lower}},
        {'P', {RangeNode::kNegative, char_range_punct}},
        {'p', {RangeNode::kPositive, char_range_punct}},
        {'S', {RangeNode::kNegative, char_range_space}},
        {'s', {RangeNode::kPositive, char_range_space}},
        {'U', {RangeNode::kNegative, char_range_upper}},
        {'u', {RangeNode::kPositive, char_range_upper}},
        {'W', {RangeNode::kNegative, char_range_word}},
        {'w', {RangeNode::kPositive, char_range_word}},
};

std::set<char> operator_table = {'*', '+', '?', '|', '(', ')'};

}  // namespace

/** 泛型输入单元 */
struct Unit {
  std::variant<char, Regex> value_;

  bool IsChar() const { return std::holds_alternative<char>(value_); }

  bool IsNode() const { return std::holds_alternative<Regex>(value_); }

  char GetChar() const { return std::get<char>(value_); }

  Regex GetNode() const { return std::get<Regex>(value_); }

  bool operator==(char ch) { return IsChar() && GetChar() == ch; }

  bool operator!=(char ch) { return !(*this == ch); }
};

/**
 * 从start位置分析一个由方括号引领的字符类表达式，成功则原地归约，失败则抛出异常
 */
void ScanRange(std::vector<Unit>& input, size_t const start) {
  int state = 1;
  char range_left = 0;
  const auto node = std::make_shared<RangeNode>();

  if (input[start] != '[') {
    throw std::runtime_error("ScanRange: invalid start char");
  } else {
    input.erase(input.begin() + start);
  }

  if (input[start] == '^') {
    node->dir_ = RangeNode::kNegative;
    input.erase(input.begin() + start);
  } else {
    node->dir_ = RangeNode::kPositive;
  }

  while (state > 0) {
    if (!input[start].IsChar()) {
      throw std::runtime_error("ScanRange: invalid input in range");
    }

    auto ch = input[start].GetChar();
    switch (state) {
      case 1: {
        if (ch == ']') {
          state = 0;
        } else if (ch == '\\') {
          state = 2;
        } else if (ch == '-' && range_left != 0) {
          state = 3;
        } else {
          range_left = ch;
          node->set_.insert(range_left);
        }
      } break;
      case 2: {  // 转义字符
        if (auto it = char_escape_table.find(ch);
            it != char_escape_table.end()) {
          node->set_.insert(it->second);
          range_left = it->second;
        } else if (auto it = char_class_table.find(ch);
                   it != char_class_table.end()) {
          auto const& [dir, set] = it->second;
          if (dir == RangeNode::kNegative) {
            throw std::runtime_error("ScanRange: negative char class in range");
          }

          node->set_.insert(set.begin(), set.end());
        } else {
          node->set_.insert(ch);
          range_left = ch;
        }
        state = 1;
      } break;
      case 3: {  // 字符范围
        if (ch == ']') {
          node->set_.insert('-');
          state = 0;
        } else if (ch == '\\') {
          state = 4;
        } else if (ch < range_left) {
          for (auto c = ch; c <= range_left; ++c) node->set_.insert(c);
          range_left = 0;
          state = 1;
        } else {
          for (auto c = range_left; c <= ch; ++c) node->set_.insert(c);
          range_left = 0;
          state = 1;
        }
      } break;
      case 4: {
        auto it = char_escape_table.find(ch);
        if (it != char_escape_table.end()) ch = it->second;

        if (ch < range_left) {
          for (auto c = ch; c <= range_left; ++c) node->set_.insert(c);
        } else {
          for (auto c = range_left; c <= ch; ++c) node->set_.insert(c);
        }

        range_left = 0;
        state = 1;
      } break;
    }

    input.erase(input.begin() + start);
  }

  if (state != 0) {
    throw std::runtime_error("ScanRange: endless range");
  }

  if (node->set_.empty()) {
    throw std::runtime_error("ScanRange: empty range");
  }

  input.insert(input.begin() + start, {node});
}

/**
 * 做词法分析，将字符和转义字符都归约成节点
 */
void Scan(std::vector<Unit>& input) {
  size_t i = 0;
  while (i < input.size()) {
    if (input[i].GetChar() == '[') {
      ScanRange(input, i);
    } else if (input[i].GetChar() == '\\') {
      if (i + 1 >= input.size()) {
        throw std::runtime_error("invalid escape sequence");
      }

      input.erase(input.begin() + i);
      if (auto it = char_escape_table.find(input[i].GetChar());
          it != char_escape_table.end()) {
        auto const node = std::make_shared<CharNode>();
        node->ch_ = it->second;
        input[i].value_ = node;
      } else if (auto it = char_class_table.find(input[i].GetChar());
                 it != char_class_table.end()) {
        auto const& [dir, set] = it->second;

        auto const node = std::make_shared<RangeNode>();
        node->dir_ = dir;
        node->set_.insert(set.begin(), set.end());
        input[i].value_ = node;
      } else {
        auto const node = std::make_shared<CharNode>();
        node->ch_ = input[i].GetChar();
        input[i].value_ = node;
      }
    } else if (input[i].GetChar() == '.') {
      auto const node = std::make_shared<RangeNode>();
      node->dir_ = RangeNode::kNegative;
      input[i].value_ = node;
    } else if (!operator_table.count(input[i].GetChar())) {
      auto const node = std::make_shared<CharNode>();
      node->ch_ = input[i].GetChar();
      input[i].value_ = node;
    }
    ++i;
  }
}

/**
 * 解析正则表达式
 *  1. 递归处理括号，将分析完毕的表达式在原地归约
 *  2. 处理后缀运算符，将分析完毕的表达式在原地归约
 *  3. 处理连接，将分析完毕的表达式在原地归约
 *  4. 处理或运算符，将分析完毕的表达式在原地归约
 */
void Parse(std::vector<Unit>& input, size_t const start = 0UL,
           size_t const depth = 0UL) {
  // 1. 递归处理括号，将分析完毕的表达式在原地归约
  for (size_t i = start; i < input.size(); ++i) {
    if (input[i] == ')') {
      if (depth == 0) {
        throw std::runtime_error("Parse: unmatched ')'");
      }

      break;
    }else if (input[i] == '(') {
      input.erase(input.begin() + i);
      Parse(input, i, depth + 1);
      if (i + 1 >= input.size() || input[i + 1] != ')') {
        throw std::runtime_error("Parse: missing ')'");
      }
      input.erase(input.begin() + i + 1);
    }
  }

  // 2. 处理后缀运算符，将分析完毕的表达式在原地归约
  for (size_t i = start; i < input.size() && input[i] != ')'; ++i) {
    if (input[i] == '*') {
      if (i == start || !input[i - 1].IsNode()) {
        throw std::runtime_error("Parse: missing operand");
      }

      auto const node = std::make_shared<KleeneNode>();
      node->child_ = input[--i].GetNode();
      input.erase(input.begin() + i);
      input[i].value_ = node;
    } else if (input[i] == '+') {
      if (i == start || !input[i - 1].IsNode()) {
        throw std::runtime_error("Parse: missing operand");
      }

      auto const node = std::make_shared<PositiveNode>();
      node->child_ = input[--i].GetNode();
      input.erase(input.begin() + i);
      input[i].value_ = node;
    } else if (input[i] == '?') {
      if (i == start || !input[i - 1].IsNode()) {
        throw std::runtime_error("Parse: missing operand");
      }

      auto const node = std::make_shared<OptionalNode>();
      node->child_ = input[--i].GetNode();
      input.erase(input.begin() + i);
      input[i].value_ = node;
    }
  }

  // 3. 处理连接，将分析完毕的表达式在原地归约
  for (size_t i = start; i < input.size() && input[i] != ')'; ++i) {
    if (i <= start || !input[i].IsNode() || !input[i - 1].IsNode()) continue;

    auto const node = std::make_shared<ConcatNode>();
    node->right_ = input[i].GetNode();
    node->left_ = input[--i].GetNode();
    input[i].value_ = node;
    input.erase(input.begin() + i + 1);
  }

  // 4. 处理或运算符，将分析完毕的表达式在原地归约
  for (size_t i = start; i < input.size() && input[i] != ')'; ++i) {
    if (input[i] != '|') continue;

    if (i == start || !input[i - 1].IsNode() || i >= input.size() ||
        !input[i + 1].IsNode()) {
      throw std::runtime_error("Parse: missing operand");
    }

    auto const node = std::make_shared<UnionNode>();
    input.erase(input.begin() + i);
    node->right_ = input[i].GetNode();
    node->left_ = input[--i].GetNode();
    input[i].value_ = node;
    input.erase(input.begin() + i + 1);
  }
}

Regex Compile(std::string const& expression) {
  std::vector<Unit> input;
  for (auto const ch : expression) {
    input.push_back({ch});
  }

  Scan(input);
  Parse(input);

  if (input.size() != 1 || !input[0].IsNode()) {
    throw std::runtime_error("invalid expression");
  }

  return input[0].GetNode();
}

}  // namespace toylang::regex
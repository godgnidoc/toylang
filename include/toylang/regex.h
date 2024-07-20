#ifndef __TOYLANG_REGEX_H__
#define __TOYLANG_REGEX_H__

#include <memory>
#include <set>
#include <stdexcept>
#include <string>

namespace toylang {

namespace regex {
struct Node;
}
using Regex = std::shared_ptr<regex::Node>;

namespace regex {

/** 正则表达式节点 */
struct Node {
  enum Type {
    /** 字符 */
    kChar,

    /** 字符类 */
    kRange,

    /** 串 */
    kConcat,

    /** 联合 */
    kUnion,

    /** 克林闭包 */
    kKleene,

    /** 正闭包 */
    kPositive,

    /** 可选 */
    kOptional,
  };

  virtual ~Node() = default;
  virtual Type type() const = 0;
};

/**
 * 单个字符节点
 */
struct CharNode : public Node {
  char ch_;

  Type type() const override { return kChar; }
};

/**
 * 字符类节点
 *
 * 支持正向匹配和反向匹配
 * 例如：[a-z]和[^a-z]
 *
 * 支持简写
 * \d 数字 == [0-9]
 * \D 非数字 == [^0-9]
 * \l 小写字母 == [a-z]
 * \L 非小写字母 == [^a-z]
 * \p 标点符号 == [!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~]
 * \s 空白字符 == [\t\n\v\f\r ]
 * \u 大写字母 == [A-Z]
 * \U 非大写字母 == [^A-Z]
 * \w 单词字符 == [a-zA-Z0-9_]
 * \W 非单词字符 == [^a-zA-Z0-9_]
 */
struct RangeNode : public Node {
  enum Direction { kNegative, kPositive };

  Direction dir_;
  std::set<char> set_;

  Type type() const override { return kRange; }
};

struct ConcatNode : public Node {
  Regex left_;
  Regex right_;

  Type type() const override { return kConcat; }
};

struct UnionNode : public Node {
  Regex left_;
  Regex right_;

  Type type() const override { return kUnion; }
};

struct KleeneNode : public Node {
  Regex child_;

  Type type() const override { return kKleene; }
};

struct PositiveNode : public Node {
  Regex child_;

  Type type() const override { return kPositive; }
};

struct OptionalNode : public Node {
  Regex child_;

  Type type() const override { return kOptional; }
};

/**
 * 编译正则表达式
 *
 * @param expr 正则表达式
 */
Regex Compile(std::string const& expr);

}  // namespace regex
}  // namespace toylang

#endif
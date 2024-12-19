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

struct LeafNode;
using LeafNodes = std::set<std::shared_ptr<LeafNode>,
                           std::owner_less<std::shared_ptr<LeafNode>>>;

/** 正则表达式节点 */
struct Node : std::enable_shared_from_this<Node> {
  enum Type {
    /** 接受 */
    kAccept,

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

  virtual bool GetNullable() = 0;
  virtual LeafNodes GetFirstpos() = 0;
  virtual LeafNodes GetLastpos() = 0;
  virtual void CalcFollowpos() = 0;
};

struct LeafNode : public Node {
  /**
   * 后继位置集合
   */
  LeafNodes followpos_;

  void CalcFollowpos() final;
  virtual bool Match(char input) = 0;
};

struct AcceptNode : public LeafNode {
  int token_id_;

  Type type() const override { return kAccept; }

  AcceptNode(int token_id) : token_id_{token_id} {}
  bool GetNullable() override;
  LeafNodes GetFirstpos() override;
  LeafNodes GetLastpos() override;
  bool Match(char input) override;
};

/**
 * 单个字符节点
 */
struct CharNode : public LeafNode {
  char ch_;

  Type type() const override { return kChar; }

  bool GetNullable() override;
  LeafNodes GetFirstpos() override;
  LeafNodes GetLastpos() override;
  bool Match(char input) override;
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
struct RangeNode : public LeafNode {
  enum Direction { kNegative, kPositive };

  Direction dir_;
  std::set<char> set_;
  std::string writing_;

  Type type() const override { return kRange; }

  bool GetNullable() override;
  LeafNodes GetFirstpos() override;
  LeafNodes GetLastpos() override;
  bool Match(char input) override;
};

struct ConcatNode : public Node {
  Regex left_;
  Regex right_;

  Type type() const override { return kConcat; }

  bool GetNullable() override;
  LeafNodes GetFirstpos() override;
  LeafNodes GetLastpos() override;
  void CalcFollowpos() override;
};

struct UnionNode : public Node {
  Regex left_;
  Regex right_;

  Type type() const override { return kUnion; }

  bool GetNullable() override;
  LeafNodes GetFirstpos() override;
  LeafNodes GetLastpos() override;
  void CalcFollowpos() override;
};

struct KleeneNode : public Node {
  Regex child_;

  Type type() const override { return kKleene; }

  bool GetNullable() override;
  LeafNodes GetFirstpos() override;
  LeafNodes GetLastpos() override;
  void CalcFollowpos() override;
};

struct PositiveNode : public Node {
  Regex child_;

  Type type() const override { return kPositive; }

  bool GetNullable() override;
  LeafNodes GetFirstpos() override;
  LeafNodes GetLastpos() override;
  void CalcFollowpos() override;
};

struct OptionalNode : public Node {
  Regex child_;

  Type type() const override { return kOptional; }

  bool GetNullable() override;
  LeafNodes GetFirstpos() override;
  LeafNodes GetLastpos() override;
  void CalcFollowpos() override;
};

/**
 * 编译正则表达式
 *
 * @param expr 正则表达式
 */
Regex Compile(std::string const& expr);

/**
 * 将两个正则表达式使用或运算连接
 *
 * @param lhs 左正则表达式
 * @param rhs 右正则表达式
 */
Regex Union(Regex const& lhs, Regex const& rhs);

/**
 * 为正则表达式标记接受节点
 * @param regex 正则表达式
 * @param token_id 接受的token id
 */
std::shared_ptr<regex::AcceptNode> Accept(Regex const& regex, int token_id);

}  // namespace regex
}  // namespace toylang

#endif
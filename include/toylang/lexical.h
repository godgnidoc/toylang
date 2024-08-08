#ifndef __TOYLANG_LEXICAL_H__
#define __TOYLANG_LEXICAL_H__

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "toylang/regex.h"
#include "toylang/token.h"

namespace toylang {

/**
 * Lexicon 类是对一套词法规则的抽象
 * Lexicon 总是尽可能匹配最长的词法单元
 * 若有多个词法单元匹配同一段文本，则优先匹配先添加的词法单元
 * Lexicon 可以基于上下文提供不同的词法分析策略
 */
class Lexicon {
 public:
  /**
   * 词法规则实现
   */
  struct Impl;

  /**
   * 词法规则构造器
   */
  class Builder;

  static constexpr auto kDefaultContext = "default";

 public:
  Lexicon(std::unique_ptr<Impl>&& impl);
  Lexicon(Lexicon const&) = delete;
  Lexicon(Lexicon&&) = delete;
  Lexicon& operator=(Lexicon const&) = delete;
  Lexicon& operator=(Lexicon&&) = delete;
  ~Lexicon() = default;

  /**
   * 获取词法记号名称
   *
   * @param token 词法记号ID
   */
  std::string NameOfToken(int token) const;

  /**
   * 获取词法记号ID
   *
   * @param name 词法记号名称
   */
  int IdOfToken(std::string const& name) const;

  /**
   * 获取所有词法记号名称
   */
  std::vector<std::string> ListTokens() const;

  /**
   * 统计词法记号数量
   */
  int CountTokens() const;

  /**
   * 获取上下文名称
   *
   * @param id 上下文ID
   */
  std::string NameOfContext(int id) const;

  /**
   * 获取上下文ID
   *
   * @param name 上下文名称
   */
  int IdOfContext(std::string const& name) const;

  /**
   * 获取所有上下文名称
   */
  std::vector<std::string> ListContexts() const;

  /**
   * 获取指定状态可接受的词法记号
   *
   * @param state 状态ID
   */
  std::optional<int> AcceptOfState(int state) const;

  /**
   * 获取指定状态的转移
   *
   * @param state 状态ID
   * @param input 输入
   */
  std::optional<int> TransferOfState(int state, int input) const;

 private:
  std::unique_ptr<Impl const> impl_;
};

/**
 * 词法分析器，加载词法规则和源码后，可以提取Token
 */
class Scanner {
 public:
  Scanner();
  Scanner(const Scanner&) = delete;
  Scanner(Scanner&&) = delete;
  Scanner& operator=(const Scanner&) = delete;
  Scanner& operator=(Scanner&&) = delete;
  ~Scanner() = default;

  /**
   * 设置词法规则，将会清空上下文
   *
   * @param lexicon 词法规则
   */
  void SetLexicon(std::shared_ptr<Lexicon const> lexicon);

  /**
   * 设置源码，将会清空位置缓冲
   *
   * @param source 源码
   */
  void SetSource(std::shared_ptr<Source const> source);

  /**
   * 修改上下文
   *
   * @param context 上下文ID
   */
  void SetContext(int context);

  /**
   * 修改上下文
   *
   * @param context 上下文名称
   */
  void SetContext(std::string const& context);

  /**
   * 提取下一个Token
   */
  Token NextToken();

 private:
  /**
   * 当前上下文
   */
  int context_;

  /**
   * 当前行号
   */
  size_t line_;
  /**
   * 当前列号
   */
  size_t column_;
  /**
   * 当前偏移量
   */
  size_t offset_;

  /**
   * 词法规则
   */
  std::shared_ptr<Lexicon const> lexicon_;
  /**
   * 源码
   */
  std::shared_ptr<Source const> source_;
};

/**
 * 词法规则构造器
 */
class Lexicon::Builder {
  struct Building;

 public:
  Builder();
  Builder(Builder const&) = delete;
  Builder(Builder&&) = delete;
  Builder& operator=(Builder const&) = delete;
  Builder& operator=(Builder&&) = delete;
  ~Builder();

  /**
   * 定义一个词法单元
   * 先添加的词法单元优先级更高
   *
   * @param name 词法单元名称
   * @param pattern 正则表达式
   * @param context 词法单元所属上下文，省略表示任意上下文
   */
  Builder& DefineToken(
      std::string const& name, Regex pattern,
      std::optional<std::set<std::string>> const& context = std::nullopt);

  /**
   * 完成词法规则构造
   */
  std::shared_ptr<Lexicon> Build();

 private:
  std::unique_ptr<Building> building_;
};

}  // namespace toylang

#endif
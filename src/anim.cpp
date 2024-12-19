#include "toylang/anim.h"

#include <iostream>

#include "nlohmann/json.hpp"
#include "spdlog/fmt/fmt.h"
#include "toylang/lexical.h"
#include "toylang/regex.h"

namespace toylang {

template <typename _to, typename _from>
std::shared_ptr<_to> to(std::shared_ptr<_from> const& ptr) {
  return std::dynamic_pointer_cast<_to>(ptr);
}

template <typename T, std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
std::string hex(T const& value) {
  return fmt::format("{:#x}", value);
}

template <typename T>
std::string hex(std::shared_ptr<T> const& ptr) {
  return hex((uint64_t)ptr.get());
}

void anim(nlohmann::json const& json) {
  fmt::print(stderr, "ANIM: {}\n", json.dump());
}

nlohmann::json jsonify(regex::RangeNode::Direction dir) {
  switch (dir) {
    case regex::RangeNode::kNegative:
      return "negative";
    case regex::RangeNode::kPositive:
      return "positive";
  }
  throw std::runtime_error{"jsonify: unknown direction"};
}

nlohmann::json jsonify(Regex regex) {
  if (auto node = to<regex::AcceptNode>(regex); node) {
    return nlohmann::json{
        {"id", hex(node)},
        {"type", "accept"},
        {"tokenId", node->token_id_},
    };
  } else if (auto node = to<regex::CharNode>(regex); node) {
    return nlohmann::json{
        {"id", hex(node)},
        {"type", "char"},
        {"char", std::string(1, node->ch_)},
    };
  } else if (auto node = to<regex::RangeNode>(regex); node) {
    return nlohmann::json{
        {"id", hex(node)},
        {"type", "range"},
        {"dir", jsonify(node->dir_)},
        {"set", node->writing_},
    };
  } else if (auto node = to<regex::ConcatNode>(regex); node) {
    return nlohmann::json{
        {"id", hex(node)},
        {"type", "concat"},
        {"lhs", jsonify(node->left_)},
        {"rhs", jsonify(node->right_)},
    };
  } else if (auto node = to<regex::UnionNode>(regex); node) {
    return nlohmann::json{
        {"id", hex(node)},
        {"type", "union"},
        {"lhs", jsonify(node->left_)},
        {"rhs", jsonify(node->right_)},
    };
  } else if (auto node = to<regex::KleeneNode>(regex); node) {
    return nlohmann::json{
        {"id", hex(node)},
        {"type", "kleene"},
        {"sub", jsonify(node->child_)},
    };
  } else if (auto node = to<regex::PositiveNode>(regex); node) {
    return nlohmann::json{
        {"id", hex(node)},
        {"type", "positive"},
        {"sub", jsonify(node->child_)},
    };
  } else if (auto node = to<regex::OptionalNode>(regex); node) {
    return nlohmann::json{
        {"id", hex(node)},
        {"type", "optional"},
        {"sub", jsonify(node->child_)},
    };
  } else {
    throw std::runtime_error{"jsonify: unknown regex"};
  }
}

nlohmann::json jsonify(Token const& token) {
  return nlohmann::json{
      {"id", token.id},
      {"name", token.NameOf()},
      {"offset", token.offset},
      {"length", token.length},
      {"start_line", token.start_line},
      {"start_column", token.start_column},
      {"end_line", token.end_line},
      {"end_column", token.end_column},
      {"text", token.TextOf()},
  };
}

void Anim::RegexCompile(std::string const& pattern, Regex regex) {
  anim({
      {"$", "RegexCompile"},
      {"pattern", pattern},
      {"regex", jsonify(regex)},
  });
}

void Anim::RegexAccept(Regex accept, toylang::regex::LeafNodes const& afters) {
  auto jaccept = jsonify(accept);
  jaccept["afters"] =
      std::accumulate(afters.begin(), afters.end(), nlohmann::json::array(),
                      [](nlohmann::json& acc, auto const& it) {
                        acc.push_back(hex(it));
                        return acc;
                      });
  anim({
      {"$", "RegexAccept"},
      {"accept", jaccept},
  });
}

void Anim::RegexUnion(Regex unode) {
  anim({
      {"$", "RegexUnion"},
      {"union", hex(unode)},
      {"lhs", hex(to<regex::UnionNode>(unode)->left_)},
      {"rhs", hex(to<regex::UnionNode>(unode)->right_)},
  });
}

void Anim::LexiconAddToken(int id, std::string const& name) {
  anim({
      {"$", "LexiconAddToken"},
      {"id", id},
      {"name", name},
  });
}
void Anim::LexiconAddState(int id, regex::LeafNodes const& poses) {
  anim({
      {"$", "LexiconAddState"},
      {"id", id},
      {"poses",
       std::accumulate(poses.begin(), poses.end(), nlohmann::json::array(),
                       [](nlohmann::json& acc, auto const& it) {
                         acc.push_back(hex(it));
                         return acc;
                       })},
  });
}
void Anim::LexiconAddTransfer(int from, int to, int input) {
  anim({
      {"$", "LexiconAddTransfer"},
      {"from", from},
      {"to", to},
      {"input", input},
  });
}
void Anim::LexiconSetAccept(int id, int token) {
  anim({
      {"$", "LexiconSetAccept"},
      {"id", id},
      {"token", token},
  });
}
void Anim::ScannerSetSource(std::string const& source) {
  anim({
      {"$", "ScannerSetSource"},
      {"source", source},
  });
}
void Anim::ScannerSetState(int state) {
  anim({
      {"$", "ScannerSetState"},
      {"state", state},
  });
}
void Anim::ScannerNextInput() {
  anim({
      {"$", "ScannerNextInput"},
  });
}
void Anim::ScannerNextLine() {
  anim({
      {"$", "ScannerNextLine"},
  });
}
void Anim::ScannerAcceptToken(Token const& token) {
  anim({
      {"$", "ScannerAcceptToken"},
      {"token", jsonify(token)},
  });
}

int Anim::main(int, char**) {
  nlohmann::json json;
  std::cin >> json;

  Lexicon::Builder builder;
  for (auto const& [name, pattern] : json["tokens"].items()) {
    auto regex = regex::Compile(pattern.get<std::string>());
    builder.DefineToken(name, regex);
  }
  auto lexicon = builder.Build();
  if (json.count("source")) {
    Scanner scanner;
    scanner.SetLexicon(lexicon);
    auto source = Source::Create(json["source"].get<std::string>());
    scanner.SetSource(source);
    auto token = scanner.NextToken();
    while (token.id > 0) {
      token = scanner.NextToken();
    }
  }
  return 0;
}

}  // namespace toylang
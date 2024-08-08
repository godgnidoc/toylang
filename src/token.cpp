#include "toylang/token.h"

#include "toylang/lexical.h"

namespace toylang {

std::string Token::TextOf() const {
  return source->content.substr(offset, length);
}

std::string Token::NameOf() const { return lexicon->NameOfToken(id); }

std::string Token::LocationOf() const {
  if (!source->path) {
    return std::to_string(start_line) + ":" + std::to_string(start_column);
  }
  return *source->path + ":" + std::to_string(start_line) + ":" +
         std::to_string(start_column);
}
}  // namespace toylang
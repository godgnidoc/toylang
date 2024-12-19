#ifndef __TOYLANG_ANIM_H__
#define __TOYLANG_ANIM_H__

#include <string>

#include "toylang/regex.h"
#include "toylang/token.h"

namespace toylang {

class Anim {
 public:
  static void RegexCompile(std::string const& pattern, Regex regex);
  static void RegexAccept(Regex accept,
                          toylang::regex::LeafNodes const& afters);
  static void RegexUnion(Regex unode);
  static void LexiconAddToken(int id, std::string const& name);
  static void LexiconAddState(int id, regex::LeafNodes const& poses);
  static void LexiconAddTransfer(int from, int to, int input);
  static void LexiconSetAccept(int state, int token);
  static void ScannerSetSource(std::string const& source);
  static void ScannerSetState(int state);
  static void ScannerNextInput();
  static void ScannerNextLine();
  static void ScannerAcceptToken(Token const& token);
  static int main(int, char**);
};

}  // namespace toylang

#endif
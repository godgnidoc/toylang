#include "toylang/lexical.h"

#include "gtest/gtest.h"

TEST(LexiconTest, Basic) {
  toylang::Scanner scanner;

  auto abc = toylang::Lexicon::Builder{}
                 .DefineToken("ABC", toylang::regex::Compile("abc"))
                 .Build();

  scanner.SetLexicon(abc);
  scanner.SetSource(toylang::Source::Create("abcabcac"));
  EXPECT_EQ(scanner.NextToken().id, abc->IdOfToken("ABC"));
  EXPECT_EQ(scanner.NextToken().id, abc->IdOfToken("ABC"));
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kError);
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kEOF);

  auto abc_asd = toylang::Lexicon::Builder{}
                     .DefineToken("ABC", toylang::regex::Compile("abc"))
                     .DefineToken("ASD", toylang::regex::Compile("asd"))
                     .Build();

  scanner.SetLexicon(abc_asd);
  scanner.SetSource(toylang::Source::Create("abcasdas"));
  EXPECT_EQ(scanner.NextToken().id, abc_asd->IdOfToken("ABC"));
  EXPECT_EQ(scanner.NextToken().id, abc_asd->IdOfToken("ASD"));
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kError);
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kEOF);

  auto ab_or_cd_asd = toylang::Lexicon::Builder{}
                          .DefineToken("ABCD", toylang::regex::Compile("ab|cd"))
                          .DefineToken("ASD", toylang::regex::Compile("asd"))
                          .Build();

  scanner.SetLexicon(ab_or_cd_asd);
  scanner.SetSource(toylang::Source::Create("abasdcd"));
  EXPECT_EQ(scanner.NextToken().id, ab_or_cd_asd->IdOfToken("ABCD"));
  EXPECT_EQ(scanner.NextToken().id, ab_or_cd_asd->IdOfToken("ASD"));
  EXPECT_EQ(scanner.NextToken().id, ab_or_cd_asd->IdOfToken("ABCD"));
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kEOF);

  auto abcdefg =
      toylang::Lexicon::Builder{}
          .DefineToken("ABCDEFG", toylang::regex::Compile("a(bc+|de?)f*g"))
          .DefineToken("SPACE", toylang::regex::Compile("\\s+"))
          .Build();

  scanner.SetLexicon(abcdefg);
  scanner.SetSource(toylang::Source::Create("abcg abccg abcfg adfg adefffg"));
  EXPECT_EQ(scanner.NextToken().id, abcdefg->IdOfToken("ABCDEFG"));
  EXPECT_EQ(scanner.NextToken().id, abcdefg->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, abcdefg->IdOfToken("ABCDEFG"));
  EXPECT_EQ(scanner.NextToken().id, abcdefg->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, abcdefg->IdOfToken("ABCDEFG"));
  EXPECT_EQ(scanner.NextToken().id, abcdefg->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, abcdefg->IdOfToken("ABCDEFG"));
  EXPECT_EQ(scanner.NextToken().id, abcdefg->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, abcdefg->IdOfToken("ABCDEFG"));
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kEOF);
}

TEST(LexiconTest, Range) {
  toylang::Scanner scanner;

  auto range =
      toylang::Lexicon::Builder{}
          .DefineToken("RANGE", toylang::regex::Compile("([a-z]|\\d)+"))
          .DefineToken("SPACE", toylang::regex::Compile("\\s+"))
          .Build();

  scanner.SetLexicon(range);
  scanner.SetSource(toylang::Source::Create("abc defg hijklmnopqrs"));
  EXPECT_EQ(scanner.NextToken().id, range->IdOfToken("RANGE"));
  EXPECT_EQ(scanner.NextToken().id, range->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, range->IdOfToken("RANGE"));
  EXPECT_EQ(scanner.NextToken().id, range->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, range->IdOfToken("RANGE"));
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kEOF);
}

TEST(LexiconTest, Class) {
  toylang::Scanner scanner;

  auto class_ = toylang::Lexicon::Builder{}
                    .DefineToken("ID", toylang::regex::Compile("\\w+"))
                    .DefineToken("SPACE", toylang::regex::Compile("\\s+"))
                    .Build();

  scanner.SetLexicon(class_);
  scanner.SetSource(toylang::Source::Create("abc df2a 3f"));
  EXPECT_EQ(scanner.NextToken().id, class_->IdOfToken("ID"));
  EXPECT_EQ(scanner.NextToken().id, class_->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, class_->IdOfToken("ID"));
  EXPECT_EQ(scanner.NextToken().id, class_->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, class_->IdOfToken("ID"));
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kEOF);
}

TEST(LexiconTest, Numbre) {
  toylang::Scanner scanner;

  auto number =
      toylang::Lexicon::Builder{}
          .DefineToken("NUMBER",
                       toylang::regex::Compile(
                           "[+-]?(0|[1-9]\\d*)(\\.\\d+)?([eE][+-]?[0-9]+)?"))
          .DefineToken(
              "BAD_NUMBER",
              toylang::regex::Compile(
                  "[+-]?\\d+(\\.\\d+)?([eE][+-]?\\d+)?(\\l|\\u|_)+"))
          .DefineToken("SPACE", toylang::regex::Compile("\\s+"))
          .Build();

  scanner.SetLexicon(number);
  scanner.SetSource(
      toylang::Source::Create("123 "
                              "123.456 "
                              "0.456 "
                              "+123.456e+789 "
                              "-123.456e-789 "
                              "123.456e789 "
                              "123.456e "
                              "123a "
                              "0. "
                              "023 "
                              "123.456e+ "
                              "123.456e- "));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("NUMBER"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("NUMBER"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("NUMBER"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("NUMBER"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("NUMBER"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("NUMBER"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("BAD_NUMBER"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("BAD_NUMBER"));
  EXPECT_EQ(scanner.NextToken().id, number->IdOfToken("SPACE"));
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kError);
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kError);
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kError);
  EXPECT_EQ(scanner.NextToken().id, toylang::Token::kError);
}

TEST(LexiconTest, Comment) {
    toylang::Scanner scanner;
    
    auto comment = toylang::Lexicon::Builder{}
                         .DefineToken("COMMENT_LINE", toylang::regex::Compile("\\/\\/[^\\n]*\\n"))
                         .DefineToken("COMMENT_BLOCK", toylang::regex::Compile("\\/\\*([^\\/]|[^\\*]\\/)*\\*\\/"))
                         .DefineToken("SPACE", toylang::regex::Compile("\\s+"))
                         .Build();
    
    scanner.SetLexicon(comment);
    scanner.SetSource(toylang::Source::Create("/* comment */ // comment支持中文\n"
    "  /** document comment \n"
    "   *  支持中文 \n"
    "   */\n"));

    EXPECT_EQ(scanner.NextToken().id, comment->IdOfToken("COMMENT_BLOCK"));
    EXPECT_EQ(scanner.NextToken().id, comment->IdOfToken("SPACE"));
    EXPECT_EQ(scanner.NextToken().id, comment->IdOfToken("COMMENT_LINE"));
    EXPECT_EQ(scanner.NextToken().id, comment->IdOfToken("SPACE"));
    EXPECT_EQ(scanner.NextToken().id, comment->IdOfToken("COMMENT_BLOCK"));
    EXPECT_EQ(scanner.NextToken().id, comment->IdOfToken("SPACE"));
    EXPECT_EQ(scanner.NextToken().id, toylang::Token::kEOF);
}
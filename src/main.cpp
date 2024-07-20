#include "toylang/regex.h"

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  using namespace toylang;

  auto regex = regex::Compile("a(b+|c)*d");

  return 0;
}
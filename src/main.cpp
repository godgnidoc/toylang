#ifndef UNIT_TEST

#include "toylang/anim.h"

int main(int argc, char **argv) {
  if (getenv("TOYLANG_ANIM") != nullptr) {
    return toylang::Anim::main(argc, argv);
  }
  return 0;
}

#endif
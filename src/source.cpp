#include "toylang/source.h"

#include <fstream>
#include <iostream>

namespace toylang {
std::shared_ptr<Source const> Source::Create(
    std::string const& content, std::optional<std::string> const& path) {
  return std::make_shared<Source>(Source{.content = content, .path = path});
}

std::shared_ptr<Source const> Source::Load(std::string const& path) {
  if (path == "-") {
    std::string content{std::istreambuf_iterator<char>{std::cin},
                        std::istreambuf_iterator<char>{}};
    return Create(content);
  }

  std::ifstream file{path};
  if (!file.is_open()) {
    return nullptr;
  }

  std::string content{std::istreambuf_iterator<char>{file},
                      std::istreambuf_iterator<char>{}};

  return Create(content, path);
}
}  // namespace toylang
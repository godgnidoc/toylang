#ifndef __TOYLANG_SOURCE_H__
#define __TOYLANG_SOURCE_H__

#include <memory>
#include <optional>
#include <string>

namespace toylang {

/**
 * Source
 *
 * 提供基于字符串的源码对象
 */
struct Source {
  std::string content;
  std::optional<std::string> path;

  /**
   * 创建源码对象
   *
   * @param content 源码内容
   * @param path 源码路径
   */
  static std::shared_ptr<Source const> Create(
      std::string const& content,
      std::optional<std::string> const& path = std::nullopt);

  /**
   * 加载源码文件
   */
  static std::shared_ptr<Source const> Load(std::string const& path);
};

}  // namespace toylang

#endif
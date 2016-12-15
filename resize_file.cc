/****************************************************************************
 * resize_file.cc
 *
 *   Copyright (c) 2016 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#include "resize_file.h"

#include <sys/stat.h>

#include <fstream>
#include <list>

namespace util {

namespace {

std::list<std::string> readFile(const std::string &path) {
  std::ifstream ifs(path, std::ios::in);
  std::list<std::string> line_list{};
  std::string str;

  if (!ifs.is_open()) return line_list;

  while (std::getline(ifs, str)) {
    line_list.push_back(str);
  }

  return line_list;
}

common::status_t overwriteFile(const std::string &path,
                               const std::list<std::string> &line_list) {
  if (line_list.empty()) return common::status_t::kFailure;

  std::ofstream ofs(path, std::ios::trunc);
  if (!ofs.is_open()) return common::status_t::kFailure;

  for (const auto &line : line_list) {
      ofs << line << std::endl;
  }

  return common::status_t::kSuccess;
}

}  // namespace

bool FileExists(const std::string &path) {
  struct stat buf;
  return (stat(path.c_str(), &buf) == 0);
}

common::status_t ResizeFile(const std::string &path, int32_t size) {
  auto line_list = readFile(path);
  if (line_list.empty()) return common::status_t::kFailure;

  auto origin_size = line_list.size();
  if (origin_size <= size) return common::status_t::kSuccess;

  for (int32_t i = 0; i < origin_size - size; ++i) {
    line_list.pop_front();
  }

  return overwriteFile(path, line_list);
}

}  // namespace util


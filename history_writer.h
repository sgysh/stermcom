/****************************************************************************
 * history_writer.h
 *
 *   Copyright (c) 2016 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#ifndef HISTORY_WRITER_H_
#define HISTORY_WRITER_H_

#include <cstdint>
#include <list>
#include <string>

#include "common_type.h"

namespace util {

class HistoryWriter final {
 public:
  HistoryWriter() = delete;
  explicit HistoryWriter(const std::string &path);
  ~HistoryWriter();

  void AddStr(std::list<uint8_t> str);
  void PopBack();
  common::status_t Write();

 private:
  std::string path_to_history_;
  std::list<uint8_t> history_buffer_;
};

}  // namespace util

#endif  // HISTORY_WRITER_H_


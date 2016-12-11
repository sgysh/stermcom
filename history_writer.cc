/****************************************************************************
 * history_writer.cc
 *
 *   Copyright (c) 2016 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#include "history_writer.h"

#include <fstream>

namespace util {

HistoryWriter::HistoryWriter(const std::string &file_name)
  : history_file_name_(file_name),
    history_buffer_() {
}

HistoryWriter::~HistoryWriter() {
}

void HistoryWriter::AddStr(std::list<uint8_t> str) {
  history_buffer_.splice(history_buffer_.end(), str);
}

void HistoryWriter::PopBack() {
  if (!history_buffer_.empty()) history_buffer_.pop_back();
}

common::status_t HistoryWriter::Write() {
  if (history_buffer_.empty()) return common::status_t::kSuccess;

  std::ofstream ofs(history_file_name_, std::ios::app);

  if (!ofs.is_open()) return common::status_t::kFailure;

  for (auto c : history_buffer_) {
    ofs << c;
  }
  ofs << std::endl;

  history_buffer_.clear();

  return common::status_t::kSuccess;
}

}  // namespace util


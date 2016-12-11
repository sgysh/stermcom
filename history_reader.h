/****************************************************************************
 * history_reader.h
 *
 *   Copyright (c) 2016 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#ifndef HISTORY_READER_H_
#define HISTORY_READER_H_

#include <cstdint>
#include <list>
#include <string>

#include "common_type.h"

namespace util {

class HistoryReader final {
 public:
  HistoryReader() = delete;
  explicit HistoryReader(const std::string &file_name);
  ~HistoryReader();

  void StartSearch();
  void Up();
  void Down();
  std::list<uint8_t> At();
  void EndSearch();
  std::list<uint8_t> ClearHistoryLine();

 private:
  common::status_t ReadHistoryFile();

  std::string history_file_name_;
  bool is_searching_;
  uint32_t pre_str_size_;
  std::list<std::string> history_list_;
  std::list<std::string>::iterator history_list_itr_;
};

}  // namespace util

#endif  // HISTORY_READER_H_


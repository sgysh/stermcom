/****************************************************************************
 * history_reader.cc
 *
 *   Copyright (c) 2016 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#include "history_reader.h"

#include <fstream>

namespace util {

namespace {

constexpr const uint8_t kKeycodeDel = 0x7f;

}  // namespace

HistoryReader::HistoryReader(const std::string &path)
  : path_to_history_(path),
    is_searching_(false),
    pre_str_size_(0),
    history_list_(),
    history_list_itr_() {
}

HistoryReader::~HistoryReader() {
}

void HistoryReader::StartSearch() {
  if (!is_searching_) {
    is_searching_ = true;
    (void)ReadHistoryFile();
    history_list_itr_ = history_list_.end();
  }
}

void HistoryReader::Up() {
  if (!is_searching_) return;

  if (history_list_itr_ != history_list_.begin()) --history_list_itr_;

  pre_str_size_ = (*history_list_itr_).size();
}

void HistoryReader::Down() {
  if (!is_searching_) return;

  if (history_list_itr_ != history_list_.end()) ++history_list_itr_;

  if (history_list_itr_ != history_list_.end()) {
    pre_str_size_ = (*history_list_itr_).size();
  } else {
    pre_str_size_ = 0;
  }
}

std::list<uint8_t> HistoryReader::At() {
  std::list<uint8_t> char_list{};

  if (!is_searching_) return char_list;
  if (history_list_itr_ == history_list_.end()) return char_list;

  for (const auto &c : *history_list_itr_) {
    char_list.push_back(c);
  }

  return char_list;
}

void HistoryReader::EndSearch() {
  if (!is_searching_) return;

  history_list_.clear();
  pre_str_size_ = 0;
  is_searching_ = false;
}

std::list<uint8_t> HistoryReader::ClearHistoryLine() {
  std::list<uint8_t> char_list{};

  if (!is_searching_) return char_list;

  for (int32_t i = 0; i < pre_str_size_; ++i) {
    char_list.push_back(kKeycodeDel);
  }

  return char_list;
}

common::status_t HistoryReader::ReadHistoryFile() {
  std::ifstream ifs(path_to_history_, std::ios::in);
  std::string str;

  if (!ifs.is_open()) return common::status_t::kFailure;

  while (std::getline(ifs, str)) {
    history_list_.push_back(str);
  }

  return common::status_t::kSuccess;
}

}  // namespace util


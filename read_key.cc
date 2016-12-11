/****************************************************************************
 * read_key.cc
 *
 *   Copyright (c) 2016 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#include "read_key.h"

#include <algorithm>
#include <sys/select.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace util {

namespace {

const std::vector<uint8_t> kKeycodeCtrlX{0x18};
const std::vector<uint8_t> kKeycodeCtrlR{0x12};
const std::vector<uint8_t> kKeycodeEnter{0x0d};
const std::vector<uint8_t> kKeycodeDel{0x7f};
const std::vector<uint8_t> kKeycodeEsc{0x1b};
const std::vector<uint8_t> kKeycodeUp{0x1b, 0x5b, 0x41};
const std::vector<uint8_t> kKeycodeDown{0x1b, 0x5b, 0x42};
const std::vector<uint8_t> kKeycodeRight{0x1b, 0x5b, 0x43};
const std::vector<uint8_t> kKeycodeLeft{0x1b, 0x5b, 0x44};

// non-blocking
bool isReadable(const int32_t &fd) {
  fd_set fds_r;
  struct timeval tv;

  FD_ZERO(&fds_r);
  FD_SET(fd, &fds_r);
  tv.tv_sec  = 0;
  tv.tv_usec = 0;

  auto ret = select(fd + 1, &fds_r, nullptr, nullptr, &tv);
  if (ret == -1) return false;

  if (FD_ISSET(fd, &fds_r)) {
    return true;
  } else {
    return false;
  }
}

}  // namespace

ReadKeyResult ReadKey(const int32_t &fd) {
  ReadKeyResult result{};
  result.key_type = key_t::kOther;

  std::list<std::pair<std::vector<uint8_t>, key_t>> key_table;
  key_table.push_back(std::make_pair(kKeycodeCtrlX, key_t::kCtrlX));
  key_table.push_back(std::make_pair(kKeycodeCtrlR, key_t::kCtrlR));
  key_table.push_back(std::make_pair(kKeycodeEnter, key_t::kEnter));
  key_table.push_back(std::make_pair(kKeycodeDel,   key_t::kDel  ));
  key_table.push_back(std::make_pair(kKeycodeEsc,   key_t::kEsc  ));
  key_table.push_back(std::make_pair(kKeycodeUp,    key_t::kUp   ));
  key_table.push_back(std::make_pair(kKeycodeDown,  key_t::kDown ));
  key_table.push_back(std::make_pair(kKeycodeRight, key_t::kRight));
  key_table.push_back(std::make_pair(kKeycodeLeft,  key_t::kLeft ));

  std::list<int32_t> index_list;
  std::list<bool>    matched_list;
  for (auto k : key_table) {
    index_list.push_back(0);
    matched_list.push_back(true);
  }

  while (true) {
    if (!isReadable(fd)) break;
    uint8_t read_char;
    auto size = read(fd, &read_char, 1);
    if (size != 1) break;
    result.read_keys.push_back(read_char);

    auto index_list_itr   = index_list.begin();
    auto matched_list_itr = matched_list.begin();
    for (auto key_table_itr = key_table.begin(); key_table_itr != key_table.end();
         ++key_table_itr, ++index_list_itr, ++matched_list_itr) {
      if (*matched_list_itr == true) {
        if (read_char == *((*key_table_itr).first.begin() + *index_list_itr)) {
          ++(*index_list_itr);
        } else {
          *matched_list_itr = false;
        }
        if ((*key_table_itr).first.begin() + *index_list_itr == ((*key_table_itr).first).end()) {
          result.key_type = (*key_table_itr).second;
        }
      }
    }

    auto matched_count = std::count_if(matched_list.begin(), matched_list.end(),
                                       [](bool m) -> bool { return m; });
    if (matched_count == 0 || matched_count == 1) break;
  }

  return result;
}

}  // namespace util


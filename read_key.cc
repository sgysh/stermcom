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

struct KeyRecord {
  const std::vector<uint8_t> keys;
  const key_t                type;
  uint32_t                   index;
  bool                       is_matched;
};

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

  std::list<KeyRecord> key_table;
  key_table.push_back({kKeycodeCtrlX, key_t::kCtrlX, 0, true});
  key_table.push_back({kKeycodeCtrlR, key_t::kCtrlR, 0, true});
  key_table.push_back({kKeycodeEnter, key_t::kEnter, 0, true});
  key_table.push_back({kKeycodeDel,   key_t::kDel  , 0, true});
  key_table.push_back({kKeycodeEsc,   key_t::kEsc  , 0, true});
  key_table.push_back({kKeycodeUp,    key_t::kUp   , 0, true});
  key_table.push_back({kKeycodeDown,  key_t::kDown , 0, true});
  key_table.push_back({kKeycodeRight, key_t::kRight, 0, true});
  key_table.push_back({kKeycodeLeft,  key_t::kLeft , 0, true});

  while (true) {
    if (!isReadable(fd)) break;
    uint8_t read_char;
    auto size = read(fd, &read_char, 1);
    if (size != 1) break;
    result.read_keys.push_back(read_char);

    for (auto &key_record : key_table) {
      if (key_record.is_matched == true) {
        if (read_char == *(key_record.keys.begin() + key_record.index)) {
          ++key_record.index;
        } else {
          key_record.is_matched = false;
        }
        if (key_record.keys.begin() + key_record.index == key_record.keys.end()) {
          result.key_type = key_record.type;
        }
      }
    }

    auto matched_count = std::count_if(
        key_table.begin(), key_table.end(),
        [](KeyRecord key_record) -> bool { return key_record.is_matched; });
    if (matched_count == 0 || matched_count == 1) break;
  }

  return result;
}

}  // namespace util


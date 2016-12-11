/****************************************************************************
 * read_key.h
 *
 *   Copyright (c) 2016 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#ifndef READ_KEY_H_
#define READ_KEY_H_

#include <cstdint>
#include <list>

namespace util {

enum class key_t : uint8_t {
  kCtrlX,
  kCtrlR,
  kEnter,
  kDel,
  kEsc,
  kUp,
  kDown,
  kRight,
  kLeft,
  kOther,
};

struct ReadKeyResult {
  key_t key_type;
  std::list<uint8_t> read_keys;
};

ReadKeyResult ReadKey(const int32_t &fd);

}  // namespace util

#endif  // READ_KEY_H_


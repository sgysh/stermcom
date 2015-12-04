/****************************************************************************
 * common_type.h
 *
 *   Copyright (c) 2015 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#ifndef COMMON_TYPE_H_
#define COMMON_TYPE_H_

#include <cstdint>

namespace common {

enum class status_t : uint8_t {
  kSuccess,
  kFailure,
  kUnknown
};

}  // namespace common

#endif  // COMMON_TYPE_H_


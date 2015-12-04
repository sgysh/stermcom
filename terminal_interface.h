/****************************************************************************
 * terminal_interface.h
 *
 *   Copyright (c) 2015 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#ifndef TERMINAL_INTERFACE_H_
#define TERMINAL_INTERFACE_H_

#include <termios.h>

#include "common_type.h"

namespace util {

enum class direction_t : uint8_t {
  kIn,
  kOut
};

class TerminalInterface final {
 public:
  explicit TerminalInterface(const int32_t &);
  ~TerminalInterface();

  common::status_t SetRawMode();
  common::status_t SetBaudRate(const uint32_t &, direction_t);

 private:
  common::status_t Flush();
  common::status_t RevertSettings();
  common::status_t BackupSettings();
  common::status_t SetNow();

  int32_t fd_;
  struct termios current_terminal_, backup_terminal_;
};

}  // namespace util

#endif  // TERMINAL_INTERFACE_H_


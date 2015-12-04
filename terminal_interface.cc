/****************************************************************************
 * terminal_interface.cc
 *
 *   Copyright (c) 2015 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#include "terminal_interface.h"

#include <map>

#include "debug.h"

namespace util {

namespace {

const std::map<uint32_t, speed_t> kBaudRateMap = {
  {0,      B0     },
  {50,     B50    },
  {75,     B75    },
  {110,    B110   },
  {134,    B134   },
  {150,    B150   },
  {200,    B200   },
  {300,    B300   },
  {600,    B600   },
  {1200,   B1200  },
  {1800,   B1800  },
  {2400,   B2400  },
  {4800,   B4800  },
  {9600,   B9600  },
  {19200,  B19200 },
  {38400,  B38400 },
  {57600,  B57600 },
  {115200, B115200},
  {230400, B230400},
};

}  // namespace

TerminalInterface::TerminalInterface(const int32_t &fd)
  : fd_(fd),
    current_terminal_(),
    backup_terminal_() {
      (void)TerminalInterface::BackupSettings();
}

TerminalInterface::~TerminalInterface() {
  DEBUG_PRINTF("Call the destructor of TerminalInterface (fd: %d)", fd_);
  (void)Flush();
  (void)RevertSettings();
}

common::status_t TerminalInterface::SetRawMode() {
  cfmakeraw(&current_terminal_);
  return TerminalInterface::SetNow();
}

common::status_t TerminalInterface::SetBaudRate(const uint32_t &baud_rate,
                                                direction_t direction) {
  auto itr = kBaudRateMap.find(baud_rate);
  if (itr == kBaudRateMap.end())
    return common::status_t::kFailure;
  if (direction == direction_t::kIn) {
    if (cfsetispeed(&current_terminal_, itr->second))
      return common::status_t::kFailure;
  } else if (direction == direction_t::kOut) {
    if (cfsetospeed(&current_terminal_, itr->second))
      return common::status_t::kFailure;
  }
  return TerminalInterface::SetNow();
}

common::status_t TerminalInterface::Flush() {
  if (tcflush(fd_, TCIOFLUSH))
    return common::status_t::kFailure;
  return common::status_t::kSuccess;
}

common::status_t TerminalInterface::BackupSettings() {
  if (tcgetattr(fd_, &backup_terminal_))
    return common::status_t::kFailure;
  return common::status_t::kSuccess;
}

common::status_t TerminalInterface::RevertSettings() {
  if (tcsetattr(fd_, TCSANOW, &backup_terminal_))
    return common::status_t::kFailure;
  return common::status_t::kSuccess;
}

common::status_t TerminalInterface::SetNow() {
  if (tcsetattr(fd_, TCSANOW, &current_terminal_))
    return common::status_t::kFailure;
  return common::status_t::kSuccess;
}

}  // namespace util


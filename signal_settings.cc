/****************************************************************************
 * signal_settings.cc
 *
 *   Copyright (c) 2015 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#include "signal_settings.h"

#include <csignal>

namespace util {

common::status_t InitializeSignalAction(void (*ignore_handler)(int32_t),
                                        void (*disconnect_handler)(int32_t)) {
  struct sigaction signal_ignore;

  if (sigemptyset(&signal_ignore.sa_mask) == -1) {
    return common::status_t::kFailure;
  }
  signal_ignore.sa_handler = ignore_handler;
  signal_ignore.sa_flags   = 0;

  if (sigaction(SIGINT, &signal_ignore, nullptr) == -1)
    return common::status_t::kFailure;
  if (sigaction(SIGQUIT, &signal_ignore, nullptr) == -1)
    return common::status_t::kFailure;
  if (sigaction(SIGPIPE, &signal_ignore, nullptr) == -1)
    return common::status_t::kFailure;
  if (sigaction(SIGUSR1, &signal_ignore, nullptr) == -1)
    return common::status_t::kFailure;
  if (sigaction(SIGUSR2, &signal_ignore, nullptr) == -1)
    return common::status_t::kFailure;

  struct sigaction signal_disconnect;

  if (sigemptyset(&signal_disconnect.sa_mask) == -1) {
    return common::status_t::kFailure;
  }
  signal_disconnect.sa_handler = disconnect_handler;
  signal_disconnect.sa_flags   = 0;

  if (sigaction(SIGHUP, &signal_disconnect, nullptr) == -1)
    return common::status_t::kFailure;
  if (sigaction(SIGTERM, &signal_disconnect, nullptr) == -1)
    return common::status_t::kFailure;

  return common::status_t::kSuccess;
}

}  // namespace util


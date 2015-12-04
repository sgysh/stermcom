/****************************************************************************
 * signal_settings.h
 *
 *   Copyright (c) 2015 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#ifndef SIGNAL_SETTINGS_H_
#define SIGNAL_SETTINGS_H_

#include "common_type.h"

namespace util {

common::status_t InitializeSignalAction(void (*ignore_handler)(int32_t),
                                        void (*disconnect_handler)(int32_t));

}  // namespace util

#endif  // SIGNAL_SETTINGS_H_


/****************************************************************************
 * resize_file.h
 *
 *   Copyright (c) 2016 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#ifndef RESIZE_FILE_H_
#define RESIZE_FILE_H_

#include <string>

#include "common_type.h"

namespace util {

common::status_t ResizeFile(const std::string &file_name, int32_t size);

}  // namespace util

#endif  // RESIZE_FILE_H_


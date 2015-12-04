/****************************************************************************
 * file_descriptor.h
 *
 *   Copyright (c) 2015 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#ifndef FILE_DESCRIPTOR_H_
#define FILE_DESCRIPTOR_H_

#include <cstdint>
#include <memory>

namespace util {

class FileDescriptor final {
 public:
  FileDescriptor() = delete;
  FileDescriptor(const char *pathname, int32_t flags);
  ~FileDescriptor();

  bool IsSuccess() const;
  operator int32_t() const;

 private:
  std::shared_ptr<int32_t> stored_fd_;
};

}  // namespace util

#endif  // FILE_DESCRIPTOR_H_


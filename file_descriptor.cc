/****************************************************************************
 * file_descriptor.cc
 *
 *   Copyright (c) 2015 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#include "file_descriptor.h"

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

#include "debug.h"

namespace util {

namespace {

std::shared_ptr<int32_t> openFd(const char *pathname, int32_t flags) {
  return std::shared_ptr<int32_t>(new int32_t{open(pathname, flags)},
                                  [](int32_t *fd) -> void {
    if (*fd != -1) close(*fd);
  });
}

}  // namespace

FileDescriptor::FileDescriptor(const char *pathname, int32_t flags)
  : error_message_(),
    stored_fd_() {
  errno = 0;
  stored_fd_ = openFd(pathname, flags);
  error_message_ = strerror(errno);
}

FileDescriptor::~FileDescriptor() {
  DEBUG_PRINTF("Call the destructor of FileDescriptor (fd: %d)", *stored_fd_);
}

bool FileDescriptor::IsSuccess() const {
  if (*stored_fd_ == -1) return false;
  return true;
}

std::string FileDescriptor::GetErrorMessage() const {
  return error_message_;
}

FileDescriptor::operator int32_t() const {
  return *stored_fd_;
}

}  // namespace util


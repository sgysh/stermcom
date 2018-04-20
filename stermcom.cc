/****************************************************************************
 * stermcom.cc
 *
 *   Copyright (c) 2015 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#include <fcntl.h>
#include <libgen.h>
#include <sys/file.h>
#include <sys/select.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <list>

#include "common_type.h"
#include "debug.h"
#include "file_descriptor.h"
#include "history_reader.h"
#include "history_writer.h"
#include "read_key.h"
#include "resize_file.h"
#include "signal_settings.h"
#include "terminal_interface.h"

namespace {

using status_t = common::status_t;

volatile sig_atomic_t g_should_continue = 1;

constexpr const char kHistoryFileName[] = ".stermcom_history";
constexpr const auto kMaxHistoryLine    = 100;

struct Options {
  std::string path_to_program;
  uint32_t baud_rate;
  std::string path_to_device_node;
  bool use_external_history;

  Options()
    : path_to_program(),
      baud_rate(9600),
      path_to_device_node(),
      use_external_history(false) {}
};

struct ParsingResult {
  bool is_success;
  Options opts;

  ParsingResult()
    : is_success(false), opts() {}
};

ParsingResult parseOptions(int argc, char *argv[]) {
  ParsingResult result;
  opterr = 0;

  result.opts.path_to_program = std::string(argv[0]);

  int opt_char;
  while ((opt_char = getopt(argc, argv, "b:h")) != -1) {
    switch (opt_char) {
      case 'b': {
        try {
          result.opts.baud_rate = std::stoi(optarg);
        }
        catch (...) {
          DEBUG_PRINTF("incorrect baud_rate");
          return result;
        }
        break;
      }
      case 'h': {
        result.opts.use_external_history = true;
        break;
      }
      default: {
        DEBUG_PRINTF("unknown option");
        return result;
      }
    }
  }

  if (optind + 1 != argc) {
    DEBUG_PRINTF("no device_node or too many options");
    return result;
  }

  result.opts.path_to_device_node = std::string(argv[optind]);

  result.is_success = true;
  return result;
}

status_t setStdinToNonblock() {
  auto flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  if (flags == -1) return status_t::kFailure;

  auto ret = fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
  if (ret == -1) return status_t::kFailure;

  return status_t::kSuccess;
}

status_t reopenStdin() {
  util::FileDescriptor fd("/dev/tty", O_RDONLY);
  if (fd.IsSuccess() == false) return status_t::kFailure;

  // Assign /dev/tty to STDIN_FILENO (/dev/tty is also being assigned to fd)
  dup2(fd, STDIN_FILENO);

  // fd is closed automatically
  return status_t::kSuccess;
}

status_t mainLoop(const int32_t &tty_fd, const Options &opts) {
  fd_set fds_r, fds_w;
  uint8_t one_char;
  std::list<uint8_t> string_buffer{};
  ssize_t rw_size;

  std::string history_file_path;
  if (opts.use_external_history) {
    if (auto path_to_home = getenv("HOME")) {
      history_file_path = std::string(path_to_home) + "/" + std::string(kHistoryFileName);
    } else {
      printf("Fail to get the value of HOME\n");
      return status_t::kFailure;
    }
  }
  util::HistoryWriter history_writer(history_file_path);
  util::HistoryReader history_reader(history_file_path);

  // Support piping and redirection
  if (setStdinToNonblock() == status_t::kFailure) return status_t::kFailure;
  while (true) {
    rw_size = read(STDIN_FILENO, &one_char, 1);
    if (rw_size > 0) {
      string_buffer.push_back(one_char);
    } else {
      break;
    }
  }
  if (reopenStdin() == status_t::kFailure) return status_t::kFailure;

  util::TerminalInterface stdin_term(STDIN_FILENO), tty_term(tty_fd);

  if (stdin_term.SetRawMode() == status_t::kFailure)
    return status_t::kFailure;
  if (tty_term.SetRawMode() == status_t::kFailure)
    return status_t::kFailure;
  if (tty_term.SetBaudRate(opts.baud_rate, util::direction_t::kOut) ==
      status_t::kFailure)
    return status_t::kFailure;
  if (tty_term.SetBaudRate(0, util::direction_t::kIn) == status_t::kFailure)
    return status_t::kFailure;

  if (stdin_term.SetNow() == status_t::kFailure)
    return status_t::kFailure;
  if (tty_term.SetNow() == status_t::kFailure)
    return status_t::kFailure;

  assert(tty_fd > STDIN_FILENO && "Assertion for select()");

  while (g_should_continue) {
    FD_ZERO(&fds_r);
    FD_ZERO(&fds_r);

    FD_SET(STDIN_FILENO, &fds_r);
    FD_SET(tty_fd, &fds_r);
    FD_SET(tty_fd, &fds_w);

    errno = 0;
    auto ret = select(tty_fd + 1, &fds_r, &fds_w, nullptr, nullptr);
    if (ret == -1) {
      if (errno == EINTR) {
        DEBUG_PRINTF("Signal was caught when select() is waiting");
      } else {
        printf("Error\n");
        return status_t::kFailure;
      }
      break;
    }

    if (FD_ISSET(STDIN_FILENO, &fds_r)) {
      auto result = util::ReadKey(STDIN_FILENO);
      if (result.key_type == util::key_t::kCtrlX) break;
      if (opts.use_external_history) {
        switch (result.key_type) {
          case util::key_t::kCtrlR: {
            DEBUG_PRINTF("KEY: CtrlR");
            break;
          }
          case util::key_t::kUp: {
            DEBUG_PRINTF("KEY: UP");
            history_reader.StartSearch();
            string_buffer.splice(string_buffer.end(), history_reader.ClearHistoryLine());
            history_reader.Up();
            string_buffer.splice(string_buffer.end(), history_reader.At());
            break;
          }
          case util::key_t::kDown: {
            DEBUG_PRINTF("KEY: DOWN");
            string_buffer.splice(string_buffer.end(), history_reader.ClearHistoryLine());
            history_reader.Down();
            string_buffer.splice(string_buffer.end(), history_reader.At());
            break;
          }
          case util::key_t::kRight: {
            DEBUG_PRINTF("KEY: RIGHT");
            string_buffer.splice(string_buffer.end(), history_reader.ClearHistoryLine());
            history_reader.EndSearch();
            break;
          }
          case util::key_t::kLeft: {
            DEBUG_PRINTF("KEY: LEFT");
            string_buffer.splice(string_buffer.end(), history_reader.ClearHistoryLine());
            history_reader.EndSearch();
            break;
          }
          case util::key_t::kEnter: {
            DEBUG_PRINTF("KEY: ENTER");
            history_writer.AddStr(history_reader.At());
            history_reader.EndSearch();
            history_writer.Write();
            string_buffer.splice(string_buffer.end(), result.read_keys);
            break;
          }
          case util::key_t::kDel: {
            DEBUG_PRINTF("KEY: DEL");
            history_writer.AddStr(history_reader.At());
            history_reader.EndSearch();
            history_writer.PopBack();
            string_buffer.splice(string_buffer.end(), result.read_keys);
            break;
          }
          case util::key_t::kEsc: {
            DEBUG_PRINTF("KEY: ESC");
            string_buffer.splice(string_buffer.end(), result.read_keys);
            break;
          }
          default: {
            history_writer.AddStr(history_reader.At());
            history_reader.EndSearch();
            history_writer.AddStr(result.read_keys);
            string_buffer.splice(string_buffer.end(), result.read_keys);
            break;
          }
        }
      } else {
        string_buffer.splice(string_buffer.end(), result.read_keys);
      }
    }
    if (FD_ISSET(tty_fd, &fds_w)) {
      if (!string_buffer.empty()) {
        rw_size = write(tty_fd, &string_buffer.front(), 1);
        if (rw_size > 0) string_buffer.pop_front();
      }
    }
    if (FD_ISSET(tty_fd, &fds_r)) {
      uint8_t tty_read_buffer;
      rw_size = read(tty_fd, &tty_read_buffer, 1);
      if (rw_size == 0) {
        printf("The terminal is closed\n");
        break;
      }
      if (rw_size > 0) write(STDOUT_FILENO, &tty_read_buffer, 1);
    }
  }

  if (opts.use_external_history && util::FileExists(history_file_path)) {
    if (util::ResizeFile(history_file_path, kMaxHistoryLine) ==
        status_t::kFailure) {
      printf("Fail to resize the history file\n");
    }
  }

  return status_t::kSuccess;
}

}  // namespace

int main(int argc, char *argv[]) {
  auto result = parseOptions(argc, argv);

  if (!result.is_success) {
    // basename() may modify the contents of path, so it may be desirable to
    // pass a copy when calling the function.
    auto path_to_program = result.opts.path_to_program;
    printf("USAGE: %s [-h] [-b baud_rate] device_node\n",
           basename(const_cast<char *>(path_to_program.c_str())));
    return EXIT_FAILURE;
  }

  util::FileDescriptor fd(result.opts.path_to_device_node.c_str(),
                          O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd.IsSuccess() == false) {
    printf("cannot open the file %s\n",
           result.opts.path_to_device_node.c_str());
    printf("Message: %s\n", fd.GetErrorMessage().c_str());
    return EXIT_FAILURE;
  }
  if (!isatty(fd)) {
    printf("%s is not a terminal\n",
           result.opts.path_to_device_node.c_str());
    return EXIT_FAILURE;
  }

  if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
    printf("cannot place an exclusive lock on %s\n",
           result.opts.path_to_device_node.c_str());
    return EXIT_FAILURE;
  }

#ifndef PRIVATE_DEBUG
  if (util::InitializeSignalAction(
        SIG_IGN,
        // +: decay operator
        +[](int32_t) -> void {
          // When signal is caught, select() is not always waiting.
          g_should_continue = 0;
        }
      ) == status_t::kFailure) return EXIT_FAILURE;
#else
  if (util::InitializeSignalAction(
        +[](int32_t) -> void {
          // write(): Async-Signal-Safe
          (void)write(STDOUT_FILENO, "Ignore signal\n", 14);
        },
        +[](int32_t) -> void {
          (void)write(STDOUT_FILENO, "Disconnect\n", 11);
          g_should_continue = 0;
        }
      ) == status_t::kFailure) return EXIT_FAILURE;
#endif  // PRIVATE_DEBUG

  auto ret = mainLoop(fd, result.opts);
  if (ret == status_t::kFailure) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}

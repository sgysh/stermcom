/****************************************************************************
 * debug.h
 *
 *   Copyright (c) 2015 Yoshinori Sugino
 *   This software is released under the MIT License.
 ****************************************************************************/
#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef PRIVATE_DEBUG
#include <cstdio>
#define DEBUG_PRINTF(format, ...) \
  printf("[%s:%u] " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(format, ...) ((void)0)
#endif  // PRIVATE_DEBUG

#endif  // DEBUG_H_


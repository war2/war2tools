/*
 * debug.h
 *
 * Copyright (c) 2014 Jean Guyomarc'h
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#define ERR(fmt, ...) \
   fprintf(stderr, "*** [%s:%i] " fmt "\n", __func__, __LINE__, ## __VA_ARGS__)

#define DIE_RETURN(code, fmt, ...) \
   do { \
      ERR(fmt, ## __VA_ARGS__); \
      return code; \
   } while (0)

#define DIE_GOTO(label, fmt, ...) \
   do { \
      ERR(fmt, ## __VA_ARGS__); \
      goto label; \
   } while (0)


#endif /* ! _DEBUG_H_ */


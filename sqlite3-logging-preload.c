#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>

#include <sqlite3.h>

int callback(unsigned flag, void *context, void *arg1, void *arg2) {
  int64_t ms;

  switch (flag) {
    case SQLITE_TRACE_STMT:
      fprintf(stderr, "TRACE: (STMT) %s\n", (char *)arg2);
      break;
    case SQLITE_TRACE_PROFILE:
      ms = (int64_t)(arg2) / 1000 / 1000 / 1000 / 1000;
      fprintf(stderr, "TRACE: (PROFILE) %ld ms\n", ms);
      break;
    /* case SQLITE_TRACE_ROW: */
    /*   fprintf(stderr, "TRACE: (STMT) %s, %s\n", arg1, arg2); */
    /*   break; */
    case SQLITE_TRACE_CLOSE:
      fprintf(stderr, "TRACE: (CLOSE)\n");
      break;
  }

  return 0;
}

int sqlite3_open(const char *filename, sqlite3 **db) {
  int flags;
  int rc;

  int (*orig_open)(const char *, sqlite3 **);

  orig_open = dlsym(RTLD_NEXT, "sqlite3_open");

  flags = SQLITE_TRACE_STMT|SQLITE_TRACE_PROFILE|SQLITE_TRACE_ROW|SQLITE_TRACE_CLOSE;

  rc = orig_open(filename, db);

  sqlite3_trace_v2(*db, flags, callback, NULL);

  return rc;
}

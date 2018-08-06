#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <sqlite3.h>

int callback_v2(unsigned flag, void *context, void *arg1, void *arg2) {
  int64_t ms;

  switch (flag) {
    case SQLITE_TRACE_STMT:
      fprintf(context, "TRACE: (STMT) %s\n", (char *)arg2);
      break;
    case SQLITE_TRACE_PROFILE:
      ms = (int64_t)(arg2) / 1000 / 1000 / 1000 / 1000;
      fprintf(context, "TRACE: (PROFILE) %ld ms\n", ms);
      break;
    /* case SQLITE_TRACE_ROW: */
    /*   fprintf(stderr, "TRACE: (STMT) %s, %s\n", arg1, arg2); */
    /*   break; */
    case SQLITE_TRACE_CLOSE:
      fprintf(context, "TRACE: (CLOSE)\n");
      break;
  }

  return 0;
}


void callback(void *context, const char *query) {
  fprintf(context, "TRACE: (STMT) %s\n", query);
}

int sqlite3_open(const char *filename, sqlite3 **db) {
  FILE *output;
  // char *out_filename;
  int flags;
  int rc;
  int (*orig_open)(const char *, sqlite3 **);

  /* out_filename = getenv("SQLITE_LOGGING_PRELOAD_OUTPUT"); */

  output = stderr;
  /* if (out_filename) { */
  /*   output = fopen(out_filename, "a+"); */

  /*   if (output == NULL) { */
  /*     output = stderr; */
  /*     fprintf(output, "Couldn't open file\n"); */
  /*   } */
  /* } */
  /* else { */
  /* } */

  fprintf(output, "OATH\n");
  fflush(output);
  orig_open = dlsym(RTLD_NEXT, "sqlite3_open");
  if (orig_open == NULL) {
    fprintf(output, "MOO\n");
  }

  flags = SQLITE_TRACE_STMT|SQLITE_TRACE_PROFILE|SQLITE_TRACE_ROW|SQLITE_TRACE_CLOSE;

  rc = orig_open(filename, db);

  if (dlsym(RTLD_NEXT, "sqlite3_trace_v2")) {
    sqlite3_trace_v2(*db, flags, callback_v2, output);
  }
  else {
    sqlite3_trace(*db, callback, output);
  }

  return rc;
}

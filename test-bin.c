// Based on https://gist.github.com/jsok/2936764

#include <stdio.h>

#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  int i;

  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");

  return 0;
}

int main(void) {
  int rc;
  char *sql;
  char *err_msg = 0;

  sqlite3 *db;
  // sqlite3_stmt *stmt;
  const char* data = "Callback function called";

  rc = sqlite3_open("test.db", &db);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Can't open database (%d): %s\n", rc, sqlite3_errmsg(db));

    return 1;
  }

  fprintf(stdout, "Opened database successfully.\n");

  sql = "CREATE TABLE test(" \
        "  id   INT PRIMARY KEY NOT NULL," \
        "  name TEXT NOT NULL" \
        ");";

  fprintf(stdout, "Creating table...");
  rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);

    return 1;
  }

  fprintf(stdout, "Done\n");

  sql = "INSERT INTO " \
        "  test (id, name)" \
        "VALUES " \
        "  (1, 'John')," \
        "  (2, 'Silvia');";

  fprintf(stdout, "Inserting data...");
  rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);

    return 1;
  }

  fprintf(stdout, "Done\n");

  fprintf(stdout, "Performing query...\n");

  sql = "SELECT * FROM test";

  rc = sqlite3_exec(db, sql, callback, (void*)data, &err_msg);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
    sqlite3_free(err_msg);

    return 1;
  }

  fprintf(stdout, "Done\n");

  sqlite3_close(db);

  return 0;
}

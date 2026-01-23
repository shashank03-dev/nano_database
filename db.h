#ifndef DB_H
#define DB_H

#include <stdint.h>
#include <stdio.h>

// In-memory index entry mapping key -> file offset
typedef struct {
  char key[256];
  long file_offset;
  uint32_t val_size;
} IndexEntry;

// Main database context
typedef struct {
  FILE *fp;
  IndexEntry *index;
  int count;
} NanoDB;

NanoDB *db_open(const char *filename);
void db_set(NanoDB *db, const char *key, const char *value);
char *db_get(NanoDB *db, const char *key);
void db_close(NanoDB *db);
void db_bootstrap(NanoDB *db);
#endif
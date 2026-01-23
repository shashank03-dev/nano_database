#include "db.h"
#include <stdlib.h>
#include <string.h>

NanoDB *db_open(const char *filename) {
  NanoDB *db = malloc(sizeof(NanoDB));
  db->fp = fopen(filename, "ab+");
  db->index = malloc(sizeof(IndexEntry) * 1000);
  db->count = 0;
  return db;
}

void db_set(NanoDB *db, const char *key, const char *value) {
  // Append-Only: Always write to the end of the file
  fseek(db->fp, 0, SEEK_END);
  long offset = ftell(db->fp);

  uint32_t len = strlen(value);
  fwrite(value, 1, len, db->fp);
  fflush(db->fp);

  strcpy(db->index[db->count].key, key);
  db->index[db->count].file_offset = offset;
  db->index[db->count].val_size = len;
  db->count++;

  printf("Saved '%s' at offset %ld\n", key, offset);
}

// DJB2 hash algorithm
unsigned long hash_function(const char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;
  return hash % 1000;
}

void update_index(NanoDB *db, const char *key, long offset, uint32_t len) {
  unsigned long index = hash_function(key);
  strcpy(db->index[index].key, key);
  db->index[index].file_offset = offset;
  db->index[index].val_size = len;
}

char *db_get(NanoDB *db, const char *key) {
  unsigned long index = hash_function(key);

  if (strcmp(db->index[index].key, key) == 0) {
    char *val = malloc(db->index[index].val_size + 1);
    fseek(db->fp, db->index[index].file_offset, SEEK_SET);
    fread(val, 1, db->index[index].val_size, db->fp);
    val[db->index[index].val_size] = '\0';
    return val;
  }
  return NULL;
}

// Load existing keys from disk into the hash index on startup
void db_bootstrap(NanoDB *db) {
  rewind(db->fp);
  char key_buffer[256];
  long offset;

  while (1) {
    offset = ftell(db->fp);
    if (fscanf(db->fp, "%255s", key_buffer) != 1)
      break;
    strcpy(db->index[db->count].key, key_buffer);
    db->index[db->count].file_offset = offset;
    db->count++;
  }
  printf("Bootstrap complete: Loaded %d keys\n", db->count);
}

void db_close(NanoDB *db) {
  if (db->fp) {
    fclose(db->fp);
  }
  free(db->index);
  free(db);
  printf("Database closed.\n");
}

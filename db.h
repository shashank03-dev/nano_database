#ifndef DB_H
#define DB_H

#include <stdio.h>
#include <stdint.h>

// This is the structure for each entry in our datbase 
typedef struct {
    char key[256];      // The key (max 255 chars + null)
    long file_offset;   // Where the value is in the Diary
    uint32_t val_size;  // size of the value
} IndexEntry;

// The Database state
typedef struct {
    FILE *fp;           // The file pointer
    IndexEntry *index;  // Our "Magic List" array
    int count;          // How many items we have
} NanoDB;

// Functions
NanoDB* db_open(const char *filename);
void db_set(NanoDB *db, const char *key, const char *value);
char* db_get(NanoDB *db, const char *key);
void db_close(NanoDB *db);

void db_bootstrap(NanoDB *db);
#endif
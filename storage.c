#include "db.h"
#include <stdlib.h>
#include <string.h>

NanoDB* db_open(const char *filename) {
    NanoDB *db = malloc(sizeof(NanoDB));
    // Open for "append and read" in binary mode
    db->fp = fopen(filename, "ab+");
    db->index = malloc(sizeof(IndexEntry) * 1000); // 1000 keys can be stored
    db->count = 0;
    
    // In a real project, you'd "replay" the file here to fill the index
    return db;
}

void db_set(NanoDB *db, const char *key, const char *value) {
    // 1. Move to the end 
    fseek(db->fp, 0, SEEK_END);
    long offset = ftell(db->fp);
    
    // 2. Write the value 
    uint32_t len = strlen(value);
    fwrite(value, 1, len, db->fp);
    fflush(db->fp); // Ensure it's saved tn the disk
    
    // 3. Update the values
    strcpy(db->index[db->count].key, key);
    db->index[db->count].file_offset = offset;
    db->index[db->count].val_size = len;
    db->count++;
    
    printf("Saved '%s' at offset %ld\n", key, offset);
}

char* db_get(NanoDB *db, const char *key) {
    for (int i = 0; i < db->count; i++) {
        if (strcmp(db->index[i].key, key) == 0) {
            // Found it in the list! Now go to the Diary.
            char *val = malloc(db->index[i].val_size + 1);
            fseek(db->fp, db->index[i].file_offset, SEEK_SET);
            fread(val, 1, db->index[i].val_size, db->fp);
            val[db->index[i].val_size] = '\0'; //Add null terminator
            return val;
        }
    }
    return NULL; // if Not found
}

void db_bootstrap(NanoDB *db) {
    // 1. Go to the start of the file
    rewind(db->fp);
    
    char key_buffer[256];
    long offset;
    
    // 2. Read through the file until the end
    // Note: This is a simplified version for your prototype!
    while (1) {
        offset = ftell(db->fp);
        
        // This is a bit tricky: we look for our saved keys
        // In a real DB, we'd store the Key Length first to make this easier
        if (fscanf(db->fp, "%255s", key_buffer) != 1) break;
        
        // Update our Magic List so we know where this key lives
        strcpy(db->index[db->count].key, key_buffer);
        db->index[db->count].file_offset = offset;
        db->count++;
    }
    printf("--- Bootstrap Complete: Loaded %d keys ---\n", db->count);
}



void db_close(NanoDB *db) {
    if (db->fp) {
        fclose(db->fp); // Close the Diary file
    }
    free(db->index);    // Throw away the Magic List from RAM
    free(db);           // Free the Robot's memory
    printf("Database closed safely.\n");
}



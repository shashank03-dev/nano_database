#include "db.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    NanoDB *db = db_open("my_database.db");
    
    // NEW: Load existing data from the file
    db_bootstrap(db);

    // Now, even if you comment out the 'db_set' lines, 
    // 'db_get' will still work because of the bootstrap!
    char *name = db_get(db, "user:1");
    if (name) {
        printf("Recovered Value: %s\n", name);
        free(name);
    }

    db_close(db);
    return 0;
}
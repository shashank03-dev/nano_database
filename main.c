#include "db.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    NanoDB *db = db_open("my_database.db");
    db_bootstrap(db);

    char *name = db_get(db, "user:1");
    if (name) {
        printf("Recovered Value: %s\n", name);
        free(name);
    }

    db_close(db);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "db.h"

#define PORT 8888
#define BUFFER_SIZE 1024

void handle_client(int client_socket, NanoDB *db) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_socket, buffer, BUFFER_SIZE, 0);

        char cmd[10], key[256], val[256];
        int parts = sscanf(buffer, "%s %s %s", cmd, key, val);

        if (strcmp(cmd, "GET") == 0) {
            char *res = db_get(db, key);
            send(client_socket, res ? res : "(error) Key not found", 22, 0);
        } 
        else if (strcmp(cmd, "SET") == 0) {
            db_set(db, key, val);
            send(client_socket, "+OK\r\n", 5, 0);
        }
        else if (strcmp(cmd, "EXISTS") == 0) {
            char *res = db_get(db, key);
            send(client_socket, res ? ":1\r\n" : ":0\r\n", 4, 0);
        }
        else if (strcmp(cmd, "STATS") == 0) {
            char stats[100];
            sprintf(stats, "Total Keys: %d\n", db->count);
            send(client_socket, stats, strlen(stats), 0);
        }
        else if (strcmp(cmd, "QUIT") == 0) {
            send(client_socket, "Bye!\n", 5, 0);
            break;
        }
    }
}

int main() {
    NanoDB *db = db_open("network_db.db");
    db_bootstrap(db);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address = { .sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(PORT) };

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("NanoDB Server started on port %d...\n", PORT);

    while (1) {
        int client_socket = accept(server_fd, NULL, NULL);
        printf("Client connected!\n");
        handle_client(client_socket, db);
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"
#include "db.h"

#define DB_INIT_SIZE 1024 * 1024 // 1MB

// Custom wrapper for SQLite error checking
void check(int rc, sqlite3 *db) {
    if (rc != SQLITE_OK && rc != SQLITE_ROW && rc != SQLITE_DONE) {
        fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
        if (db) sqlite3_close(db);
        exit(1);
    }
}


// read string until ';' from stdin
char *read_full_command(){
    size_t m = 64;
    char *cmd = malloc(m);
    int len = 0;

    char c;
    for(;;){
        c = getc(stdin);
        
        if(c == EOF){
            free(cmd);
            return NULL;
        }

        if(len - 1 >= m){
            m *= 2;
            cmd = realloc(cmd, m);
        }

        cmd[len++] = c;

        if(c == ';') break;
    }

    cmd[len++] = '\0';

    return cmd;
}

int run_repl(const char *filename) {
    sqlite3 *db = NULL;
    
    DB_info_tail *info = load_metadata(filename);
    unsigned char *db_buffer = load_db(filename);
    
    int rc = sqlite3_open(":memory:", &db);
    check(rc, db);
    
    // Load empty or pre-filled database into memory
    rc = sqlite3_deserialize(
        db, "main", db_buffer, (info)? info->len : 0, (info)? info->len : 0,
        SQLITE_DESERIALIZE_RESIZEABLE
    );
    check(rc, db);

    free(info);
    printf("SQLite REPL started (in-memory DB)\n");
    printf("queries have to be ended with ';'\n");

    while (1) {
        printf("sql> ");
        char *cmd = read_full_command();
        if (cmd == NULL) break;

        sqlite3_stmt *stmt = NULL;
        rc = sqlite3_prepare_v2(db, cmd, -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Error: %s\n", sqlite3_errmsg(db));
            continue;
        }

        int ncols = sqlite3_column_count(stmt);
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            for (int i = 0; i < ncols; ++i) {
                const char *txt = (const char *)sqlite3_column_text(stmt, i);
                printf("%s\t", txt ? txt : "NULL");
            }
            printf("\n");
        }

        sqlite3_finalize(stmt);


        sqlite3_int64 new_size;
        db_buffer = sqlite3_serialize(db, "main", &new_size, 0);

        update_db(filename, new_size, db_buffer);
        
        free(cmd);
    }

    sqlite3_close(db);
    free(db_buffer);
    return 0;
}

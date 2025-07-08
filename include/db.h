#pragma once
#include <inttypes.h>

#define DB_INFO_TAIL_MARKER ".db_info"

// this struct sits right before the binary EOF
typedef struct {
    uint64_t len;
    char marker[sizeof(DB_INFO_TAIL_MARKER)];
} DB_info_tail;


DB_info_tail *load_metadata(const char *filename);

unsigned char *load_db(const char *filename);

void update_db(const char *filename, unsigned long long buffer_len, unsigned char *buffer);


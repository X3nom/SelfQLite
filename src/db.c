#define _GNU_SOURCE
#include "db.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>



DB_info_tail *load_metadata(const char *filename){
    DB_info_tail *info = malloc(sizeof(DB_info_tail));
    FILE *f = fopen(filename, "rb");

    fseek(f, -sizeof(DB_info_tail), SEEK_END);
    fread(info, 1, sizeof(DB_info_tail), f);

    fclose(f);

    if(strcmp(info->marker, DB_INFO_TAIL_MARKER) != 0){ // marker not found
        free(info);
        return NULL;
    }

    return info;
}

unsigned char *load_db(const char *filename){
    DB_info_tail *info = load_metadata(filename);
    if(!info){
        return NULL; // return 0 size buffer
    }

    FILE *f = fopen(filename, "rb");
    
    fseek(f, -(sizeof(DB_info_tail) + info->len), SEEK_END);

    unsigned char *buffer = malloc(info->len);

    fread(buffer, 1, info->len, f);

    fclose(f);

    free(info);

    return buffer;
}

void update_db(const char *filename, unsigned long long buffer_len, unsigned char *buffer){
    DB_info_tail *info = load_metadata(filename);
    if(info){
        // truncate the db data
        FILE *f = fopen(filename, "rb");
        fseek(f, 0, SEEK_END);
        long file_size = ftell(f);
        fclose(f);

        truncate(filename, file_size - (sizeof(DB_info_tail) + info->len));
        free(info);
    }
    DB_info_tail new_info;
    strcpy(new_info.marker, DB_INFO_TAIL_MARKER);
    new_info.len = buffer_len;

    FILE *f = fopen(filename, "a");

    fwrite(buffer, 1, buffer_len, f);
    fwrite(&new_info, 1, sizeof(DB_info_tail), f);

    fclose(f);
}
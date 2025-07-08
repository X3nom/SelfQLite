#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "repl.h"

#define IN_MEM_FILENAME "temp_bin"

#define BUF_SIZE 1024

void run_from_memory(char *self_binary_path){

    int mem_fd = memfd_create(IN_MEM_FILENAME, 0);
    if(mem_fd <= 0) exit(1);

    FILE *self_bin = fopen(self_binary_path, "rb");
    if(self_bin == NULL) exit(1);

    char buffer[BUF_SIZE];
    for(;;){
        int bytes = fread(buffer, 1, BUF_SIZE, self_bin);
        if(bytes == 0) break;
        else if(bytes < 0) exit(1);
        write(mem_fd, buffer, bytes);
    }
    fclose(self_bin);

    // mark in-mem file as executable
    fchmod(mem_fd, 0755);
    char *argv[] = { self_binary_path, NULL };
    // execute
    fexecve(mem_fd, argv, environ);
}




int main(int argc, const char **argv){
    char self_file_path[512];
    int fp_len = readlink("/proc/self/exe", self_file_path, 512);
    self_file_path[fp_len] = 0;

    // is in memory
    if(strstr(self_file_path, "/memfd:" IN_MEM_FILENAME)){
        run_repl(argv[0]);
    }
    // is not in memory
    else {
        run_from_memory(self_file_path);
    }
}

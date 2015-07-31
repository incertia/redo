#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "build.h"
#include "redo.h"
#include "util.h"

extern char **environ;

int main(int argc, char **argv){
    int i = 1;
    for(; i < argc; i++){
        int r = redo(argv[i]);

        if(r) return r;
    }
    return 0;
}

char *basenameNoExt(const char *const path){
    ssize_t i = strlen(path) - 1;
    char *bn = NULL;
    for(; i >= 0; i--) if(path[i] == '/') break;
    bn = strdup(path + i + 1);
    i = strlen(bn) - 1;
    for(; i >= 0; i--){
        if(bn[i] == '.'){
            bn[i] = '\0';
            break;
        }
    }
    return bn;
}

BOOL hasExtension(const char *const path){
    ssize_t i = strlen(path) - 1;
    for(; i >= 0; i--) if(path[i] == '.') return TRUE;
    return FALSE;
}

char *extension(const char *const path){
    ssize_t i = strlen(path) - 1;
    for(; i >= 0; i--) if(path[i] == '.') return strdup(path + i);
    return strdup("");
}

char *xextension(const char *const path){
    char *s = strdup(path);
    ssize_t i = strlen(s) - 1;
    for(; i >= 0; i--) if(s[i] == '.'){ s[i] = '\0'; break; }
    return s;
}

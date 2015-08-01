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

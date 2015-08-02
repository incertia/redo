#include "build.h"
#include "redo.h"
#include "util.h"

int main(int argc, char **argv){
    if(argc == 1){
        return redo("");
    } else {
        int i = 1;
        for(; i < argc; i++){
            int r = redo(argv[i]);

            if(r) return r;
        }
    }
}

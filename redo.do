echo "basename=$2"
gcc -o $3 -g -O0 redo.c build.c util.c

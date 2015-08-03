if [ ! -e obj ]; then
    mkdir obj
fi

gcc -c -g -o $3 src/$2.c

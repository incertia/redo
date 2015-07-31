#include <stdlib.h>
#include <string.h>

#include "redo.h"
#include "util.h"

/* gets the basename without extension */
char *basenameNoExt(const char *const path){
    char *basename = xbasename(path);
    char *ret = removeExtension(basename);
    free(basename);
    return ret;
}

/* gets the basename with extension */
char *xbasename(const char *const path){
    char *b = strrchr(path, '/');
    return b ? strdup(b + 1) : strdup(path);
}

/* true if the basename has an extension */
BOOL hasExtension(const char *const path){
    const char *b = strrchr(path, '/');
    b = b ? b : path;
    return strchr(b, '.') != NULL;
}

/* gets the extension from path */
char *xextension(const char *const path){
    char *basename = xbasename(path);
    char *dot = strrchr(basename, '.');
    dot = dot ? strdup(dot) : strdup("");
    free(basename);
    return dot;
}

/* removes the extension from path */
char *removeExtension(const char *const path){
    char *ret = strdup(path);
    char *dot = strrchr(ret, '.');
    if(dot) *dot = '\0';
    return ret;
}

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

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

/* gets the directory leading up to path */
char *xdirname(const char *const path){
    char *ret = strdup(path);
    char *s = strrchr(ret, '/');
    /* set the part after the / to null */
    if(s) *(s + 1) = '\0';
    return ret;
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

/* TRUE if the file at path exists */
BOOL fileExists(const char *const path){
    struct stat s;
    if(stat(path, &s) == -1){
        if(errno == ENOENT) return FALSE;
        else return 2;
    } else {
        return TRUE;
    }
}

/* replaces the basename without extension */
char *replaceBasename(const char *const path, const char *const basename){
    char *dname, *ext, *ret;
    size_t dnamelen, extlen, basenamelen;

    dname = xdirname(path);
    ext = xextension(path);

    dnamelen    = strlen(dname);
    extlen      = strlen(ext);
    basenamelen = strlen(basename);

    ret = malloc((dnamelen + extlen + basenamelen + 1) * sizeof(char));
    memset(ret, 0, (dnamelen + extlen + basenamelen + 1) * sizeof(char));
    strcat(ret, dname);
    strcat(ret, basename);
    strcat(ret, ext);

    free(ext);
    free(dname);

    return ret;
}

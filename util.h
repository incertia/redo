#ifndef __UTIL_H__
#define __UTIL_H__

typedef int BOOL;

/* gets the name without a directory or extension */
char *basenameNoExt(const char *const path);
BOOL hasExtention(const char *const path);
char *extension(const char *const path);
char *xextension(const char *const path);

#endif /* #ifndef __UTIL_H__ */

#ifndef __UTIL_H__
#define __UTIL_H__

typedef int BOOL;

/* gets the basename without extension */
char *basenameNoExt(const char *const path);

/* gets the basename with extension */
char *xbasename(const char *const path);

/* true if the basename has an extension */
BOOL hasExtention(const char *const path);

/* gets the extension from path */
char *xextension(const char *const path);

/* removes the extension from path */
char *removeExtension(const char *const path);

/* TRUE if the file at path exists */
BOOL fileExists(const char *const path);

#endif /* #ifndef __UTIL_H__ */

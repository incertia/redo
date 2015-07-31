#ifndef __BUILD_H__
#define __BUILD_H__

int fexec(char *const redoscript, char *const target);
int redo(char *const target);
int redoDefault(const char *const target, const char *const doFile, const char *const ddoFile);

#endif /* #ifndef __BUILD_H__ */

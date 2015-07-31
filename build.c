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

int fexec(char *const redoscript, char *const target){
    int pid = fork();
    if(pid == 0){
        /* child */
        char *const args[] = {SH, "-x", redoscript, "-", "-", target, NULL};
        int e;

        execvp(SH, args);

        /* we had an error */
        e = errno;
        fprintf(stderr, "execv error(%d): %s\n", errno, strerror(e));
        exit(e);
    } else if(pid > 0){
        /* parent -- wait for child and get the result */
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)){
            return WEXITSTATUS(status);
        } else if(WIFSIGNALED(status)){
            int termsig = WTERMSIG(status);
            fprintf(stderr, "terminated with signal %d\n", termsig);
            return termsig;
        } else {
            return -2;
        }
    } else {
        /* error */
        fprintf(stderr, "error: fork() failed\n");
        return -1;
    }
}

int redo(char *const target){
    char *targetBasename, *ext, *doFile, *ddoFile;
    ssize_t len, elen, doFileLen, ddoFileLen;
    struct stat s;
    int ret;

    /* compute target basename related fields */
    targetBasename = basenameNoExt(target);
    ext = extension(target);
    len = strlen(target), elen = strlen(ext);

    /* compute doFile */
    doFileLen = len + strlen(DO_EXT) + 1;
    doFile = malloc(doFileLen * sizeof(char));
    memset(doFile, 0, doFileLen * sizeof(char));
    strncat(doFile, target, len);
    strncat(doFile, DO_EXT, strlen(DO_EXT));

    /* compute ddoFile (default doFile) */
    ddoFileLen = strlen(DEFAULT) + elen + strlen(DO_EXT) + 1;
    ddoFile = malloc(ddoFileLen * sizeof(char));
    memset(ddoFile, 0, ddoFileLen * sizeof(char));
    strncat(ddoFile, DEFAULT, strlen(DEFAULT));
    strncat(ddoFile, ext, elen);
    strncat(ddoFile, DO_EXT, strlen(DO_EXT));

    /* DEBUG */
    printf("basename=%s,ext=%s,target=%s,doFile=%s,ddoFile=%s\n", targetBasename, ext, target, doFile, ddoFile);

    /* check if doFile exists */
    /* TODO: refactor this into its own function */
    if(stat(doFile, &s) == -1){
        int e = errno;
        if(e == ENOENT){
            fprintf(stderr, "error: %s: no such file or directory\n", doFile);
            ret = redoDefault(target, doFile, ddoFile);
        } else {
            ret = 50;
        }
    } else {
        ssize_t redoTargetLen = strlen(targetBasename) + strlen(REDO_EXT) + 1;
        char *redoTarget = malloc(redoTargetLen * sizeof(char));
        memset(redoTarget, 0, redoTargetLen * sizeof(char));
        strncat(redoTarget, targetBasename, strlen(targetBasename));
        strncat(redoTarget, REDO_EXT      , strlen(REDO_EXT));

        ret = fexec(doFile, redoTarget);
        if(ret == 0){
            ret = rename(redoTarget, target);
            if(ret) perror("rename");
        } else {
            fprintf(stderr, "error: failed to build target %s\n", targetBasename);
        }

        free(redoTarget);
    }

    /* free stuffs */
    free(ddoFile);
    free(doFile);
    free(ext);
    free(targetBasename);

    return ret;
}

int redoDefault(const char *const target, const char *const doFile, const char *const ddoFile){
    return 1;
}

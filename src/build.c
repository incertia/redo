#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "build.h"
#include "redo.h"
#include "util.h"

/* forward declares */
static int build(char *const target, char *const redoTarget, char *const doFile, char *const basename);

/* exec do script with params */
static int fexec(char *const redoscript, char *const target, char *const targetBasename){
    int pid = fork();
    if(pid == 0){
        /* child */
        char *const args[] = {SH, "-e", "-x", redoscript, "-", targetBasename, target, NULL};
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

/* redo target */
int redo(char *const target){
    char *targetBasename, *ext, *doFile, *ddoFile, *redoTarget;
    ssize_t len, elen, doFileLen, ddoFileLen, redoTargetLen;
    int ret;

    /* compute target basename related fields */
    targetBasename = basenameNoExt(target);
    ext = xextension(target);
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
    printf("targetBasename=%s,ext=%s,target=%s,doFile=%s,ddoFile=%s\n", targetBasename, ext, target, doFile, ddoFile);

    /* compute build target */
    redoTargetLen = strlen(target) + strlen(REDO_EXT) + 1;
    redoTarget = malloc(redoTargetLen * sizeof(char));
    memset(redoTarget, 0, redoTargetLen * sizeof(char));
    strncat(redoTarget, target  , strlen(target));
    strncat(redoTarget, REDO_EXT, strlen(REDO_EXT));

    /* use doFile if it exists */
    if(fileExists(doFile)){
        printf("===redo %s===\n", doFile);
        ret = build(target, redoTarget, doFile, targetBasename);
    /* otherwise use ddoFile if it exists */
    } else if(fileExists(ddoFile)){
        printf("===redo %s===\n", ddoFile);
        ret = build(target, redoTarget, ddoFile, targetBasename);
    /* otherwise error */
    } else {
        fprintf(stderr, "error: No do file for target '%s'. Consider writing %s.do or %s%s.do.\n", target, target, DEFAULT, ext);
        ret = 2;
    }

    /* free stuffs */
    free(redoTarget);
    free(ddoFile);
    free(doFile);
    free(ext);
    free(targetBasename);

    return ret;
}

/* run the doFile with given parameters */
static int build(char *const target, char *const redoTarget, char *const doFile, char *const basename){
    if(!fileExists(doFile)){
        fprintf(stderr, "error: %s: no such file or directory\n", doFile);
        return -1;
    } else {
        int ret = fexec(doFile, redoTarget, basename);
        if(ret == 0){
            int e;
            ret = rename(redoTarget, target);
            e = errno; /* we should save this somewhere */
            if(ret){
                if(e == ENOENT){
                    /* phony target */
                    fprintf(stderr, "debug: phony target detected\n");
                    ret = 0;
                } else {
                    perror("rename");
                }
            }
        }
        return ret;
    }
}

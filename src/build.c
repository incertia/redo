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
    char *targetBasename, *doFile, *ddoFile, *redoTarget, *newname;
    ssize_t len, doFileLen, ddoFileLen, redoTargetLen, newnameLen;
    int ret;

    /* compute target basename related fields */
    targetBasename = basenameNoExt(target);
    len = strlen(target);

    /* compute doFile */
    doFileLen = len + strlen(DO_EXT) + 1;
    doFile = malloc(doFileLen * sizeof(char));
    memset(doFile, 0, doFileLen * sizeof(char));
    strcat(doFile, target);
    strcat(doFile, DO_EXT);

    /* base for ddoFile */
    newname = replaceBasename(target, DEFAULT);
    newnameLen = strlen(newname);

    /* compute ddoFile (default doFile) */
    ddoFileLen = newnameLen + strlen(DO_EXT) + 1;
    ddoFile = malloc(ddoFileLen * sizeof(char));
    memset(ddoFile, 0, ddoFileLen * sizeof(char));
    strcat(ddoFile, newname);
    strcat(ddoFile, DO_EXT);

    /* DEBUG */
    printf("targetBasename=%s,target=%s,doFile=%s,ddoFile=%s\n", targetBasename, target, doFile, ddoFile);

    /* compute build target */
    redoTargetLen = strlen(target) + strlen(REDO_EXT) + 1;
    redoTarget = malloc(redoTargetLen * sizeof(char));
    memset(redoTarget, 0, redoTargetLen * sizeof(char));
    strcat(redoTarget, target  );
    strcat(redoTarget, REDO_EXT);

    /* redo */
    printf("===redo %s===\n", target);
    /* use doFile if it exists */
    if(strlen(target) && fileExists(doFile)){
        ret = build(target, redoTarget, doFile, targetBasename);
    /* otherwise use ddoFile if it exists */
    } else if(fileExists(ddoFile)){
        ret = build(target, redoTarget, ddoFile, targetBasename);
    /* otherwise error */
    } else {
        fprintf(stderr, "error: No do file for target '%s'. Consider writing %s or %s.\n", target, doFile, ddoFile);
        ret = 2;
    }

    /* free stuffs */
    free(redoTarget);
    free(ddoFile);
    free(newname);
    free(doFile);
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

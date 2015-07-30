#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SH          "sh"
#define DO_EXT      ".do"
#define REDO_EXT    "--redoing"

extern char **environ;

int fexec(char *const redoscript, char *const target);
int redo(char *const target);

/* gets the name without a directory or extension */
char *basenameNoExt(const char *const path, const char *const ext);

int main(int argc, char **argv){
    int i = 1;
    for(; i < argc; i++){
        int r = redo(argv[i]);

        if(r) return r;
    }
    return 0;
}

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
    char *targetBasename = basenameNoExt(target, DO_EXT);
    ssize_t len = strlen(targetBasename);
    struct stat s;
    int ret;
    char *doFile = malloc((len + 3 + 1) * sizeof(char));
    strncpy(doFile, targetBasename, len);
    strncat(doFile, DO_EXT, strlen(DO_EXT));

    /* check if doFile exists */
    if(stat(doFile, &s) == -1){
        int e = errno;
        if(e == ENOENT){
            fprintf(stderr, "error: %s: no such file or dirrectory\n", doFile);
            ret = 1;
        } else {
            ret = 50;
        }
    } else {
        char *redoTarget = malloc((strlen(targetBasename) + strlen(REDO_EXT) + 1) * sizeof(char));
        strncpy(redoTarget, targetBasename, strlen(targetBasename));
        strncat(redoTarget, REDO_EXT      , strlen(REDO_EXT));

        ret = fexec(doFile, redoTarget);
        if(ret == 0){
            ret = rename(redoTarget, targetBasename);
            if(ret) perror("rename");
        } else {
            fprintf(stderr, "error: failed to build target %s\n", targetBasename);
        }

        free(redoTarget);
    }

    free(doFile);
    free(targetBasename);

    return ret;
}

char *basenameNoExt(const char *const path, const char *const ext){
    ssize_t i = strlen(path) - 1;
    char *bn = NULL;
    for(; i >= 0; i--) if(path[i] == '/') break;
    bn = strdup(path + i + 1);
    i = strlen(bn) - 1;
    for(; i >= 0; i--){
        if(bn[i] == '.'){
            if(strcmp(bn + i, ext) == 0) bn[i] = '\0';
            break;
        }
    }
    return bn;
}

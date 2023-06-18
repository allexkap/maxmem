#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>



int child_alive = 1;

void sigchld(int) {
    child_alive = 0;
}


size_t get_rss(pid_t pid) {
    static char path[32] = {};
    size_t size = 0;

    snprintf(path, 32, "/proc/%ld/status", pid);
    FILE *file = fopen(path, "r");
    if (!file) return 0;

    char *pattern = "VmRSS";
    int pos = 0;
    while (!feof(file) && !size) {
        if (getc(file) == pattern[pos]) ++pos;
        else if (pattern[pos]) pos = 0;
        else fscanf(file, "%lu", &size);
    }

    return size;
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        return 1;
    }

    signal(SIGCHLD, &sigchld);

    pid_t pid = fork();
    if (!pid) {
        execvp(argv[1], argv+2);
        perror(NULL);
        return 1;
    }

    int size = 0, res = 0;
    while (child_alive) {
        res = get_rss(pid);
        if (res > size) size = res;
    }

    fprintf(stdout, "Required memory %lu kB\n", size);

    return 0;
}

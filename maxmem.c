#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>



size_t get_rss(pid_t pid) {

    static char path[64] = {};
    FILE *file = NULL;
    size_t size = 0;


    snprintf(path, 64, "/proc/%lu/status", pid);
    file = fopen(path, "r");
    if (!file) return size;

    char *pattern = "VmRSS";
    int pos = 0;
    while (!feof(file) && !size) {
        if (getc(file) == pattern[pos]) ++pos;
        else if (pattern[pos]) pos = 0;
        else fscanf(file, "%lu", &size);
    }
    fclose(file);


    snprintf(path, 64, "/proc/%lu/task/%lu/children", pid, pid);
    file = fopen(path, "r");
    if (!file) return size;

    size_t pids[64];    // todo
    pos = 0;
    while (!feof(file) && pos != 64 && fscanf(file, "%lu", pids + pos++) > 0);
    fclose(file);

    for (int i = 0; i < pos; ++i) size += get_rss(pids[i]);

    return size;
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        return 1;
    }

    pid_t pid = fork();
    if (!pid) {
        execvp(argv[1], argv+1);
        perror(NULL);
        return 1;
    }

    int size = 0, res = 0;
    while (1) {
        if ((res = get_rss(pid)) > size) size = res;
        if (waitpid(pid, NULL, WNOHANG) == pid) break;
    }

    fprintf(stdout, "Required memory %lu kB\n", size);

    return 0;
}

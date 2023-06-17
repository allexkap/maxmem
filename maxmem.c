#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


size_t collect(pid_t pid) {
    static char path[32] = {};
    static FILE *file = NULL;
    size_t size = 0;

    snprintf(path, 32, "/proc/%d/status", pid);
    file = fopen(path, "r");

    char *buff = malloc(64);
    while (!feof(file)) {
        getline(&buff, &(size_t){64}, file);
        if (strncmp(buff, "VmRSS:\t", 6)) continue;
        printf("%s", buff+7);
    }
    pclose(file);
    free(buff);

    return size;
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        return 1;
    }

    pid_t pid = fork();
    if (!pid) {
        execvp(argv[1], argv+2);
        perror(NULL);
        return 1;
    }

    printf("pid %d\n", pid);


    int size = 0, res = 0;
    for (int i = 0; i < 100; ++i) {
        res = collect(pid);
        if (res > size) size = res;
    }

    printf("%d kB\n", res);

    return 0;
}

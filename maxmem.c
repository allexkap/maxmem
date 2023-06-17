#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


size_t collect(pid_t pid) {
    static char cmd[32] = {};
    static FILE *pipe = NULL;

    snprintf(cmd, 32, "ps -horss -p%d", pid);
    pipe = popen(cmd, "r");

    size_t size = 0;
    fscanf(pipe, "%d", &size);
    pclose(pipe);

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

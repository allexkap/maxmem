#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


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

    return 0;
}

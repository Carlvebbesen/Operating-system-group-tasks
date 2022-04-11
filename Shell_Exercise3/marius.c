#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

char *getWorkingDir()
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        return cwd;
    }
    else
    {
        perror("getcwd() error");
        return "error";
    }
}

int changeDir(const char *path) {
    int dirVal = chdir(path);
    if (dirVal == -1) {
        printf("cd: no such file or directory: %s\n", path);
    }
}

int main()
{
    char command[25];
    char cwd[PATH_MAX];
    while (1)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            bzero(command, 25);
            printf("%s: ", cwd);
            scanf("%25s", command);
            //printf("Your command was: %s\n", command);
            if (strcmp(command, "cd")) {
                changeDir(command);
            }

            fflush(stdout);
        }
        else
        {
            perror("getcwd() error");
            return 0;
        }
    }

    return 0;
}
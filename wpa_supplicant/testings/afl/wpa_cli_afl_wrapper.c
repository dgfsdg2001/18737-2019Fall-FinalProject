#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

char targetFile[] = "./../../wpa_cli";
char targetFileName[] = "wpa_cli"; 

int parseBufToCmdsArgs(char* str, char* argv[], int max_argc) {
    char* pch;
    int argc = 0;

    if (!str || !argv)
        return -1;

    /* Make argv[0] equal to file name */
    if (argc < max_argc) {
        argv[argc++] = targetFileName;
    }

    pch = strtok (str," ");
    while (pch != NULL) {
        if (argc < max_argc) {
            argv[argc++] = pch;
            pch = strtok(NULL, " ");
        } else
            return -1;   
    }
    argv[argc] = NULL; // null terminated for exec, make sure the len of argv is larger then max_argc
    return argc;
}

uint8_t* readFile(const char* file) {
    FILE* fd;
    uint8_t *buf;
    size_t len, result;

    fd = fopen(file, "r");
    if (fd == NULL) {
        printf("Open %s fail\n", file);
	    return NULL;
    }

    fseek(fd, 0, SEEK_END); len = ftell(fd); rewind(fd);
    buf = malloc(len);
    if (!buf) {
        printf("malloc fail, len %ld\n", len);
	    return NULL;
    }

    result = fread(buf, 1, len, fd);
    if (result != len) {
	    printf("read file faile\n");
        free(buf);
	    return NULL;
    }
    fclose(fd);
    return buf;
}

int main(int argc, char** argv) {

    char *argv_wpa_cli[MAX_ARGC+1];
    int ret = -1;

    if (argc < 1) {
        printf("usage: %s <string> <size>\n", argv[0]);
        return -1;
    }

    if (parseBufToCmdsArgs((char*)readFile(argv[1]), argv_wpa_cli, MAX_ARGC)) {
        ret = execv(targetFile, argv_wpa_cli);
        printf("Ouch! something went wrong when executing %s, %s\n", targetFile, strerror(errno));
    }

    return ret;
}
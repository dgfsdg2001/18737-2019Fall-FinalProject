#include <stdio.h>
#include <stdlib.h>
#include <klee/klee.h>

/******************************/
/* Target Entry (wpa_cli.c)   */
/******************************/
int main(int argc, char *argv[]);
int main_wrapper(int argc, char** argv) {

    /* Make command-line arguments readable 
     * This is a workaround since: 
     *   1. The klee arguement "-readable-posix-inputs" does not work.
     *   2. The klee_prefer_cex() API does not work.
     */ 
    int i, flag = 1;
    for (i = 1; i < argc; ++i) {
        char *p = argv[i];
        do {
            if (*p < 33 || *p > 126) {
                flag = 0;
                break;
            } else
                p += 1;
        } while (*p);
    }

    if (flag != 0)
        main(argc, argv);
    
    return 0;
}
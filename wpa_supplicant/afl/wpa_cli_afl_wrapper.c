#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

char targetFile[] = "./../wpa_cli";
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

#ifndef TEST_STANDALONE
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
#else
#include "eloop.h"
/******************************/
/* Fake Variables             */
/******************************/

/******************************/
/* Fake Functions             */
/******************************/
// os_unix.c
/*
int os_program_init(void) {return 0;}
int os_daemonize(const char *pid_file) {return 0;}
void os_daemonize_terminate(const char *pid_file) {return;}
void os_program_deinit(void) {return;}
*/

// eloop.c
int eloop_init(void) {return 0;}
int eloop_register_signal_terminate(eloop_signal_handler handler, void *user_data) {return 0;}
int eloop_register_timeout(unsigned int secs, unsigned int usecs, eloop_timeout_handler handler, void *eloop_data, void *user_data) {return 0;}
int eloop_cancel_timeout(eloop_timeout_handler handler, void *eloop_data, void *user_data) {return 0;}
int eloop_register_read_sock(int sock, eloop_sock_handler handler, void *eloop_data, void *user_data) {return 0;}
int eloop_sock_requeue(void) {return 0;}
int eloop_terminated(void) {return 0;}
void eloop_unregister_read_sock(int sock) {return;}
void eloop_terminate(void) {return;}
void eloop_run(void) {return;}
void eloop_destroy(void) {return;}

// wpa_ctrl.c
struct wpa_ctrl {
#ifdef CONFIG_CTRL_IFACE_UDP
	int s;
#ifdef CONFIG_CTRL_IFACE_UDP_IPV6
	struct sockaddr_in6 local;
	struct sockaddr_in6 dest;
#else /* CONFIG_CTRL_IFACE_UDP_IPV6 */
	struct sockaddr_in local;
	struct sockaddr_in dest;
#endif /* CONFIG_CTRL_IFACE_UDP_IPV6 */
	char *cookie;
	char *remote_ifname;
	char *remote_ip;
#endif /* CONFIG_CTRL_IFACE_UDP */
#ifdef CONFIG_CTRL_IFACE_UNIX
	int s;
	struct sockaddr_un local;
	struct sockaddr_un dest;
#endif /* CONFIG_CTRL_IFACE_UNIX */
#ifdef CONFIG_CTRL_IFACE_NAMED_PIPE
	HANDLE pipe;
#endif /* CONFIG_CTRL_IFACE_NAMED_PIPE */
} g_ctrl;
struct wpa_ctrl * wpa_ctrl_open2(const char *ctrl_path, const char *cli_path) {return &g_ctrl;}
struct wpa_ctrl * wpa_ctrl_open(const char *ctrl_path) {return wpa_ctrl_open2(ctrl_path, NULL);}
int wpa_ctrl_attach(struct wpa_ctrl *ctrl) {return 0;}
int wpa_ctrl_detach(struct wpa_ctrl *ctrl) {return 0;}
int wpa_ctrl_get_fd(struct wpa_ctrl *ctrl) {return 0;}
int wpa_ctrl_pending(struct wpa_ctrl *ctrl) {return 0;}
int wpa_ctrl_recv(struct wpa_ctrl *ctrl, char *reply, size_t *reply_len) {return 0;}
int wpa_ctrl_request(struct wpa_ctrl *ctrl, const char *cmd, size_t cmd_len,char *reply, size_t *reply_len, void (*msg_cb)(char *msg, size_t len)) {
    *reply_len = 0;
    return 0;
}
void wpa_ctrl_close(struct wpa_ctrl *ctrl) {return;}

// others
char* getenv (const char* name) {return 0;}
#endif
#include <stdio.h>
#include <stdlib.h>
#include <klee/klee.h>
#include "../../src/utils/eloop.h"

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

/******************************/
/* Fake Variables             */
/******************************/

/******************************/
/* Fake Functions             */
/******************************/
// os_unix.c
int os_program_init(void) {return 0;}
int os_daemonize(const char *pid_file) {return 0;}
void os_daemonize_terminate(const char *pid_file) {return;}
void os_program_deinit(void) {return;}

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
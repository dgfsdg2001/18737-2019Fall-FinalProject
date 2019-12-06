#include "wpa_cli_wrapper_common.h"
#include "eloop.h"

/******************************/
/* Fake Functions             */
/******************************/
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
struct wpa_ctrl g_ctrl;
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
#ifndef _H_WPA_CLI_WRAPPER_H_
#define _H_WPA_CLI_WRAPPER_H_

#include <stddef.h>
#include "eloop.h"

/******************************/
/* Fake Functions             */
/******************************/

// eloop.c
int eloop_init(void);
int eloop_register_signal_terminate(eloop_signal_handler handler, void *user_data);
int eloop_register_timeout(unsigned int secs, unsigned int usecs, eloop_timeout_handler handler, void *eloop_data, void *user_data);
int eloop_cancel_timeout(eloop_timeout_handler handler, void *eloop_data, void *user_data);
int eloop_register_read_sock(int sock, eloop_sock_handler handler, void *eloop_data, void *user_data);
int eloop_sock_requeue(void);
int eloop_terminated(void);
void eloop_unregister_read_sock(int sock);
void eloop_terminate(void);
void eloop_run(void);
void eloop_destroy(void);

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
};
struct wpa_ctrl * wpa_ctrl_open2(const char *ctrl_path, const char *cli_path);
struct wpa_ctrl * wpa_ctrl_open(const char *ctrl_path);
int wpa_ctrl_attach(struct wpa_ctrl *ctrl);
int wpa_ctrl_detach(struct wpa_ctrl *ctrl);
int wpa_ctrl_get_fd(struct wpa_ctrl *ctrl);
int wpa_ctrl_pending(struct wpa_ctrl *ctrl);
int wpa_ctrl_recv(struct wpa_ctrl *ctrl, char *reply, size_t *reply_len);
int wpa_ctrl_request(struct wpa_ctrl *ctrl, const char *cmd, size_t cmd_len,char *reply, size_t *reply_len, void (*msg_cb)(char *msg, size_t len));
void wpa_ctrl_close(struct wpa_ctrl *ctrl);
#endif
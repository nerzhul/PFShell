#       $OpenBSD: Makefile $

.include <bsd.obj.mk>

PROG=   BSDRouterd
SRCS=   cmd_command.c cmd_command.h cmd_conf.c cmd_conf.h cmd_conf_acl.c cmd_conf_acl.h \
	cmd_conf_fw.c cmd_conf_fw.h cmd_conf_if.c cmd_conf_if.h cmd_conf_rd.c cmd_conf_rd.h \
	cmd_conf_router_ospf.c cmd_conf_router_ospf.h cmd_conf_router_rip.c cmd_conf_router_rip.h \
	cmd_enable.c cmd_enable.h cmd_user.c cmd_user.h configuration.c configuration.h \
	daemon_socket.c daemon_socket.h firewall.c firewall.h interface.c interface.h \
	iputils.c iputils.h main.c prompt.c prompt.h prompt_msg.h \
	route.c route.h string_mgmt.c string_mgmt.h sysunix.c sysunix.h

MAN=

CFLAGS+= -Wall -I${.CURDIR}
CFLAGS+= -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+= -Wmissing-declarations
CFLAGS+= -Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+= -Wsign-compare
CFLAGS+= -DVERSION=\"alpha\ 0.26\"
LDADD+= -lm

.include <bsd.prog.mk>

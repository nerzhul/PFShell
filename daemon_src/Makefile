#       $OpenBSD: Makefile $

PROG=   BSDRouterd

SRCS=   cmd/command.c cmd/command.h cmd/conf.c cmd/conf.h cmd/conf_acl.c cmd/conf_acl.h \
	cmd/conf_fw.c cmd/conf_fw.h cmd/conf_if.c cmd/conf_if.h cmd/conf_rd.c cmd/conf_rd.h \
	cmd/conf_router_ospf.c cmd/conf_router_ospf.h cmd/conf_router_rip.c cmd/conf_router_rip.h \
	cmd/enable.c cmd/enable.h cmd/user.c cmd/user.h configuration.c configuration.h \
	daemon_socket.c daemon_socket.h firewall.c firewall.h interface.c interface.h \
	iputils.c iputils.h main.c prompt/prompt.c prompt/prompt.h prompt/prompt_msg.h \
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

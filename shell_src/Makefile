#       $OpenBSD: Makefile $

PROG=   BSDRouterShell
SRCS=   cli_socket.c cli_socket.h command.c command.h configuration.h \
        main.c prompt.c prompt.h unix_signal.c unix_signal.h

MAN=

CFLAGS+= -Wall -I${.CURDIR}
CFLAGS+= -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+= -Wmissing-declarations
CFLAGS+= -Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+= -Wsign-compare
CFLAGS+= -lncurses -lm
LDADD+= -lncurses -lm

.include <bsd.prog.mk>

bin_PROGRAMS = ripd

AM_CPPFLAGS = -Wall -I${.CURDIR} -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations \
	-Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare


ripd_SOURCES = auth.c carp.c control.c interface.c kroute.c \
        log.c message.c name2id.c neighbor.c packet.c parse.y \
        printconf.c rde.c rde_rib.c ripe.c ripd.c

ripd_LDADD = -levent -lutil

ripd_LDFLAGS = ${LIBEVENT} ${LIBUTIL}



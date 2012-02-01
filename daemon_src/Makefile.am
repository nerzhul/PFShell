bin_PROGRAMS = BSDRouterd

AM_CPPFLAGS = -lncurses -lm -std=gnu99

BSDRouterd_SOURCES = \
	cmd/command.c \
	cmd/command.h \
	cmd/command_conf.c \
	cmd/command_conf.h \
	cmd/command_conf_acl.c \
	cmd/command_conf_acl.h \
	cmd/command_conf_fw.c \
	cmd/command_conf_fw.h \
	cmd/command_conf_if.c \
	cmd/command_conf_if.h \
	cmd/command_conf_rd.c \
	cmd/command_conf_rd.h \
	cmd/command_enable.c \
	cmd/command_enable.h \
	cmd/command_user.c \
	cmd/command_user.h \
	configuration.c \
	configuration.h \
	daemon_socket.c \
	daemon_socket.h \
	firewall.c \
	firewall.h \
	interface.c \
	interface.h \
	iputils.c \
	iputils.h \
	main.c \
	prompt/prompt.c \
	prompt/prompt.h \
	prompt/prompt_msg.h \
	route.c \
	route.h \
	sysunix.c \
	sysunix.h

BSDRouterd_LDADD = -lm

BSDRouterd_LDFLAGS =

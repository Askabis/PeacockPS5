# PeacockPS5 - Background connectivity daemon.
# Copyright (C) 2026 Askabis
# SPDX-License-Identifier: GPL-3.0-or-later

ifndef PS5_PAYLOAD_SDK
$(error PS5_PAYLOAD_SDK is undefined)
endif

include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk

ELF := peacockps5-daemon.elf
SOURCES := main.c peacock_daemon_config.c
OBJECTS := $(SOURCES:.c=.o)

CFLAGS := -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror \
	-ffunction-sections -fdata-sections
LDFLAGS := -Wl,--gc-sections
LIBS := -lSceNet -lSceSsl -lSceHttp2

.PHONY: all clean

all: $(ELF)

$(ELF): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c peacock_daemon_config.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f -- $(ELF) $(OBJECTS)

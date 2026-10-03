# PeacockPS5 - Read-only game diagnostic.
# Copyright (C) 2026 Askabis
# SPDX-License-Identifier: GPL-3.0-or-later
ifndef PS5_PAYLOAD_SDK
$(error PS5_PAYLOAD_SDK is undefined)
endif
include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk
CFLAGS := -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror

peacockps5-inspect.elf: main.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f -- peacockps5-inspect.elf

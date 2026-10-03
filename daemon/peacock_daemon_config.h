/*
 * PeacockPS5 - Background daemon configuration.
 * Copyright (C) 2026 Askabis
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <stdint.h>

#define PEACOCK_DAEMON_CONFIG_PATH "/data/peacockps5/peacock.conf"
#define PEACOCK_DAEMON_LOG_PATH "/data/peacockps5/daemon.log"
#define PEACOCK_DAEMON_STATUS_PATH "/data/peacockps5/status.txt"

typedef struct peacock_daemon_config
{
    char host[64];
    uint16_t port;
    char path[256];
    unsigned int interval_seconds;
} peacock_daemon_config_t;

void peacock_daemon_config_defaults(peacock_daemon_config_t *config);
int peacock_daemon_config_load(const char *path, peacock_daemon_config_t *config);

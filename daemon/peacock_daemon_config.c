/*
 * PeacockPS5 - Background daemon configuration.
 * Copyright (C) 2026 Askabis
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "peacock_daemon_config.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *trim(char *text)
{
    char *end = NULL;

    while (*text != '\0' && isspace((unsigned char)*text) != 0)
        ++text;
    end = text + strlen(text);
    while (end > text && isspace((unsigned char)end[-1]) != 0)
        --end;
    *end = '\0';
    return text;
}

static int parse_unsigned(const char *text, unsigned long minimum, unsigned long maximum,
                          unsigned long *value)
{
    char *end = NULL;
    unsigned long parsed = 0;

    errno = 0;
    parsed = strtoul(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || parsed < minimum || parsed > maximum)
        return -1;
    *value = parsed;
    return 0;
}

void peacock_daemon_config_defaults(peacock_daemon_config_t *config)
{
    if (config == NULL)
        return;
    memset(config, 0, sizeof(*config));
    snprintf(config->host, sizeof(config->host), "%s", "192.168.1.10");
    config->port = 80;
    snprintf(config->path, sizeof(config->path), "%s",
             "/authentication/api/configuration/Init");
    config->interval_seconds = 15;
}

int peacock_daemon_config_load(const char *path, peacock_daemon_config_t *config)
{
    FILE *file = NULL;
    char line[512] = {0};

    if (path == NULL || config == NULL)
        return -1;
    peacock_daemon_config_defaults(config);
    file = fopen(path, "r");
    if (file == NULL)
        return -1;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        char *key = NULL;
        char *value = NULL;
        char *separator = NULL;
        unsigned long parsed = 0;

        key = trim(line);
        if (*key == '\0' || *key == '#' || *key == ';')
            continue;
        separator = strchr(key, '=');
        if (separator == NULL)
            continue;
        *separator = '\0';
        value = trim(separator + 1);
        key = trim(key);

        if (strcmp(key, "host") == 0 && *value != '\0')
            snprintf(config->host, sizeof(config->host), "%s", value);
        else if (strcmp(key, "path") == 0 && value[0] == '/')
            snprintf(config->path, sizeof(config->path), "%s", value);
        else if (strcmp(key, "port") == 0 && parse_unsigned(value, 1, 65535, &parsed) == 0)
            config->port = (uint16_t)parsed;
        else if (strcmp(key, "interval_seconds") == 0 &&
                 parse_unsigned(value, 5, 3600, &parsed) == 0)
            config->interval_seconds = (unsigned int)parsed;
    }

    fclose(file);
    return 0;
}

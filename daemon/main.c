/*
 * PeacockPS5 - Background connectivity daemon.
 * Copyright (C) 2026 Askabis
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This payload performs bounded HTTP reachability checks only. It does not
 * inspect or modify another process and contains no game or firmware offsets.
 */

#include "peacock_daemon_config.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

typedef struct notify_request
{
    char reserved[45];
    char message[3075];
} notify_request_t;

int sceKernelSendNotificationRequest(int device, notify_request_t *request, size_t size,
                                     int flags);
int sceNetInit(void);
int sceNetPoolCreate(const char *name, int size, int flags);
int sceNetPoolDestroy(int pool_id);
int sceSslInit(size_t pool_size);
int sceSslTerm(int context_id);
int sceHttp2Init(int net_pool_id, int ssl_context_id, size_t pool_size, int max_requests);
int sceHttp2Term(int context_id);
int sceHttp2CreateTemplate(int context_id, const char *user_agent, int http_version,
                           int auto_proxy);
int sceHttp2DeleteTemplate(int template_id);
int sceHttp2CreateRequestWithURL(int template_id, const char *method, const char *url,
                                 uint64_t content_length);
int sceHttp2DeleteRequest(int request_id);
int sceHttp2SendRequest(int request_id, const void *data, size_t size);
int sceHttp2GetStatusCode(int request_id, int *status_code);

typedef struct http_runtime
{
    int net_pool;
    int ssl_context;
    int http_context;
    int template_id;
} http_runtime_t;

static void notify(const char *message)
{
    notify_request_t request;

    memset(&request, 0, sizeof(request));
    snprintf(request.message, sizeof(request.message), "%s", message);
    (void)sceKernelSendNotificationRequest(0, &request, sizeof(request), 0);
}

static void ensure_data_directory(void)
{
    (void)mkdir("/data/peacockps5", 0755);
}

static void write_state(const peacock_daemon_config_t *config, bool reachable, int status_code)
{
    FILE *status = NULL;
    FILE *log = NULL;
    time_t now = time(NULL);

    status = fopen(PEACOCK_DAEMON_STATUS_PATH, "w");
    if (status != NULL)
    {
        fprintf(status, "reachable=%d\nstatus_code=%d\nhost=%s\nport=%u\npath=%s\n",
                reachable ? 1 : 0, status_code, config->host, (unsigned int)config->port,
                config->path);
        fclose(status);
    }

    log = fopen(PEACOCK_DAEMON_LOG_PATH, "a");
    if (log != NULL)
    {
        fprintf(log, "%lld reachable=%d status=%d target=%s:%u%s\n", (long long)now,
                reachable ? 1 : 0, status_code, config->host, (unsigned int)config->port,
                config->path);
        fclose(log);
    }
}

static int http_runtime_init(http_runtime_t *runtime)
{
    memset(runtime, 0xff, sizeof(*runtime));
    if (sceNetInit() != 0)
        return -1;
    runtime->net_pool = sceNetPoolCreate("peacockps5_daemon", 64 * 1024, 0);
    if (runtime->net_pool < 0)
        return -1;
    runtime->ssl_context = sceSslInit(128 * 1024);
    if (runtime->ssl_context < 0)
        return -1;
    runtime->http_context =
        sceHttp2Init(runtime->net_pool, runtime->ssl_context, 128 * 1024, 1);
    if (runtime->http_context < 0)
        return -1;
    runtime->template_id =
        sceHttp2CreateTemplate(runtime->http_context, "PeacockPS5-Daemon/0.2", 3, 1);
    return runtime->template_id < 0 ? -1 : 0;
}

static void http_runtime_finish(http_runtime_t *runtime)
{
    if (runtime->template_id >= 0)
        (void)sceHttp2DeleteTemplate(runtime->template_id);
    if (runtime->http_context >= 0)
        (void)sceHttp2Term(runtime->http_context);
    if (runtime->ssl_context >= 0)
        (void)sceSslTerm(runtime->ssl_context);
    if (runtime->net_pool >= 0)
        (void)sceNetPoolDestroy(runtime->net_pool);
}

static int probe(http_runtime_t *runtime, const peacock_daemon_config_t *config)
{
    char url[512] = {0};
    int request_id = -1;
    int status_code = 0;

    if (snprintf(url, sizeof(url), "http://%s:%u%s", config->host,
                 (unsigned int)config->port, config->path) >= (int)sizeof(url))
        return -1;
    request_id = sceHttp2CreateRequestWithURL(runtime->template_id, "GET", url, 0);
    if (request_id < 0)
        return -1;
    if (sceHttp2SendRequest(request_id, NULL, 0) != 0 ||
        sceHttp2GetStatusCode(request_id, &status_code) != 0)
        status_code = -1;
    (void)sceHttp2DeleteRequest(request_id);
    return status_code;
}

int main(void)
{
    http_runtime_t runtime;
    peacock_daemon_config_t config;
    bool previous_reachable = false;
    bool previous_known = false;

    ensure_data_directory();
    if (http_runtime_init(&runtime) != 0)
    {
        notify("PeacockPS5 daemon: network initialization failed");
        http_runtime_finish(&runtime);
        return 1;
    }

    notify("PeacockPS5 daemon started");
    for (;;)
    {
        int status_code = 0;
        bool reachable = false;
        char message[256] = {0};

        (void)peacock_daemon_config_load(PEACOCK_DAEMON_CONFIG_PATH, &config);
        status_code = probe(&runtime, &config);
        reachable = status_code >= 200 && status_code < 500;
        write_state(&config, reachable, status_code);

        if (!previous_known || reachable != previous_reachable)
        {
            snprintf(message, sizeof(message), "PeacockPS5: %s (%s:%u, HTTP %d)",
                     reachable ? "Peacock reachable" : "Peacock unavailable", config.host,
                     (unsigned int)config.port, status_code);
            notify(message);
            previous_known = true;
            previous_reachable = reachable;
        }
        sleep(config.interval_seconds);
    }

    http_runtime_finish(&runtime);
    return 0;
}

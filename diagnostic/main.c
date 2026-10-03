/* PeacockPS5 - Read-only game diagnostic.
 * Copyright (C) 2026 Askabis
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <ps5/kernel.h>
#include <ps5/mdbg.h>

/* App-info ABI and process-record PID offset follow SDK test_privileges.
 * Fail closed on malformed records or an ambiguous target.
 */
typedef struct
{
    uint32_t app_id;
    uint64_t unknown1;
    char title_id[14];
    char unknown2[0x3c];
} app_info_t;
int sceKernelGetAppInfo(pid_t pid, app_info_t *info);

static int is_hitman(pid_t pid)
{
    app_info_t info = {0};
    return sceKernelGetAppInfo(pid, &info) == 0 &&
           memcmp(info.title_id, "PPSA01769", sizeof("PPSA01769")) == 0;
}

static int read_error_state(FILE *log, pid_t pid, intptr_t base)
{
    const unsigned char refs[2][7] = {
        {0x48, 0x8d, 0x3d, 0x5f, 0x36, 0x01, 0x03},
        {0x48, 0x8d, 0x3d, 0x56, 0x35, 0x01, 0x03},
    };
    const intptr_t sites[2] = {0xcda67a, 0xcda783};
    for (size_t i = 0; i < 2; ++i)
    {
        unsigned char actual[7] = {0};
        if (mdbg_copyout(pid, base + sites[i], actual, sizeof(actual)) != 0 ||
            memcmp(actual, refs[i], sizeof(actual)) != 0)
        {
            fprintf(log, "STATE_STOP: singleton reference mismatch\n");
            return -1;
        }
    }
    /* Both validated RIP-relative references resolve to this object.
     * Read only selected state fields, never the authcode/account buffers.
     */
    if (base > INTPTR_MAX - 0x3cee580)
        return -1;
    const intptr_t object = base + 0x3cedce0;
    uint32_t state = 0, text_meta = 0;
    uint64_t error = 0, text_ptr = 0;
    unsigned char flags[3] = {0};
    if (!is_hitman(pid) || kernel_dynlib_mapbase_addr(pid, 0) != base ||
        mdbg_copyout(pid, object + 0x580, &state, sizeof(state)) != 0 ||
        mdbg_copyout(pid, object + 0x5c8, &error, sizeof(error)) != 0 ||
        mdbg_copyout(pid, object + 0x5d0, &text_meta, sizeof(text_meta)) != 0 ||
        mdbg_copyout(pid, object + 0x5d8, &text_ptr, sizeof(text_ptr)) != 0 ||
        mdbg_copyout(pid, object + 0x895, flags, sizeof(flags)) != 0)
    {
        fprintf(log, "STATE_STOP: read failed\n");
        return -1;
    }
    fprintf(log,
            "STATE_V1 state_580=%" PRIu32 " error_5c8=%" PRIx64 " text_meta=%" PRIx32
            " flags_895_897=%u,%u,%u\n",
            state, error, text_meta, (unsigned)flags[0], (unsigned)flags[1], (unsigned)flags[2]);
    const struct
    {
        uintptr_t address;
        const char *label;
    } messages[] = {
        {0x2113fdc, "Authenticating RequestAuthCode"},
        {0x2170e4b, "Authenticating emptyCode"},
        {0x212ff7f, "Authenticate sceNpGetOnlineId"},
        {0x21587e4, "Authenticate sceNpGetAccountId"},
        {0x2170e77, "PollNpCheckRequest sceNpGetOnlineId"},
        {0x2184a6e, "PollPSNSigninDialog sceNpGetOnlineId"},
    };
    const char *label = "UNKNOWN_OR_NONSTATIC (not dereferenced)";
    for (size_t i = 0; i < sizeof(messages) / sizeof(messages[0]); ++i)
    {
        if (text_ptr == (uintptr_t)base + messages[i].address)
            label = messages[i].label;
    }
    fprintf(log, "STATE_V1 known_error=%s\n", label);
    fprintf(log, "STATE_V1 is a non-atomic observation, not a proven last failure\n");
    return 0;
}

static pid_t find_game(FILE *log)
{
    int mib[4] = {1, 14, 8, 0};
    size_t size = 0;
    if (sysctl(mib, 4, NULL, &size, NULL, 0) != 0 || size == 0 || size > 16777216)
    {
        fprintf(log, "PROCESS_LIST_SIZE_FAILED errno=%d\n", errno);
        return -1;
    }
    uint8_t *buffer = malloc(size);
    if (buffer == NULL)
        return -1;
    size_t capacity = size;
    if (sysctl(mib, 4, buffer, &size, NULL, 0) != 0 || size > capacity)
    {
        fprintf(log, "PROCESS_LIST_FAILED errno=%d\n", errno);
        free(buffer);
        return -1;
    }
    pid_t found = -1;
    for (size_t offset = 0; offset < size;)
    {
        int record_size = 0;
        pid_t pid = -1;
        if (size - offset < 76)
            goto malformed;
        memcpy(&record_size, buffer + offset, sizeof(record_size));
        if (record_size < 76 || (size_t)record_size > size - offset)
            goto malformed;
        memcpy(&pid, buffer + offset + 72, sizeof(pid));
        if (pid > 0 && pid != getpid() && is_hitman(pid))
        {
            if (found != -1 && found != pid)
            {
                fprintf(log, "AMBIGUOUS_TARGET\n");
                free(buffer);
                return -1;
            }
            found = pid;
        }
        offset += (size_t)record_size;
    }
    free(buffer);
    return found;
malformed:
    fprintf(log, "UNSUPPORTED_PROCESS_RECORD\n");
    free(buffer);
    return -1;
}

int main(void)
{
    if (mkdir("/data/peacockps5", 0777) != 0 && errno != EEXIST)
        return 1;
    FILE *log = fopen("/data/peacockps5/inspect.log", "w");
    if (log == NULL)
        return 1;
    setvbuf(log, NULL, _IONBF, 0);
    fprintf(log, "PeacockPS5 READ_ONLY diagnostic; no game writes or network requests\n");
    pid_t pid = find_game(log);
    if (pid <= 0)
    {
        fprintf(log, "STOP: HITMAN PPSA01769 not uniquely found\n");
        fclose(log);
        return 1;
    }
    intptr_t base = kernel_dynlib_mapbase_addr(pid, 0);
    intptr_t entry = kernel_dynlib_entry_addr(pid, 0);
    fprintf(log, "pid=%d base=%" PRIxPTR " entry=%" PRIxPTR "\n", (int)pid, (uintptr_t)base,
            (uintptr_t)entry);
    if (base <= 0 || base > INTPTR_MAX - 0x2200000 || entry != base + 0x70)
    {
        fprintf(log, "STOP: unexpected main-module layout\n");
        fclose(log);
        return 1;
    }
    /* Image-relative addresses from the user's local binary. These are
     * markers only, not proof of a complete build match or patch locations.
     */
    const struct
    {
        intptr_t offset;
        const char *text;
    } markers[] = {
        {0x2175d38, "config.hitman.io"},
        {0x212ae9d, "https://auth.hitman.io"},
        {0x212ae1c, "grant_type=external_psn"},
    };
    for (size_t i = 0; i < sizeof(markers) / sizeof(markers[0]); ++i)
    {
        char actual[64] = {0};
        size_t length = strlen(markers[i].text);
        if (!is_hitman(pid) || kernel_dynlib_mapbase_addr(pid, 0) != base ||
            mdbg_copyout(pid, base + markers[i].offset, actual, length) != 0)
        {
            fprintf(log, "STOP: marker %zu read failed errno=%d\n", i, errno);
            fclose(log);
            return 1;
        }
        if (memcmp(actual, markers[i].text, length) != 0)
        {
            fprintf(log, "STOP: marker %zu mismatch\n", i);
            fclose(log);
            return 1;
        }
        fprintf(log, "marker %zu MATCH\n", i);
    }
    fprintf(log, "READ_PROBE_OK; not authentication, not full build verification\n");
    const struct
    {
        intptr_t offset;
        size_t size;
    } regions[] = {
        {0xcd6180, 0x2b0}, {0xcd6430, 0x132}, {0xcd6562, 0x112},
        {0xcf9609, 0x95},  {0xd1be8a, 0x8aa},
    };
    FILE *snapshot = fopen("/data/peacockps5/auth-code-v1.bin", "wb");
    if (snapshot == NULL)
    {
        fprintf(log, "STOP: cannot create code snapshot\n");
        fclose(log);
        return 1;
    }
    /* Fixed executable regions only. No object data, credentials, or tokens.
     * This does not pause the target; it is not an atomic process snapshot.
     */
    for (size_t i = 0; i < sizeof(regions) / sizeof(regions[0]); ++i)
    {
        unsigned char code[4096] = {0};
        if (regions[i].size > sizeof(code) || !is_hitman(pid) ||
            kernel_dynlib_mapbase_addr(pid, 0) != base ||
            mdbg_copyout(pid, base + regions[i].offset, code, regions[i].size) != 0 ||
            fwrite(code, 1, regions[i].size, snapshot) != regions[i].size)
        {
            fprintf(log, "STOP: code region %zu read/write failed\n", i);
            fclose(snapshot);
            fclose(log);
            return 1;
        }
        fprintf(log, "code region %zu collected (%zu bytes)\n", i, regions[i].size);
    }
    if (fclose(snapshot) != 0)
    {
        fprintf(log, "STOP: snapshot flush failed\n");
        fclose(log);
        return 1;
    }
    fprintf(log, "CODE_SNAPSHOT_V1_OK; compare locally before drawing conclusions\n");
    for (unsigned sample = 0; sample < 2; ++sample)
    {
        fprintf(log, "STATE_SAMPLE %u\n", sample);
        if (read_error_state(log, pid, base) != 0)
        {
            fclose(log);
            return 1;
        }
        if (sample == 0)
            sleep(1);
    }
    fclose(log);
    return 0;
}

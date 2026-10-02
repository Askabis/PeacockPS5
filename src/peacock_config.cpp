/*
 * PeacockPS5 - Runtime configuration.
 * Copyright (C) 2026 Askabis
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "peacock_config.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <span>

namespace
{
constexpr const char *download_config = "/download0/peacock.conf";
constexpr const char *asset_config = "/app0/assets/peacock.conf";

bool read_text_file(const char *path, std::span<char> output) noexcept
{
    if (output.empty())
        return false;
    output[0] = '\0';
    FILE *file = std::fopen(path, "rb");
    if (file == nullptr)
        return false;
    const auto count = std::fread(output.data(), 1, output.size() - 1, file);
    output[count] = '\0';
    std::fclose(file);
    return count > 0;
}

char *trim(char *text) noexcept
{
    while (*text != '\0' && std::isspace(static_cast<unsigned char>(*text)) != 0)
        ++text;
    char *end = text + std::strlen(text);
    while (end != text && std::isspace(static_cast<unsigned char>(*(end - 1))) != 0)
        *--end = '\0';
    return text;
}

void assign_bounded(char *destination, std::size_t capacity, const char *value) noexcept
{
    if (capacity == 0)
        return;
    std::snprintf(destination, capacity, "%s", value);
}

void apply_entry(peacock::Config &config, char *line) noexcept
{
    char *entry = trim(line);
    if (*entry == '\0' || *entry == '#')
        return;
    char *equals = std::strchr(entry, '=');
    if (equals == nullptr)
        return;
    *equals = '\0';
    const char *key = trim(entry);
    const char *value = trim(equals + 1);
    if (std::strcmp(key, "host") == 0)
        assign_bounded(config.host, sizeof(config.host), value);
    else if (std::strcmp(key, "path") == 0)
        assign_bounded(config.path, sizeof(config.path), value);
    else if (std::strcmp(key, "port") == 0)
    {
        const int port = std::atoi(value);
        if (port > 0 && port <= 65535)
            config.port = static_cast<std::uint16_t>(port);
    }
    else if (std::strcmp(key, "timeout_ms") == 0)
    {
        const int timeout = std::atoi(value);
        if (timeout >= 250 && timeout <= 30000)
            config.timeout_ms = timeout;
    }
}

void parse_config_text(peacock::Config &config, char *text) noexcept
{
    for (char *line = std::strtok(text, "\n"); line != nullptr; line = std::strtok(nullptr, "\n"))
        apply_entry(config, line);
}
} // namespace

namespace peacock
{
Config load_config() noexcept
{
    Config config{};
    std::array<char, 1024> buffer{};
    if (read_text_file(download_config, std::span{buffer}) ||
        read_text_file(asset_config, std::span{buffer}))
        parse_config_text(config, buffer.data());
    return config;
}

bool parse_ipv4(const char *text, std::uint32_t &network_order_address) noexcept
{
    unsigned parts[4]{};
    const int matched =
        std::sscanf(text, "%u.%u.%u.%u", &parts[0], &parts[1], &parts[2], &parts[3]);
    if (matched != 4)
        return false;
    for (const auto part : parts)
    {
        if (part > 255)
            return false;
    }
    network_order_address = (parts[0] << 24U) | (parts[1] << 16U) | (parts[2] << 8U) | parts[3];
    return true;
}
} // namespace peacock

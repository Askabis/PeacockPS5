#pragma once
/*
 * PeacockPS5 - Runtime configuration.
 * Copyright (C) 2026 Askabis
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <cstdint>

namespace peacock
{
struct Config
{
    char host[64] = "192.168.1.10";
    char path[128] = "/authentication/api/configuration/Init";
    std::uint16_t port = 80;
    int timeout_ms = 3000;
};

Config load_config() noexcept;
bool parse_ipv4(const char *text, std::uint32_t &network_order_address) noexcept;
} // namespace peacock

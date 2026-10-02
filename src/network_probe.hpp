#pragma once
/*
 * PeacockPS5 - Network probe interface.
 * Copyright (C) 2026 Askabis
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "peacock_config.hpp"

#include <cstddef>

namespace peacock
{
struct ProbeResult
{
    bool ok = false;
    int status_code = 0;
    int transport_code = 0;
    char summary[128] = "not run";
    char first_line[128] = "";
};

ProbeResult run_probe(const Config &config) noexcept;
} // namespace peacock

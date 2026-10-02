/*
 * PeacockPS5 - Local Peacock connectivity probe.
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Builds a sandboxed native PS5 app that checks whether a user-owned console
 * can reach a local Peacock server on the LAN.
 */

#include "demo_renderer.hpp"
#include "network_probe.hpp"
#include "peacock_config.hpp"

#include <array>
#include <cstdio>
#include <span>

namespace
{
std::array<char, 48> banner{};
peacock::Config config{};
peacock::ProbeResult probe{};
std::array<char, 96> target_line{};
std::array<char, 96> status_line{};
std::array<char, 96> detail_line{};

void draw_scene(ps5::demo::Canvas &canvas) noexcept
{
    using ps5::demo::Color;

    canvas.clear(Color::background);
    canvas.rectangle(120, 410, 1680, 470, Color::panel);
    canvas.rectangle(120, 375, 1680, 8, Color::white);

    canvas.text(120, 90, "PEACOCK PS5", 14, Color::white);
    canvas.text(120, 245, "LAN CONNECTIVITY PROBE", 6, Color::white);
    canvas.text(120, 315, banner.data(), 5, Color::cyan);

    canvas.text(170, 485, target_line.data(), 4, Color::white);
    canvas.text(170, 575, status_line.data(), 5, probe.ok ? Color::cyan : Color::magenta);
    canvas.text(170, 680, detail_line.data(), 4, Color::white);
    canvas.text(170, 790, "LOG: /download0/peacock_probe.log", 4, Color::yellow);
}
} // namespace

int main()
{
    ps5::demo::read_asset_text("/app0/assets/banner.txt", std::span{banner}, "APP0 ASSET FAILED");
    config = peacock::load_config();
    probe = peacock::run_probe(config);
    std::snprintf(target_line.data(), target_line.size(), "TARGET: http://%s:%u%s", config.host,
                  static_cast<unsigned>(config.port), config.path);
    std::snprintf(status_line.data(), status_line.size(), "%s", probe.summary);
    std::snprintf(detail_line.data(), detail_line.size(), "%s",
                  probe.first_line[0] == '\0' ? "NO RESPONSE LINE" : probe.first_line);
    ps5::demo::run(draw_scene, banner.data());
}

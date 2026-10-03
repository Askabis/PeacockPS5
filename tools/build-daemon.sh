#!/usr/bin/env bash
# PeacockPS5 - Build the background connectivity daemon.
# Copyright (C) 2026 Askabis
# SPDX-License-Identifier: GPL-3.0-or-later

set -euo pipefail

root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
bash "$root/tools/setup-native-dependencies.sh" >/dev/null
sdk="$root/.deps/native/ps5-payload-sdk"

make -C "$root/daemon" --no-print-directory clean \
    PS5_PAYLOAD_SDK="$sdk"
make -C "$root/daemon" --no-print-directory \
    PS5_PAYLOAD_SDK="$sdk" \
    PS5_CLANG="${PS5_CLANG:-$(command -v clang-18 || command -v clang)}"
mkdir -p "$root/dist"
cp "$root/daemon/peacockps5-daemon.elf" "$root/dist/peacockps5-daemon.elf"
cp "$root/assets/peacock-daemon.conf" "$root/dist/peacock-daemon.conf"
printf 'Daemon: %s\n' "$root/dist/peacockps5-daemon.elf"

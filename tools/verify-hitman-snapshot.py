#!/usr/bin/env python3
# PeacockPS5 - Verify bounded read-only executable snapshots.
# Copyright (C) 2026 Askabis
# SPDX-License-Identifier: GPL-3.0-or-later
"""Check five code regions; not full-build verification or patch authorization."""

import argparse
import hashlib
import json
from pathlib import Path


REGIONS = (
    ("authentication", 0xcd6180, 0x2b0, "da679c9cfa4706a6a1d89ed2d752e263c37dfea60255fc1464fa6e69c8c90a28"),
    ("platform_check", 0xcd6430, 0x132, "cf6c4c8c2fe0471ba581f3827d7a290311129f0a65e4c180dcce6cf2cd94b3c1"),
    ("authcode_request", 0xcd6562, 0x112, "d8f8b834787b1743641a96c61e983a16f58ee60bdcd4b2ee718d3eadfe4b5898"),
    ("configuration", 0xcf9609, 0x95, "ac820917acd2530656d477b30a2efba2e948d291db2164bb67476d6c758b9379"),
    ("oauth_body", 0xd1be8a, 0x8aa, "743d45f63875d67290ff8d09cffbf08f4739b1f9f1118648a4410ec9b527c54b"),
)


def verify(data, regions=REGIONS):
    expected_size = sum(size for _, _, size, _ in regions)
    if len(data) != expected_size:
        raise ValueError(f"Snapshot size {len(data)}; expected {expected_size}")
    offset = 0
    results = []
    for name, address, size, expected in regions:
        digest = hashlib.sha256(data[offset:offset + size]).hexdigest()
        results.append({"name": name, "image_address": hex(address),
                        "size": size, "sha256": digest, "matches": digest == expected})
        offset += size
    return {"format": "auth-code-v1", "all_match": all(r["matches"] for r in results),
            "limitations": "Five static code regions only; not an execution trace or full-build identity",
            "regions": results}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("snapshot", type=Path)
    args = parser.parse_args()
    try:
        result = verify(args.snapshot.read_bytes())
    except (OSError, ValueError) as error:
        parser.exit(2, f"Verification failed: {error}\n")
    print(json.dumps(result, indent=2))
    return 0 if result["all_match"] else 1


if __name__ == "__main__":
    raise SystemExit(main())

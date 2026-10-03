# PeacockPS5 - Snapshot verifier tests.
# Copyright (C) 2026 Askabis
# SPDX-License-Identifier: GPL-3.0-or-later
import hashlib
import importlib.util
from pathlib import Path
import unittest

SPEC = importlib.util.spec_from_file_location(
    "snapshot", Path(__file__).resolve().parents[1] / "tools/verify-hitman-snapshot.py")
MODULE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODULE)


class SnapshotTests(unittest.TestCase):
    def setUp(self):
        self.regions = (("first", 1, 3, hashlib.sha256(b"abc").hexdigest()),
                        ("second", 4, 2, hashlib.sha256(b"de").hexdigest()))

    def test_exact(self):
        self.assertTrue(MODULE.verify(b"abcde", self.regions)["all_match"])

    def test_changed(self):
        result = MODULE.verify(b"abcdf", self.regions)
        self.assertFalse(result["all_match"])
        self.assertTrue(result["regions"][0]["matches"])
        self.assertFalse(result["regions"][1]["matches"])

    def test_wrong_order(self):
        self.assertFalse(MODULE.verify(b"deabc", self.regions)["all_match"])

    def test_wrong_length(self):
        for data in (b"", b"abcd", b"abcdef"):
            with self.subTest(data=data), self.assertRaises(ValueError):
                MODULE.verify(data, self.regions)


if __name__ == "__main__":
    unittest.main()

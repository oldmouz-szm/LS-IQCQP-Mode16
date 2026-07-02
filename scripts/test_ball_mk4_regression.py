#!/usr/bin/env python3
import re
import subprocess
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOLVER_DIR = ROOT / "src" / "LS-IQCQP"
SOLVER = SOLVER_DIR / "build" / "LS-IQCQP"


class BallMk4RegressionTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        subprocess.run(["make"], cwd=SOLVER_DIR, check=True)

    def test_infeasible_instances_are_not_reported_as_block_repairs(self):
        for instance in ("ball_mk4_10.lp", "ball_mk4_15.lp"):
            with self.subTest(instance=instance):
                result = subprocess.run(
                    [
                        str(SOLVER),
                        "1",
                        "1",
                        str(ROOT / "data" / "all_lp" / instance),
                        "1",
                        "12",
                        "3",
                        "8",
                    ],
                    check=True,
                    capture_output=True,
                    text=True,
                    timeout=10,
                )
                output = result.stdout + result.stderr

                self.assertIn("solution_status = NO_FEASIBLE_SOLUTION", output)
                self.assertNotIn("best obj min=", output)
                candidates = re.search(r"candidates_total = (\d+)", output)
                self.assertIsNotNone(candidates)
                self.assertGreater(int(candidates.group(1)), 0)
                self.assertIn("repair_success_total = 0", output)


if __name__ == "__main__":
    unittest.main()

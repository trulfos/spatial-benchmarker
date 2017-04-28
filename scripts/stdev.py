#!/usr/bin/python3
import math


class StddevSamp:
    """
    Standard deviation aggregation function for sqlite.
    """

    def __init__(self):
        self.k = 0
        self.S = 0.0
        self.M = 0.0

    def step(self, value):
        try:
            v = float(value)
        except (ValueError, TypeError):
            return

        previous_M = self.M

        self.k += 1
        self.M += (v - self.M) / self.k
        self.S += (v - previous_M) * (v - self.M)

    def finalize(self):
        if self.k < 3:
            return None

        return math.sqrt(self.S / (self.k - 1))

#!/usr/bin/python
# -*- coding: utf-8 -*-

import unittest

import numpy as np

from pypitch.pypitch import Analyzer
from pypitch.pypitch import Tone


class AnalyzerTest(unittest.TestCase):

    def setUp(self):
        self.rate = 44100
        self.instr = np.array([[1, 1],
                               [1, 2]], dtype=np.float32)
        self.analyzer = Analyzer(self.rate)

    def test_init(self):
        Analyzer(self.rate)

    def test_peak(self):
        self.analyzer.input(self.instr)
        self.analyzer.process()
        cpeak = self.analyzer.getPeak()
        peak = 6.020599913279624

        self.assertEqual(cpeak, peak)

    def test_getFormants(self):
        self.analyzer.input(self.instr)
        self.analyzer.process()
        cformants = self.analyzer.getFormants()
        formants = [None] * 3

        self.assertEqual(cformants, formants)

    #def test_findTone(self):
    #    self.analyzer.input(self.instr)
    #    self.analyzer.process()
    #    ctone = self.analyzer.findTone()

    #    self.assertIsInstance(ctone, Tone)

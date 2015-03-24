__author__ = 'mzapp'


import math
import itertools

def sin(freq=1.0, length=-1.0, sr=44100, phase=0.0):
    count = 0
    while count != length:
        yield math.sin(phase * 2 * math.pi)
        phase += float(freq) / float(sr)
        count += 1


class Sine(object):
    '''A sine wave oscillator.'''

    sr = 44100
    ksmps = 10

    def __init__(self, amp=1.0, freq=440, phase=0.0):
        self.amp = amp
        self.freq = float(freq)
        self.phase = phase

    def __iter__(self):
        self.index = 0
        return self

    def next(self):

        self.index += 1
        v = math.sin(self.phase * 2 * math.pi)
        self.phase += self.freq / self.sr
        return v * self.amp

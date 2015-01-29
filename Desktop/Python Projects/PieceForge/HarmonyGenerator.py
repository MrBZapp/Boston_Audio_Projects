__author__ = 'mzapp'

import itertools
import collections as c

RootChords = {'C': c.deque([0, 4, 7]),
              'D': c.deque([2, 6, 9]),
              'E': c.deque([4, 8, 11]),
              'F': c.deque([5, 9, 0]),
              'G': c.deque([7, 11, 2]),
              'A': c.deque([9, 1, 4]),
              'B': c.deque([11, 3, 6])}

IntervalNames =
dict.fromkeys(['P1', 'd2'], 0)
dict.fromkeys(['m2', 'A1'], 1)
dict.fromkeys(['M2', 'd3'], 2)
dict.fromkeys(['m3', 'A2'], 3)
dict.fromkeys(['M3', 'd4'], 4)
dict.fromkeys(['P4', 'A3'], 5)
dict.fromkeys(['A4', 'd5'], 6)
dict.fromkeys(['P5', 'd6'], 7)
dict.fromkeys(['m6', 'A5'], 8)
dict.fromkeys(['M6', 'd7'], 9)
dict.fromkeys(['m7', 'A6'], 10)
dict.fromkeys(['M7', 'd8'], 11)

def FindRoot(chord=c.deque([])):
    deltas = []
    for a, b in itertools.combinations(chord, 2):
        deltas.append(abs(a - b))
    print chord
    print sorted(deltas)
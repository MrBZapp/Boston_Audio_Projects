__author__ = 'mzapp'

from fractions import gcd
from collections import namedtuple
import string
import random

FormSectionT = namedtuple("FormSection",
                         "duration,"
                         "section,"
                         "subsection,"
                         "time_signature")

def FormSection(duration, section, subsection=0, time_signature=[]):
    return FormSectionT(duration, section, subsection, time_signature)

class FormGenerator(object):

    def __init__(self,
                 duration=273,
                 segments=3,
                 subsegments=2,
                 minsegsecs=5,
                 maxsegsecs=40,
                 maxdivisor=4
                 ):

        self._duration = duration
        self._segments = segments
        self._sub = subsegments
        self._minsegsecs = minsegsecs
        self._maxsegsecs = maxsegsecs
        self._maxdivisor = maxdivisor

        # the completed form
        self.form = []

    def generate_segments(self, n):
        # create an empty list
        segments = []

        # generate n segments
        for i in range(n):
            name = string.ascii_lowercase[i]
            if self._minsegsecs != self._maxsegsecs:
                duration = random.randrange(self._minsegsecs, self._maxsegsecs, 1)
            else:
                duration = self._minsegsecs

            segment = FormSection(duration=duration,
                                  section=name)

            segments.append(segment)

        return segments

    def calc_segment_relationship_level(self, segments=[]):
        # calculate how closely related the segments can be
        return reduce(gcd, [seg.duration for seg in segments])

    def fill_form_from_segments(self, segments=[], duration=273):
        occupied_size = 0
        while occupied_size < duration:
            segNum = random.randrange(0, len(segments), 1)
            segment = segments[segNum]
            self.form.append(segment.section)
            occupied_size += segment.duration
        return self.form
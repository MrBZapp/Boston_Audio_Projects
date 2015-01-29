__author__ = 'mzapp'

import math
import numpy
import InterpolatingList as IL

class DelayLine(IL.InterpolatingList):
    def __init__(self, delaySizeIn=1, interptype="nearest"):
        super(DelayLine, self).__init__(interptype)
        self._readHead = delaySizeIn
        self._writeHead = 0
        delaySizeIn = self._MinWindowSize + delaySizeIn
        self.delaySize = int(math.ceil(delaySizeIn))

    def read(self, advance=0):
        if len(self) >= self.delaySize:
            retVal = self.pop(self._readHead)
        else:
            retVal = 0

        self._readHead = (self._readHead + advance) % self.delaySize
        return retVal

    def write(self, value):
        self.insert(self._writeHead, value)
        while self.__len__() > self.delaySize:
            self.pop(self.delaySize)

    def reset(self, delaySizeIn=1, interptype="nearest"):
        self.__init__(delaySizeIn, interptype)
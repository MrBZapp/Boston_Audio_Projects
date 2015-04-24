__author__ = 'mzapp'

import math

class InterpolatingList(list):
    def __init__(self, interptype="nearest"):
        list.__init__(self)
        self.InterpTypes = {"nearest": self._interp_Nearest,
                            "floor": self._interp_Floor,
                            "ceiling": self._interp_Ceil,
                            "linear": self._interp_Linear}
        self._ActiveInterp = interptype
        self._MinWindowSize = 1
        self.setInterpType(interptype)

    def __getitem__(self, index):
        if isinstance(index, float):
            return self._ActiveInterp(index)
        else:
            return super(InterpolatingList, self).__getitem__(index)

    def setInterpType(self, type, params="none"):
        self._ActiveInterp = self.InterpTypes[type]

    """
    INTERPOLATION TYPES
    """
    def _interp_Nearest(self, index):
        index = int(round(index))
        return super(InterpolatingList, self).__getitem__(index)

    def _interp_Floor(self, index):
        index = int(math.floor(index))
        return super(InterpolatingList, self).__getitem__(index)

    def _interp_Ceil(self, index):
        index = int(math.ceil(index))
        return super(InterpolatingList, self).__getitem__(index)

    def _interp_Linear(self, index):
        a = self._interp_Floor(index)
        b = self._interp_Ceil(index)
        ratB = index - math.floor(index)
        ratA = 1 - ratB
        return (a * ratA) + (b * ratB)
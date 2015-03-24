__author__ = 'mzapp'

import DelayLine

class StreamProcess(object):
    __name__ = "GenericStreamProcess"
    # corrects the potential use of different names to describe processing steps
    _paramTranslation = {}

    def __init__(self):
        self.params = {}  # the parameters and their defaults for each process
        self.minBufSize = 0  # the buffer size required to process audio

    def getParams(self):
        return self.params.keys()

    def getDefaults(self):
        return self.params.values()

    def process(self, stream=[]):
        print "Processing " + self.__name__ + " with the following parameters:"
        for key in self.params:
            print "\t " + key + ": " + str(self.params[key])

    def _preProcessParams(self, params):
        # make sure all param keys are lower case
        for func in params:
            value = params.pop(func)
            key = str(func).lower()

            try:
                key = self._paramTranslation[key]
                params[key] = value
            except KeyError:
                params[key] = value
        return params

    def getBuffer(self):
        if self.Buffer:
            return self.Buffer


    def setParams(self, params):
        # ensure the parameters are all appropriately named
        params = self._preProcessParams(params)

        for key in params:
            if key in self.params:
                self.params[key] = params[key]
            else:
                print "Parameter '" + key + "' not found in " + self.__name__


class Echo(StreamProcess):
    __name__ = "echo"

    # class initialization
    def __init__(self, interptype="linear"):
        StreamProcess.__init__(self)

        # define the defaults
        self.params = {"delay": 1000,
                       "feedback": 0.5,
                       "wet-dry": 0.5,
                       "interpolation": "linear"}

        # define param names
        self._paramTranslation.update(dict.fromkeys(["samples", "delay"], "delay"))

        self.buffer = []

    def process(self, stream=[]):
        super(Echo, self).process()

        delSamps = self.params["delay"]
        wetlevel = 1 - self.params["wet-dry"]
        drylevel = self.params["wet-dry"]
        feedback = self.params["feedback"]

        # set up the delay line
     #   self.buffer.reset(self.params["delay"],
      #                    self.params["interpolation"])

        # write the buffer into memory
        for sampDry in stream:
            if len(self.buffer) >= delSamps:
                sampWet = self.buffer.pop(0)
            else:
                sampWet = 0

            self.buffer.append(sampDry + (sampWet * feedback))

            outsamp = (sampDry * drylevel) + (sampWet * wetlevel)
            index = stream.index(sampDry)
            stream.__setitem__(index, outsamp)

        return stream
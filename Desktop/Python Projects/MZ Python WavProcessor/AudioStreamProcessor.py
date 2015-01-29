__author__ = 'mzapp'

import StreamProcesses

class AudioStreamProcessor:
    sampRate = 0
    procDict = {"echo": StreamProcesses.Echo}
    def __init__(self, sampleRate):
        if isinstance(sampleRate, int):
            self.sampRate = sampleRate
        else:
            print "sample rate must be an integer"
            raise TypeError

    def process(self, stream, process):
        if isinstance(stream, list) and isinstance(process, dict):
            type = str(process["type"]).lower()
            params = process["params"]

            try:
                processorDelegate = self.procDict[type]()
                processorDelegate.setParams(params)
                stream = processorDelegate.process(stream)
                del processorDelegate
                return stream

            except KeyError:
                print "No available process named " + process["type"]
                raise KeyError
        else:
            print "*.process() must be provided with a 'list' of data, and a 'dict' of processing parameters."
            raise TypeError
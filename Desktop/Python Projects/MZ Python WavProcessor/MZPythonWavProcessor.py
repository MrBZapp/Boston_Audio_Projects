__author__ = 'mzapp'

import sys
import json
import wave
import struct
import AudioStreamProcessor as ASP
import DelayLine
import WaveGenerator as WG
import math

processes = []

# ensure processing is possible
with open(sys.argv[1], 'rb') as jsonFile:
    JSONfile = json.load(jsonFile)
    paramsDecoder = json.JSONDecoder()

    procList = JSONfile['process']

    # insert all the processes into the process list.
    for key in procList.iteritems():
        processes.insert(int(key[0]), key[1])

    print "Process file parsed successfully"

"""
Read the wave file
"""
# ensure the wave file is openable
try:
    ifile = wave.open(sys.argv[2])
except IOError:
    print "Wave file was not able to be opened"

try:
    ofile = wave.open(sys.argv[3], 'w')
except IOError:
    print "Output file was not able to be created"

# set the params of the output file
ofile.setparams(ifile.getparams())
params = ofile.getparams()
frameSize = params[1] * params[0]

# read everything
dataRaw = ifile.readframes(ifile.getnframes())
audioLeft = []
audioRight = []

print "File was read into memory successfully"

"""ifile.getnframes() * frameSize"""
# break raw data out into L and R streams
for i in range(0, ifile.getnframes() * frameSize, frameSize):
    frame = dataRaw[i:i + frameSize]
    frame = struct.unpack("<hh", frame)
    audioLeft.append(frame[0])
    audioRight.append(frame[1])

print "Wave was read successfully"

"""
Process the file
"""
audioLeft = [x * y for x,y in zip(audioLeft, WG.sin(1))]
audioRight = [x * y for x,y in zip(audioRight, WG.sin(1, phase=2*math.pi))]

'''
processor = ASP.AudioStreamProcessor(ifile.getframerate())
audioLeft = processor.process(audioLeft, processes[1])
audioRight = processor.process(audioRight, processes[1])
'''

"""
Write the File back to disk
"""
# clear the raw data
dataRaw = ""

for i in range(0, len(audioLeft)):
    dataRaw += struct.pack("<hh", audioLeft[i], audioRight[i])

# write everything
ofile.writeframes(dataRaw)
print "File was written from memory successfully"

# close the files
ofile.close()
ifile.close()


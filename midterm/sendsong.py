import numpy as np
import serial
import time

waitTime = 0.1


song1 = [196,261,329,261,174,261,220,261,
196,293,246,293,196,329,261,329,
196,261,329,261,174,261,220,261,
196,293,246,293,261,196,261,261]

song2 = [261,261,392,293,293,293,261,293,
329,349,392,293,293,261,246,196,
261,261,392,293,293,293,261,293,
329,349,392,392,392,523,493,392]

song3 = [233,277,349,277,174,220,261,220,
185,233,277,233,138,174,207,174,
233,277,349,277,415,369,349,220,
185,233,277,233,138,174,207,174]
#song3=np.array(song3)


# output formatter
formatter = lambda x: "%d" % x


# send the waveform table to K66F
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)
print("Sending signal ...")
print("sending song")
for data in song1:
  s.write(bytes(formatter(data), 'UTF-8'))
  print(bytes(formatter(data), 'UTF-8'))
  time.sleep(waitTime)
for data in song2:
  s.write(bytes(formatter(data), 'UTF-8'))
  print(bytes(formatter(data), 'UTF-8'))
  time.sleep(waitTime)
for data in song3:
  s.write(bytes(formatter(data), 'UTF-8'))
  print(bytes(formatter(data), 'UTF-8'))
  time.sleep(waitTime)
s.close()
print("Signal sended")
#synctime - synchronize your chronos watch to your computer
#
#
# Copyright (c) 2010 Sean Brewer 
# Heavily modified by Huan Truong (htruong@tnhh.net) for Chronos-OTP
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
#
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# You can contact me at seabre986@gmail.com or on reddit (seabre) if you want.
#


#****TONS**** of mistakes in the last version. Sorry about that everybody.

# import serial
import time
import array
import serial
import base64
import sys

from time import sleep
from ctypes import *

def startAccessPoint():
  return b'\xff\x07\x03'
  
def stopAccessPoint():
  return b'\xff\x09\x03'

def truong_epoch(timestamp):
  # Truong_epoch starts at 1262304000, which happens to be 1/1/2010
  return timestamp - 1262304000

"""
kinda messy

Here's how this works.
A packet for the watch sync looks like this:
FF 31 16 03 94 34 01 07 DA 01 17 06 1E 00 00 00 00 00 00 00 00 00
00 00 00 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18
You have the first four bytes which we're just going to ignore.
Byte 0 -> 3: 	the truong_epoch timestamp you want to sync (small-endian).
Byte 4:			the UTCOffset want to sync to.
Byte 5 -> 14:	the key you want to sync to.
Byte 15 -> 18:	pad 0.
"""

def syncKey(tz, secretkey):
    cmd = b'\xff\x31\x16'
    
    cmd += b'\x03'

    hexTime = truong_epoch(int(time.time()))
    cmd += c_uint32(hexTime)
    
    cmd += c_int8(tz)
    
    cmd += base64.b32decode(secretkey, True)
    
    cmd += b'\x00\x00\x00\x00'
    
    return cmd



if len(sys.argv) < 4:
    sys.stderr.write('Usage: sync.py device tiemzone privatekey\n')
    sys.stderr.write('  Example: sync.py /dev/ttyACM0 -6 2345678923456789\n\n')
    sys.exit(1)
else:
	print('Syncing with ', sys.argv[1], ' tz: ', int(sys.argv[2]), ' key: ', sys.argv[3]);
	
t = syncKey(int(sys.argv[2]), sys.argv[3].encode("utf-8"))
count = -4
print(" ---   DATA TO BE SYNC'ED ---")
for c in t: 
  print(count, hex(c))
  count+=1;
print(" --- / DATA TO BE SYNC'ED ---")


#Open COM port 6 (check your system info to see which port
#yours is actually on.)
#argments are 5 (COM6), 115200 (bit rate), and timeout is set so
#the serial read function won't loop forever.
ser = serial.Serial(sys.argv[1],115200,timeout=1)

#Start access point
ser.write(startAccessPoint())

input("Please turn your watch to sync mode, wait until both the sync icon of the watch and the led of the ap flash about 1 time/sec then press enter...")
print("Syncing...")

ser.write(t)

#Need to pause to make sure the watch actually gets the sync packet
print("OK. Your watch should have been sync'd. Pausing for two seconds..")

sleep(2)

#The start access point command needs to come before the stop access point command
#in order for the access point to turn off.
ser.write(startAccessPoint())
ser.write(stopAccessPoint())
    
ser.close()
    

#!/usr/bin/python
import serial, sys, io, os

port0 = sys.argv[1]
file = sys.argv[2]

ser = serial.Serial()
ser_io = io.TextIOWrapper(io.BufferedRWPair(ser, ser, 1),
                             line_buffering = False)

#initialization and open the port
ser.port = port0
ser.baudrate = 115200
ser.bytesize = serial.EIGHTBITS #number of bits per bytes
ser.parity = serial.PARITY_NONE #set parity check: no parity
ser.stopbits = serial.STOPBITS_ONE #number of stop bits
ser.timeout = 240            #non-block read (20s time out)
ser.writeTimeout = 1     #timeout for write

uart_oad_block_size = 64

try:
  ser.open()
except Exception, e:
  print("error opening serial port: %s", str(e))
  exit()

if ser.isOpen():
  try:
    ser.flushInput()  #flush input buffer, discarding all its contents
    ser.flushOutput() #flush output buffer, aborting current output and discard all that is in buffer
  except Exception, e1:
    print("error communicating...: %s", str(e1))
else:
  print("cannot open serial port")


f = open(file, 'rb')
print 'opening file:'
print str(f)

f.seek(0, os.SEEK_END)
size = f.tell()
f.seek(0,0)

print 'file size: ' + str(size)

#ser.write('6')
#ser.flush()

print 'sending header'
# Send Header
byteIdx = 0
#22 Byte image ID Payload (+ removed bytes)
while(byteIdx < (22 + 14)):        

    #Extract Image Identify Payload from header
    #Discard CRC
    if((byteIdx == 8) or (byteIdx == 9) or (byteIdx == 10) or (byteIdx == 11) or 
    #Discard Tech Type
       (byteIdx == 15) or (byteIdx == 16) or
    #Discard Img Valid
       (byteIdx == 20) or (byteIdx == 21) or (byteIdx == 22) or (byteIdx == 23) or
    #Dscard Prg Entry
       (byteIdx == 28) or (byteIdx == 29) or (byteIdx == 30) or (byteIdx == 31)):
      #Read and discard byte
      dmummyByte = f.read(1)
      byteIdx+=1
      continue
    

    byte = f.read(1)
      
    byteIdx+=1
        
    ser.write(byte) #byte.decode('hex'))
    ser.flush()
    print 'Out: ' + str(byte.encode('hex'))
    

f.seek(0,0)

blocksSent = 0;

print 'sending blocks'
sys.stdout.flush()
while(size > 0 ):
    #print 'reading block num'
    #blockNum = ser.read(2)
    #print 'block: ' + str(blockNum.encode('hex'))

    #print 'writing block num'
    #ser.write(blockNum)
    #ser.flush()

    #print 'reading block num'
    blockCntByte1 = ser.read(1)
    blockCntByte2 = ser.read(1)

    #blockNum = blockCntByte1.encode('hex') | (blockCntByte2.encode('hex') << 8)

    print 'writing block num: ' + str(blockCntByte1.encode('hex')) + str(blockCntByte2.encode('hex'))
    #send back block number
    ser.write(blockCntByte2)
    ser.flush()
    ser.write(blockCntByte1)
    ser.flush()
    #ser.write(blockCntByte2)
    #ser.flush()

    blockByteIdx = 0
    if(( size == 0) or (size < uart_oad_block_size)):
        blockSize = size
    else:
        blockSize = uart_oad_block_size

    print 'Reading: ' + str(blockSize) + 'bytes'
    print 'Blocks sent: ' + str(blocksSent)
    blocksSent+=1

    while(blockByteIdx < blockSize):
        blockByte = f.read(1)
        ser.write(blockByte)
        ser.flush()
        blockByteIdx+=1
        #print 'Out: ' + str(blockByte.encode('hex'))
        #sys.stdout.flush()

    size = size - uart_oad_block_size

    remander = uart_oad_block_size - blockSize

#send dummy bytes to complete the block
print 'Writing ' + str(remander) + 'dummy bytes'
idx = 0
dummyByte = 'ff'
while(idx < remander):
    ser.write(dummyByte.decode("hex"))
    ser.flush()
    idx+=1

print 'All blockes sent'

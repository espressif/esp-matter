import serial
import sys
####Serial Port Settings######
####Serial(param1,param2,timeout)
####param1 = COM port. "0" for COM1, "1" for COM2 etc.
####param2 = Baudrate
####param3 = Timeout. The PC reads the UART port <timeout> secs after it does a write into the UART port 
sp=serial.Serial(port="/dev/ttyUSB0",baudrate=115200,timeout=0.01)
#############################

resp="OK"
global buf
def root(out):
    global resp
    global buf
    if resp!="":
        sp.write(out)
	resp=""
        count=0
	buf=""
        while resp !="\n":	
            resp=sp.read(1)
            buf = buf + resp
            count=count+1
            if count == 500000:
                print "NO RESPONSE"
                exit()
	if buf[0:1] == 'E':
            print buf
            print "due to error"
            exit()
	if buf[0:2] == 'OK':
	    print "OK"	
       
def set_cert(filetype,cert):
    cert = cert.replace('\r\n','\n')
    print "Set certificate\n"
    num =len (cert)
    print "Length of certificate:",num
    length_cert=str(num)
    if sys.argv[1] == '2':
     num =len (cert) + 1
     length_cert=str(num)
     out='at+rsi_cert=' + filetype + ','+length_cert+',,'+cert+'\0'+'\r\n'
    else:
     out='at+rsi_cert=' + filetype + ','+length_cert+',,'+cert+'\r\n'
    print out
    root(out)

def read_file(filename):
    f = open(filename, 'r')
    txt = f.read()
    f.close()
    return txt

def main(filetype, filepath):
    cert = read_file(filepath)
    set_cert(filetype, cert) 

if __name__ == "__main__":
    if len(sys.argv) == 3:
        main(sys.argv[1],sys.argv[2])
    else:
        print 'Usage: %s <certificate_type> <certificate_path>' % sys.argv[0]
    

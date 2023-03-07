#server side
# echo client
import time
from socket import *
from ssl import *
import datetime

def timer():
    now = time.localtime(time.time())
    return now[5]

#server is not finnished
finnished =False
pkt_cnt=0
message1 = ''
total_length = 0
#while not finnished


present_pkt = ""
previous_pkt = ""


data_new = 0
data_previous = 0

current_sec1 = 0
current_sec2 = 0

time_taken = 0

total_sec = 0

total_sec1 = 0

def ssl_rx():

  data_count = 0
  count = 0
  while not finnished:  

    #data_in = ""
    data_in=connection.recv(1370)


    #print "data_in is %s" %data_in

    #print "length of data is %d\n" %len(data_in)

    #print "count is %d" %count


    data_count = data_count + len(data_in)

    #print "data_count is %d" %data_count

    #print "size of data is %d" % data_count

    count = count + 1

    #print "count is %d" %count
    #This should match with MAX_TX_PKTS in SSL_TX App
    if(count == 10000):

        etime= datetime.datetime.now()

        ttime = (etime-stime)

        total_sec1 = (ttime.seconds) + (ttime.microseconds/1000000.0)
	
        #print "total sec is %f" %total_sec1

        throughput = (float) (data_count  * 8) / (1000 * 1000 * (total_sec1))

        print("data_count is %f" %data_count)

        print("throughput is %f" %throughput)
	
	#break
        exit()


#create socket
server_socket=socket(AF_INET, SOCK_STREAM)

#Bind to an unused port on the local machine
server_socket.bind(('0.0.0.0',5001))

#listen for connection
server_socket.listen (10)
tls_server = wrap_socket(server_socket, ssl_version=PROTOCOL_TLSv1, cert_reqs=CERT_NONE, server_side=True, keyfile='server-key.pem', certfile='server-cert.pem')

print('server started')

#accept connection
connection, client_address= tls_server.accept()
print ('connection from', client_address)

stime = datetime.datetime.now()
ssl_rx()

exit()
 
#close the connection
connection.shutdown(SHUT_RDWR)
connection.close()

#close the server socket
server_socket.shutdown(SHUT_RDWR)
server_socket.close()


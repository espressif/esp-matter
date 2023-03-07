#server side
# echo client
import sys
import time
from socket import *
from ssl import *


if sys.version_info[0] < 3:

    import thread

else:
    import _thread




message='start12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456712345678901234563747er737487er871234567890123456789012345678901234567890123456789012345678901234567890endend' #input ('enter message:   ')


data_out= message.encode ()


def ssl_tx(controller_ip,port):

    
  pkt_cnt = 0

  while 1:  

    #print "send data"
    #data_out= message.encode ()

    #send data out
    connection.send(data_out)    
    pkt_cnt = pkt_cnt+1
    #print('Send count:',pkt_cnt)
    
    #number of packets received, 10000. 
    if(pkt_cnt == 10000):

        print('Send count:',pkt_cnt)
        break;
    #receive data
    # data_in=tls_client.recv(1024)


    #decode message
    #response= data_in.decode()
    #print('Received from client:', response)
  
    #time.sleep(0.01)


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



#thread.start_new_thread(ssl_tx,(client_address,443))

connection.send(data_out)    
print("start data")
ssl_tx(client_address,443)
print("Bye...")
exit()
print("Bye...2222")

pkt_recv = 0
while not finnished:

    
    data_previous = data_new

    #send and receive data from the client socket
    data_in=connection.recv(1370)

    #pkt_recv = pkt_recv + 1
    #print "pkt rcv count is %d" %pkt_recv




    #print "data_in is %s" %data_in

    present_pkt = data_in[0:8]

    print("present pkt is %s" %present_pkt)

    data_new = int(present_pkt)

    print("data is %d" %data_new)

    if(not((data_new - data_previous) == 1)):
        exit()

    #print "diff b/w previous and nwe is %d" %(data_new - data_previous)
    


    #exit()
    
    #message=data_in.decode()

    #print "data is %s" %data_in
    
  
        #connection.send(data_out)
    #messge1 = 'quit'
#close the connection
connection.shutdown(SHUT_RDWR)
connection.close()

#close the server socket
server_socket.shutdown(SHUT_RDWR)
server_socket.close()


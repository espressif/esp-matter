/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>

int main()
{
  int clientSocket;
  int byte_recv;
  char buffer[1500];
  char buffer1[1500];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  int pkt_length, flags, err, ret;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(5001);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("192.168.2.5");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  flags = fcntl(clientSocket, F_GETFL, 0);
  err   = fcntl(clientSocket, flags | O_NONBLOCK);

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
  if ((connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size)) >= 0) {
    printf("\n connection success \n");
  } else {
    exit(0);
  }

  /*---- Read the message from the server into the buffer ----*/
  do {
    for (pkt_length = 1; pkt_length < 1460; pkt_length++) {

      ret = send(clientSocket, buffer, pkt_length, 0);
      if (ret <= 0) {
        printf("\nSend error: %d\n", ret);
        continue;
      }

      recv(clientSocket, buffer1, pkt_length, 0);
    }
    for (pkt_length = 1; pkt_length < 1460; pkt_length++) {

      int len = (rand() % 1460) + 1;

      ret = send(clientSocket, buffer, len, 0);
      if (ret <= 0) {
        printf("\nSend error: %d\n", ret);
        continue;
      }

      recv(clientSocket, buffer1, len, 0);
    }
  } while (1);

  return 0;
}

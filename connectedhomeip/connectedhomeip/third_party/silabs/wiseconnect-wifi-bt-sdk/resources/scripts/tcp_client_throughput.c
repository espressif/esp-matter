/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#define REQ_THRUPT_IN_MBPS 10 //! Configure tx rate, for 1Mbps-1; 5Mbps-5; 10Mbps -10

static int last_tx_print_time;
static int last_rx_print_time;

int total_tx_bytes;
int total_rx_bytes;
int secs, sec_cnt = 0;

void measure_throughput(int pkt_length, int tx_rx)
{
  static int current_time;
  static int last_print_time;
  int total_bytes;
  struct timeval tv1;
  float through_put;

  gettimeofday(&tv1, NULL);
  current_time = tv1.tv_sec * 1000000 + tv1.tv_usec;
  if (tx_rx == 0) {
    total_tx_bytes += pkt_length;
    total_bytes     = total_tx_bytes;
    last_print_time = last_tx_print_time;
  } else {
    total_rx_bytes += pkt_length;
    total_bytes     = total_rx_bytes;
    last_print_time = last_rx_print_time;
  }
  if ((current_time - last_print_time) >= 1000000) //!for 1 sec
  {
    through_put = ((float)(total_bytes) / ((current_time - last_print_time))) * 8;
    if (tx_rx == 0) {
      printf("\nSecs: %d     Bytes Transmitted %d,Throughput for last %d seconds is = %3.1f Mbps\n",
             secs++,
             total_bytes,
             (current_time - last_print_time) / 1000000,
             through_put);
      last_tx_print_time = current_time;
      total_tx_bytes     = 0;
    } else {
      printf("\nSecs: %d     Bytes Received %d,Throughput for last %d seconds is = %3.1f Mbps\n",
             secs++,
             total_bytes,
             (current_time - last_print_time) / 1000000,
             through_put);
      last_rx_print_time = current_time;
      total_rx_bytes     = 0;
    }
  }
  return;
}

int packet        = 0;
double start_time = 0, end_time = 0, tot_time = 0, start_time1 = 0, end_time1 = 0;
int main()
{
  int clientSocket;
  int byte_recv;
  char buffer[1500];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  int pkt_length;
  struct timeval tv1;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);

  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(5001);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("192.168.77.202");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /*---- Connect the socket to the server using the address struct ----*/
  addr_size = sizeof serverAddr;
  if ((connect(clientSocket, (struct sockaddr *)&serverAddr, addr_size)) >= 0) {
    printf("\n connection success \n");
  } else {
    exit(0);
  }

  memset(buffer, 'i', 1460);
  /*---- Read the message from the server into the buffer ----*/
  gettimeofday(&tv1, NULL);
  start_time  = tv1.tv_sec * 1000 + tv1.tv_usec / 1000;
  start_time1 = tv1.tv_sec * 1000 + tv1.tv_usec / 1000;
  sec_cnt     = 0;
  do {
    send(clientSocket, buffer, 1460, 0);
    end_time1 = tv1.tv_sec * 1000 + tv1.tv_usec / 1000;
    if (end_time1 - start_time1 > 20000)
      exit(0);

    packet++;

    if (packet == (int)REQ_THRUPT_IN_MBPS * 86) {
      gettimeofday(&tv1, NULL);
      end_time = tv1.tv_sec * 1000 + tv1.tv_usec / 1000;
      tot_time = (1000000 - ((end_time - start_time) * 1000));
      if (tot_time > 0) {
        usleep((int)tot_time);
      }
      gettimeofday(&tv1, NULL);
      start_time = tv1.tv_sec * 1000 + tv1.tv_usec / 1000;
      packet     = 0;
    }
  } while (1);

  return 0;
}

/*******************************************************************************
* @file  firmware_update_tcp_server.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#define RPS_HEADER 0x01
#define RPS_DATA   0x00
#include "stdio.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

int processRequest(int, FILE *);
signed int sock_id, fd;
void handler(int signo)
{
  close(fd);
  close(sock_id);
  exit(1);
}

int main(int argc, char **argv)
{
  signed int dst_len;
  struct sockaddr_in sock;
  struct sockaddr_in dst_sock;
  int window_size = 2920;
  FILE *fp        = NULL;

  char sendip[50];

  //! Checking all proper Command line Arguements
  if (argc < 3) {
    printf("Usage ./a.out <local port> <RPS file path>\n");
    exit(0);
  }

  //! Creating of the TCP Socket
  if ((sock_id = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("Socket create fail\n");
    return;
  }

  //! filling the structure to connect with the client
  sock.sin_family      = AF_INET;
  sock.sin_port        = htons(atoi(argv[1]));
  sock.sin_addr.s_addr = htonl(0);

  //! try to bind to the specified socket
  if (bind(sock_id, (struct sockaddr *)&sock, sizeof(sock)) == -1) {
    printf("Socket bind fail\n");
    return;
  }

  signal(SIGINT, handler);
  setsockopt(sock_id, SOL_SOCKET, SO_RCVBUF, (char *)&window_size, sizeof(window_size));

  if (listen(sock_id, 1) != 0) {
    printf("Socket Listen fail\n");
    return;
  }

  printf("Listen passed\n");

  dst_len = sizeof(struct sockaddr_in);
  while (1) {
    //! Accept client request
    printf("waiting for client to connect\n");
    //fd = accept( sock_id,  (struct sockaddr *)&dst_sock, (socklen_t *)&dst_len);
    fd = accept(sock_id, (struct sockaddr *)&dst_sock, &dst_len);

    printf("accept success \n");
    fp = fopen(argv[2], "r");
    if (fp == NULL) {
      printf("unabled to open rps file\n");
      return;
    }
    //strcpy(sendip,(const char *)inet_ntop(AF_INET,(void *)&dst_sock.sin_addr,sendip,sizeof(sendip)));
    //printf("Connect Req from %s accepted\n",sendip);

    if (fork() == 0) {
      close(sock_id);
      processRequest(fd, fp);
      exit(0);
    }
    close(fd);
  }
}

int processRequest(int fd, FILE *fp)
{
  int ctr            = 0;
  signed int ret_len = 0, tx_len = 0;
  int errno;
  unsigned char data[3];
  unsigned char *ptr = data;
  unsigned char cmd_type;
  char data1[1500];
  int length = 0;

  while (1) {

again:

    printf("waiting for recv\n");
    ret_len = recv(fd, ptr, 100, 0);
    printf("recv length == 0x%x\n", ret_len);
    if (ret_len < 0 && errno == EINTR) {
      printf("Recv error\n");
      goto again;
    } else if (ret_len < 0) {
      printf("error while receiving\n");
      return (0);
    }

    if (ret_len > 0) {
      cmd_type = data[0];
      if (cmd_type == RPS_HEADER) {

        length   = fread((data1 + 3), 1, 64, fp);
        data1[0] = RPS_HEADER;
        data1[1] = (length & 0xff);
        data1[2] = ((length >> 8) & 0xff);
        printf("length of first chunk==%d\n", length);
      } else if (cmd_type == RPS_DATA) {
        length   = fread((data1 + 3), 1, 1024, fp);
        data1[0] = RPS_DATA;
        data1[1] = (length & 0x00ff);
        data1[2] = ((length >> 8) & 0x00ff);
        if (feof(fp)) {
          fclose(fp);
          printf("reach end of file\n");
          tx_len   = send(fd, data1, length + 3, 0);
          length   = 0;
          data1[0] = RPS_DATA;
          data1[1] = (length & 0x00ff);
          data1[2] = ((length >> 8) & 0x00ff);
          tx_len   = send(fd, data1, length + 3, 0);
          return;
        }
      }
    } else if (ret_len == 0) {
      fclose(fp);
      printf("reach end of file\n");
      tx_len = send(fd, data1, length + 3, 0);
      return;
    }

    printf("size of data1==%d\n", length);
    tx_len = send(fd, data1, length + 3, 0);
    printf("send returns %d\n", tx_len);

    if (!tx_len) {
      printf("error while sending\n");
      return (0);
    }
    printf("Pkt sent no:%d\n", ++ctr);
  }
}

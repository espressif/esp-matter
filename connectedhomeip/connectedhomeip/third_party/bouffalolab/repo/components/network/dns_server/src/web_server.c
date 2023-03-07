
/* Brief: This demo shows how to use BL602 as a webserver
 * 
 * - use a sta connect to this ap
 * - open the browser and input the ip_address of this BL602(the default ip_adress is: 192.168.4.1 )
 * - and you can see "hello,this is BL602" on this web page
 *
 */
 
#include "FreeRTOS.h"
#include "portmacro.h"
#include "event_groups.h"
#include "lwip/err.h"
#include "string.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "web_server.h"

#define TAG  "lwip_udp"

const static char http_html_hdr[] =
    "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static char http_index_hml[] = "<!DOCTYPE html>"
      "<html>\n"
      "<head>\n"
      "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
      "  <style type=\"text/css\">\n"
      "    html, body, iframe { margin: 0; padding: 0; height: 100%; }\n"
      "    iframe { display: block; width: 100%; border: none; }\n"
      "  </style>\n"
      "<title>HELLO BL602</title>\n"
      "</head>\n"
      "<body>\n"
      "<h1>Hello World, from BL602!</h1>\n"
      "</body>\n"
      "</html>\n";

static void web_http_server(struct netconn *conn)
{
  struct netbuf *inputbuf;
  char *buf;
  u16_t buflen;
  err_t err;

  err = netconn_recv(conn, &inputbuf);
  if (err == ERR_OK) {
    netbuf_data(inputbuf, (void**)&buf, &buflen);
    printf("the received data:\n%s\n",buf);
    /* Judge if this is an HTTP GET command */
    if (buflen >= 5 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T' && buf[3] == ' ' && buf[4] == '/' ) {
      netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);

      if(buf[5]=='h') {
        netconn_write(conn, http_index_hml, sizeof(http_index_hml)-1, NETCONN_NOCOPY);
      }
      else if(buf[5]=='l') {
        netconn_write(conn, http_index_hml, sizeof(http_index_hml)-1, NETCONN_NOCOPY);
      }
//      else if(buf[5]=='j') {
//    	  netconn_write(conn, json_unformatted, strlen(json_unformatted), NETCONN_NOCOPY);
//      }
      else {
          netconn_write(conn, http_index_hml, sizeof(http_index_hml)-1, NETCONN_NOCOPY);
      }
    }
  }
  netconn_close(conn);
  netbuf_delete(inputbuf);
}


void web_server2(void *pvParameters)
{
  struct netconn *conn, *newconn;
  err_t err;
  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, NULL, 80);
  netconn_listen(conn);
  while (1) {
    err = netconn_accept(conn, &newconn);
    if (err == ERR_OK) {
      web_http_server(newconn);
      netconn_delete(newconn);
    }
    else {
      netconn_close(conn);
      netconn_delete(conn);
      break;
    }
  }
}

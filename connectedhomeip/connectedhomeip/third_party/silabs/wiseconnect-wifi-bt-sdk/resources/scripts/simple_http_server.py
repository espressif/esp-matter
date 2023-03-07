"""
Description: This script is a simple python web server which accepts a PUT request and writes the supplied data to a file.
There is a known issue with the signal module when trying to run this on Windows 7.  It has worked in various *nix environments.

This script is a combination of the following links:
https://gist.githubusercontent.com/codification/1393204/raw/3fd4a48d072ec8f7f453d146814cb6e7fc6a129f/server.py
http://www.acmesystems.it/python_httpserver
"""
#!/usr/bin/python
import sys
import signal
from os import curdir, sep
import cgi
from threading import Thread

if sys.version_info[0] < 3:

    from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler

else:

    from http.server import BaseHTTPRequestHandler, HTTPServer



class _Getch:
    """Gets a single character from standard input.  Does not echo to the screen."""
    def __init__(self):
        try:
            self.impl = _GetchWindows()
        except ImportError:
            self.impl = _GetchUnix()

    def __call__(self): return self.impl()


class _GetchUnix:
    def __init__(self):
        import tty, sys

    def __call__(self):
        import sys, tty, termios
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch


class _GetchWindows:
    def __init__(self):
        import msvcrt

    def __call__(self):
        import msvcrt
        return msvcrt.getch()


getch = _Getch()

class PUTHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        print('\n--------- REQUEST METHOD: ','POST----------')
        print(self.headers)
        length = int(self.headers['Content-Length'])
        data_content = self.rfile.read(length)
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()
        print("Data Content: ",data_content, "\n")

        print('-------------  POST SUCCESS  --------------', "\n")

    def do_PUT(self):
        print('\n----------- REQUEST METHOD: ','PUT ------------')
        print(self.headers)
        length = int(self.headers['Content-Length'])
        try:
            with open(curdir + self.path, "wb") as dst:
                dst.write(self.rfile.read(length))

            self.send_response(200)
            print('-------------   PUT SUCCESS  --------------\n')
        except Exception as e:
            print('PUT Failed ')
            print(e)


    def do_HEAD(s):
        s.send_response(200)
        s.send_header("Content-type", "text/html")
        s.end_headers()
    #Handler for the GET requests
    def do_GET(self):
        print('\n----------- REQUEST METHOD: ','GET -------------\n')
        print(self.headers)
        if self.path=="/":
            self.path="/index.html"

        try:
            #Check the file extension required and
            #set the right mime type

            sendReply = False
            if self.path.endswith(".html"):
                    mimetype='text/html'
                    sendReply = True
            if self.path.endswith(".jpg"):
                    mimetype='image/jpg'
                    sendReply = True
            if self.path.endswith(".gif"):
                    mimetype='image/gif'
                    sendReply = True
            if self.path.endswith(".js"):
                    mimetype='application/javascript'
                    sendReply = True
            if self.path.endswith(".css"):
                    mimetype='text/css'
                    sendReply = True
            if self.path.endswith(".mp3"):
                    mimetype='text/plain'
                    sendReply = True
            if self.path.endswith(".txt"):
                    mimetype='text/plain'
                    sendReply = True
            if sendReply == True:
                    #Open the static file requested and send it
                    f = open(curdir + sep + self.path, 'rb')
                    self.send_response(200)
                    print('--------------   GET SUCCESS  --------------\n')
                    self.send_header('Content-type',mimetype)
                    self.end_headers()
                    self.wfile.write(f.read())
                    f.close()
            return

        except IOError:
            self.send_error(404,'File Not Found: %s' % self.path)
            
def run_on(port):
    print("\nLaunching HTTP server on port %i ..." % port)
    server_address = ('', port)
    httpd = HTTPServer(server_address, PUTHandler)
    print("Server successfully acquired the socket with port:", port)
    print("Press Ctrl+C to shut down the server and exit.")
    print("\nAwaiting New connection\n")
    httpd.serve_forever()
    

if __name__ == "__main__":
    if(len(sys.argv) < 2):
        print("Usage: put-server.py <port #>")
        sys.exit()
    ports = [int(arg) for arg in sys.argv[1:]]
    for port_number in ports:
        server = Thread(target=run_on, args=[port_number])
        server.daemon = True # Do not make us wait for you to exit
        server.start()
    #signal.pause() # Wait for interrupt signal, e.g. KeyboardInterrupt
    while  1:
       x = getch
        

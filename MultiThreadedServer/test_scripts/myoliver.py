from enum import Enum
import argparse, logging, os, select, toml, time, sys, shutil, re, socket

class Req_State(Enum):
    CONNECTED = 1
    SENT_LINE = 2
    SENT_HEADERS = 3
    SENDING_BODY = 4
    SENT = 5
    RECEIVED = 6

class Req_Method(Enum):
    GET = 1
    PUT = 2
    APPEND = 3

    def to_string(self):
        if self == Req_Method.GET:
            return "GET"
        elif self == Req_Method.PUT:
            return "PUT"
        else:
            return "APPEND"

class request():
    ''' Reprsents a new connection with a client '''

    def __init__(self, hostname, port, method, uri, rid, inbody, outfile, block=False):

        '''
        Creates a new client request:
        hostname: the name of the host to connect to
        port: the port to connect with
        uri: the URI to refernece in the request
        method: The method to use, of time Req_Method
        rid: The Request ID to use
        inbody: the data to send (must be encoded as UTF-8, I hope?)
        out_body: where to place the message body part of the output
        '''

        request_line = f"{method.to_string()} /{uri} HTTP/1.1\r\n"
        headers = [f"Request-Id: {rid}\r\n"]

        if len(inbody) > 0:
            headers.append(f"Content-Length: {len(inbody)}\r\n")

        self.bytez = [bytearray(request_line.encode('UTF-8'))]
        headerz = bytearray()
        for h in headers:
            headerz.extend(h.encode('UTF-8'))
        headerz.extend(b"\r\n")
        self.bytez.append(headerz)
        self.bytez.append(inbody)

        assert (len(self.bytez) == 3)

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((socket.gethostbyname(hostname), port))
        self.state = Req_State.CONNECTED

        self.rid = rid
        self.outfile = outfile
        self.sent = 0
        self.state = Req_State.CONNECTED
        self.received = bytearray()

        self.block = (block == 0)

        # set SO_RCVBUF to 4096 for level  SOL_SOCKET when block.
        # This makes everything just a bit slow.
        if block > 1:
            self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, block)

    def __del__(self):
        self.sock.close()

    def send_line(self):
        assert (self.state == Req_State.CONNECTED)
        self.sock.sendall(self.bytez[0])
        self.state = Req_State.SENT_LINE

    def send_headers(self):
        assert (self.state == Req_State.SENT_LINE)
        self.sock.sendall(self.bytez[1])
        self.state = Req_State.SENT_HEADERS

    def send_body(self, size):
        assert (self.state == Req_State.SENT_HEADERS or\
                self.state == Req_State.SENDING_BODY)

        start = self.sent
        end = self.sent + size

        if size == -1 or len(self.bytez[-1]) - self.sent < size:
            end = len(self.bytez[-1])

        self.sock.sendall(self.bytez[-1][start : end])
        self.sent = end

        if len(self.bytez[-1]) == self.sent:
            self.state = Req_State.SENT
            self.sock.shutdown(socket.SHUT_WR)
            return self.sock
        else:
            self.state = Req_State.SENDING_

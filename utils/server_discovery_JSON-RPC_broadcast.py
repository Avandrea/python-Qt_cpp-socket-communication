# Send a json message via broadcast udp at port 54546 to be discovered over the network

import socket
import sys
import struct
import array
import random
import time
import json

# Data structure

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

# server_address = ('localhost', 8000)
# print('[SERVER] starting up on %s port %s' % server_address)
# sock.bind(server_address)

message = {
    'msgtype': 'Identity',
    'services': {
        'ftp': {
        'port': 30000,
        'addresses': ['10.10.42.59']
    },
    'rpc': {
        'port': 34653,
        'addresses': ['10.10.42.59']
    },
    'multicast': {
        'address': '239.255.31.251',
        'port': 42854
    }
    },
    'diagnostics': {
        'grabber': {'isalive': True},
        'camera': {'isalive': True},
        'scanner': {'isalive': False}
    },
    'publisher': 'Specim',
    'system': {
        'name': 'FX50#123456',
        'type': 'cortex'
    },
    'id': 123456,
    'version': 0.1
}
# tmpMessage = json.dumps(message)
messageJson = json.dumps(message)
s = "ABCD"
b = bytearray()
b.extend(messageJson.encode())
print(messageJson)

# Set a timeout so the socket does not block
# indefinitely when trying to receive data.
sock.settimeout(0.2)
# message = b"your very important message"
while True:
    sock.sendto(b, ('<broadcast>', 54546))
    print("Sending discovery json to broadcast 54546")
    time.sleep(1)

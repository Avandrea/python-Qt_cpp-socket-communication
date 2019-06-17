# Send a json message via broadcast udp at port 54546 to be discovered over the network

import socket
import sys
import struct
import array
import random
import time
import json

# Data structure

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.settimeout(50000)

message = {"jsonrpc": "2.0", "result": True, "id": 1}

messageJson = json.dumps(message)
b = bytearray()
b.extend(messageJson.encode())
print(messageJson)

# Set a timeout so the socket does not block
# indefinitely when trying to receive data.
sock.bind(('0.0.0.0', 34653))
sock.listen(1) 
conn, addr = sock.accept()

while True:
    print('[SERVER RPC] waiting to receive commands')
    data, recaddress = conn.recv(4096)

    print('[SERVER RPC] received %s bytes from %s' % (len(data), recaddress))
    print(data.decode())

    address = (recaddress[0], 34653)
	
    if data.decode() == "send me the image":
        while True:
            sock.sendto(b, address)
            print("[SERVER RPC] Sending answer json to broadcast 34653")
            time.sleep(1)

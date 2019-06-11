import socket
import sys
import time
import struct

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

server_address = ('192.168.0.171', 8000)
message = 'send me the image'

while True:
    try:
        # Send data
        print('[CLIENT] sending request: "%s"' % message)
        sent = sock.sendto(message.encode(), server_address)

        # Receive response
        #print('[CLIENT] waiting to receive')
        #data, server = sock.recvfrom(4096)
        #format = 'IiiiiiiBhhh' + 'i'*300
        #data_unpack = struct.unpack(format, data)
        #print('[CLIENT] received ', data_unpack)
    except:
        print("[CLIENT] Error in client ")
    # finally:
    #     print('[CLIENT] closing socket')
    #     sock.close()
    time.sleep(5)
print('[CLIENT] closing socket')
sock.close()

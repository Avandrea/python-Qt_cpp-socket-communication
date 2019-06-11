import socket
import sys
import struct
import array
import random
import time
# from fletcherChecksum import Fletcher16

# Format  C Type              Python type
# x       pad byte            no value
# c       char                bytes of length 1   1
# b       signed char         integer             1
# B       unsigned char       integer             1
# ?       _Bool               bool                1
# h       short               integer             2
# H       unsigned short      integer             2
# i       int                 integer             4
# I       unsigned int        integer             4
# l       long                integer             4
# L       unsigned long       integer             4
# q       long long           integer             8
# Q       unsigned long long  integer             8
# n       ssize_t             integer
# N       size_t              integer
# e                           float               2
# f       float               float               4
# d       double              float               8
# s       char[]              bytes
# p       char[]              bytes
# P       void *              integer

def buildBytes(type, val):
    return struct.pack(type, val)

# Building the structure to be sent
# // Header size 33 bytes + 2 byte of checksum
# //[-----------------------------------------------------------------------------------------------------------------------------------------------------------+-----------------------------------]
# //[                                                             Header                                                                                        | metadata | data |   timestamps    ]
# //[-----------------------------------------------------------------------------------------------------------------------------------------------------------+-----------------------------------]
# //[ startcode | frame number | width | height | data type | metadata size | data size | timestamp count | metadata checksum | data checksum | header checksum | metadata | data |   timestamps    ]
# //[-----------------------------------------------------------------------------------------------------------------------------------------+----------+------+-----------------------------------]
# //[     4     |      4       |   4   |   4    |     4     |       4       |     4     |       1         |         2         |       2       |        2        |     ~    |  ~   |      ~          ]
# //[-----------------------------------------------------------------------------------------------------------------------------------------+----------+------+-----------------------------------]

dataSize = 26*13

# Create the data: random 0-255 int
final_data = []
# final_data = [random.randint(0, 255) for _ in range(dataSize)]
final_data = [
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,0,0,255,255,255,255,255,255,255,255,255,255,0,0,255,255,255,255,255,255,
255,255,255,255,0,0,0,255,255,255,255,255,0,0,255,255,255,255,255,0,0,0,255,255,255,255,
255,255,0,0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255,0,0,0,0,0,255,255,
255,0,0,0,0,0,0,0,255,255,255,0,0,0,0,255,255,255,0,0,0,0,0,0,0,255,
255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,
255,0,0,0,0,255,255,0,0,0,255,0,0,0,0,255,0,0,0,255,255,0,0,0,0,255,
255,255,0,0,255,255,255,255,255,255,255,255,0,0,255,255,255,255,255,255,255,255,0,0,255,255,
255,255,255,0,0,255,255,255,255,255,255,255,0,0,255,255,255,255,255,255,255,0,0,255,255,255,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255]

final_data1 = [
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,
255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]

final_data2 = [
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
]

print(final_data)
# fletcher = Fletcher16()
# fletcher.update(final_buf)
# checksum = fletcher.hexdigest()
# print(checksum)

# a = struct.pack('B'*4,1,1)
startcode = struct.pack('>I', 1759330036)
frame_number = struct.pack('>i', 0)
width = struct.pack('>i', 26)
height = struct.pack('>i', 13)
data_type = struct.pack('>i', 36)
metadata_size = struct.pack('>i', 0)
data_size = struct.pack('>i', dataSize)
timestamp_count = struct.pack('>B', 0)
metadata_checksum = struct.pack('>h', 0)
data_checksum = struct.pack('>h', 0)
header_checksum = struct.pack('>h', 0)

# test = struct.pack('>i', 1759330036)

# Fill the header
final_header = [] # array.array('b', b'\0' * 36)
final_header.append(1759330036) # startcode
final_header.append(0) # frame_number
final_header.append(300) # width
final_header.append(1) # height
final_header.append(0) # data_type
final_header.append(0) # metadata_size
final_header.append(dataSize) # data_size
final_header.append(0) # timestamp_count
final_header.append(0) # metadata_checksum
final_header.append(0) # data_checksum
final_header.append(0) # header_checksum


final_header.extend(final_data)
final_total = final_header

# final_buf = struct.pack('>IiiiiiiBhhh' + 'i'*data_size, *final_total)
final_buf = struct.pack('>' + 'B'*dataSize, *final_data)
final_buf1 = struct.pack('>' + 'B'*dataSize, *final_data1)
final_buf2 = struct.pack('>' + 'B'*dataSize, *final_data2)

print(final_buf)

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# Bind the socket to the port
server_address = ('localhost', 54547)
print('[SERVER] starting up on %s port %s' % server_address)
sock.bind(server_address)


# print(struct.calcsize(test))
switch = 0


while True:
    print('\n[SERVER] waiting to receive message')
    data, recaddress = sock.recvfrom(4096)

    print('[SERVER] received %s bytes from %s' % (len(data), recaddress))
    print(data.decode())

    address = (recaddress[0], 30000)
	
    if data.decode() == "send me the image":
        while True:
            print("Sending data to ", address)
            sleepTime = 0.05
            sent = sock.sendto(startcode, address)
            time.sleep(sleepTime)
            sent += sock.sendto(frame_number, address)
            time.sleep(sleepTime)
            sent += sock.sendto(width, address)
            time.sleep(sleepTime)
            sent += sock.sendto(height, address)
            time.sleep(sleepTime)
            sent += sock.sendto(data_type, address)
            time.sleep(sleepTime)
            sent += sock.sendto(metadata_size, address)
            time.sleep(sleepTime)
            sent += sock.sendto(data_size, address)
            time.sleep(sleepTime)
            sent += sock.sendto(timestamp_count, address)
            time.sleep(sleepTime)
            sent += sock.sendto(metadata_checksum, address)
            time.sleep(sleepTime)
            sent += sock.sendto(data_checksum, address)
            time.sleep(sleepTime)
            sent += sock.sendto(header_checksum, address)
            time.sleep(sleepTime)
            if switch == 0:
                sent = sock.sendto(final_buf, address)
                switch = 1
            elif switch == 1:
                sent = sock.sendto(final_buf1, address)
                switch = 2
            else:
                sent = sock.sendto(final_buf2, address)
                switch = 0
            time.sleep(5)


        print('[SERVER] sent %s bytes back to %s' % (sent, address))

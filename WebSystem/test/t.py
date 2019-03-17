import socket

sockfd = socket.socket(family=socket.AF_INET,type=socket.SOCK_DGRAM)

# sockfd.sendto("12345\n".encode(),("192.168.191.128",8888))
sockfd.sendto("12345\n".encode(),("192.168.191.5",7777))
sockfd.close()

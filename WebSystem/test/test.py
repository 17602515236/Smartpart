import socket

#create socket
sockfd = socket.socket(family=socket.AF_INET,type=socket.SOCK_DGRAM)
#Bind
sockfd.bind(("",8888))
print("start receive")
while True:
    data,client_addr = sockfd.recvfrom(1024)
    sockfd.sendto(data,0,client_addr);
    print(data)


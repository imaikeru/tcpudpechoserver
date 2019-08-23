TCP server usage:
gcc tcpechoserver.c -o tcpechoserver && ./tcpechoserver <port>
if port is missing, default is 7777

UDP server usage:
gcc udpechoserver.c -o udpechoserver && ./udpechoserver <port>
if port is missing, default is 7778

TCP & UDP server usage:
gcc tcpudpechoserver.c -o tcpudpechoserver && ./tcpudpechoserver <port1> <port2>
if tcpPort is missing, default is 7777
if udpPort is missing, default is 7778
UDP and TCP are different processes. TCP process spawns its own children in order to serve multiple clients. 

TCP client usage:
gcc tcpechoclient.c -o tcpechoclient && ./tcpechoclient <port>
if port is missing, default is 7777
terminate with input "over"

UDP client usage:
gcc udpechoclient.c -o udpechoclient && ./udpechoclient <port>
if port is missing, default is 7778
terminate with input "over"


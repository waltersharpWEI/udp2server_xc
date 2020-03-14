/*
 * UDPServer.cpp
 *
 *  Created on: Dec 23, 2019
 *      Author: ubuntu
 */


#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h> /* the L2 protocols */

#include "../include/UDPServer.h"

#define UDPLITE_SEND_CSCOV  10
#define UDPLITE_RECV_CSCOV  11

UDPServer::UDPServer() {
	 if((packet_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	 {
		 perror ("packet socket fail");
		 exit(1);
	 }
}

UDPServer::~UDPServer() {
	close(packet_socket);
}

//initialize the IP and port
int UDPServer::init(char *IPx, int portx) {
	strcpy(IP,IPx);
	port = portx;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(IP);
	struct timeval read_timeout;
	read_timeout.tv_sec = 0;
	read_timeout.tv_usec = 10;
	setsockopt(packet_socket, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
}

int UDPServer::bind_x() {
	if(-1 == (bind(packet_socket,(struct sockaddr*)&addr,sizeof(addr)))) {
		perror("UDP Server Bind Failed:");
		exit(1);
	}
}

ssize_t UDPServer::recvfrom_x(void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen) {

	return recvfrom(packet_socket, buf, len, flags, src_addr, addrlen);
}

//clear all UDP options,
//call before set the UDP-mode
void UDPServer::optclear() {

}
//set to UDP-Default
void UDPServer::setOptDefault() {
	int buffer_size = 0;
	setsockopt(packet_socket, SOL_SOCKET, SO_RCVBUF, (int*)&buffer_size, sizeof(int));
}
//set to UDP-Lite
void UDPServer::setOptLite(int cov) {
	close(packet_socket);
	if((packet_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDPLITE)) < 0)
	{
		perror ("UDP-Lite socket");
		exit(1);
	}
	int val = 8;
	setsockopt(packet_socket, IPPROTO_UDP, UDPLITE_SEND_CSCOV, &val, sizeof(int));
}
//set to UDP-Soomro
void UDPServer::setOptSoomro(int recv_buffer_size){
	int buffer_size = 26214400;
	//buffer_size = recv_buffer_size;
	setsockopt(packet_socket, SOL_SOCKET, SO_RCVBUF, (int*)&buffer_size, sizeof(int));
}
//set the buffer size to size in bytes
void UDPServer::setBuffer(int size){
	int ret = 0;
	int reuse = 1;
	ret = setsockopt(packet_socket, SOL_SOCKET, SO_REUSEADDR,(const void *)&reuse , sizeof(int));
	if (ret < 0) {
			perror("setsockopt");
			_exit(-1);
	}
	ret = setsockopt(packet_socket, SOL_SOCKET, SO_REUSEPORT,(const void *)&reuse , sizeof(int));
	if (ret < 0) {
		perror("setsockopt");
		_exit(-1);
	}
}

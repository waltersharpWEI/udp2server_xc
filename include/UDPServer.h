/*
 * UDPServer.h
 *
 *  Created on: Dec 23, 2019
 *      Author: ubuntu
 */

#ifndef SRC_UDPServer_H_
#define SRC_UDPServer_H_

#define ADDR_LEN 100

class UDPServer {
public:
	int send_buffer_size;
	int mss;
	int current_delay;
	double current_loss_event_rate;
	int current_througput;
	int current_delay_jitter;
	double current_psnr;
	int current_rtt;
	int smoothed_rtt;
	char IP[ADDR_LEN];
	int port;
	int packet_socket,len;
	struct sockaddr_in addr;
public:
	UDPServer();
	virtual ~UDPServer();
	//initialize the IP and port
	int init(char *IP, int port);
	//bind wrapper
	int bind_x();
	//recvfrom oop wrapper
	ssize_t recvfrom_x(void *buf, size_t len, int flags,
	                 struct sockaddr *src_addr, socklen_t *addrlen);
	//clear all UDP options,
	//call before set the UDP-mode
	void optclear();
	//set to UDP-Default
	void setOptDefault();
	//set to UDP-Lite
	void setOptLite(int cov);
	//set to UDP-Soomro
	void setOptSoomro(int recv_buffer_size);
	//set the buffer size to size in bytes
	void setBuffer(int size);
};

#endif /* SRC_UDPServer_H_ */

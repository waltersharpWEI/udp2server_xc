
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
#include <chrono>
#include "../include/UDPServer.h"
#include "Frame.h"

using namespace std;

#define ETH_HDR_LEN 14
#define IP_HDR_LEN 20
#define UDP_HDR_LEN 8
#define TCP_HDR_LEN 20
#define PACKET_LOG_LEN 300

#define MAX_MSG_LEN 1500

long long gettimestamp() {
	chrono::time_point<std::chrono::system_clock> now = chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	return millis;
}

int distractFrame(const void * buf, size_t len, struct Frame* fra) {
	int offset = 0;
	memset(fra,0,sizeof(*fra));
	memcpy(&fra->dss, buf, sizeof(fra->dss));
	offset += sizeof(fra->dss);
	memcpy(&fra->timestamp, buf + offset, sizeof(fra->timestamp));
	offset += sizeof(fra->timestamp);
	memcpy(fra->content,buf+offset, len - offset);
	return len - offset;
}

struct packet_log_node {
	long long delay;
	struct packet_log_node *next;
};
//pdl, packet_delay_log
struct packet_log_node *pdl_head, *pdl_tail;

double compute_jitter(struct packet_log_node *head) {
	struct packet_log_node * cursor;
	cursor = head;
	double jitter = 0;
	int count = 0;
	while (cursor && cursor->next != nullptr) {
		jitter += abs(cursor->delay - cursor->next->delay);
		cursor = cursor->next;
		++count;
	}
	if (count == 0) return 0;
	return jitter / count;
}

int main() {
	pdl_head = new struct packet_log_node;
	pdl_head->delay = 0;
	pdl_head->next = nullptr;
	pdl_tail = pdl_head;
	UDPServer udpS1;
	char buffer[MAX_MSG_LEN];
	int coverage = 10;
	int buffer_unit =  1024 * 1024; //1MB
	int recv_buffer = 25 * buffer_unit;

	udpS1.init("192.168.6.136",12345);

	//udpS1.setOptLite(10);
	udpS1.setOptSoomro(200000);
	udpS1.setOptDefault();
	udpS1.bind_x();

	struct sockaddr_in remote_addr;
	int sin_size;
	memset(&remote_addr,0,sizeof(sockaddr_in));
	struct Frame fra;
	int latest_dss = 0;
	long long loss_count = 0;
	long long packet_count = 0;
	double current_delay_jitter = 0;
        int ac = 0;
        int dc = 10;
	while (true) {
                ++ac;
                if (ac > 1000) {
                        int bsc0 = rand() % 1000000;
                        int bsc1 = rand() % 1000;
                        int bsc = rand() % 10 > 5? bsc0:bsc1;
                        if (ac % 10000 == 0) {
                        		udpS1.setOptSoomro(bsc);
                                
                        }
                        printf("After:%d===",bsc);
                }
		bzero(buffer,MAX_MSG_LEN);
		//int len = udpS1.recvfrom_x(buffer,MAX_MSG_LEN,0,(struct sockaddr *)&remote_addr, (socklen_t*)sin_size);
		int len = udpS1.recvfrom_x(buffer,MAX_MSG_LEN,0, NULL, NULL);
		//non-blocking io
		if (len < 0) {
			//printf("Timeout.\n");
			continue;
		}
		//int slen = sendto(udpS1.packet_socket,buffer,len,0,(struct sockaddr *)&remote_addr,sin_size);
		int length = distractFrame(buffer, len, &fra);
		++packet_count;
		if (fra.dss != latest_dss + 1) {
			++loss_count;
		}
		latest_dss = fra.dss;
		double cumulative_loss = (double)loss_count / packet_count;
		//printf("%d,%d,%s\n",len,length,fra.content);
		long long this_delay = gettimestamp() - fra.timestamp;
		current_delay_jitter = compute_jitter(pdl_head);
		printf("%lld ,%lld,%lld,%f,%f\n",fra.dss,fra.timestamp,gettimestamp() - fra.timestamp
				, cumulative_loss, current_delay_jitter);
		//printf("1");
		//usleep(1000);
		pdl_tail->next = new struct packet_log_node;
		pdl_tail->next->delay = this_delay;
		pdl_tail->next->next = nullptr;
		pdl_tail = pdl_tail->next;
		if (packet_count > PACKET_LOG_LEN) {
			struct packet_log_node * temp = pdl_head;
			pdl_head = pdl_head->next;
			delete temp;
			packet_count = 0;
			loss_count = 0;
		}
	}
	return 0;
}

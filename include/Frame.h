
#ifndef SRC_FRAME_H_
#define SRC_FRAME_H_

struct Frame{
	long long timestamp;
	unsigned long long dss;
	char content[1400];
};

#endif /* SRC_FRAME_H_ */

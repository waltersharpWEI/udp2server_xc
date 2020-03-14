import os
import sys
import subprocess
from time import sleep
from datetime import datetime
import argparse


def change_link(loss = 0,delay = 0,th = 0,target_NIC='ens6'):
	os.system("sudo tc qdisc change dev " +  target_NIC + " root netem delay " + str(delay) + "ms loss "+str(loss)+"%")



if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument("-d","--delay",help="delay of simulator",type=int)
	parser.add_argument("-l","--loss",help="loss of simulator(%)",type=int)
	args = parser.parse_args()
	delay = args.delay
	loss = args.loss
	while True:
		#interrupt the link quality every 10s
		sleep(3)
		#print('start')
		#print(datetime.now())
		change_link(loss,delay)
		#the handover takes 2s
		sleep(1)
		#print('finish')
		#print(datetime.now())
		#the handover finished
		change_link(0,0)

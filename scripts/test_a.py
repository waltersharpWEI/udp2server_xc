import os
import sys
import subprocess
import time

def run_flow(flow_size=1000000,exp_mode = 0, loss_x = 0, delay_x = 0, balance_factor = 0, model_of_running=-1, switch_interval = 100):
	#handover_num = 4
	loss = [0]
	delay = [0]
	NIC1ip = '172.31.18.64'
	NIC2ip = '172.31.67.209'
	#flow_size = 2000000
	time_to_kill = 1
	content_size = 1024
	protocol1 = 0
	protocol2 = 0
	segments_to_switch = flow_size / 2
	segment_size = 2048
	NIC1 = 'ens5'
	NIC2 = 'ens6'
	target_NIC = 'ens6'
	throughput_log_file = 'log.txt'
	
	content_sizes = [64]
	
	cmd_start_remote_server = 'ssh ubuntu@' + NIC1ip + ' /home/ubuntu/mpnext2/tools/mpnext' 
	cmd_kill_remote_server = 'ssh ubuntu@' + NIC1ip + ' killall mpnext'
	cmd_start_remote = 'ssh ubuntu@'+ NIC1ip + ' ifstat -b -n -i '+  target_NIC + ' -t 0.1 ' + '>' + throughput_log_file 
	
	remote_monitor = subprocess.Popen(cmd_start_remote, shell=True, stdout=subprocess.PIPE)
	print(cmd_start_remote_server)
	print(cmd_start_remote)
	
	for i in range(len(delay)):
	  for j in range(len(loss)):
	    os.system("sudo tc qdisc change dev " + target_NIC + " root netem loss 0% delay 0ms")
	
	    #time.sleep(6)
	    content_size = content_sizes[0]
	    remote_server = subprocess.Popen(cmd_start_remote_server, shell=True, stdout=subprocess.PIPE)
	    #remote_server.wait()
	    time.sleep(5)
	    cmd_simulator = "python3 simulator1.py" + " -l " + str(loss_x) + " -d " + str(delay_x)
	    jammer = subprocess.Popen(cmd_simulator, shell=True)
	    os.system("sudo tc qdisc change dev " +  target_NIC + " root netem delay " + str(delay[i]) + "ms loss "+str(loss[j])+"%")
	    cmd = "../tools/mpnext " + " -i " + str(switch_interval) + " -b " + str(balance_factor) + " -e " + str(exp_mode) + " -f " + str(flow_size) + " -s " + str(segment_size) + " -k " + str(time_to_kill) + ' -m ' + str(mode_of_running) + ' -c ' + str(content_size) + ' -p ' + str(protocol1) + ' -q ' + str(protocol2) + ' -w ' + str(segments_to_switch) + ' -t ' + NIC1ip + ' -u ' + NIC2ip
	    #os.system(cmd)
	    os.system(cmd+'>'+'loss'+str(loss[j])+'delay'+str(delay[i])+'.txt')
	    jammer.kill()
	    time.sleep(5)
	    remote_server_killer = subprocess.Popen(cmd_kill_remote_server, shell=True, stdout=subprocess.PIPE)
	    remote_server_killer.wait()
	    print("killcmd sent")
	    time.sleep(5)
	    print(str(delay[i]) + ':' + str(loss[j]))
	    print(cmd)
	    os.system("sudo tc qdisc change dev " + target_NIC + " root netem loss 0% delay 0ms")
	
	remote_monitor.kill()
	
	#os.system("python3 preprocessor.py log.txt")


def run_exp(delay_x=0,loss_x=0,balance=50, model_of_running=-1, switch_interval = 100):
	flow_size_list = [500000]
	exp_mode_list = [0,1,2,3]
	for flow_size in flow_size_list:
		for exp_mode in exp_mode_list:
			run_flow(flow_size,exp_mode,loss_x,delay_x,balance, model_of_running,switch_interval)
			dir_name = "flow" + str(flow_size) + "exp_mode" + str(exp_mode)
			cmd1 = "mkdir " + dir_name
			cmd2 = "mv *.txt " + dir_name
			os.system(cmd1)
			os.system(cmd2)

if __name__ == "__main__":
	balance = 50
	for loss in [0]:
		for delay in [100]:
			for model in [0,1]:
				for switch_interval in [50000, 100000,150000]:
					run_exp(loss,delay,model,switch_interval)
					file_name = "loss"+str(loss)+"delay"+str(delay)+"balance"+str(balance)+"model"+str(model)+"interval"+str(switch_interval)+".zip"
					os.system("zip " + file_name + " -r flow*")
					os.system("mv " + file_name + " ~")
				os.system("rm -rf flow*")

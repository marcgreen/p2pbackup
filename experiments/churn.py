#!/usr/bin/python

import httplib2
import math
import os
import random
import subprocess
import time
import urllib

DEBUG = True
TEST_ROOT = '/media/10g/'
BASE_WEB_LOC = 'http://users.wpi.edu/~bouffard.daniel/'
TEST_STATE_URL = BASE_WEB_LOC + 'test_state'
LATEST_PROGRAMS_URL = BASE_WEB_LOC + 'latest.zip'
ZIP_TARGET = TEST_ROOT + 'latest.zip'
DEVICE_NAME = '/dev/vdb'
BACKUP_FILE_NAME = '100M.txt'
DOWNLOAD_AVG = 18.22405692
DOWNLOAD_STD = 12.03330857
UPLOAD_AVG = 8.857064737
UPLOAD_STD = 8.680355839
DOWNLOAD_UPLOAD_RATIO = 2.527582624

def gen_file(numMs, numGs, filename):
    oneMBString = '0' * 1024 * 1024
    f = open(filename, 'w')
    
    if DEBUG:
        print("Generating " + filename + " (this might take a while)")
    
    if numGs == 0:
        for mb in range(0, numMs): # Number of MB
            f.write(oneMBString)
    else:
        for gb in range(0, numGs): # Number of GB
            for mb in range(0, numMs): # Number of MB
                f.write(oneMBString)
            print(str(gb + 1) + 'GB')

def systemcmd(cmd):
    if DEBUG:
        print(cmd)
    os.system(cmd)

def syslogger(msg):
    systemcmd('logger -p local3.info "' + msg + '"')

# In Kbit/sec
def pick_download_limit():
    picked_valid = False
    while not picked_valid:
        picked_valid = True
        speed = random.normalvariate(DOWNLOAD_AVG, DOWNLOAD_STD)
        if speed < 1.0:
            picked_valid = False
        elif speed > ((DOWNLOAD_STD * 2) + DOWNLOAD_AVG):
            speed = speed ** 2
    return speed * 1024 # Multiply by 1024 to convert from Mbits to Kbits

# In Kbit/sec
def get_upload_limit(download_limit):
    return download_limit / DOWNLOAD_UPLOAD_RATIO

def set_network_limits():
    download = int(math.floor(pick_download_limit()))
    upload = int(math.floor(get_upload_limit(download)))
    systemcmd('wondershaper eth0 ' + str(download) + ' ' + str(upload))

def report_network_usage():
    p = subprocess.Popen("sar -n DEV 1 1 | grep eth0 | grep -v Average | awk '{print $1, $2, $6, $7, \"\\n\";}'", shell=True, stdout=subprocess.PIPE)
    output = p.stdout.readline()
    syslogger(output.rstrip())

def report_bytes_sent(bytes_sent):
    syslogger(str(bytes_sent))

def get_bytes_sent():
    p = subprocess.Popen("ifconfig eth0 | grep 'TX bytes' | awk -F : '{print $3}' | awk '{print $1}'", shell=True, stdout=subprocess.PIPE)
    output = p.stdout.readline()
    return int(output.rstrip())

def get_uptime():
    index = random.random() # [0,1)
    c = 50
    if index >= 0 and index < 0.2:
        result = 10 * index * c
    elif index >= 0.2 and index < 0.8:
        result = (4 * (index - 0.2) + 2) * c
    else:
        result = (math.e ** (21 * (index - 0.8)) + 3.5) * c
    return result

def get_downtime():
    return get_uptime() / 2

# curr_state is True for online, False for offline
def get_time_left(curr_state):
    if curr_state:
        return int(round(get_uptime()))
    else:
        return int(round(get_downtime()))

def test_is_running():
    h = httplib2.Http()
    resp, content = h.request(TEST_STATE_URL, 'GET',
                              headers={'content-type':'text/plain'})
    return content == '1'

def get_software(latest_programs_url, zip_target, zip_dir):
    latest_programs = urllib.URLopener()
    latest_programs.retrieve(latest_programs_url, zip_target)
    # Unpack the software
    systemcmd('unzip -o ' + zip_target + ' -d ' + zip_dir)

def setup_env(test_root, latest_programs_url, device_name, backup_file_name):
    zip_target = test_root + 'latest.zip'
    p2pbackup_dir = test_root + 'p2pbackup/'
    btsync_dir = test_root + 'btsync/'
    
    # Mount and reinitialize the 10GB disk
    systemcmd('rm -rf ' + test_root)
    systemcmd('mkdir ' + test_root)
    systemcmd('mount -t ext4 ' + device_name + ' ' + test_root)
    
    # Get the latest software versions
    get_software(latest_programs_url, zip_target, test_root)
    
    # Reinitialize the SyncAPI Directory
    systemcmd('rm -rf ' + btsync_dir + '.SyncAPI/')
    systemcmd('mkdir ' + btsync_dir + '.SyncAPI/')
    
    # Remove the .btbackup directory if it exists
    systemcmd('rm -rf ' + test_root + '.btbackup/')
    
    # User wondershaper to set upload and download limits
    set_network_limits()
    
    # Create the 100MB file if needed
    if not os.path.isfile(test_root + backup_file_name):
        gen_file(100, 0, test_root + backup_file_name)
    
    # Start BitTorrent Sync
    systemcmd(btsync_dir + 'btsync --config ' + btsync_dir + 'conf')

def run_test(exe_loc, backup_file_loc):
    btbackup = subprocess.Popen(exe_loc, shell=True, stdin=subprocess.PIPE)
    
    # btbackup failed to start
    if btbackup.poll() != None:
        syslogger('Fatal: btbackup failed to start')
        return
    
    # Initiate the backup
    btbackup.stdin.write('backup ' + backup_file_loc + '\n')
    
    # True = online, False = offline
    curr_state = True
    time_left_in_state = get_time_left(curr_state)
    
    starting_sent_bytes = get_bytes_sent()
    last_sent_bytes = get_bytes_sent()
    curr_sent_bytes = 0
    if DEBUG:
        print(str(starting_sent_bytes))
        print('Picked ' + str(time_left_in_state) + ' for uptime')
    ending_time = time.time() + 3600 # One hour from now
    
    # If this loop ever exits, the program has crashed
    # Things that need to happen in this loop:
    # - Send network usage to the syslog-ng server
    # - Wait for a change in state
    # -- If the state counter == 0, flip the state, restart the counter using the exponentiral distribution functions
    while (btbackup.poll() == None or not curr_state) and time.time() < ending_time:
        if curr_state:
            curr_sent_bytes = get_bytes_sent()
            syslogger(str(curr_sent_bytes - last_sent_bytes))
            last_sent_bytes = curr_sent_bytes
            #report_network_usage()
        time.sleep(1) # sleep for one second
        # A time left that is less than zero represents no churn
        if time_left_in_state >= 0:
            # update the state
            if time_left_in_state == 0:
                curr_state = not curr_state
                time_left_in_state = get_time_left(curr_state)
                if not curr_state: # Tell btbackup to exit
                    if DEBUG:
                        print('Picked ' + str(time_left_in_state) +
                              ' for downtime')
                    btbackup.stdin.write('exit\n')
                    syslogger('Offline')
                else:
                    if DEBUG:
                        print('Picked ' + str(time_left_in_state) +
                              ' for uptime')
                    btbackup = subprocess.Popen(exe_loc,
                                                shell=True,
                                                stdin=subprocess.PIPE)
                    syslogger('Online')
            else:
                time_left_in_state -= 1
    ending_sent_bytes = get_bytes_sent()
    delta_sent_bytes = ending_sent_bytes - starting_sent_bytes
    syslogger('Test end. Bytes sent = ' + str(delta_sent_bytes))

def churntest():
    if DEBUG:
        print('Sleeping for 10 seconds to let system initialize...')
    time.sleep(10)
    setup_env(TEST_ROOT, LATEST_PROGRAMS_URL, DEVICE_NAME, BACKUP_FILE_NAME)
    os.system('apt-get -y install sysstat')
    
    syslogger('Waiting for the test to start...')
    # Wait for the signal to start
    while not test_is_running():
        if DEBUG:
            print('Waiting for the test to start...')
        time.sleep(60)
    
    os.chdir(TEST_ROOT + 'p2pbackup/')
    btbackup_loc = TEST_ROOT + 'p2pbackup/btbackup'
    backup_file_loc = TEST_ROOT + BACKUP_FILE_NAME
    run_test(btbackup_loc, backup_file_loc)

if __name__ == '__main__':
    churntest()

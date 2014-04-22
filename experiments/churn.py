#!/usr/bin/python

import httplib2
import os
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

def report_network_usage():
    p = subprocess.Popen("sar -n DEV 1 1 | grep eth0 | grep -v Average | awk '{print $1, $2, $6, $7, \"\\n\";}'", shell=True, stdout=subprocess.PIPE)
    output = p.stdout.readline()
    syslogger(output)

def get_downtime():
    return 1

def get_uptime():
    return 1

# curr_state is True for online, False for offline
def get_time_left(curr_state):
    if curr_state:
        return get_uptime()
    else:
        return get_downtime()

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

def setup_env(test_root, latest_programs_url, device_name):
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
    
    # Create the 1GB file if needed
    if not os.path.isfile(test_root + '1G.txt'):
        gen_file(1024, 1, test_root + '1G.txt')
    
    # Start BitTorrent Sync
    systemcmd(btsync_dir + 'btsync --config ' + btsync_dir + 'conf')

def run_test(exe_loc, backup_file_loc):
    btbackup = subprocess.Popen(exe_loc, shell=True, stdin=subprocess.PIPE)
    
    if btbackup.poll() != None:
        syslogger('Fatal: btbackup failed to start')
        return
    
    # Initiate the backup
    btbackup.stdin.write('backup ' + backup_file_loc + '\n')
    
    # True = online, False = offline
    curr_state = True
    time_left_in_state = get_time_left(curr_state)
    
    # If this loop ever exits, the program has crashed
    # Things that need to happen in this loop:
    # - Send network usage to the syslog-ng server
    # - Wait for a change in state
    # -- If the state counter == 0, flip the state, restart the counter using the exponentiral distribution functions
    while btbackup.poll() == None:
        if curr_state:
            report_network_usage()
        time.sleep(1) # sleep for one second
        # update the state
        if time_left_in_state == 0:
            curr_state = not curr_state
            time_left_in_state = get_time_left(curr_state)
            if not curr_state: # Tell btbackup to exit
                btbackup.stdin.write('exit\n')
            else:
                btbackup = subprocess.Popen(exe_loc,
                                            shell=True,
                                            stdin=subprocess.PIPE)
        else:
            time_left_in_state -= 1

def churntest():
    syslogger('Ran script')
    setup_env(TEST_ROOT, LATEST_PROGRAMS_URL, DEVICE_NAME)
    
    # Wait for the signal to start
    while not test_is_running():
        if DEBUG:
            print('Waiting for the test to start...')
        time.sleep(1)
    
    btbackup_loc = TEST_ROOT + 'p2pbackup/btbackup'
    backup_file_loc = TEST_ROOT + '1G.txt'
    run_test(btbackup_loc, backup_file_loc)

if __name__ == '__main__':
    churntest()

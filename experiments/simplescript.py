#!/usr/bin/python

# All path variables are assumed to be suffixed with a foward slash.

import cleanbt
import datetime
import getpass
import httplib2
import json
import os
import shutil
import subprocess
import sys
import time

TEST_ROOT = '/media/64g/'
BTBACKUP_LOC = TEST_ROOT + '.btbackup/'
P2PBACKUP_LOC = TEST_ROOT + 'p2pbackup/'
BTSYNC_LOC = TEST_ROOT + 'btsync/'
BANDWIDTH = 100 * 1024 # In Kbit/sec, 100 Mbit/sec

def syslogger(msg):
    os.system('logger -p local3.info "' + msg + '"')

def loop_waiting():
    # Do this until the program is terminated
    while True:
        p = subprocess.Popen("sar -n DEV 1 1 | grep eth0 | grep -v Average | awk '{print $1, $2, $6, $7, \"\\n\";}'", shell=True, stdout=subprocess.PIPE)
        output = p.stdout.readline()
        print(output)
        syslogger(output)
        #p.close()

def phase1():
    # Stop any BitTorrent Sync instances
    #os.system('killall btsync')
    
    # Start BitTorrent Sync with the config file
    #os.system(BTSYNC_LOC + 'btsync --config ' + BTSYNC_LOC + 'conf')
    
    # start off clean
    #cleanbt.clean(BTBACKUP_LOC)
    
    #print('Limiting bandwidth. Command:')
    #print('wondershaper eth0 ' + str(BANDWIDTH) + ' ' + str(BANDWIDTH));
    
    # Limit the upload and download bandiwdth
    #os.system('wondershaper eth0 ' + str(BANDWIDTH) + ' ' + str(BANDWIDTH))
    
    # Start btbackup with a command if backing up, and no command
    # if acting as as node.
    if len(sys.argv) > 2 and sys.argv[2] == 'false':
        #os.system('echo "backup ' + TEST_ROOT + '10g.txt" | ' +
        btbackup = subprocess.Popen(P2PBACKUP_LOC + 'btbackup',
                                    shell=True,
                                    stdin=subprocess.PIPE,
                                    stdout=open(P2PBACKUP_LOC + 'phase1.log', 'w+'))
        for x in range(0, 1000):
            btbackup.stdin.write('backup ' + TEST_ROOT + '1M.txt\n')
            time.sleep(1)
    else:
        subprocess.Popen(P2PBACKUP_LOC + 'btbackup > ' + P2PBACKUP_LOC +
                         'phase1.log',
                         shell=True)
    loop_waiting()

def phase2():
    # Save the local_backup_info file in the testing root
    """try:
        shutil.copyfile(BTBACKUP_LOC + 'local_backup_info',
                        TEST_ROOT + 'local_backup_info')
    except:
        x = 1 # Just a filler statement
    
    # Cleanup again
    cleanbt.clean(BTBACKUP_LOC)"""
    
    # Startup BitTorrent Sync again, restoring the previous state
    # Start btbackup with a script input
    subprocess.Popen(P2PBACKUP_LOC + 'btbackup ' + TEST_ROOT +
                     'local_backup_info > ' + P2PBACKUP_LOC + 'phase2.log',
                     shell=True)
    
    loop_waiting()

if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == '1':
        phase1()
    else:
        phase2()

#!/usr/bin/python

import httplib2
import getpass
import json
import shutil

def get_secrets():
    h = httplib2.Http()	
    h.add_credentials('mqp', 'btsync')
    resp, content = h.request("http://127.0.0.1:8888/api?method=get_folders", 
                              "GET", 
                              headers={'content-type':'text/plain'})
    parsed = json.loads(content)
    folders = []
    for f in parsed:
    	folders.append(f['secret'])
    return folders

def remove_folder(secret):
    h = httplib2.Http()
    h.add_credentials('mqp', 'btsync')
    resp, content = h.request("http://127.0.0.1:8888/api?method=remove_folder&secret=" + secret, 
    	  "GET", 
    	  headers={'content-type':'text/plain'})
    return content

def clean(backup_location):
    for secret in get_secrets():
        remove_folder(secret)
    try:
        shutil.rmtree(backup_location)
    except:
        return

if __name__ == '__main__':
    #clean('/home/' + getpass.getuser() + '/.btbackup')
    clean('/media/64g/.btbackup')

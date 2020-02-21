#!/usr/bin/python
# Author: Rob Raniszewski 
# Date Created: 03/17/2019
#
# Last Modified: 03/17/2019 
#
# serverSetup: Sets up wireless communication with ground station via socket
# serverSend: transmits data when connection is established

import socket

s = -1   # just a filler value; will be overwritten

def serverSetup():
  global s
  s = socket.socket()
  host = '192.168.0.101' #ip of raspberry pi  --- fixed in router configuration page --- if connected to the tplink, visit tplinklogin.net or 192.168.0.1 in a web browser and click on DHCP -> Address Reservation to find out the address if you aren't sure what it is.  If necessary, compare MAC address to that for "blueberrypi" in DHCP -> DHCP Clients List .
  port = 12345
  s.bind((host, port))

def serverSend(message):
  global s
  s.listen()
  while True:
    c, addr = s.accept()
    print ('Got connection from',addr)
    c.send(str(message))   ## message may not be a string
    c.send('Thank you for connecting')
    c.close()

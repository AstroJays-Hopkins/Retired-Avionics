#!/usr/bin/python
# Author: Rob Raniszewski 
# Date Created: 03/17/2019
#
# Last Modified: 03/17/2019 
#
# serverSetup: begins wireless communication with ground station via socket
# serverSend: transmits data when connection is established

import socket

def serverSetup():
  s = socket.socket()
  host = '' #ip of raspberry pi
  port = 12345
  s.bind((host, port))

def serverSend():
  s.listen(5)
  while True:
    c, addr = s.accept()
    print ('Got connection from',addr)
    c.send('Thank you for connecting')
    c.close()

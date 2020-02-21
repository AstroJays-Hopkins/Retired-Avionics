import socket               

s = socket.socket()        
host = '192.168.0.101' #ip of raspberry pi  --- fixed in router configuration page --- if connected to the tplink, visit tplinklogin.net or 192.168.0.1 in a web browser and click on DHCP -> Address Reservation to find out the address if you aren't sure what it is.  If necessary, compare MAC address to that for "blueberrypi" in DHCP -> DHCP Clients List .
port = 12345               
s.connect((host, port))
print(s.recv(1024))
s.close()

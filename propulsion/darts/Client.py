import socket               

s = socket.socket()        
host = ''# ip of raspberry pi 
port = 12345               
s.connect((host, port))
print(s.recv(1024))
s.close()

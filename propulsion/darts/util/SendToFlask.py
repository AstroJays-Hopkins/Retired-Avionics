import json
import requests
from random import randint, random

# inputs = [str(randint(0, 40)), str(randint(0,40)), 
# 	    str(randint(450,1100)), str(randint(450,1100)),
# 	    str(randint(0, 90)), str(randint(0,3)),
# 	    str(randint(0, 1)), str(randint(0,1))]
# data = ' '.join(inputs)
# print(data)
# payload = {'data':data}
# requests.post('http://0.0.0.0:5000/loadData',params=payload)

def sendData(data_array):
    requests.post('http://0.0.0.0:5000/loadData', json=data_array)

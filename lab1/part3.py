import os
import json
import sys
import matplotlib.pyplot as plt

def parse_pcap(file_name):
    transmission_stats = []
    # stat = {'pkt_size': 0, 'pkt_count': 0, 'pkt_to_site': 0, 'pkt_to_host': 0}
    # ts = 0
    with open(file_name, 'r') as file:
        for line in file:
            info = line.split()
            tcp_line = map_tcp_info(info)

            if 'from_site' not in tcp_line.keys() or 'flag' not in tcp_line.keys():
                continue

            transmission_stats.append(tcp_line)

            # if 'R' in tcp_line['flag']:
            #     stat['ts'] = info[0]
            #     transmission_stats.append(stat)
            #     stat = {'pkt_size': 0, 'pkt_count': 0, 'pkt_to_site': 0, 'pkt_to_host': 0}

            # stat['pkt_size'] += tcp_line['length']
            # stat['pkt_count'] += 1
            # stat['pkt_to_site'] += 1 if tcp_line['from_site'] else 0
            # stat['pkt_to_host'] += 0 if tcp_line['from_site'] else 1
            # ts = info[0]

        # stat['ts'] = ts
        # transmission_stats.append(stat)

    result_file = file_name.split('.')[0]
    with open(result_file + '_result.txt', 'w') as result_file:
        for i in transmission_stats:
            print(json.dumps(i), file=result_file)

def map_tcp_info(info):
    tcp_map = {}
    prev_token = ''
    for field in info:
        field = field.strip()

        if field[-1] == ':' or field[-1] == ',':
            field = field[:-1]

        if prev_token == 'IP':
            tcp_map['src_ip'] = field
            if 'https' in field:
                tcp_map['from_site'] = True
        elif prev_token == '>':
            tcp_map['dst_ip'] = field.split(':')[0]
            if 'https' in field:
                tcp_map['from_site'] = False
        elif prev_token == 'Flags':
            tcp_map['flag'] = field
        elif prev_token == 'length':
            tcp_map['length'] = int(field)

        prev_token = field
    
    return tcp_map

# def plot_info(data):


if __name__ == '__main__':
    # folder = 'default_browser'
    folder = sys.argv[1]

    file_list = os.listdir(folder)

    for file in file_list:
        if 'pcap' in file:
            continue

        parse_pcap(os.path.join(folder, file))
        print(f'done {file}')
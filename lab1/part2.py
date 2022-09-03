from ipwhois import IPWhois
from pprint import pprint

def aggregate_ip_networks(file_name):
    # mappings = {}
    name_networks = {}
    with open(file_name) as file:
        for ip in file:
            results = None
            try:
                ip = ip.strip()
                obj = IPWhois(ip)
                results = obj.lookup_rdap(depth=1)
            except:
                print(f'error with ip {ip}')
                continue
            
            if results is None:
                continue

            # cidr = ip[:ip.rfind('.')]

            # cidr = results['network']['cidr']
            name = results['network']['name']

            # if cidr not in mappings.keys():
            #     mappings[cidr] = []

            # mappings[cidr].append(ip)

            if name not in name_networks.keys():
                name_networks[name] = []

            name_networks[name].append(ip)


    # with open('network_map', 'w') as network_map:
    #     pprint(mappings, network_map)
    
    with open('name_network_map', 'w') as name_file:
        pprint(name_networks, name_file)

if __name__ == '__main__':
    aggregate_ip_networks('results.csv')
import os
import subprocess

def create_client():
#    os.system('./client')
    subprocess.run('./client', shell=True)

print('Starting DOS attack')

for i in range(100):
    create_client()

print('Completed DOS attack')


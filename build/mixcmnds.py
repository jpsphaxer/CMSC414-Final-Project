import os
import time

os.system('./atm -n 100.00 -a alice -i 127.0.0.1 -p 3000')
os.system('./atm -p 3000 -d 100.00 -a alice ')
os.system('./atm -g -a alice ')
os.system('./atm -w 50.00 -a alice -c alice.card ')
os.system('./atm -a alice -g')
time.sleep(1)
os.system('./atm  -a bob -n 15000.00 -i 127.0.0.1 -p 3000')
os.system('./atm -i 127.0.0.1 -a bob -d 15000.00 -p 3000 ')
os.system('./atm -a bob  -i 127.0.0.1 -p 3000 -g')
os.system('./atm -w 5000.00 -i 127.0.0.1 -p 3000 -a bob ')
os.system('./atm -i 127.0.0.1 -a bob -p 3000  -g ')
#time.sleep(1)

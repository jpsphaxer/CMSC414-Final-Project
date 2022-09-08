import os
import time

os.system('./atm -a alice -n 100.00')
os.system('./atm -a alice -d 100.00')
os.system('./atm -a alice -g')
os.system('./atm -a alice -w 50.00')
os.system('./atm -a alice -g')
time.sleep(1)
os.system('./atm -i 127.0.0.1 -p 3000 -a bob -n 15000.00')
os.system('./atm -i 127.0.0.1 -p 3000 -a bob -d 15000.00')
os.system('./atm -i 127.0.0.1 -p 3000 -a bob -g')
os.system('./atm -i 127.0.0.1 -p 3000 -a bob -w 5000.00')
os.system('./atm -i 127.0.0.1 -p 3000 -a bob -g ')
#time.sleep(1)
os.system('./atm -i 127.0.0.1 -p 3000 -c proj4.card -a jac -n 100000.00')
os.system('./atm -i 127.0.0.1 -p 3000 -c proj4.card -a jac -d 100000.00')
os.system('./atm -i 127.0.0.1 -p 3000 -c proj4.card -a jac -g')
os.system('./atm -i 127.0.0.1 -p 3000 -c proj4.card -a jac -w 50000.00')
os.system('echo $?')
time.sleep(1)
os.system('./atm -i 127.0.0.1 -p 3000 -c dave.card -a dave -n 100000.00')
os.system('./atm -i 127.0.0.1 -p 3000 -a dave -d 4294967295.99')
os.system('./atm -i 127.0.0.1 -p 3000 -a dave -g')
os.system('./atm -i 127.0.0.1 -p 3000 -a dave -w 0.99')
os.system('./atm -i 127.0.0.1 -p 3000 -a dave -g')







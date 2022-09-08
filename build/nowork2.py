import os



os.system('./atm -i 127.0.0.1 -p 3000 -c dave.card -a dave -n 100000.00')#ok

os.system('./atm -i 127.0.0.1 -p 3000 -a dave -d 4294967296.99') #over limit 

os.system('./atm -i 127.0.0.1 -p 3000 -a dave -g')#ok
os.system('./atm -i 127.0.0.1 -p 3000 -a dave -w 0.99')#ok
os.system('./atm -i 127.0.0.1 -p 3000 -s noexist.auth -a dave -g') #authfile doesnt exist
os.system('./atm -i 127.02.04.1 -p 3010 -a bob -d 15000.00')#wrong ip
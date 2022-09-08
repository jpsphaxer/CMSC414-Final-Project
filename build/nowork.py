import os
import time

os.system('./atm -a alice -n 100.00')#ok 
os.system('./atm -a alice -d -100.00') #neg amnt
os.system('./atm -a alice -g') #ok
os.system('./atm -a alice -w 50.00 -g -g ') #dup cmnd
os.system('./atm -g') #255
time.sleep(1)
os.system('./atm ')
os.system('./atm -a bob -n 100.00') #ok
os.system('./atm -i 127.0.0.1 -p 3000 -c alice.card -a bob -g') #wrong card
os.system('./atm -i 127.0.0.1 -p 3000 -a bob -w 5000.00') #bal below zero 255
os.system('./atm -i 127.0.0.1 -p 3000 -a bob -g ')#ok
time.sleep(1)



os.system('./atm -a blah -n 5.00') # less than 10 to make acc
import os
import time
# JULIO's EXAMPLES (CW POST 973)
os.system('./atm -p 3000 -i 127.0.0.1 -a chris -d 4294967295.99')
os.system('echo $?')
os.system('./atm -p 3000 -i 127.0.0.1 -a aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa -n 10.01')
os.system('echo $?')

os.system('./atm -p 3000 -i 127.0.0.1 -a ted -d 0.00')
os.system('echo $?')

os.system('./atm -p 3000 -i 127.0.0.1 -a chrisasdf -d 0.00')
os.system('echo $?')

os.system('./atm -p 3000 -i 127.0.0.1 -a ted -g')
os.system('echo $?')

os.system('./atm  -p 3000 -i 127.0.0.1 -a joe -g -g')
os.system('echo $?')

os.system('./atm -p 3000 -i 127.0.0.1 -a alice -c ted.card -d 21.0')
os.system('echo $?')

os.system('./atm -p 3000 -i 127.0.0.1 -a whjdpkvzdv -w 1357411.71')
os.system('echo $?')

os.system('./atm -p 3000 -i 127.0.0.1 -a e61ghehtsg -d 77903.85')
os.system('echo $?')
import serial
import time

#Open com, zet goeie com hier neer
ser = serial.Serial('COM6', 19200)

i=0

print(ser)
time.sleep(4)
t=0
file1 = open("Temperatuur.txt", "a")
file1.truncate(0)# Zorgt ervoor dat Temperatuur.txt leeg wordt
file1.close()

file2 = open("Licht.txt", "a")
file2.truncate(0) # Zorgt ervoor dat Licht.txt leeg wordt
file2.close()


while True:

    firstBitcheck = ser.read()
    j = firstBitcheck.hex()
    checker = int(j, 16)
    
    if checker == 1:
        time.sleep(1)
        tempRead = ser.read()
        
        #Write naar Temperatuur.txt
        file1 = open("Temperatuur.txt", "a")
       
        temp = int(tempRead.hex(), 16)
        print("temp")
        print(temp)
        
        
        #s = "FFBB"
        #print("temp" + s[0:2]) Temp is de eerste twee die binnenkomen vanaf de Arduino
        #print("licht" + s[2:4])    Licht zijn de laatste twee die binnenkomen vanaf de Arduino
        #Write naar temperatuur.txt
        file1.write(str(t)+ "," + str(temp) + "\n")
        
        file1.close()
     
        
    elif checker == 2:
        time.sleep(1)
        lichtRead = ser.read()
        
        file2 = open("Licht.txt", "a")
        licht = int(lichtRead.hex(), 16)
        file2.write(str(t)+ "," + str(licht) + "\n")
        print("Licht:")
        print(licht)
        file2.close()
        
    else:
        time.sleep(1)
	
    pullData = open("Afstand.txt", "a")
    file = open("Afstand.txt", "r").read()
    if file != "":
        data = file.encode()
        ser.write(data)
        pullData.truncate(0)
        print(data)
	
    t += 1
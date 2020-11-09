import serial
import time

#Open com, zet goeie com hier neer
ser = serial.Serial('COM4', 19200)

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
    #Write naar Temperatuur.txt
    file1 = open("Temperatuur.txt", "a")
    ser.write(5)
    val = ser.read()
    time.sleep(1)
    val2 = ser.read()
    s = val.hex()
    i = int(s, 16)
    print("temp")
    print(i)
    
    
    #s = "FFBB"
    #print("temp" + s[0:2]) Temp is de eerste twee die binnenkomen vanaf de Arduino
    #print("licht" + s[2:4])    Licht zijn de laatste twee die binnenkomen vanaf de Arduino
    #Write naar temperatuur.txt
    file1.write(str(t)+ "," + str(i) + "\n")
 
    #Write naar licht.txt
    file2 = open("Licht.txt", "a")
    ser.write(5)
    p = val2.hex()
    h = int(p, 16)
    file2.write(str(t)+ "," + str(h) + "\n")
    print("Licht:")
    print(h)
    
    t += 1
    file1.close()
    time.sleep(40) #Moet op 40 sec zodat hij iedere 40 sec iets op de grafiek zet.
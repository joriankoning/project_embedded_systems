import serial
import time

ser = serial.Serial('COM4', 19200)

i=0

print(ser)
time.sleep(4)
t=0
file1 = open("Temperatuur.txt", "a")
file1.truncate(0)
file1.close()

file2 = open("Licht.txt", "a")
file2.truncate(0)
file2.close()
while True:
    
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
    
    file1.write(str(t)+ "," + str(i) + "\n")
 
    
    file2 = open("Licht.txt", "a")
    ser.write(5)
    p = val2.hex()
    h = int(p, 16)
    file2.write(str(t)+ "," + str(h) + "\n")
    print("licht:")
    print(h)
    
    t += 1
    file1.close()
    time.sleep(2) #Moet op 40 sec
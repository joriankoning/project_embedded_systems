import serial
import time

ser = serial.Serial('COM4', 19200)



i=0

print(ser)
time.sleep(4)
x=[]
t=0
while True:
    file1 = open("Temperatuur.txt", "a")
    ser.write(5)
    val = ser.read()
    s = val.hex()
    i = int(s, 16)

    print(i)
    x.append(t)
    print(t)

    file1.write(str(t)+ "," + str(i) + "\n")
    t += 1
    file1.close()
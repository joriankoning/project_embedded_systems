import serial
import time

ser = serial.Serial('COM5', 19200)

file1 = open("Temperatuur.txt", "a")

i=0

print(ser)
time.sleep(4)
x=[]
t=0
while True:
    ser.write(5)
    val = ser.read()
    s = val.hex()
    i = int(s, 16)

    print(i)
    x.append(t)
    print(t)

    file1.write(str(t))
    t += 1
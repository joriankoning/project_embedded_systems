import serial
import time

#Open com, zet goeie com hier neer
ser = serial.Serial('COM6', 19200)
#maakt een waarde voor de x as
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
    #haalt een waarde op van de arduino
    firstBitcheck = ser.read()
    j = firstBitcheck.hex()
    checker = int(j, 16)
    #controlleert of de volgende waaarde een 1 is zodat het programma weet dat er een nieuwe loop is
    if checker == 1:
        time.sleep(1)
        tempRead = ser.read()
        
        #Write naar Temperatuur.txt
        file1 = open("Temperatuur.txt", "a")
       
        temp = int(tempRead.hex(), 16) #Hex omzetten
        print("temp")# Waarde laten zien die Temperatuur stuurt
        print(temp)
        
        
      
        #Write naar temperatuur.txt
        file1.write(str(t)+ "," + str(temp) + "\n")
        
        file1.close()
     
        
    elif checker == 2: # Als de waarde 2 gestuurd wordt dan wordt de licht waarde gestuurd vanaf de arduino
        time.sleep(1)
        lichtRead = ser.read()

        #schrijft naar licht.txt
        file2 = open("Licht.txt", "a")
        licht = int(lichtRead.hex(), 16)#Hex omzetten
        file2.write(str(t)+ "," + str(licht) + "\n") # Toevoegen aan licht.txt
        print("Licht:") # Waarde laten zien die lichtsensor stuurt
        print(licht)
        file2.close()
        
    else:
        time.sleep(1)
	#leest uit afstand.txt zodat er geschreven kan worden naar de arduino
    pullData = open("Afstand.txt", "a")
    file = open("Afstand.txt", "r").read()
    if file != "":
        data = file.encode()
        ser.write(data)
        pullData.truncate(0)
        print(data)

    t += 1
import serial
from time import sleep
ser = serial.Serial('COM12')

#line = ser.readline()
#print "Valor lido: " + line + "\n" 

leituras = []
print "Enviando 'envie'..."
ser.write("envie")
print "...enviado"
a = ser.readline()
c, d = a.split(" ")

e = 2*int(d)
for i in range(e):
	valor = ser.readline()
	print valor
	leituras.append(valor)
print "T"
print leituras
print "Tamanho: ", d
print "QNT ", len(leituras)
ser.close()

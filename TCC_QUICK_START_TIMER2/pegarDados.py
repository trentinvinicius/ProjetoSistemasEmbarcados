import sys
import serial
import numpy as np
import os

class PegarDados(object):
   '''
   Funcao que recebe os dados armazenados na memoria EEPROM da placa SAM D21
   e gera um arquivo com esses dados
   '''

   def __init__(self, arquivo, porta):


      self.ser = serial.Serial(porta, 9600, timeout = 1) ## estabelece a conexao com a porta  serial informada e define o timeout em 1 segundo

      self.arquivo = arquivo  ## nome do arquivo que sera gerado

      self.lista = []
      self.pwm = []
      self.tempo = []
      self.pegar()

   def pegar(self):

         self.ser.write("envie")  ## envia a palavra "envie" que solicita ao uC o envio das informacoes de luminosidade e tempo armazenadas
         msg = self.ser.readline()
         if (msg == "Dados lidos:\n"): ## cararcteriza o inicio da transmissao dos dados
            i = 0 ## indice para separar as informacoes
            while (msg != "fim"):  ## enquanto a mensagem recebi nao for "fim" le a serial
               msg = self.ser.readline()
               if (msg == ""):
                  print "Nenhuma informacao foi recebida"
                  break
               if (msg == "fim"):
                  break
               if (msg == "\n"):
                  pass
               else:
                  valor = msg.split("\n");
                  if (i%2==0):
                     self.pwm.append(valor[0])
                  else:
                    self.tempo.append(valor[0])
                  i += 1                     
               self.lista.append(msg)
            self.gravar()
         else:
            print "Desculpe, nao foi possivel receber os dados"

   def gravar(self):
      try:
         if (self.pwm[0] == '0' and self.tempo[0] == '0'):
            self.pwm.pop(0)
            self.tempo.pop(0)
            self.lista.pop(0)
            self.lista.pop(1)
         np.save(arquivo, self.lista)  ## grava o arquivo com o nome informado
      except:
         pass

if __name__ == '__main__':
   '''
   Para inicilizar o programa deve-se informar o nome do arquivo que sera gerado e a porta serial em que o uC esta conectado
   '''
   os.system('cls') #windows
#   os.system('clear') #linux
   if (len(sys.argv) == 3):
      arquivo = sys.argv[1]
      porta = sys.argv[2]
      tipo = arquivo.split(".")
      if (tipo[1] == "npy"):
         p = PegarDados(arquivo, porta)
         print "PWM  TEMPO"
         for i in range(len(p.pwm)):
            print p.pwm[i] + "    " + p.tempo[i]
      else:
         print "Nome do arquivo invalido"
   else:
      print "Por favor insira o nome do arquivo onde os dados devem ser salvos e a porta serial em que a placa esta conectada."

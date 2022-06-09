# TCP-client-server
Laboratorio 1 Redes de computadores

## Dependencias
1. libsodium
2. cmake

## Instrucciones compilacion:
1. mkdir build 
2. cd build
3. cmake ..
4. make

## Modo de Uso

### Generar clave secreta
1. ./keygen
 
### Iniciar servidor
1. ./server \[opciones\]
  Opciones:
  1. -k : Entregar el nombre del archivo con la llave secreta
  2. -p : Entregar el puerto donde se abrira el servidor
  
2. ./client <ip> <archivo> \[opciones\]
  ip: Direccion ipv4 del servidor
  archivo : Direccion del archivo que se desea enviar
  
  Opciones:
  1. -k : Entregar el nombre del archivo con la llave secreta
  2. -p : Entregar el puerto del servidor al que se conectara
  3. -u : Enviar archivo sin encriptar

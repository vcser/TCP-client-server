# TCP-client-server
Redes de Computadores – Laboratorio 1: Transferencia de archivos por TCP  
  
**Integrantes:**
- Giorgio Riffo
- Vicente Cser

## Dependencias
- libsodium
- cmake

## Instrucciones compilacion
```
mkdir build 
cd build
cmake ..
make
```

## Modo de Uso

### Generar clave secreta
```
./keygen
```
 
### Iniciar servidor
```
./server [opciones]
```
  Opciones:
  - -k : Entregar el nombre del archivo con la llave secreta
  - -p : Entregar el puerto donde se abrira el servidor

### Conectarse al servidor
```
./client <ip> <archivo> [opciones]
```
  ip: Direccion ipv4 del servidor  
  archivo : Direccion del archivo que se desea enviar  
  Opciones:
  - -k : Entregar el nombre del archivo con la llave secreta
  - -p : Entregar el puerto del servidor al que se conectara
  - -u : Enviar archivo sin encriptar

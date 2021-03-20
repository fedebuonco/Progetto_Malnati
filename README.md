# Progetto Backup 

Progetto Backup per il corso di Programmazione di Sistema.  
Composto da Client e Server. 

// TODO IMMAGINE + VELOCE RECAP. 

## Usage
Clona la repo e da terminale fai cd su server/client.
Si compilano allo stesso modo.
```bash
mkdir build && cd build
cmake ..
make 
```
Verrà generato un eseguibile (server/server.exe o client/client.exe).

### Client

* ```-c , --credential``` Specify the user credential.  
Example: ```-c username password```


* ```-f,--folder``` Specify the backup folder

* ``` -s,--server``` Specify the server address
* ``` -h,--help``` Show this help message\n"
* ``` -d,--debug``` Show the debug messages during the execution
### Server
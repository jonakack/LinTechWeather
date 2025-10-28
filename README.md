
# LinTechWeather README

_______________________________________________________________________

# MAKEFILE GUIDE

The Makefile in the root folder handles both the server- and client
makefiles. Below are some of the most common commands: 

make                - Build both server and client
make server         - Build just server  
make client         - Build just client
make run            - Build and run both (server -> client -> cleanup)
make run-server     - Just run server
make run-client     - Just run client
make clean          - Clean all build files

_______________________________________________________________________

# TCP FLOWCHART

      SERVER SIDE                           CLIENT SIDE
   ────────────────────────             ────────────────────────
   socket()                           → socket()
   bind()                             → 
   listen()                           → 
   accept() ←─────────────── connect()
   read()/write()  ⇄  write()/read()
   close()                           → close()

_______________________________________________________________________

# HTTP FLOWCHART

       SERVER SIDE                              CLIENT SIDE
   ────────────────────────              ────────────────────────
   socket()                            → socket()
   bind()                              → 
   listen()                            → 
   accept()  ←────────────── connect()
   recv() (HTTP request)  ←── send() (HTTP request)
   send() (HTTP response)  ─→ recv() (HTTP response)
   close()                             → close()

_______________________________________________________________________
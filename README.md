
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

main.c → http.c → tcp.c → Network
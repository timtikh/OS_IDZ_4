gcc UDPLib.c Client.c -o Client
gcc UDPLib.c Server.c -o Server
./Server 5000 5

./Client 127.0.0.1 5000 1
./Client 127.0.0.1 5000 2
./Client 127.0.0.1 5000 3
./Client 127.0.0.1 5000 4




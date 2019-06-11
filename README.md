# python-Qt_cpp-socket-communication
Socket communication between a python server and a Qt-cpp client

## Discovery  
The discovery class extends QUdpSocket and waits for any incoming broadcast communication. Every time a new message is received the function `ipIsInList()` check if the ip of the sender is already in the list of devices; if that is not the case, the incoming json is parsed and if its structure matches the known one (i.e. if the field "publisher" is equal to "Specim" and the json has a "services" field) than the ip is added to the list of devices.  

`QMap<QString, QMap<QString, QString>> m_addressesList`: a list of devices. The list has the following structure:  
```
"192.168.0.171":
	{ "portFTP": "21",
  	  "portRPC": "34653" },
"192.168.0.111":
	{ "portFTP": "21",
  	  "portRPC": "34653" },
...
```

`void discovery(quint16 port)`: listen to every incoming communication at port "port".

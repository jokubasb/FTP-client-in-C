Baby FTP Server by Pablo Software Solutions 1.24

I get a lot of request for a simple FTP server, without a facny UI or lots of features.
This baby FTP server, has only the most necesarry features and is yet powerfull enough to be a basis for a more complex server.

Features

Supports most RFC959 FTP commands
Supports PASV and non-PASV mode
Only allows anonymous connections
Multi threaded
Real time server log
Configure home directory (same for all connections)
Set permissions for download/upload/rename/delete/create directory

The server has only 4 classes (plus one UI class):

FTP Server classes:
CListenSocket
This socket accepts all incoming connections. 
When a client connects to the server, CListenSocket accepts the connection and creates a new thread (CClientThread) that will take care of all further communication between the client and the server. After the thread has been created, CListenSocket will return to its waiting state.

CClientThread
This thread will handle all communication between the client and the server using CControlSocket.

CControlSocket
This socket class will process all incoming FTP commands and send back the response to the client.

CDataSocket
When data needs to be send or received, a CDataSocket will be created by CControlSocket. 
The CDataSocket class will transfer this data (such as directory listings and files) on a separate port.

User Interface class:
CServerDlg
Displays the server's status and allows you to configure some basic features:

Home directory
Permissions for download/upload/rename/delete/create directory.

Copyright Pablo Software Solutions 
http://www.pablovandermeer.nl

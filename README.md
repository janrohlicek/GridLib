# GridLib

Simple tool using wxWidgets for communication between two PCs
The aim is that there is a list of jobs that need to be proceeded and there are several PCs in the same network that can make the work.

Master
- is supposed to store a list of jobs
- several clients can connect to the server
  
Slave
- is supposed to make the work and send it back
- one client can be connected just to one server
  

The communication between them uses wxSocketClient and wxSocketServer
Slave and Master, they both have client and server running. Server is a thread expecting incomming messages and client is for sending messages

The currrent communication allows sending just text (wxString) data, but it can be simply changed to sending also the binary data, look at class GridCommunication
The current communication uses GridCommunication::SendData2 and GridCommunication::ReceiveData2. These functions confirms the recieving of the message.

Look in MyGridMaster.cpp and MyGridSlave.cpp to see and example of the communication.


Example for slave:
Use this to get all received messages
GridSlaveBase::getReceivedMessages()

And use this to send messages
GridSlaveBase::sendMessage(wxString msg, long long msgID)

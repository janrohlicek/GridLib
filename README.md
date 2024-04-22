# GridLib

Simple tool using wxWidgets (3.1.7) for communication between two PCs
The purpose is to involve other PCs in the same network in the calculation of demanding tasks. In other words, the purpose is to enable the distribution of computational tasks over the network.

Master
- is supposed to store a list of jobs
- several clients can connect to the master
  
Slave
- is supposed to make the work and send it back to master
- one client can be connected just to one master
- one IP address allows just one slave
  

The communication between master and slaves uses wxSocketClient and wxSocketServer
Slave and Master, they both have client and server running. Server is a thread expecting incomming messages and client is for sending messages. 

The currrent communication allows sending just text (wxString) data, but it can be simply changed for sending also the binary data, look at class GridCommunication.
Now, it uses GridCommunication::SendData2 and GridCommunication::ReceiveData2. These functions confirms the recieving of the message.

Look in MyGridMaster.cpp and MyGridSlave.cpp to see an example of the communication and how to use this simple code.


Example for slave:
Use this to get all received messages
GridSlaveBase::getReceivedMessages()

And use this to send messages
GridSlaveBase::sendMessage(wxString msg, long long msgID)

On Master:
GridMasterBase::getReceivedMsgs()
and
SendMsgToSlave(wxString msg, wxString slaveIP, long long msgID, bool refresh_slave_list)

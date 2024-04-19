#include "SocketThreadServer.h"

SocketThreadServer::SocketThreadServer(  wxSocketBase         *pSocket, 
                                         wxString              workingDir, 
                                         wxEvtHandler         *parent,
                                         wxIPV4address         address)
:wxThread(wxTHREAD_JOINABLE)
{
    m_socket = pSocket;
    //Notify() cannot be called in thread context. We have to detach from main loop
    //before switching thread contexts.
    m_socket->Notify(false);
    m_socket->SetFlags(wxSOCKET_WAITALL|wxSOCKET_BLOCK);
    //pSocket->GetPeer(m_peer);
    m_working_dir = workingDir;
    m_parent = parent;
    m_address = address;
    m_socket_error = false;
}
long SocketThreadServer::GetId()
{
   return wxThread::GetId();
}
wxIPV4address SocketThreadServer::GetAddress()
{
    return m_address;
}
wxThread::ExitCode SocketThreadServer::Entry()
{
    WriteLogMessage("Thread Entry");    
    while (!TestDestroy()) { 
        if(!m_socket->IsConnected()) {
            break;
        }
        //WriteLogMessage("looping...", "Thread_log.txt");        
        wxThread::Yield(); // this seems to be important before WaitForRead()
        if(m_socket->WaitForRead(1, 0)) {

            WriteLogMessage("Something to read...");
            ReadSocket();
        }
        if(m_socket_error) {
            break;
        }
    }
   
    m_socket->Destroy();
    m_socket = 0;
    WriteLogMessage("Leaving thread");
    return 0;
}
void SocketThreadServer::WriteLogMessage(wxString msg)
{
   wxString name;
#ifdef WIN32
   name = GetWorkingDir() + _T("\\") + "Thread_server_" + to_string(GetId())+".txt";
#else
   name = GetWorkingDir() + _T("/") + filename;
#endif
   wxFile logfile(name, wxFile::write_append);
   if(logfile.IsOpened())
   {
      wxDateTime datetime = wxDateTime::Now();
      logfile.Write(datetime.Format(_T("%X ")) + msg + _T("\n"));
      logfile.Close();
   }  
}
bool SocketThreadServer::ReadSocket()
{
    WriteLogMessage("ReadSocket() start");

    MSGINFO_REC mi_rec;
    vector<char> data;
    short er = ReceiveData2(m_socket, mi_rec.ID, data, true);
    if(er!=0) {
        WriteLogMessage("ERROR: ReadData returns with error code: " + to_string(er));
        m_socket_error = true;
        return false;
    }
    mi_rec.msg = wxString::FromAscii(data.data());
    WriteLogMessage("msg.ID="+to_string(mi_rec.ID)+" saving new message (len="+to_string(mi_rec.msg.length())+")");
    mi_rec.recieved = getTimeStampMinutes();
    //mi_rec.socketID = GetId();
    mi_rec.IP = GetAddress().IPAddress();
    
    wxMutexLocker locker(m_messages_in_mutex);
    if (!locker.IsOk()) {
        WriteLogMessage("ERROR: getReceivedMessages() - !locker.IsOk()");
        return false;
    }
    m_messages_in.push_back(mi_rec);

    WriteLogMessage("ReadSocket() end");

    return true;
}
vector<GridCommunication::MSGINFO_REC> SocketThreadServer::getReceivedMessages()
{// returns non-processed mesgs and set them as processed

    vector<MSGINFO_REC> res;
    wxMutexLocker locker(m_messages_in_mutex);
    if (!locker.IsOk()) {
        WriteLogMessage("ERROR: getReceivedMessages() - !locker.IsOk()");
        return res;
    }

    for(int i=0;i<m_messages_in.size();i++) {
        if(m_messages_in[i].processed==0) {
            res.push_back(m_messages_in[i]);
            m_messages_in[i].processed = getTimeStampMinutes();
        }
    }

    //removing processed msgs from the m_messages_in field
    m_messages_in.erase(std::remove_if(m_messages_in.begin(), m_messages_in.end(),
                        [](const MSGINFO_REC& msg) { return msg.processed != 0; }),
                        m_messages_in.end());
    
    return res;    
}


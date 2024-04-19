#ifndef __SOCKETTHREADSERVER_H // Make sure to only declare these classes once
#define __SOCKETTHREADSERVER_H

#include <wx/thread.h>
#include "GridCommunication.h"
#include <wx/frame.h>

class SocketThreadServer : public wxThread, GridCommunication
{
    public:
        SocketThreadServer(wxSocketBase         *pSocket, 
                     wxString              workingDir, 
                     wxEvtHandler         *parent,
                     wxIPV4address         address);
        virtual ExitCode Entry();
        
        vector<GridCommunication::MSGINFO_REC> getReceivedMessages();
        long GetId();
        wxIPV4address GetAddress();
        //wxSocketBase *getSocket();

    private:
        void WriteLogMessage(wxString msg);
        bool ReadSocket();

        vector<MSGINFO_REC>  m_messages_in; 
        wxMutex              m_messages_in_mutex;
        wxSocketBase        *m_socket;
        wxEvtHandler             *m_parent;
        wxIPV4address        m_address;
        bool                 m_socket_error;     
};
#endif


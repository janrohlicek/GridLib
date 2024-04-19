#ifndef __GRIDCLIENT_H // Make sure to only declare these classes once
#define __GRIDCLIENT_H

#include <wx/frame.h>
#include <wx/socket.h>
#include <wx/file.h>
#include "wx/timer.h"
#include "GridCommunication.h"
#include <vector>
#include <algorithm>
#include <wx/thread.h>
#include "SocketThreadClient.h"



class GridClient: public wxEvtHandler, GridCommunication
{
   
    public:
        GridClient(wxString working_dir, wxString IP);
        ~GridClient();

        bool ConnectClient(int nbOfTrial, wxString hostname, unsigned short port);
        void OnSocketEvent(wxSocketEvent &event);
        void OnTimerEvent(wxTimerEvent& event);
        vector<MSGINFO_SENT> getCopyMsgsToBeSent();
        bool getCopyMsgToBeSent(long long msgID, MSGINFO_SENT &msg);        

        wxString GetIP();
        void setAutoReconnectWhenConnectionLost(bool reconnect);

        //vector<wxString> getReceivedMsgsUnprocessed();

        //returns the unique ID of the message
        long long addMessageToSend(wxString msg, long long msgID);
        bool isConnected();

        //wxString GetWorkingDir();

        void printMsgs();
        void Delete();

    protected:
        //void WriteLogMessage(wxString msg);
        //void CheckMessagesToBeSent();
        //void CleanMessageLists();
        void refreshClientThreadState();
        void CleanMessagesToBeSent();
        void DeleteMyself();
        //void Disconnect();

        wxString                    m_hostname;
        vector<MSGINFO_SENT>        m_messages_to_be_send;
        unsigned int                m_keep_at_least_last_m_messages_to_be_send;
        unsigned int                m_keep_minutes_m_messages_to_be_send;
        wxTimer                   * m_sendingTimer;
        wxMutex                     m_messages_to_be_send_mutex;
        bool                        m_need_socket_connect;
        bool                        m_reconnect_when_connection_lost;
        unsigned short              m_port;

        SocketThreadClient         *m_socket_thread_client;        
        wxMutex                     m_socket_thread_client_mutex;
        wxString                    m_IP;
        bool                        m_delete;

        DECLARE_EVENT_TABLE()

};


#endif


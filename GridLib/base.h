 #ifndef __BASE_H // Make sure to only declare these classes once
#define __BASE_H
#include <wx/frame.h>
#include <wx/textctrl.h>
#include "MyGridSlave.h"
#include "MyGridMaster.h"
#include <random>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

class MainApp: public wxApp // MainApp is the class for our application
{ // MainApp just acts as a container for the window,
    public: // or frame in MainFrame
    virtual bool OnInit();
};

class MainFrame: public wxFrame // MainFrame is the class for our window,
{ // It contains the window and all objects in it
    public:
    MainFrame( const wxString& title, const wxPoint& pos, const wxSize& size );
    wxTextCtrl *MainEditBox;
    wxMenuBar *MainMenu;
    void Quit(wxCommandEvent& event);
    void RunMaster(wxCommandEvent& event);
    void RunSlave(wxCommandEvent& event);
    void RunNetstat(wxCommandEvent& event);
    void OnMasterTimerEvent(wxTimerEvent& event);
    void OnSlaveTimerEvent(wxTimerEvent& event);
    wxString GenerateRandomText(size_t length);

    wxString printSlaveState();
    wxString printMasterState();

    MyGridMaster *m_grid_master;
    MyGridSlave  *m_grid_slave;
    wxTimer * m_MasterTimer;
    wxTimer * m_slaveTimer;
    
    DECLARE_EVENT_TABLE()
};

enum
{
    TEXT_Main = wxID_HIGHEST + 1, // declares an id which will be used to call our button
    MENU_MASTER,
    MENU_SLAVE,
    MENU_NETSTAT,
    MENU_Quit,
    MY_MASTER_TIMER_EVT,
    MY_SLAVE_TIMER_EVT
};

#endif
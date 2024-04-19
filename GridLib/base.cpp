#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "base.h"

BEGIN_EVENT_TABLE ( MainFrame, wxFrame )
EVT_MENU(MENU_MASTER, MainFrame::RunMaster)
EVT_MENU(MENU_SLAVE, MainFrame::RunSlave)
EVT_MENU(MENU_NETSTAT, MainFrame::RunNetstat)
EVT_MENU(MENU_Quit, MainFrame::Quit)
EVT_TIMER(MY_MASTER_TIMER_EVT, MainFrame::OnMasterTimerEvent)
EVT_TIMER(MY_SLAVE_TIMER_EVT, MainFrame::OnSlaveTimerEvent)
END_EVENT_TABLE()


IMPLEMENT_APP(MainApp) // Initializes the MainApp class and tells our program
// to run it
bool MainApp::OnInit()
{
    wxSocketBase::Initialize();
    MainFrame *MainWin = new MainFrame(wxT("Hello World!"), wxPoint(1,1),
    wxDefaultSize);//wxSize(300, 200)); // Create an instance of our frame, or window
    MainWin->Show(TRUE); // show the window
    SetTopWindow(MainWin);// and finally, set it as the main window

    return TRUE;
}

MainFrame::MainFrame(const wxString& title,
const wxPoint& pos, const wxSize& size)
    : wxFrame((wxFrame *) NULL, -1, title, pos, size)
{
    CreateStatusBar(2);
    MainMenu = new wxMenuBar();
    wxMenu *FileMenu = new wxMenu();

    FileMenu->Append(MENU_MASTER, wxT("&Run Master"),
        wxT("Start server"));
    FileMenu->Append(MENU_SLAVE, wxT("&Run Slave"),
        wxT("Start Client"));    
    FileMenu->Append(MENU_NETSTAT, wxT("&Run netstat"),
        wxT("Start Client"));  
    FileMenu->Append(MENU_Quit, wxT("&Quit"),
        wxT("Quit the editor"));

    MainMenu->Append(FileMenu, wxT("File"));
    SetMenuBar(MainMenu);

    MainEditBox = new wxTextCtrl(this, TEXT_Main,
        wxT("localhost"), wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_RICH , wxDefaultValidator, wxTextCtrlNameStr);
        Maximize();
    
    m_grid_slave = NULL;
    m_grid_master = NULL;
    m_MasterTimer = NULL;
    m_slaveTimer = NULL;
}
void MainFrame::OnMasterTimerEvent(wxTimerEvent& event)
{
    /*
    wxString randomText = GenerateRandomText(400000);
    if(m_grid_master==NULL) return;
    m_grid_master->addJob(randomText, 1234, 100);
    */
    MainEditBox->SetValue(printMasterState());
    m_MasterTimer->Start(10000, true);
}
void MainFrame::OnSlaveTimerEvent(wxTimerEvent& event)
{
    wxString randomText = GenerateRandomText(400000);
       
    if(m_grid_slave==NULL) return;
    m_grid_slave->SendResult(randomText, m_slaveTimer->GetInterval()/1000, 1, 1);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.1, 5.0);
    int duration = dis(gen);
    m_slaveTimer->Start(duration*1000, true);
    
    MainEditBox->SetValue(printSlaveState()); 
}
void MainFrame::RunMaster(wxCommandEvent& WXUNUSED(event))
{
    wxDirDialog dlg(this, _T("Choose working directory (check write and read permissions!)"));
    if(dlg.ShowModal()!=wxID_OK) return;

    if(m_grid_master==NULL) {
        m_grid_master = new MyGridMaster(dlg.GetPath());
        m_grid_master->InitializeCommunication();

        wxString randomText = GenerateRandomText(400000);
        m_grid_master->addJob(randomText, 1234, 100);
    }
    m_MasterTimer = new wxTimer(this, MY_MASTER_TIMER_EVT);
    m_MasterTimer->Start(10000, true);
    MainEditBox->SetValue(printMasterState()); 
}
void MainFrame::RunSlave(wxCommandEvent& WXUNUSED(event))
{
    wxDirDialog dlg(this, _T("Choose working directory (check write and read permissions!)"));
    if(dlg.ShowModal()!=wxID_OK) return;
    
    m_grid_slave = new MyGridSlave(dlg.GetPath());
    m_grid_slave->ConnectToMaster(1, MainEditBox->GetValue());
    m_grid_slave->SetNoAvailCPUs(10);
    
    m_slaveTimer = new wxTimer(this, MY_SLAVE_TIMER_EVT);
    m_slaveTimer->Start(10*1000, true);

    MainEditBox->SetValue(printSlaveState());    
}
void MainFrame::RunNetstat(wxCommandEvent& event)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen("netstat -an", "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    MainEditBox->AppendText(result);
}
wxString MainFrame::printMasterState()
{
    if(m_grid_master==NULL) return "";

    wxString out;
    out+= "MASTER MODE\n";
    out+= "Connection status:\n";
    out+= "Server is ";
    out += (m_grid_master->isServerListening() ? "listening\n":"not listening\n");
    vector<wxString> slaves = m_grid_master->getSlavesIPs();
    out+= "There are "+to_string(slaves.size())+" connected slaves:\n";
    for(int i=0;i<slaves.size();i++) {
        out+= " -> "+slaves[i]+"\n";
    }
    out += "Jobs: "+to_string(m_grid_master->getJobs().size()) + "\n";
    out += "Results received: "+to_string(m_grid_master->getResults().size())+"\n";

    return out;
}
wxString MainFrame::printSlaveState()
{
    if(m_grid_slave==NULL) return "";
    wxString out;
    out+= "SLAVE MODE\n";
    out+= "Connection status: ";
    out+= (m_grid_slave->isConnectedToMaster() ? "connected to "+m_grid_slave->getHostName()+"\n":"disconnected\n");
    out+= "CPUs all:  "+to_string(m_grid_slave->getCPUsAll()) + "\n";
    out+= "CPUs free: "+to_string(m_grid_slave->getCPUsFree()) + "\n";
    out+= "Jobs received: "+to_string(m_grid_slave->getJobs().size()) + "\n";
    out+= "results sent: "+to_string(m_grid_slave->getResults().size());    
    
    return out;
}
wxString MainFrame::GenerateRandomText(size_t length) {
    const char characters[] =
        "0123456789 ABCDEFGHI JKLM NOPQRSTUVWXYZ abcdefghijkl mnopqr stuvw xyz";
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, sizeof(characters) - 2);

    wxString result;
    result.reserve(length);

    for(size_t i = 0; i < length; ++i) {
        result += characters[distribution(generator)];
    }

    return result;
}
void MainFrame::Quit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE); // Tells the OS to quit running this process
}
#include "MyGridMaster.h"

BEGIN_EVENT_TABLE(MyGridMaster, wxEvtHandler)
    EVT_TIMER(ID_GRID_MASTER_CHECK_SLAVES,                         MyGridMaster::OnCheckSlavesTimerEvent)
END_EVENT_TABLE()

MyGridMaster::MyGridMaster(wxString working_dir): GridMasterBase(working_dir)
{
    m_checkSlaveStateTimer = new wxTimer(this, ID_GRID_MASTER_CHECK_SLAVES);
    m_timer_iter = 1;
    m_checkSlaveStateTimer->Start(1000, true);
}
MyGridMaster::~MyGridMaster()
{

}
bool MyGridMaster::addJob(wxString data, int ID, int no_runs)
{
    wxMutexLocker l(m_jobs_mutex);
    MasterJob mj;
    mj.ID = ID;
    mj.data = data;
    mj.nb_runs = no_runs;
    m_jobs.push_back(mj);
    WriteLogMessage("new job added");

    return true;
}
void MyGridMaster::OnCheckSlavesTimerEvent(wxTimerEvent& event)
{
    WriteLogMessage("OnCheckSlavesTimerEvent");    

    ProcessMessagesFromSlaves();
    
    if((m_timer_iter % 10)==0) {
        AskSlavesState();
    }
        
    m_checkSlaveStateTimer->Start(1*1000, true);
    m_timer_iter++;
}
vector<wxString> MyGridMaster::getResults()
{
    wxMutexLocker l(m_results_mutex);
    return m_results;
}
void MyGridMaster::ProcessMessagesFromSlaves()
{
    vector<GridMasterBase::REC_MSG> msgs = getReceivedMsgs();

    for(int i=0;i<msgs.size();i++) {
        if(msgs[i].msg.length()>100000) {
            WriteLogMessage("We got a result from Slave!");
            wxMutexLocker l(m_results_mutex);
            m_results.push_back(" "); //just to count - for testing only

            //TODO: save the result
        } else if(msgs[i].msg.Contains("FreeCPUs")) {
            WriteLogMessage("We have answer from slave about its state, sending job...");
            
            //TODO analyse and send result if needed
            WriteLogMessage("Sending some job");
            SendSomeJob(msgs[i].IP);
        }
    }
}
vector<MasterJob> MyGridMaster::getJobs()
{
    wxMutexLocker l(m_jobs_mutex);
    return m_jobs;
}
bool MyGridMaster::SendSomeJob(wxString slaveIP)
{       
    MasterJob mj;
    bool found = false;
    wxMutexLocker l(m_jobs_mutex);
    {
        for(int i=0;i<m_jobs.size();i++) {
            if(m_jobs[i].getNbFreeJobs() > 0) {
                mj = m_jobs[i];
                found = true;
            }
        }
    }
    if(found) { 
        if(SendMsgToSlave(mj.data, slaveIP)==-1) {
            return false;
        }
    } else {
        WriteLogMessage("No job found");
        return false;
    }
    return true;
}
void MyGridMaster::AskSlavesState()
{
    SendMsgtoAll("state");
}

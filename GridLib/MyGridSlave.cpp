#include "MyGridSlave.h"

BEGIN_EVENT_TABLE(MyGridSlave, wxEvtHandler)
    EVT_TIMER(ID_GRID_CHECK_SLAVE,                         MyGridSlave::OnCheckSlaveTimerEvent)
END_EVENT_TABLE()

MyGridSlave::MyGridSlave(wxString m_working_dir): GridSlaveBase(m_working_dir)
{
    m_checkSlaveTimer = new wxTimer(this, ID_GRID_CHECK_SLAVE);
    m_CPUs_all = 1;
    m_CPUs_free = 1;
    m_checkSlaveTimer->Start(3000, true);
}
MyGridSlave::~MyGridSlave()
{

}
vector<ResultInfo> MyGridSlave::getResults()
{
    return m_results;
}
void MyGridSlave::SendResult(wxString result, long duration, long jobID, long resultID)
{
    wxString outmsg = "duration="+to_string(duration)+" jobID="+to_string(jobID)+" resultID="+to_string(resultID)+"\n"+result;
    WriteLogMessage("GridSlave::SendResult");
    
    if(!sendMessage(outmsg)) {
        WriteLogMessage("ERROR: sendMessage returns false!");     

        //TODO process result later
        return;
    }
    
    ResultInfo ri;
    ri.data = " ";
    ri.duration_seconds = duration;
    ri.jobID = jobID;
    ri.resultID = resultID;
    m_results.push_back(ri);
}
void MyGridSlave::OnCheckSlaveTimerEvent(wxTimerEvent& event)
{
    WriteLogMessage("GridSlave::OnCheckSlaveTimerEvent");
    vector<GridCommunication::MSGINFO_REC> msgs = getReceivedMessages();
    if(msgs.size()!=0) {
        ProcessMsgs(msgs);
    }

    WriteLogMessage("GridSlave: There are new "+to_string(msgs.size())+" received messages");

    m_checkSlaveTimer->Start(1000, true);
}
void MyGridSlave::SetNoAvailCPUs(int CPUs)
{
    m_CPUs_all = CPUs;
}
void MyGridSlave::ProcessMsgs(vector<GridCommunication::MSGINFO_REC> &msgs)
{//TODO: This should be modified according to the communications
    
    //just for testing...
    for(int i=0;i<msgs.size();i++) {
        if(msgs[i].msg.length()<100) {
            if(msgs[i].msg.compare("state")==0) {
                WriteLogMessage("ProcessMsgs: asking for my state, sending answer");
                sendMessage("FreeCPUs="+to_string(m_CPUs_free)+" allCPUs="+to_string(m_CPUs_all));
            } else {
                WriteLogMessage("ProcessMsgs: Unknown question");
            }
        } else {            
            WriteLogMessage("Job received");
            MasterJob mj;
            mj.data = " ";//save just empty string - just for testing
            mj.nb_runs = 0;                    
            m_jobs.push_back(mj);
        }        
    }
}
vector<MasterJob> MyGridSlave::getJobs()
{
    return m_jobs;
}
int MyGridSlave::getCPUsAll()
{
    return m_CPUs_all;
}
int MyGridSlave::getCPUsFree()
{
    return m_CPUs_free;
}
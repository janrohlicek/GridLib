#ifndef __MYGRIDSLAVE_H // Make sure to only declare these classes once
#define __MYGRIDSLAVE_H
#include "GridSlaveBase.h"

class MyGridSlave: public GridSlaveBase, wxEvtHandler
{
    public:
        MyGridSlave(wxString m_working_dir);
        ~MyGridSlave();

        void OnCheckSlaveTimerEvent(wxTimerEvent& event);
        vector<MasterJob> getJobs();
        vector<ResultInfo> getResults();
        void SetNoAvailCPUs(int CPUs);
        void SendResult(wxString result, long duration, long jobID, long resultID);
        int getCPUsAll();
        int getCPUsFree();


    protected:
        void ProcessMsgs(vector<GridCommunication::MSGINFO_REC> &msgs);

        wxTimer     *m_checkSlaveTimer;

        //think about mutex
        vector<ResultInfo> m_results;  

        //think about mutex
        vector<MasterJob>  m_jobs;

        int          m_CPUs_all;
        int          m_CPUs_free;

        DECLARE_EVENT_TABLE()
};
#endif


#ifndef __MYGRIDMASTER_H // Make sure to only declare these classes once
#define __MYGRIDMASTER_H
#include "GridMasterBase.h"


class MyGridMaster: public GridMasterBase, wxEvtHandler
{
    public:
        MyGridMaster(wxString working_dir);
        ~MyGridMaster();

        void OnCheckSlavesTimerEvent(wxTimerEvent& event);
        bool addJob(wxString data, int ID, int no_runs);
        void AskSlavesState();
        void ProcessMessagesFromSlaves();
        vector<wxString> getResults();
        bool SendSomeJob(wxString slaveIP);
        vector<MasterJob> getJobs();

    protected:
        wxTimer                 *m_checkSlaveStateTimer;
        vector<MasterJob>        m_jobs;
        wxMutex                  m_jobs_mutex;
        unsigned long            m_timer_iter;
        vector<wxString>         m_results;
        wxMutex                  m_results_mutex;

        DECLARE_EVENT_TABLE()
};
#endif


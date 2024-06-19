#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <wx/wx.h>
#include <vector>
#include <queue> // For priority queue

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    // Event handlers
    void OnAddProcess(wxCommandEvent& event);
    void OnDeleteProcess(wxCommandEvent& event);
    void OnRunFCFSScheduler(wxCommandEvent& event);
    void OnRunSJFScheduler(wxCommandEvent& event);
    void OnRunSRTFScheduler(wxCommandEvent& event);
    void OnRunRRScheduler(wxCommandEvent& event);
    // void OnRunPriorityPreemptiveScheduler(wxCommandEvent& event);
    // void OnRunPriorityNonPreemptiveScheduler(wxCommandEvent& event);
    void OnPaint(wxPaintEvent& event);

    // Process structure
    struct Process {
        int id;
        int arrivalTime;
        int burstTime;
        int priority; // For Priority Scheduling

        bool operator<(const Process& other) const {
            // Min-heap based on burstTime for SJF and SRTF
            return burstTime > other.burstTime;
        }
    };

    // struct CompareProcessByPriority {
    //     bool operator()(const Process& lhs, const Process& rhs) const {
    //         // Define the comparison logic based on priority (or any other criteria)
    //         return lhs.priority > rhs.priority; // Change '>' as per your required ordering
    //     }
    // };

    // GUI Components
    wxPanel* mainPanel;
    wxBoxSizer* mainSizer;
    wxButton* addButton;
    wxButton* deleteButton;
    wxButton* runFCFSButton;
    wxButton* runSJFButton;
    wxButton* runSRTFButton;
    wxButton* runRRButton;
    // wxButton* runPriorityPreemptiveButton;
    // wxButton* runPriorityNonPreemptiveButton;
    wxListBox* processList;
    wxTextCtrl* outputArea;

    // Process data
    std::vector<Process> processes;

    // Scheduler output
    std::vector<int> completionTime;
    std::vector<int> turnaroundTime;
    std::vector<int> waitingTime;
    std::vector<int> responseTime;
    std::vector<int> burstTime; // Define burstTime as a member variable
    int totalBurstTime = 0;
    int currentTime = 0;

    void RefreshProcessList();
    void RunFCFSScheduler();
    void RunSJFScheduler();
    void RunSRTFScheduler();
    void RunRRScheduler(int timeQuantum);
    // void RunPriorityPreemptiveScheduler();
    // void RunPriorityNonPreemptiveScheduler();

    wxDECLARE_EVENT_TABLE();
};

enum {
    ID_Add = 1,
    ID_Delete,
    ID_RunFCFS,
    ID_RunSJF,
    ID_RunSRTF,
    ID_RunRR,
    // ID_RunPriorityPreemptive,
    // ID_RunPriorityNonPreemptive
};

#endif // MAINFRAME_H

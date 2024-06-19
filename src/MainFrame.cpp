#include "MainFrame.h"
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(ID_Add, MainFrame::OnAddProcess)
    EVT_BUTTON(ID_Delete, MainFrame::OnDeleteProcess)
    EVT_BUTTON(ID_RunFCFS, MainFrame::OnRunFCFSScheduler)
    EVT_BUTTON(ID_RunSJF, MainFrame::OnRunSJFScheduler)
    EVT_BUTTON(ID_RunSRTF, MainFrame::OnRunSRTFScheduler)
    EVT_BUTTON(ID_RunRR, MainFrame::OnRunRRScheduler)
    // EVT_BUTTON(ID_RunPriorityPreemptive, MainFrame::OnRunPriorityPreemptiveScheduler)
    // EVT_BUTTON(ID_RunPriorityNonPreemptive, MainFrame::OnRunPriorityNonPreemptiveScheduler)
    EVT_PAINT(MainFrame::OnPaint)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {
    mainPanel = new wxPanel(this, wxID_ANY);
    mainSizer = new wxBoxSizer(wxVERTICAL);

    addButton = new wxButton(mainPanel, ID_Add, "Add Process");
    deleteButton = new wxButton(mainPanel, ID_Delete, "Delete Process");
    runFCFSButton = new wxButton(mainPanel, ID_RunFCFS, "Run FCFS");
    runSJFButton = new wxButton(mainPanel, ID_RunSJF, "Run SJF");
    runSRTFButton = new wxButton(mainPanel, ID_RunSRTF, "Run SRTF");
    runRRButton = new wxButton(mainPanel, ID_RunRR, "Run RR");
    // runPriorityPreemptiveButton = new wxButton(mainPanel, ID_RunPriorityPreemptive, "Run Priority (Preemptive)");
    // runPriorityNonPreemptiveButton = new wxButton(mainPanel, ID_RunPriorityNonPreemptive, "Run Priority (Non-preemptive)");
    processList = new wxListBox(mainPanel, wxID_ANY);
    outputArea = new wxTextCtrl(mainPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

    mainSizer->Add(addButton, 0, wxALL, 5);
    mainSizer->Add(deleteButton, 0, wxALL, 5);
    mainSizer->Add(runFCFSButton, 0, wxALL, 5);
    mainSizer->Add(runSJFButton, 0, wxALL, 5);
    mainSizer->Add(runSRTFButton, 0, wxALL, 5);
    mainSizer->Add(runRRButton, 0, wxALL, 5);
    // mainSizer->Add(runPriorityPreemptiveButton, 0, wxALL, 5);
    // mainSizer->Add(runPriorityNonPreemptiveButton, 0, wxALL, 5);
    mainSizer->Add(processList, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(outputArea, 1, wxEXPAND | wxALL, 5);

    mainPanel->SetSizer(mainSizer);
}

void MainFrame::OnAddProcess(wxCommandEvent& event) {
    int processID = processes.size() + 1;

    wxTextEntryDialog arrivalDialog(this, "Enter arrival time:", "Add Process", "0");
    if (arrivalDialog.ShowModal() == wxID_OK) {
        long arrivalTime;
        if (!arrivalDialog.GetValue().ToLong(&arrivalTime) || arrivalTime < 0) {
            wxMessageBox("Invalid input. Please enter a non-negative integer for arrival time.", "Error", wxICON_ERROR);
            return;
        }

        wxTextEntryDialog burstDialog(this, "Enter burst time:", "Add Process", "1");
        if (burstDialog.ShowModal() == wxID_OK) {
            long burstTime;
            if (!burstDialog.GetValue().ToLong(&burstTime) || burstTime <= 0) {
                wxMessageBox("Invalid input. Please enter a positive integer for burst time.", "Error", wxICON_ERROR);
                return;
            }

            wxTextEntryDialog priorityDialog(this, "Enter priority (0 for RR):", "Add Process", "0");
            if (priorityDialog.ShowModal() == wxID_OK) {
                long priority;
                if (!priorityDialog.GetValue().ToLong(&priority) || priority < 0) {
                    wxMessageBox("Invalid input. Please enter a non-negative integer for priority.", "Error", wxICON_ERROR);
                    return;
                }

                processes.push_back({ processID, static_cast<int>(arrivalTime), static_cast<int>(burstTime), static_cast<int>(priority) });
                RefreshProcessList();
            }
        }
    }
}

void MainFrame::OnDeleteProcess(wxCommandEvent& event) {
    int selection = processList->GetSelection();
    if (selection != wxNOT_FOUND) {
        processes.erase(processes.begin() + selection);
        RefreshProcessList();
    }
}

void MainFrame::OnRunFCFSScheduler(wxCommandEvent& event) {
    RunFCFSScheduler();
    Refresh(); // Trigger repaint to draw the Gantt chart
}

void MainFrame::OnRunSJFScheduler(wxCommandEvent& event) {
    RunSJFScheduler();
    Refresh(); // Trigger repaint to draw the Gantt chart
}

void MainFrame::OnRunSRTFScheduler(wxCommandEvent& event) {
    RunSRTFScheduler();
    Refresh(); // Trigger repaint to draw the Gantt chart
}

void MainFrame::OnRunRRScheduler(wxCommandEvent& event) {
    // For Round Robin, ask for time quantum
    wxString timeQuantumStr = wxGetTextFromUser("Enter time quantum:", "Round Robin Scheduler", "1");
    long timeQuantum;
    if (timeQuantumStr.ToLong(&timeQuantum) && timeQuantum > 0) {
        RunRRScheduler(static_cast<int>(timeQuantum));
        Refresh(); // Trigger repaint to draw the Gantt chart
    } else {
        wxMessageBox("Invalid input. Please enter a positive integer for time quantum.", "Error", wxICON_ERROR);
    }
}

// void MainFrame::OnRunPriorityPreemptiveScheduler(wxCommandEvent& event) {
//     RunPriorityPreemptiveScheduler();
//     Refresh(); // Trigger repaint to draw the Gantt chart
// }

// void MainFrame::OnRunPriorityNonPreemptiveScheduler(wxCommandEvent& event) {
//     RunPriorityNonPreemptiveScheduler();
//         Refresh(); // Trigger repaint to draw the Gantt chart
// }

void MainFrame::RefreshProcessList() {
    processList->Clear();
    for (const auto& process : processes) {
        processList->Append(wxString::Format("ID: %d, Arrival: %d, Burst: %d",
                                              process.id, process.arrivalTime, process.burstTime));
    }
}

void MainFrame::RunFCFSScheduler() {
    if (processes.empty()) {
        wxMessageBox("No processes to schedule!", "Error", wxICON_ERROR);
        return;
    }

    // Sort processes by arrival time
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    currentTime = 0;
    totalBurstTime = 0;
    std::ostringstream output;
    output << "FCFS Scheduling\n";
    output << "ID\tArrival\tBurst\tCompletion\tTurnaround\tWaiting\tResponse\n";

    completionTime.resize(processes.size());
    turnaroundTime.resize(processes.size());
    waitingTime.resize(processes.size());
    responseTime.resize(processes.size());
    burstTime.clear();

    for (const auto& process : processes) {
        burstTime.push_back(process.burstTime);
    }
    
    for (size_t i = 0; i < processes.size(); ++i) {
        const auto& process = processes[i];
        int startTime = std::max(currentTime, process.arrivalTime);
        completionTime[i] = startTime + process.burstTime;
        turnaroundTime[i] = completionTime[i] - process.arrivalTime;
        waitingTime[i] = turnaroundTime[i] - process.burstTime;
        responseTime[i] = startTime - process.arrivalTime;

        currentTime = completionTime[i];
        totalBurstTime += process.burstTime;

        output << process.id << "\t"
               << process.arrivalTime << "\t"
               << process.burstTime << "\t"
               << completionTime[i] << "\t"
               << turnaroundTime[i] << "\t"
               << waitingTime[i] << "\t"
               << responseTime[i] << "\n";
    }

    double cpuUtilization = (totalBurstTime * 100.0) / currentTime;
    double throughput = static_cast<double>(processes.size()) / currentTime;

    output << "CPU Utilization: " << std::fixed << std::setprecision(2) << cpuUtilization << "%\n";
    output << "Throughput: " << std::fixed << std::setprecision(2) << throughput << " processes/unit time\n";

    outputArea->SetValue(output.str());

    // Store burst times for drawing
    Refresh(); // Trigger repaint to draw the Gantt chart
}

void MainFrame::RunSJFScheduler() {
    if (processes.empty()) {
        wxMessageBox("No processes to schedule!", "Error", wxICON_ERROR);
        return;
    }

    std::priority_queue<Process> pq; // Min-heap based on burst time
    std::vector<Process> scheduledProcesses;
    currentTime = 0;
    totalBurstTime = 0;
    std::ostringstream output;
    output << "SJF Scheduling (Shortest Job First)\n";
    output << "ID\tArrival\tBurst\tCompletion\tTurnaround\tWaiting\tResponse\n";

    completionTime.resize(processes.size());
    turnaroundTime.resize(processes.size());
    waitingTime.resize(processes.size());
    responseTime.resize(processes.size());
    burstTime.clear();

    for (const auto& process : processes) {
        pq.push(process);
    }

    while (!pq.empty()) {
        Process current = pq.top();
        pq.pop();

        int startTime = std::max(currentTime, current.arrivalTime);
        completionTime[current.id - 1] = startTime + current.burstTime;
        turnaroundTime[current.id - 1] = completionTime[current.id - 1] - current.arrivalTime;
        waitingTime[current.id - 1] = turnaroundTime[current.id - 1] - current.burstTime;
        responseTime[current.id - 1] = startTime - current.arrivalTime;

        currentTime = completionTime[current.id - 1];
        totalBurstTime += current.burstTime;

        burstTime.push_back(current.burstTime);
        scheduledProcesses.push_back(current);

        output << current.id << "\t"
               << current.arrivalTime << "\t"
               << current.burstTime << "\t"
               << completionTime[current.id - 1] << "\t"
               << turnaroundTime[current.id - 1] << "\t"
               << waitingTime[current.id - 1] << "\t"
               << responseTime[current.id - 1] << "\n";
    }

    double cpuUtilization = (totalBurstTime * 100.0) / currentTime;
    double throughput = static_cast<double>(processes.size()) / currentTime;

    output << "CPU Utilization: " << std::fixed << std::setprecision(2) << cpuUtilization << "%\n";
    output << "Throughput: " << std::fixed << std::setprecision(2) << throughput << " processes/unit time\n";

    outputArea->SetValue(output.str());

    // Store burst times for drawing
    Refresh(); // Trigger repaint to draw the Gantt chart
}

void MainFrame::RunSRTFScheduler() {
    if (processes.empty()) {
        wxMessageBox("No processes to schedule!", "Error", wxICON_ERROR);
        return;
    }

    std::priority_queue<Process> pq; // Min-heap based on burst time
    std::vector<Process> scheduledProcesses;
    currentTime = 0;
    totalBurstTime = 0;
    std::ostringstream output;
    output << "SRTF Scheduling (Shortest Remaining Time First)\n";
    output << "ID\tArrival\tBurst\tCompletion\tTurnaround\tWaiting\tResponse\n";

    completionTime.resize(processes.size());
    turnaroundTime.resize(processes.size());
    waitingTime.resize(processes.size());
    responseTime.resize(processes.size());
    burstTime.clear();

    int remainingProcesses = processes.size();
    int lastCompletionTime = 0;

    while (remainingProcesses > 0) {
        // Push processes that have arrived by currentTime into the priority queue
        for (const auto& process : processes) {
            if (process.arrivalTime <= currentTime) {
                pq.push(process);
            }
        }

        if (pq.empty()) {
            currentTime++;
            continue;
        }

        Process current = pq.top();
        pq.pop();

        int startTime = std::max(currentTime, current.arrivalTime);
        if (startTime > lastCompletionTime) {
            lastCompletionTime = startTime;
        }

        int remainingBurst = current.burstTime - (lastCompletionTime - current.arrivalTime);
        if (remainingBurst <= 0) {
            // Process completed
            completionTime[current.id - 1] = lastCompletionTime + current.burstTime;
            turnaroundTime[current.id - 1] = completionTime[current.id - 1] - current.arrivalTime;
            waitingTime[current.id - 1] = turnaroundTime[current.id - 1] - current.burstTime;
            responseTime[current.id - 1] = lastCompletionTime - current.arrivalTime;

            lastCompletionTime = completionTime[current.id - 1];
            totalBurstTime += current.burstTime;
            remainingProcesses--;

            burstTime.push_back(current.burstTime);
            scheduledProcesses.push_back(current);

            output << current.id << "\t"
                   << current.arrivalTime << "\t"
                   << current.burstTime << "\t"
                   << completionTime[current.id - 1] << "\t"
                   << turnaroundTime[current.id - 1] << "\t"
                   << waitingTime[current.id - 1] << "\t"
                   << responseTime[current.id - 1] << "\n";
        } else {
            // Process partially completed, push back with updated burst time
            current.burstTime = remainingBurst;
            pq.push(current);
            currentTime = lastCompletionTime;
        }
    }

    double cpuUtilization = (totalBurstTime * 100.0) / currentTime;
    double throughput = static_cast<double>(processes.size()) / currentTime;

    output << "CPU Utilization: " << std::fixed << std::setprecision(2) << cpuUtilization << "%\n";
    output << "Throughput: " << std::fixed << std::setprecision(2) << throughput << " processes/unit time\n";

    outputArea->SetValue(output.str());

    // Store burst times for drawing
    Refresh(); // Trigger repaint to draw the Gantt chart
}

void MainFrame::RunRRScheduler(int timeQuantum) {
    if (processes.empty()) {
        wxMessageBox("No processes to schedule!", "Error", wxICON_ERROR);
        return;
    }

    std::deque<Process> readyQueue;
    std::vector<Process> scheduledProcesses;
    int currentTime = 0; // Initialize currentTime properly
    int totalBurstTime = 0; // Initialize totalBurstTime properly
    std::ostringstream output;
    output << "Round Robin Scheduling (Time Quantum: " << timeQuantum << ")\n";
    output << "ID\tArrival\tBurst\tCompletion\tTurnaround\tWaiting\tResponse\n";

    // Initialize vectors to store results
    std::vector<int> completionTime(processes.size(), 0);
    std::vector<int> turnaroundTime(processes.size(), 0);
    std::vector<int> waitingTime(processes.size(), 0);
    std::vector<int> responseTime(processes.size(), 0);
    std::vector<int> burstTime; // Stores burst times for drawing

    int remainingProcesses = processes.size();

    while (remainingProcesses > 0) {
        // Add processes that have arrived by currentTime to the ready queue
        for (const auto& process : processes) {
            if (process.arrivalTime <= currentTime && process.burstTime > 0) {
                readyQueue.push_back(process);
            }
        }

        if (readyQueue.empty()) {
            currentTime++;
            continue;
        }

        Process current = readyQueue.front();
        readyQueue.pop_front();

        int startTime = std::max(currentTime, current.arrivalTime);
        int executionTime = std::min(timeQuantum, current.burstTime);
        currentTime = startTime + executionTime;

        current.burstTime -= executionTime;

        if (current.burstTime <= 0) {
            // Process completed
            completionTime[current.id - 1] = currentTime;
            turnaroundTime[current.id - 1] = currentTime - current.arrivalTime;
            waitingTime[current.id - 1] = turnaroundTime[current.id - 1] - current.burstTime - current.arrivalTime;
            responseTime[current.id - 1] = startTime - current.arrivalTime;

            totalBurstTime += executionTime; // Update totalBurstTime correctly
            remainingProcesses--;

            burstTime.push_back(executionTime);
            scheduledProcesses.push_back(current);

            output << current.id << "\t"
                   << current.arrivalTime << "\t"
                   << current.burstTime << "\t"
                   << completionTime[current.id - 1] << "\t"
                   << turnaroundTime[current.id - 1] << "\t"
                   << waitingTime[current.id - 1] << "\t"
                   << responseTime[current.id - 1] << "\n";
        } else {
            // Process not completed, push back to ready queue
            readyQueue.push_back(current);

            burstTime.push_back(executionTime);
            scheduledProcesses.push_back(current);
        }
    }

    double cpuUtilization = (totalBurstTime * 100.0) / currentTime;
    double throughput = static_cast<double>(processes.size()) / currentTime;

    output << "CPU Utilization: " << std::fixed << std::setprecision(2) << cpuUtilization << "%\n";
    output << "Throughput: " << std::fixed << std::setprecision(2) << throughput << " processes/unit time\n";

    outputArea->SetValue(output.str());

    Refresh(); // Trigger repaint to draw the Gantt chart
}


// void MainFrame::RunPriorityPreemptiveScheduler {
//     if (processes.empty()) {
//         wxMessageBox("No processes to schedule!", "Error", wxICON_ERROR);
//         return;
//     }

//     std::priority_queue<Process> pq; // Max-heap based on default operator< (largest priority first)
//     std::vector<Process> scheduledProcesses;
//     int currentTime = 0;
//     int totalBurstTime = 0;
//     std::ostringstream output;
//     output << "Priority Scheduling (Preemptive)\n";
//     output << "ID\tArrival\tBurst\tPriority\tCompletion\tTurnaround\tWaiting\tResponse\n";

//     std::vector<int> completionTime(processes.size(), 0);
//     std::vector<int> turnaroundTime(processes.size(), 0);
//     std::vector<int> waitingTime(processes.size(), 0);
//     std::vector<int> responseTime(processes.size(), 0);
//     std::vector<int> burstTime;

//     int remainingProcesses = processes.size();

//     while (remainingProcesses > 0) {
//         // Push processes that have arrived by currentTime into the priority queue
//         for (const auto& process : processes) {
//             if (process.arrivalTime <= currentTime && process.burstTime > 0) {
//                 pq.push(process);
//             }
//         }

//         if (pq.empty()) {
//             currentTime++;
//             continue;
//         }

//         Process current = pq.top();
//         pq.pop();

//         int startTime = std::max(currentTime, current.arrivalTime);
//         int executionTime = std::min(current.burstTime, 1); // Process one unit of time
//         current.burstTime -= executionTime;
//         currentTime += executionTime;

//         if (current.burstTime <= 0) {
//             // Process completed
//             completionTime[current.id - 1] = currentTime;
//             turnaroundTime[current.id - 1] = currentTime - current.arrivalTime;
//             waitingTime[current.id - 1] = turnaroundTime[current.id - 1] - current.burstTime;
//             responseTime[current.id - 1] = startTime - current.arrivalTime;

//             totalBurstTime += executionTime; // Use executionTime for totalBurstTime
//             remainingProcesses--;

//             burstTime.push_back(executionTime);
//             scheduledProcesses.push_back(current);

//             output << current.id << "\t"
//                    << current.arrivalTime << "\t"
//                    << current.burstTime << "\t"
//                    << current.priority << "\t"
//                    << completionTime[current.id - 1] << "\t"
//                    << turnaroundTime[current.id - 1] << "\t"
//                    << waitingTime[current.id - 1] << "\t"
//                    << responseTime[current.id - 1] << "\n";
//         } else {
//             // Process not completed, push back to priority queue
//             pq.push(current);

//             burstTime.push_back(executionTime);
//             scheduledProcesses.push_back(current);
//         }
//     }

//     double cpuUtilization = (totalBurstTime * 100.0) / currentTime;
//     double throughput = static_cast<double>(processes.size()) / currentTime;

//     output << "CPU Utilization: " << std::fixed << std::setprecision(2) << cpuUtilization << "%\n";
//     output << "Throughput: " << std::fixed << std::setprecision(2) << throughput << " processes/unit time\n";

//     outputArea->SetValue(output.str());

//     Refresh(); // Trigger repaint to draw the Gantt chart
// }

// void MainFrame::RunPriorityNonPreemptiveScheduler() {
//     if (processes.empty()) {
//         wxMessageBox("No processes to schedule!", "Error", wxICON_ERROR);
//         return;
//     }

//     // Min-heap based on priority (lower value means higher priority)
//     std::priority_queue<Process, std::vector<Process>, CompareProcessByPriority> pq;
//     std::vector<Process> scheduledProcesses;
//     int currentTime = 0;
//     int totalBurstTime = 0;
//     std::ostringstream output;
//     output << "Priority Scheduling (Non-preemptive)\n";
//     output << "ID\tArrival\tBurst\tPriority\tCompletion\tTurnaround\tWaiting\tResponse\n";

//     // Initialize vectors to store scheduling metrics
//     std::vector<int> completionTime(processes.size(), 0);
//     std::vector<int> turnaroundTime(processes.size(), 0);
//     std::vector<int> waitingTime(processes.size(), 0);
//     std::vector<int> responseTime(processes.size(), 0);
//     std::vector<int> burstTime;

//     // Push all processes into the priority queue
//     for (const auto& process : processes) {
//         pq.push(process);
//     }

//     // Process each task in priority order
//     while (!pq.empty()) {
//         Process current = pq.top(); // Get the highest priority task
//         pq.pop(); // Remove it from the priority queue

//         int startTime = std::max(currentTime, current.arrivalTime);
//         int completion = startTime + current.burstTime;

//         // Record scheduling metrics
//         completionTime[current.id - 1] = completion;
//         turnaroundTime[current.id - 1] = completion - current.arrivalTime;
//         waitingTime[current.id - 1] = turnaroundTime[current.id - 1] - current.burstTime;
//         responseTime[current.id - 1] = startTime - current.arrivalTime;

//         currentTime = completion; // Update current time
//         totalBurstTime += current.burstTime; // Update total burst time

//         burstTime.push_back(current.burstTime); // Store burst time
//         scheduledProcesses.push_back(current); // Store scheduled process

//         // Append scheduling results to output string
//         output << current.id << "\t"
//                << current.arrivalTime << "\t"
//                << current.burstTime << "\t"
//                << current.priority << "\t"
//                << completionTime[current.id - 1] << "\t"
//                << turnaroundTime[current.id - 1] << "\t"
//                << waitingTime[current.id - 1] << "\t"
//                << responseTime[current.id - 1] << "\n";
//     }

//     // Calculate CPU utilization and throughput
//     double cpuUtilization = (totalBurstTime * 100.0) / currentTime;
//     double throughput = static_cast<double>(processes.size()) / currentTime;

//     // Append CPU utilization and throughput to output string
//     output << "CPU Utilization: " << std::fixed << std::setprecision(2) << cpuUtilization << "%\n";
//     output << "Throughput: " << std::fixed << std::setprecision(2) << throughput << " processes/unit time\n";

//     // Display the output in the GUI output area
//     outputArea->SetValue(output.str());

//     // Trigger a repaint to update the GUI with Gantt chart or other visualizations
//     Refresh();
// }


void MainFrame::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);

    if (completionTime.empty()) return;

    int panelWidth = GetSize().GetWidth();
    int panelHeight = GetSize().GetHeight();
    int timeUnitWidth = panelWidth / (totalBurstTime > 0 ? totalBurstTime : 1); // Avoid division by zero

    int xOffset = 10; // Start a bit away from the edge
    int yOffset = panelHeight - 100; // Position the Gantt chart near the bottom

    for (size_t i = 0; i < burstTime.size(); ++i) {
        int burstWidth = burstTime[i] * timeUnitWidth;

        // Draw the process block
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(wxBrush(wxColour(100 + (i * 30) % 155, 100 + (i * 60) % 155, 200)));
        dc.DrawRectangle(xOffset, yOffset, burstWidth, 40);

        // Draw process ID in the block
        dc.SetTextForeground(*wxBLACK);
        dc.DrawText(wxString::Format("P%d", processes[i].id), xOffset + burstWidth / 2 - 10, yOffset + 10);

        // Draw the timeline below the Gantt chart
        dc.DrawText(wxString::Format("%d", completionTime[i]), xOffset, yOffset + 50);

        xOffset += burstWidth;
    }

    // Draw the final time tick
    dc.DrawText(wxString::Format("%d", currentTime), xOffset, yOffset + 50);
}



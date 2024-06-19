#ifndef GANTTCHARTPANEL_H
#define GANTTCHARTPANEL_H

#include <wx/wx.h>
#include <vector>

class GanttChartPanel : public wxPanel {
public:
    GanttChartPanel(wxWindow* parent);

    void SetData(const std::vector<int>& completionTime, const std::vector<int>& burstTime, int totalTime);
    
private:
    void OnPaint(wxPaintEvent& event);

    std::vector<int> completionTime;
    std::vector<int> burstTime;
    int totalTime;

    wxDECLARE_EVENT_TABLE();
};

#endif // GANTTCHARTPANEL_H

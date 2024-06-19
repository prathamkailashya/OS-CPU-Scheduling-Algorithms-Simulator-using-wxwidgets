#include "GanttChartPanel.h"

wxBEGIN_EVENT_TABLE(GanttChartPanel, wxPanel)
    EVT_PAINT(GanttChartPanel::OnPaint)
wxEND_EVENT_TABLE()

GanttChartPanel::GanttChartPanel(wxWindow* parent)
    : wxPanel(parent), totalTime(0) {
}

void GanttChartPanel::SetData(const std::vector<int>& completionTime, const std::vector<int>& burstTime, int totalTime) {
    this->completionTime = completionTime;
    this->burstTime = burstTime;
    this->totalTime = totalTime;
    Refresh();
    Update(); // Add Update to force immediate repaint
}

void GanttChartPanel::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);

    if (completionTime.empty()) return;

    int panelWidth = GetSize().GetWidth();
    int panelHeight = GetSize().GetHeight();
    int timeUnitWidth = panelWidth / totalTime;

    dc.SetBrush(*wxLIGHT_GREY_BRUSH);
    dc.Clear();

    int xOffset = 0;
    for (size_t i = 0; i < burstTime.size(); ++i) {
        int burstWidth = burstTime[i] * timeUnitWidth;

        // Draw the process block
        dc.SetPen(*wxBLACK_PEN);
        dc.SetBrush(wxBrush(wxColour(100 + (i * 30) % 155, 100 + (i * 60) % 155, 200)));
        dc.DrawRectangle(xOffset, 20, burstWidth, 40);

        // Draw process ID in the block
        dc.SetTextForeground(*wxBLACK);
        dc.DrawText(wxString::Format("P%d", static_cast<int>(i + 1)), xOffset + burstWidth / 2 - 10, 30);

        // Draw the timeline below the Gantt chart
        dc.DrawText(wxString::Format("%d", completionTime[i]), xOffset, 70);

        xOffset += burstWidth;
    }

    // Draw the final time tick
    dc.DrawText(wxString::Format("%d", totalTime), xOffset, 70);
}

#include <wx/wx.h>
#include "MainFrame.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MainFrame* mainFrame = new MainFrame("OS Scheduler Simulator", wxPoint(50, 50), wxSize(800, 600));
    mainFrame->Show(true);
    return true;
}

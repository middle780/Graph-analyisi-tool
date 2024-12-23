#include "App.h"
#include "MainFrame.h"
#include <wx/msgdlg.h>

IMPLEMENT_APP(App)

bool App::OnInit() {
    if (!wxApp::OnInit()) {
        wxMessageBox("Failed to initialize wxApp", "Error");
        return false;
    }

    try {
        MainFrame* mainFrame = new MainFrame();
        if (!mainFrame) {
            wxMessageBox("Failed to create MainFrame", "Error");
            return false;
        }
        mainFrame->Show();
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Exception: %s", e.what()), "Error");
        return false;
    }
    catch (...) {
        wxMessageBox("Unknown error occurred", "Error");
        return false;
    }

    return true;
}

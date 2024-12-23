#pragma once
#include <wx/wx.h>
#include "Graph.h"
#include <vector>

class MainFrame : public wxFrame {
private:
    Graph* graph;
    wxPanel* drawPanel;
    wxTextCtrl* resultText;
    wxTextCtrl* startNodeCtrl;
    wxTextCtrl* endNodeCtrl;
    vector<wxPoint> nodePositions;
    static constexpr int NODE_RADIUS = 20;
    static constexpr double PI = 3.14159265358979323846;

    void OnLoad(wxCommandEvent& evt);
    void OnMST(wxCommandEvent& evt);
    void OnPath(wxCommandEvent& evt);
    void OnPaint(wxPaintEvent& evt);
    void CalculateNodePositions();
    void DrawGraph(wxDC& dc);

public:
    MainFrame();
    ~MainFrame();
};

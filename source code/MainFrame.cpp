#include "MainFrame.h"
#include <wx/filedlg.h>
#include <cmath>

enum {
    ID_LOAD = 1,
    ID_MST,
    ID_PATH
};

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "Graph Visualizer", wxDefaultPosition, wxSize(1024, 768)),
    graph(nullptr)
{
    // Create main panel and sizer
    wxPanel* mainPanel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Create top control panel
    wxPanel* controlPanel = new wxPanel(mainPanel);
    wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);

    // Add buttons
    wxButton* loadBtn = new wxButton(controlPanel, ID_LOAD, "Load Graph");
    wxButton* mstBtn = new wxButton(controlPanel, ID_MST, "Show MST");
    wxButton* pathBtn = new wxButton(controlPanel, ID_PATH, "Find Path");

    controlSizer->Add(loadBtn, 0, wxALL, 5);
    controlSizer->Add(mstBtn, 0, wxALL, 5);
    controlSizer->Add(pathBtn, 0, wxALL, 5);

    // Add node input controls
    controlSizer->Add(new wxStaticText(controlPanel, wxID_ANY, "Start:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    startNodeCtrl = new wxTextCtrl(controlPanel, wxID_ANY, "0", wxDefaultPosition, wxSize(40, -1));
    controlSizer->Add(startNodeCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    controlSizer->Add(new wxStaticText(controlPanel, wxID_ANY, "End:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    endNodeCtrl = new wxTextCtrl(controlPanel, wxID_ANY, "1", wxDefaultPosition, wxSize(40, -1));
    controlSizer->Add(endNodeCtrl, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    controlPanel->SetSizer(controlSizer);

    // Create drawing panel
    drawPanel = new wxPanel(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    drawPanel->SetBackgroundColour(*wxWHITE);

    // Create result text area
    resultText = new wxTextCtrl(mainPanel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 100),
        wxTE_MULTILINE | wxTE_READONLY | wxBORDER_SIMPLE);

    // Add everything to main sizer
    mainSizer->Add(controlPanel, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(drawPanel, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(resultText, 0, wxEXPAND | wxALL, 5);

    mainPanel->SetSizer(mainSizer);

    // Bind events
    loadBtn->Bind(wxEVT_BUTTON, &MainFrame::OnLoad, this);
    mstBtn->Bind(wxEVT_BUTTON, &MainFrame::OnMST, this);
    pathBtn->Bind(wxEVT_BUTTON, &MainFrame::OnPath, this);
    drawPanel->Bind(wxEVT_PAINT, &MainFrame::OnPaint, this);

    // Center window
    Centre();
}

MainFrame::~MainFrame() {
    delete graph;
}

void MainFrame::OnLoad(wxCommandEvent& evt) {
    wxFileDialog dlg(this, "Open graph file", "", "", "Text files (*.txt)|*.txt",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (dlg.ShowModal() == wxID_OK) {
        delete graph;
        graph = new Graph(0);
        if (graph->loadFromFile(dlg.GetPath().ToStdString())) {
            CalculateNodePositions();
            resultText->SetValue("Graph loaded successfully");
            drawPanel->Refresh();
        }
        else {
            resultText->SetValue("Failed to load graph");
            delete graph;
            graph = nullptr;
        }
    }
}

void MainFrame::OnMST(wxCommandEvent& evt) {
    if (!graph) {
        resultText->SetValue("No graph loaded");
        return;
    }

    auto mst = graph->getMST();
    wxString result = "Minimum Spanning Tree:\n";
    for (const auto& edge : mst) {
        result += wxString::Format("Edge %d - %d (weight: %d)\n", edge[0], edge[1], edge[2]);
    }
    resultText->SetValue(result);
}

void MainFrame::OnPath(wxCommandEvent& evt) {
    if (!graph) {
        resultText->SetValue("No graph loaded");
        return;
    }

    long start, end;
    if (!startNodeCtrl->GetValue().ToLong(&start) || !endNodeCtrl->GetValue().ToLong(&end)) {
        resultText->SetValue("Invalid node numbers");
        return;
    }

    auto path = graph->getShortestPath(start, end);
    if (path.empty()) {
        resultText->SetValue("No path found");
        return;
    }

    wxString result = "Shortest path: ";
    for (size_t i = 0; i < path.size(); i++) {
        result += wxString::Format("%d", path[i]);
        if (i < path.size() - 1) result += " → ";
    }
    resultText->SetValue(result);
}

void MainFrame::CalculateNodePositions() {
    if (!graph) return;

    nodePositions.clear();
    int numNodes = graph->getNumVertices();

    // Get panel dimensions
    wxSize size = drawPanel->GetSize();
    int centerX = size.GetWidth() / 2;
    int centerY = size.GetHeight() / 2;

    // Calculate radius for node placement
    double radius = std::min(centerX, centerY) - NODE_RADIUS - 20;

    // Calculate positions in a circle
    for (int i = 0; i < numNodes; i++) {
        double angle = 2 * PI * i / numNodes - PI / 2;  // Start from top
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        nodePositions.push_back(wxPoint(x, y));
    }
}

void MainFrame::OnPaint(wxPaintEvent& evt) {
    wxPaintDC dc(drawPanel);
    DrawGraph(dc);
}

void MainFrame::DrawGraph(wxDC& dc) {
    if (!graph || nodePositions.empty()) return;

    // If panel size changed, recalculate positions
    static wxSize lastSize = drawPanel->GetSize();
    wxSize currentSize = drawPanel->GetSize();
    if (currentSize != lastSize) {
        CalculateNodePositions();
        lastSize = currentSize;
    }

    // Draw edges first
    dc.SetPen(wxPen(*wxBLACK, 1));
    for (int i = 0; i < graph->getNumVertices(); i++) {
        for (int j = i + 1; j < graph->getNumVertices(); j++) {
            int weight = graph->getEdgeWeight(i, j);
            if (weight > 0) {
                // Draw edge
                dc.DrawLine(nodePositions[i], nodePositions[j]);

                // Calculate midpoint with offset to avoid overlap
                wxPoint mid((nodePositions[i].x + nodePositions[j].x) / 2,
                    (nodePositions[i].y + nodePositions[j].y) / 2);

                // Add slight offset to weight text to avoid overlapping lines
                double dx = nodePositions[j].x - nodePositions[i].x;
                double dy = nodePositions[j].y - nodePositions[i].y;
                double angle = atan2(dy, dx);

                // Offset perpendicular to the edge
                int offset = 15;  // Pixels to offset the text
                mid.x += offset * sin(angle);
                mid.y -= offset * cos(angle);

                wxString weightStr = wxString::Format("%d", weight);
                wxSize textSize = dc.GetTextExtent(weightStr);

                // Draw weight with white background for better visibility
                dc.SetBrush(*wxWHITE_BRUSH);
                dc.DrawRectangle(mid.x - textSize.GetWidth() / 2 - 2,
                    mid.y - textSize.GetHeight() / 2 - 2,
                    textSize.GetWidth() + 4,
                    textSize.GetHeight() + 4);
                dc.DrawText(weightStr,
                    mid.x - textSize.GetWidth() / 2,
                    mid.y - textSize.GetHeight() / 2);
            }
        }
    }

    // Draw nodes with yellow fill
    dc.SetPen(wxPen(*wxBLACK, 2));
    dc.SetBrush(wxBrush(wxColour(255, 255, 0)));
    dc.SetTextForeground(*wxBLACK);

    for (int i = 0; i < nodePositions.size(); i++) {
        // Draw node circle
        dc.DrawCircle(nodePositions[i], NODE_RADIUS);

        // Draw node number
        wxString label = wxString::Format("%d", i);
        wxSize textSize = dc.GetTextExtent(label);
        dc.DrawText(label,
            nodePositions[i].x - textSize.GetWidth() / 2,
            nodePositions[i].y - textSize.GetHeight() / 2);
    }
}

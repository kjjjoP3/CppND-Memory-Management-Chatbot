#include <wx/filename.h>
#include <wx/colour.h>
#include <wx/image.h>
#include <string>
#include "chatbot.h"
#include "chatlogic.h"
#include "chatgui.h"

#include <memory>

// size of chatbot window
const int WIDTH = 414;
const int HEIGHT = 736;

// wxWidgets APP
IMPLEMENT_APP(ChatBotApp);

std::string DATA_PATH = "../";
std::string IMAGE_BASE_PATH = DATA_PATH + "images/";

bool ChatBotApp::OnInit()
{
    // Create window with title and show it
    auto chatBotFrame = new ChatBotFrame(wxT("Udacity ChatBot"));
    chatBotFrame->Show(true);

    return true;
}

// wxWidgets FRAME
ChatBotFrame::ChatBotFrame(const wxString &title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(WIDTH, HEIGHT))
{
    // Create panel with background image
    auto ctrlPanel = new ChatBotFrameImagePanel(this);

    // Create controls and assign them to control panel
    _panelDialog = new ChatBotPanelDialog(ctrlPanel, wxID_ANY);

    // Create text control for user input
    const int textCtrlID = 1;
    _userTextCtrl = new wxTextCtrl(ctrlPanel, textCtrlID, "", wxDefaultPosition, wxSize(WIDTH, 50), wxTE_PROCESS_ENTER, wxDefaultValidator, wxTextCtrlNameStr);
    Connect(textCtrlID, wxEVT_TEXT_ENTER, wxCommandEventHandler(ChatBotFrame::OnEnter));

    // Create vertical sizer for panel alignment and add panels
    auto vertBoxSizer = new wxBoxSizer(wxVERTICAL);
    vertBoxSizer->AddSpacer(90);
    vertBoxSizer->Add(_panelDialog, 6, wxEXPAND | wxALL, 0);
    vertBoxSizer->Add(_userTextCtrl, 1, wxEXPAND | wxALL, 5);
    ctrlPanel->SetSizer(vertBoxSizer);

    // Position window in screen center
    this->Centre();
}

void ChatBotFrame::OnEnter(wxCommandEvent &WXUNUSED(event))
{
    // Retrieve text from user input
    wxString userText = _userTextCtrl->GetLineText(0);

    // Add new user text to dialog
    _panelDialog->AddDialogItem(userText, true);

    // Clear the text control
    _userTextCtrl->Clear();

    // Send user text to chatbot 
    _panelDialog->GetChatLogicHandle()->SendMessageToChatbot(std::string(userText.mb_str()));
}

BEGIN_EVENT_TABLE(ChatBotFrameImagePanel, wxPanel)
EVT_PAINT(ChatBotFrameImagePanel::paintEvent) // Catch paint events
END_EVENT_TABLE()

ChatBotFrameImagePanel::ChatBotFrameImagePanel(wxFrame *parent) : wxPanel(parent)
{
}

void ChatBotFrameImagePanel::paintEvent(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void ChatBotFrameImagePanel::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void ChatBotFrameImagePanel::render(wxDC &dc)
{
    // Load background image from file
    wxString imgFile = IMAGE_BASE_PATH + "sf_bridge.jpg";
    wxImage image;
    image.LoadFile(imgFile);

    // Rescale image to fit window dimensions
    wxSize sz = this->GetSize();
    wxImage imgSmall = image.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);
    _image = wxBitmap(imgSmall);
    
    dc.DrawBitmap(_image, 0, 0, false);
}

BEGIN_EVENT_TABLE(ChatBotPanelDialog, wxPanel)
EVT_PAINT(ChatBotPanelDialog::paintEvent) // Catch paint events
END_EVENT_TABLE()

ChatBotPanelDialog::ChatBotPanelDialog(wxWindow *parent, wxWindowID id)
    : wxScrolledWindow(parent, id)
{
    // Sizer will take care of determining the needed scroll size
    _dialogSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_dialogSizer);

    // Allow for PNG images to be handled
    wxInitAllImageHandlers();

    // Create chat logic instance using smart pointer
    _chatLogic = std::make_unique<ChatLogic>();

    // Pass pointer to chatbot dialog so answers can be displayed in GUI
    _chatLogic->SetPanelDialogHandle(this);

    // Load answer graph from file
    _chatLogic->LoadAnswerGraphFromFile(DATA_PATH + "src/answergraph.txt");
}

ChatBotPanelDialog::~ChatBotPanelDialog()
{
    // Destructor logic is now handled by smart pointer, no need to delete manually
}

void ChatBotPanelDialog::AddDialogItem(wxString text, bool isFromUser)
{
    // Add a single dialog element to the sizer
    auto item = new ChatBotPanelDialogItem(this, text, isFromUser);
    _dialogSizer->Add(item, 0, wxALL | (isFromUser ? wxALIGN_LEFT : wxALIGN_RIGHT), 8);
    _dialogSizer->Layout();

    // Make scrollbar show up
    this->FitInside(); // Ask the sizer about the needed size
    this->SetScrollRate(5, 5);
    this->Layout();

    // Scroll to bottom to show newest element
    int dx, dy;
    this->GetScrollPixelsPerUnit(&dx, &dy);
    int sy = dy * this->GetScrollLines(wxVERTICAL);
    this->DoScroll(0, sy);
}

void ChatBotPanelDialog::PrintChatbotResponse(std::string response)
{
    // Convert string into wxString and add dialog element
    wxString botText(response.c_str(), wxConvUTF8);
    AddDialogItem(botText, false);
}

void ChatBotPanelDialog::paintEvent(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void ChatBotPanelDialog::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void ChatBotPanelDialog::render(wxDC &dc)
{
    wxImage image;
    image.LoadFile(IMAGE_BASE_PATH + "sf_bridge_inner.jpg");

    wxSize sz = this->GetSize();
    wxImage imgSmall = image.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);

    _image = wxBitmap(imgSmall);
    dc.DrawBitmap(_image, 0, 0, false);
}

ChatBotPanelDialogItem::ChatBotPanelDialogItem(wxPanel *parent, wxString text, bool isFromUser)
    : wxPanel(parent, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_NONE)
{
    // Retrieve image from chatbot
    wxBitmap *bitmap = isFromUser ? nullptr : ((ChatBotPanelDialog*)parent)->GetChatLogicHandle()->GetImageFromChatbot(); 

    // Create image and text
    _chatBotImg = new wxStaticBitmap(this, wxID_ANY, (isFromUser ? wxBitmap(IMAGE_BASE_PATH + "user.png", wxBITMAP_TYPE_PNG) : *bitmap), wxPoint(-1, -1), wxSize(-1, -1));
    _chatBotTxt = new wxStaticText(this, wxID_ANY, text, wxPoint(-1, -1), wxSize(150, -1), wxALIGN_CENTRE | wxBORDER_NONE);
    _chatBotTxt->SetForegroundColour(isFromUser ? wxColor(*wxBLACK) : wxColor(*wxWHITE));

    // Create sizer and add elements
    auto horzBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    horzBoxSizer->Add(_chatBotTxt, 8, wxEXPAND | wxALL, 1);
    horzBoxSizer->Add(_chatBotImg, 2, wxEXPAND | wxALL, 1);
    this->SetSizer(horzBoxSizer);

    // Wrap text after 150 pixels
    _chatBotTxt->Wrap(150);

    // Set background color
    this->SetBackgroundColour(isFromUser ? wxT("YELLOW") : wxT("BLUE"));
}

#include <wx/filename.h>
#include <wx/colour.h>
#include <wx/image.h>
#include <string>
#include "chatbot.h"
#include "chatlogic.h"
#include "chatgui.h"

// Dimensions for ChatBot application window
const int windowWidth = 414;
const int windowHeight = 736;

// wxWidgets APP
IMPLEMENT_APP(ChatBotApp);

std::string projectDataPath = "../";
std::string imageBasePath = projectDataPath + "images/";

bool ChatBotApp::OnInit()
{
    // Instantiate main frame for chatbot and display it
    ChatBotFrame *mainFrame = new ChatBotFrame(wxT("Udacity ChatBot"));
    mainFrame->Show(true);

    return true;
}

// wxWidgets FRAME
ChatBotFrame::ChatBotFrame(const wxString &title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(windowWidth, windowHeight))
{
    // Create primary panel with background
    ChatBotFrameImagePanel *primaryPanel = new ChatBotFrameImagePanel(this);

    // Initialize dialog panel within main panel
    _panelDialog = new ChatBotPanelDialog(primaryPanel, wxID_ANY);

    // Setup input text control for user
    int inputTextId = 1;
    _userTextCtrl = new wxTextCtrl(primaryPanel, inputTextId, "", wxDefaultPosition, wxSize(windowWidth, 50), wxTE_PROCESS_ENTER, wxDefaultValidator, wxTextCtrlNameStr);
    Connect(inputTextId, wxEVT_TEXT_ENTER, wxCommandEventHandler(ChatBotFrame::OnEnter));

    // Setup vertical layout and arrange components
    wxBoxSizer *verticalSizer = new wxBoxSizer(wxVERTICAL);
    verticalSizer->AddSpacer(90);
    verticalSizer->Add(_panelDialog, 6, wxEXPAND | wxALL, 0);
    verticalSizer->Add(_userTextCtrl, 1, wxEXPAND | wxALL, 5);
    primaryPanel->SetSizer(verticalSizer);

    // Center the main window
    this->Centre();
}

void ChatBotFrame::OnEnter(wxCommandEvent &WXUNUSED(event))
{
    // Get user input text
    wxString userTextInput = _userTextCtrl->GetLineText(0);

    // Add user text to dialog display
    _panelDialog->AddDialogItem(userTextInput, true);

    // Clear text input after sending
    _userTextCtrl->Clear();

    // Send user text to chatbot for response
    _panelDialog->GetChatLogicHandle()->SendMessageToChatbot(std::string(userTextInput.mb_str()));
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
    // Load background image from file path
    wxString imageFilePath = imageBasePath + "sf_bridge.jpg";
    wxImage img;
    img.LoadFile(imageFilePath);

    // Rescale image to fit window dimensions
    wxSize panelSize = this->GetSize();
    wxImage resizedImage = img.Rescale(panelSize.GetWidth(), panelSize.GetHeight(), wxIMAGE_QUALITY_HIGH);
    _image = wxBitmap(resizedImage);
    
    dc.DrawBitmap(_image, 0, 0, false);
}

BEGIN_EVENT_TABLE(ChatBotPanelDialog, wxPanel)
EVT_PAINT(ChatBotPanelDialog::paintEvent) // Catch paint events
END_EVENT_TABLE()

ChatBotPanelDialog::ChatBotPanelDialog(wxWindow *parent, wxWindowID id)
    : wxScrolledWindow(parent, id)
{
    // Setup sizer for handling scrolling
    _dialogSizer = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(_dialogSizer);

    // Initialize PNG support for images
    wxInitAllImageHandlers();

    // Create chat logic and connect to dialog panel
    _chatLogic = std::make_unique<ChatLogic>();
    _chatLogic->SetPanelDialogHandle(this);

    // Load answer graph for chatbot
    _chatLogic->LoadAnswerGraphFromFile(projectDataPath + "src/answergraph.txt");
}

ChatBotPanelDialog::~ChatBotPanelDialog()
{
    std::cout << "ChatBotPanelDialog destructor" << std::endl;
}

void ChatBotPanelDialog::AddDialogItem(wxString text, bool isFromUser)
{
    // Create and add a dialog element to display text
    ChatBotPanelDialogItem *dialogItem = new ChatBotPanelDialogItem(this, text, isFromUser);
    _dialogSizer->Add(dialogItem, 0, wxALL | (isFromUser ? wxALIGN_LEFT : wxALIGN_RIGHT), 8);
    _dialogSizer->Layout();

    // Enable scrolling
    this->FitInside();
    this->SetScrollRate(5, 5);
    this->Layout();

    // Scroll to show the latest dialog
    int xScroll, yScroll;
    this->GetScrollPixelsPerUnit(&xScroll, &yScroll);
    int scrollLength = yScroll * this->GetScrollLines(wxVERTICAL);
    this->DoScroll(0, scrollLength);
}

void ChatBotPanelDialog::PrintChatbotResponse(std::string response)
{
    // Display chatbot response as dialog item
    wxString botReplyText(response.c_str(), wxConvUTF8);
    AddDialogItem(botReplyText, false);
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
    wxImage img;
    img.LoadFile(imageBasePath + "sf_bridge_inner.jpg");

    wxSize currentSize = this->GetSize();
    wxImage resizedImg = img.Rescale(currentSize.GetWidth(), currentSize.GetHeight(), wxIMAGE_QUALITY_HIGH);

    _image = wxBitmap(resizedImg);
    dc.DrawBitmap(_image, 0, 0, false);
}

ChatBotPanelDialogItem::ChatBotPanelDialogItem(wxPanel *parent, wxString text, bool isFromUser)
    : wxPanel(parent, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_NONE)
{
    // Choose image based on message sender
    wxBitmap *displayImage = isFromUser ? nullptr : ((ChatBotPanelDialog*)parent)->GetChatLogicHandle()->GetImageFromChatbot();

    // Create image and text elements for dialog item
    _chatBotImg = new wxStaticBitmap(this, wxID_ANY, (isFromUser ? wxBitmap(imageBasePath + "user.png", wxBITMAP_TYPE_PNG) : *displayImage), wxPoint(-1, -1), wxSize(-1, -1));
    _chatBotTxt = new wxStaticText(this, wxID_ANY, text, wxPoint(-1, -1), wxSize(150, -1), wxALIGN_CENTRE | wxBORDER_NONE);
    _chatBotTxt->SetForegroundColour(isFromUser ? wxColor(*wxBLACK) : wxColor(*wxWHITE));

    // Setup horizontal sizer and add text and image
    wxBoxSizer *horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
    horizontalSizer->Add(_chatBotTxt, 8, wxEXPAND | wxALL, 1);
    horizontalSizer->Add(_chatBotImg, 2, wxEXPAND | wxALL, 1);
    this->SetSizer(horizontalSizer);

    // Wrap text to fit within the specified width
    _chatBotTxt->Wrap(150);

    // Set background color for dialog item
    this->SetBackgroundColour(isFromUser ? wxT("YELLOW") : wxT("BLUE"));
}

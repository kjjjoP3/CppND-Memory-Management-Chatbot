#include <wx/filename.h>
#include <wx/colour.h>
#include <wx/image.h>
#include <string>
#include "chatbot.h"
#include "chatlogic.h"
#include "chatgui.h"

// Kích thước cửa sổ chatbot
constexpr int WINDOW_WIDTH = 414;
constexpr int WINDOW_HEIGHT = 736;

// Đường dẫn dữ liệu và ảnh
const std::string DATA_PATH = "../";
const std::string IMAGE_PATH = DATA_PATH + "images/";

IMPLEMENT_APP(ChatBotApp);

bool ChatBotApp::OnInit()
{
    // Khởi tạo cửa sổ chính của ứng dụng
    auto chatBotFrame = new ChatBotFrame(wxT("Udacity ChatBot"));
    chatBotFrame->Show(true);
    return true;
}

ChatBotFrame::ChatBotFrame(const wxString &title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(WINDOW_WIDTH, WINDOW_HEIGHT))
{
    // Tạo panel nền
    auto controlPanel = new ChatBotFrameImagePanel(this);
    _panelDialog = new ChatBotPanelDialog(controlPanel, wxID_ANY);

    // Tạo trường nhập liệu cho người dùng
    int userTextID = 1;
    _userTextCtrl = new wxTextCtrl(controlPanel, userTextID, "", wxDefaultPosition, wxSize(WINDOW_WIDTH, 50), wxTE_PROCESS_ENTER);
    Connect(userTextID, wxEVT_TEXT_ENTER, wxCommandEventHandler(ChatBotFrame::OnEnter));

    // Sử dụng vertical sizer cho căn chỉnh
    auto verticalSizer = new wxBoxSizer(wxVERTICAL);
    verticalSizer->AddSpacer(90);
    verticalSizer->Add(_panelDialog, 6, wxEXPAND | wxALL, 0);
    verticalSizer->Add(_userTextCtrl, 1, wxEXPAND | wxALL, 5);
    controlPanel->SetSizer(verticalSizer);

    Centre();
}

void ChatBotFrame::OnEnter(wxCommandEvent &WXUNUSED(event))
{
    wxString userInput = _userTextCtrl->GetLineText(0);
    _panelDialog->AddDialogItem(userInput, true);
    _userTextCtrl->Clear();
    _panelDialog->GetChatLogicHandle()->SendMessageToChatbot(std::string(userInput.mb_str()));
}

BEGIN_EVENT_TABLE(ChatBotFrameImagePanel, wxPanel)
EVT_PAINT(ChatBotFrameImagePanel::paintEvent)
END_EVENT_TABLE()

ChatBotFrameImagePanel::ChatBotFrameImagePanel(wxFrame *parent) : wxPanel(parent) {}

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
    wxString imgFile = IMAGE_PATH + "sf_bridge.jpg";
    wxImage image;
    if (image.LoadFile(imgFile))
    {
        wxImage scaledImage = image.Scale(GetSize().GetWidth(), GetSize().GetHeight(), wxIMAGE_QUALITY_HIGH);
        _image = wxBitmap(scaledImage);
        dc.DrawBitmap(_image, 0, 0, false);
    }
}

BEGIN_EVENT_TABLE(ChatBotPanelDialog, wxPanel)
EVT_PAINT(ChatBotPanelDialog::paintEvent)
END_EVENT_TABLE()

ChatBotPanelDialog::ChatBotPanelDialog(wxWindow *parent, wxWindowID id)
    : wxScrolledWindow(parent, id), _dialogSizer(new wxBoxSizer(wxVERTICAL))
{
    SetSizer(_dialogSizer);
    wxInitAllImageHandlers();
    _chatLogic = std::make_unique<ChatLogic>();
    _chatLogic->SetPanelDialogHandle(this);
    _chatLogic->LoadAnswerGraphFromFile(DATA_PATH + "src/answergraph.txt");
}

ChatBotPanelDialog::~ChatBotPanelDialog()
{
    std::cout << "ChatBotPanelDialog destructor" << std::endl;
}

void ChatBotPanelDialog::AddDialogItem(wxString text, bool isFromUser)
{
    auto *dialogItem = new ChatBotPanelDialogItem(this, text, isFromUser);
    _dialogSizer->Add(dialogItem, 0, wxALL | (isFromUser ? wxALIGN_LEFT : wxALIGN_RIGHT), 8);
    _dialogSizer->Layout();

    FitInside();
    SetScrollRate(5, 5);
    Layout();

    int dx, dy;
    GetScrollPixelsPerUnit(&dx, &dy);
    DoScroll(0, dy * GetScrollLines(wxVERTICAL));
}

void ChatBotPanelDialog::PrintChatbotResponse(std::string response)
{
    AddDialogItem(wxString::FromUTF8(response.c_str()), false);
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
    if (image.LoadFile(IMAGE_PATH + "sf_bridge_inner.jpg"))
    {
        wxImage scaledImage = image.Scale(GetSize().GetWidth(), GetSize().GetHeight(), wxIMAGE_QUALITY_HIGH);
        _image = wxBitmap(scaledImage);
        dc.DrawBitmap(_image, 0, 0, false);
    }
}

ChatBotPanelDialogItem::ChatBotPanelDialogItem(wxPanel *parent, wxString text, bool isFromUser)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
{
    wxBitmap *bitmap = isFromUser ? nullptr : ((ChatBotPanelDialog*)parent)->GetChatLogicHandle()->GetImageFromChatbot();
    _chatBotImg = new wxStaticBitmap(this, wxID_ANY, isFromUser ? wxBitmap(IMAGE_PATH + "user.png", wxBITMAP_TYPE_PNG) : *bitmap);
    _chatBotTxt = new wxStaticText(this, wxID_ANY, text, wxDefaultPosition, wxSize(150, -1), wxALIGN_CENTRE);

    _chatBotTxt->SetForegroundColour(isFromUser ? *wxBLACK : *wxWHITE);

    auto *horizontalSizer = new wxBoxSizer(wxHORIZONTAL);
    horizontalSizer->Add(_chatBotTxt, 8, wxEXPAND | wxALL, 1);
    horizontalSizer->Add(_chatBotImg, 2, wxEXPAND | wxALL, 1);
    SetSizer(horizontalSizer);
    _chatBotTxt->Wrap(150);

    SetBackgroundColour(isFromUser ? "YELLOW" : "BLUE");
}

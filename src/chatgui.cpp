#include <wx/filename.h>
#include <wx/colour.h>
#include <wx/image.h>
#include <string>
#include "chatbot.h"
#include "chatlogic.h"
#include "chatgui.h"

// Kích thước cửa sổ chatbot
const int windowWidth = 414;
const int windowHeight = 736;

// Ứng dụng wxWidgets
IMPLEMENT_APP(ChatBotApp);

std::string resourcePath = "../";  // Đường dẫn đến tài nguyên
std::string imageResourcePath = resourcePath + "images/";

bool ChatBotApp::OnInit()
{
    // Tạo cửa sổ với tên và hiển thị nó
    ChatBotFrame* chatBotFrame = new ChatBotFrame(wxT("Udacity ChatBot"));
    chatBotFrame->Show(true);

    return true;
}

// Cửa sổ chính wxWidgets
ChatBotFrame::ChatBotFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(windowWidth, windowHeight))
{
    // Tạo panel chứa hình nền
    ChatBotFrameImagePanel* controlPanel = new ChatBotFrameImagePanel(this);

    // Tạo các điều khiển và gán chúng vào panel
    _panelDialog = new ChatBotPanelDialog(controlPanel, wxID_ANY);

    // Tạo ô nhập văn bản cho người dùng
    int textCtrlId = 1;
    _userTextCtrl = new wxTextCtrl(controlPanel, textCtrlId, "", wxDefaultPosition, wxSize(windowWidth, 50), wxTE_PROCESS_ENTER);
    Connect(textCtrlId, wxEVT_TEXT_ENTER, wxCommandEventHandler(ChatBotFrame::OnEnter));

    // Tạo sizer dọc cho việc căn chỉnh các phần tử và thêm các panel
    wxBoxSizer* vertSizer = new wxBoxSizer(wxVERTICAL);
    vertSizer->AddSpacer(90);
    vertSizer->Add(_panelDialog, 6, wxEXPAND | wxALL, 0);
    vertSizer->Add(_userTextCtrl, 1, wxEXPAND | wxALL, 5);
    controlPanel->SetSizer(vertSizer);

    // Căn giữa cửa sổ trên màn hình
    this->Centre();
}

void ChatBotFrame::OnEnter(wxCommandEvent& event)
{
    // Lấy văn bản từ ô nhập liệu
    wxString userInput = _userTextCtrl->GetLineText(0);

    // Thêm văn bản của người dùng vào dialog
    _panelDialog->AddDialogItem(userInput, true);

    // Xóa văn bản trong ô nhập liệu
    _userTextCtrl->Clear();

    // Gửi văn bản người dùng tới chatbot
    _panelDialog->GetChatLogicHandle()->SendMessageToChatbot(std::string(userInput.mb_str()));
}

BEGIN_EVENT_TABLE(ChatBotFrameImagePanel, wxPanel)
EVT_PAINT(ChatBotFrameImagePanel::paintEvent) // Bắt sự kiện vẽ
END_EVENT_TABLE()

ChatBotFrameImagePanel::ChatBotFrameImagePanel(wxFrame* parent) : wxPanel(parent)
{
}

void ChatBotFrameImagePanel::paintEvent(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    render(dc);
}

void ChatBotFrameImagePanel::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void ChatBotFrameImagePanel::render(wxDC& dc)
{
    // Tải hình nền từ file
    wxString imagePath = imageResourcePath + "sf_bridge.jpg";
    wxImage image;
    image.LoadFile(imagePath);

    // Thay đổi kích thước hình ảnh sao cho vừa với cửa sổ
    wxSize windowSize = this->GetSize();
    wxImage scaledImage = image.Rescale(windowSize.GetWidth(), windowSize.GetHeight(), wxIMAGE_QUALITY_HIGH);
    _image = wxBitmap(scaledImage);

    dc.DrawBitmap(_image, 0, 0, false);
}

BEGIN_EVENT_TABLE(ChatBotPanelDialog, wxPanel)
EVT_PAINT(ChatBotPanelDialog::paintEvent) // Bắt sự kiện vẽ
END_EVENT_TABLE()

ChatBotPanelDialog::ChatBotPanelDialog(wxWindow* parent, wxWindowID id)
    : wxScrolledWindow(parent, id)
{
    // Sizer sẽ tự động quyết định kích thước cuộn
    _dialogSizer = new wxBoxSizer(wxVERTICAL);  
    this->SetSizer(_dialogSizer);

    // Khởi tạo trình xử lý ảnh PNG
    wxInitAllImageHandlers();

    // Tạo đối tượng ChatLogic
    _chatLogic = std::make_unique<ChatLogic>(); // Dùng unique_ptr cho ChatLogic

    // Truyền con trỏ đến dialog để hiển thị câu trả lời
    _chatLogic->SetPanelDialogHandle(this);

    // Tải đồ thị câu trả lời từ file
    _chatLogic->LoadAnswerGraphFromFile(resourcePath + "src/answergraph.txt");
}

ChatBotPanelDialog::~ChatBotPanelDialog()
{
    std::cout << "ChatBotPanelDialog destructor" << std::endl; // Kiểm tra có được gọi hay không
}

void ChatBotPanelDialog::AddDialogItem(wxString text, bool isFromUser)
{
    // Thêm một phần tử dialog vào sizer
    ChatBotPanelDialogItem* item = new ChatBotPanelDialogItem(this, text, isFromUser);
    _dialogSizer->Add(item, 0, wxALL | (isFromUser ? wxALIGN_LEFT : wxALIGN_RIGHT), 8);
    _dialogSizer->Layout();

    // Hiển thị thanh cuộn
    this->FitInside();
    this->SetScrollRate(5, 5);
    this->Layout();

    // Cuộn đến cuối để hiển thị phần tử mới nhất
    int dx, dy;
    this->GetScrollPixelsPerUnit(&dx, &dy);
    int sy = dy * this->GetScrollLines(wxVERTICAL);
    this->DoScroll(0, sy);
}

void ChatBotPanelDialog::PrintChatbotResponse(std::string response)
{
    // Chuyển đổi chuỗi thành wxString và thêm vào dialog
    wxString botResponse(response.c_str(), wxConvUTF8);
    AddDialogItem(botResponse, false);
}

void ChatBotPanelDialog::paintEvent(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    render(dc);
}

void ChatBotPanelDialog::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void ChatBotPanelDialog::render(wxDC& dc)
{
    wxImage image;
    image.LoadFile(imageResourcePath + "sf_bridge_inner.jpg");

    wxSize panelSize = this->GetSize();
    wxImage scaledImage = image.Rescale(panelSize.GetWidth(), panelSize.GetHeight(), wxIMAGE_QUALITY_HIGH);

    _image = wxBitmap(scaledImage);
    dc.DrawBitmap(_image, 0, 0, false);
}

ChatBotPanelDialogItem::ChatBotPanelDialogItem(wxPanel* parent, wxString text, bool isFromUser)
    : wxPanel(parent, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_NONE)
{
    // Tải hình ảnh từ chatbot nếu là câu trả lời từ chatbot
    wxBitmap* bitmap = isFromUser ? nullptr : ((ChatBotPanelDialog*)parent)->GetChatLogicHandle()->GetImageFromChatbot();

    // Tạo hình ảnh và văn bản
    _chatBotImg = new wxStaticBitmap(this, wxID_ANY, (isFromUser ? wxBitmap(imageResourcePath + "user.png", wxBITMAP_TYPE_PNG) : *bitmap), wxPoint(-1, -1), wxSize(-1, -1));
    _chatBotTxt = new wxStaticText(this, wxID_ANY, text, wxPoint(-1, -1), wxSize(150, -1), wxALIGN_CENTRE | wxBORDER_NONE);
    _chatBotTxt->SetForegroundColour(isFromUser ? wxColor(*wxBLACK) : wxColor(*wxWHITE));

    // Tạo sizer ngang và thêm các phần tử
    wxBoxSizer* horzSizer = new wxBoxSizer(wxHORIZONTAL);
    horzSizer->Add(_chatBotTxt, 8, wxEXPAND | wxALL, 1);
    horzSizer->Add(_chatBotImg, 2, wxEXPAND | wxALL, 1);
    this->SetSizer(horzSizer);

    // Gói văn bản lại sau 150 pixel
    _chatBotTxt->Wrap(150);

    // Thiết lập màu nền
    this->SetBackgroundColour(isFromUser ? wxT("YELLOW") : wxT("BLUE"));
}

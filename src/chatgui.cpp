#include <wx/filename.h>
#include <wx/colour.h>
#include <wx/image.h>
#include <string>
#include "chatbot.h"
#include "chatlogic.h"
#include "chatgui.h"

// Kích thước cửa sổ chatbot
const int WIDTH = 414;
const int HEIGHT = 736;

// Khởi tạo ứng dụng wxWidgets
IMPLEMENT_APP(ChatBotApp);

std::string dataPath = "../";
std::string imgBasePath = dataPath + "images/";

bool ChatBotApp::OnInit()
{
    // Tạo cửa sổ với tên và hiển thị
    ChatBotFrame* chatBotFrame = new ChatBotFrame(wxT("Udacity ChatBot"));
    chatBotFrame->Show(true);

    return true;
}

// Cửa sổ chính wxWidgets
ChatBotFrame::ChatBotFrame(const wxString &title) 
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(WIDTH, HEIGHT))
{
    // Tạo panel với hình nền
    ChatBotFrameImagePanel* ctrlPanel = new ChatBotFrameImagePanel(this);

    // Tạo các điều khiển và gán chúng vào panel
    _panelDialog = new ChatBotPanelDialog(ctrlPanel, wxID_ANY);

    // Tạo text control cho người dùng nhập liệu
    int idTextCtrl = 1;
    _userTextCtrl = new wxTextCtrl(ctrlPanel, idTextCtrl, "", wxDefaultPosition, wxSize(WIDTH, 50), wxTE_PROCESS_ENTER, wxDefaultValidator, wxTextCtrlNameStr);
    Connect(idTextCtrl, wxEVT_TEXT_ENTER, wxCommandEventHandler(ChatBotFrame::OnEnter));

    // Tạo sizer theo chiều dọc để sắp xếp các panel
    wxBoxSizer* vertBoxSizer = new wxBoxSizer(wxVERTICAL);
    vertBoxSizer->AddSpacer(90);
    vertBoxSizer->Add(_panelDialog, 6, wxEXPAND | wxALL, 0);
    vertBoxSizer->Add(_userTextCtrl, 1, wxEXPAND | wxALL, 5);
    ctrlPanel->SetSizer(vertBoxSizer);

    // Đặt cửa sổ ở vị trí giữa màn hình
    this->Centre();
}

void ChatBotFrame::OnEnter(wxCommandEvent& event)
{
    // Lấy văn bản từ điều khiển nhập liệu
    wxString userText = _userTextCtrl->GetLineText(0);

    // Thêm văn bản người dùng vào dialog
    _panelDialog->AddDialogItem(userText, true);

    // Xóa văn bản trong text control
    _userTextCtrl->Clear();

    // Gửi văn bản người dùng tới chatbot
    _panelDialog->GetChatLogicHandle()->SendMessageToChatbot(std::string(userText.mb_str()));
}

BEGIN_EVENT_TABLE(ChatBotFrameImagePanel, wxPanel)
EVT_PAINT(ChatBotFrameImagePanel::paintEvent) // Xử lý sự kiện vẽ
END_EVENT_TABLE()

ChatBotFrameImagePanel::ChatBotFrameImagePanel(wxFrame* parent) 
    : wxPanel(parent)
{
}

void ChatBotFrameImagePanel::paintEvent(wxPaintEvent& evt)
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
    // Tải hình nền từ tệp
    wxString imgFile = imgBasePath + "sf_bridge.jpg";
    wxImage image;
    image.LoadFile(imgFile);

    // Chỉnh kích thước hình ảnh phù hợp với kích thước cửa sổ
    wxSize sz = this->GetSize();
    wxImage imgSmall = image.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);
    _image = wxBitmap(imgSmall);
    
    dc.DrawBitmap(_image, 0, 0, false);
}

BEGIN_EVENT_TABLE(ChatBotPanelDialog, wxPanel)
EVT_PAINT(ChatBotPanelDialog::paintEvent) // Xử lý sự kiện vẽ
END_EVENT_TABLE()

ChatBotPanelDialog::ChatBotPanelDialog(wxWindow* parent, wxWindowID id)
    : wxScrolledWindow(parent, id)
{
    // Sizer sẽ tự động tính toán kích thước cần thiết cho thanh cuộn
    _dialogSizer = new wxBoxSizer(wxVERTICAL);  // sử dụng new, nhưng cần phải delete sau khi không còn sử dụng
    this->SetSizer(_dialogSizer);

    // Cho phép xử lý hình ảnh PNG
    wxInitAllImageHandlers();

    // Tạo đối tượng chat logic
    _chatLogic = std::make_unique<ChatLogic>();

    // Gán panel dialog vào chat logic để hiển thị câu trả lời trên giao diện
    _chatLogic->SetPanelDialogHandle(this);

    // Tải đồ thị câu trả lời từ tệp
    _chatLogic->LoadAnswerGraphFromFile(dataPath + "src/answergraph.txt");
}

ChatBotPanelDialog::~ChatBotPanelDialog()
{
    std::cout << "ChatBotPanelDialog destructor" << std::endl; // Xác nhận hàm destructor được gọi
}

void ChatBotPanelDialog::AddDialogItem(wxString text, bool isFromUser)
{
    // Thêm một phần tử dialog vào sizer
    ChatBotPanelDialogItem* item = new ChatBotPanelDialogItem(this, text, isFromUser);
    _dialogSizer->Add(item, 0, wxALL | (isFromUser ? wxALIGN_LEFT : wxALIGN_RIGHT), 8);
    _dialogSizer->Layout();

    // Làm cho thanh cuộn xuất hiện
    this->FitInside();
    this->SetScrollRate(5, 5);
    this->Layout();

    // Cuộn xuống dưới để hiển thị phần tử mới nhất
    int dx, dy;
    this->GetScrollPixelsPerUnit(&dx, &dy);
    int sy = dy * this->GetScrollLines(wxVERTICAL);
    this->DoScroll(0, sy);
}

void ChatBotPanelDialog::PrintChatbotResponse(std::string response)
{
    // Chuyển đổi chuỗi thành wxString và thêm phần tử vào dialog
    wxString botText(response.c_str(), wxConvUTF8);
    AddDialogItem(botText, false);
}

void ChatBotPanelDialog::paintEvent(wxPaintEvent& evt)
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
    image.LoadFile(imgBasePath + "sf_bridge_inner.jpg");

    wxSize sz = this->GetSize();
    wxImage imgSmall = image.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);

    _image = wxBitmap(imgSmall);
    dc.DrawBitmap(_image, 0, 0, false);
}

ChatBotPanelDialogItem::ChatBotPanelDialogItem(wxPanel* parent, wxString text, bool isFromUser)
    : wxPanel(parent, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_NONE)
{
    // Lấy hình ảnh từ chatbot khi tin nhắn đến từ chatbot
    wxBitmap* bitmap = isFromUser ? nullptr : ((ChatBotPanelDialog*)parent)->GetChatLogicHandle()->GetImageFromChatbot(); 

    // Tạo hình ảnh và văn bản
    _chatBotImg = new wxStaticBitmap(this, wxID_ANY, (isFromUser ? wxBitmap(imgBasePath + "user.png", wxBITMAP_TYPE_PNG) : *bitmap), wxPoint(-1, -1), wxSize(-1, -1));
    _chatBotTxt = new wxStaticText(this, wxID_ANY, text, wxPoint(-1, -1), wxSize(150, -1), wxALIGN_CENTRE | wxBORDER_NONE);
    _chatBotTxt->SetForegroundColour(isFromUser ? wxColor(*wxBLACK) : wxColor(*wxWHITE));

    // Tạo sizer ngang và thêm các phần tử vào
    wxBoxSizer* horzBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    horzBoxSizer->Add(_chatBotTxt, 8, wxEXPAND | wxALL, 1);
    horzBoxSizer->Add(_chatBotImg, 2, wxEXPAND | wxALL, 1);
    this->SetSizer(horzBoxSizer);

    // Cắt văn bản sau 150 pixel
    _chatBotTxt->Wrap(150);

    // Đặt màu nền
    this->SetBackgroundColour(isFromUser ? wxT("YELLOW") : wxT("BLUE"));
}

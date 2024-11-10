#ifndef CHATGUI_H_
#define CHATGUI_H_

#include <wx/wx.h>  // app.h trong wx là nơi có main()
#include <memory>   // AC thêm để có thể dùng smart pointers
#include <iostream> // debug

class ChatLogic; // khai báo trước lớp ChatLogic

// Phần giữa cửa sổ chứa đối thoại giữa người dùng và chatbot
class ChatBotPanelDialog : public wxScrolledWindow
{
private:
    wxBoxSizer* _dialogSizer;  // Đối tượng sizer cho dialog
    std::unique_ptr<ChatLogic> _chatLogic; // Chỉ con trỏ thông minh cho ChatLogic
    wxBitmap _image;  // Bitmap cho hình ảnh

public:
    ChatBotPanelDialog(wxWindow* parent, wxWindowID id);
    virtual ~ChatBotPanelDialog();

    void AddDialogItem(wxString text, bool isFromUser); 
    void PrintChatbotResponse(std::string response);

    void paintEvent(wxPaintEvent& event);  // Phương thức vẽ
    void paintNow();  // Phương thức vẽ ngay
    void render(wxDC& dc); // Phương thức xử lý vẽ

    // Lấy đối tượng ChatLogic
    ChatLogic* GetChatLogicHandle() const { return _chatLogic.get(); }
};

// Phần chính giao diện người dùng chứa các thành phần như nút và văn bản người dùng
class ChatBotFrame : public wxFrame
{
private:
    wxTextCtrl* _userTextCtrl;   // Ô nhập văn bản
    ChatBotPanelDialog* _panelDialog; // Panel cho đối thoại

public:
    ChatBotFrame(const wxString& title);
    void OnEnter(wxCommandEvent& event);
};

// Panel chứa hình nền
class ChatBotFrameImagePanel : public wxPanel
{
private:
    wxBitmap _image;

public:
    ChatBotFrameImagePanel(wxFrame* parent);
    void paintEvent(wxPaintEvent& event);  // Phương thức vẽ
    void paintNow(); // Phương thức vẽ ngay
    void render(wxDC& dc); // Phương thức xử lý vẽ
};

// Một phần tử trong cửa sổ chat, sẽ chứa cả hình ảnh và văn bản
class ChatBotPanelDialogItem : public wxPanel
{
private:
    wxStaticBitmap* _chatBotImg; // Hình ảnh chatbot hoặc người dùng
    wxStaticText* _chatBotTxt;   // Văn bản của chatbot hoặc người dùng

public:
    ChatBotPanelDialogItem(wxPanel* parent, wxString text, bool isFromUser);
};

#endif // CHATGUI_H_

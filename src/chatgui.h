#ifndef CHATGUI_H_
#define CHATGUI_H_

#include <wx/wx.h>
#include <memory>

class ChatLogic; // forward declaration

// Panel chính trong cửa sổ hiển thị cuộc trò chuyện giữa người dùng và chatbot
class ChatBotPanelDialog : public wxScrolledWindow
{
private:
    // Các điều khiển giao diện
    wxBoxSizer *_dialogSizer;
    wxBitmap _image;  // Hình ảnh hiển thị

    // Kết nối với logic của Chatbot
    std::unique_ptr<ChatLogic> _chatLogic;

public:
    // Constructor và Destructor
    ChatBotPanelDialog(wxWindow *parent, wxWindowID id);
    ~ChatBotPanelDialog();

    // Getter cho ChatLogic
    ChatLogic* GetChatLogicHandle() { return _chatLogic.get(); }

    // Các sự kiện
    void paintEvent(wxPaintEvent &evt);
    void paintNow();
    void render(wxDC &dc);

    // Các hàm nội bộ
    void AddDialogItem(wxString text, bool isFromUser = true);
    void PrintChatbotResponse(std::string response);

    DECLARE_EVENT_TABLE()
};

// Đối tượng hiển thị một phần cuộc hội thoại trong ChatBotPanelDialog
class ChatBotPanelDialogItem : public wxPanel
{
private:
    // Các điều khiển giao diện
    wxStaticBitmap *_chatBotImg;
    wxStaticText *_chatBotTxt;

public:
    // Constructor và Destructor
    ChatBotPanelDialogItem(wxPanel *parent, wxString text, bool isFromUser);
};

// Khung chứa tất cả các điều khiển
class ChatBotFrame : public wxFrame
{
private:
    // Các điều khiển giao diện
    ChatBotPanelDialog *_panelDialog;
    wxTextCtrl *_userTextCtrl;

    // Xử lý sự kiện khi người dùng nhấn Enter
    void OnEnter(wxCommandEvent &WXUNUSED(event));

public:
    // Constructor và Destructor
    ChatBotFrame(const wxString &title);
};

// Panel hiển thị hình nền trong khung ChatBotFrame
class ChatBotFrameImagePanel : public wxPanel
{
    // Các điều khiển giao diện
    wxBitmap _image;

public:
    // Constructor và Destructor
    ChatBotFrameImagePanel(wxFrame *parent);

    // Các sự kiện
    void paintEvent(wxPaintEvent &evt);
    void paintNow();
    void render(wxDC &dc);

    DECLARE_EVENT_TABLE()
};

// Ứng dụng wxWidgets, không cần hàm main()
class ChatBotApp : public wxApp
{
public:
    // Khởi tạo ứng dụng
    virtual bool OnInit();
};

#endif /* CHATGUI_H_ */

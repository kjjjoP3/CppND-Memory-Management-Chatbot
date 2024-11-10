#ifndef CHATGUI_H_
#define CHATGUI_H_

#include <wx/wx.h>
#include <memory> 
#include <iostream> 

class ChatLogic; 

// Lớp panel chứa các đối thoại giữa người dùng và chatbot
class ChatBotPanelDialog : public wxScrolledWindow
{
private:
    // Các phần tử điều khiển
    wxBoxSizer* _dialogSizer;
    wxBitmap _image;

    // Chỉ số hình ảnh của chatbot
    wxStaticBitmap* _chatBotImg;

    // Text để hiển thị đối thoại
    wxStaticText* _chatBotTxt;
    
public:
    ChatBotPanelDialog(wxWindow* parent, wxWindowID id);
    ~ChatBotPanelDialog();

    // Thêm phần tử đối thoại vào panel
    void AddDialogItem(wxString text, bool isFromUser);
    void PrintChatbotResponse(std::string response);
    void render(wxDC& dc);

    // Nhận điều khiển chat logic
    ChatLogic* GetChatLogicHandle() { return _chatLogic.get(); }

private:
    // Nhận đối tượng từ chatbot
    void paintEvent(wxPaintEvent& evt);
    void paintNow();
};

// Lớp chính hiển thị cửa sổ chatbot
class ChatBotFrame : public wxFrame
{
private:
    // Các đối tượng cần thiết cho frame
    ChatBotPanelDialog* _panelDialog;
    wxTextCtrl* _userTextCtrl;

public:
    ChatBotFrame(const wxString &title);
    void OnEnter(wxCommandEvent& event);
};

// Panel chứa hình nền cho cửa sổ
class ChatBotFrameImagePanel : public wxPanel
{
private:
    wxBitmap _image;

public:
    ChatBotFrameImagePanel(wxFrame* parent);

    // Vẽ hình ảnh lên cửa sổ
    void render(wxDC& dc);
    void paintEvent(wxPaintEvent& evt);
    void paintNow();
};

#endif /* CHATGUI_H_ */

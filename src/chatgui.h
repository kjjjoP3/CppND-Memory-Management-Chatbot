#ifndef CHATGUI_H_
#define CHATGUI_H_

#include <wx/wx.h>
#include <memory>

class ChatLogic;

class ChatBotPanelDialog : public wxScrolledWindow
{
private:
    wxBoxSizer *_dialogSizer;
    wxBitmap _image;
    std::unique_ptr<ChatLogic> _chatLogic;

public:
    ChatBotPanelDialog(wxWindow *parent, wxWindowID id);
    ~ChatBotPanelDialog();

    ChatLogic *GetChatLogicHandle() { return _chatLogic.get(); }

    void paintEvent(wxPaintEvent &evt);
    void paintNow();
    void render(wxDC &dc);
    void AddDialogItem(wxString text, bool isFromUser = true);
    void PrintChatbotResponse(std::string response);

    DECLARE_EVENT_TABLE()
};

class ChatBotPanelDialogItem : public wxPanel
{
private:
    wxStaticBitmap *_chatBotImg;
    wxStaticText *_chatBotTxt;

public:
    ChatBotPanelDialogItem(wxPanel *parent, wxString text, bool isFromUser);
};

class ChatBotFrame : public wxFrame
{
private:
    ChatBotPanelDialog *_panelDialog;
    wxTextCtrl *_userTextCtrl;

    void OnEnter(wxCommandEvent &WXUNUSED(event));

public:
    ChatBotFrame(const wxString &title);
};

class ChatBotFrameImagePanel : public wxPanel
{
    wxBitmap _image;

public:
    ChatBotFrameImagePanel(wxFrame *parent);

    void paintEvent(wxPaintEvent &evt);
    void paintNow();
    void render(wxDC &dc);

    DECLARE_EVENT_TABLE()
};

class ChatBotApp : public wxApp
{
public:
    virtual bool OnInit();
};

#endif /* CHATGUI_H_ */

#ifndef CHATGUI_H_
#define CHATGUI_H_

#include <wx/wx.h>
#include <memory>

class ChatLogic; // Forward declaration

// Middle part of the window containing the dialog between user and chatbot
class ChatBotPanelDialog : public wxScrolledWindow
{
private:
    // Control elements
    wxBoxSizer *_dialogSizer;
    wxBitmap _image;

    // Smart pointer to chat logic
    std::unique_ptr<ChatLogic> _chatLogic;

public:
    // Constructor / Destructor
    ChatBotPanelDialog(wxWindow *parent, wxWindowID id);
    ~ChatBotPanelDialog();

    // Getter / Setter
    ChatLogic *GetChatLogicHandle() { return _chatLogic.get(); }

    // Events
    void paintEvent(wxPaintEvent &evt);
    void paintNow();
    void render(wxDC &dc);

    // Proprietary functions
    void AddDialogItem(wxString text, bool isFromUser = true);
    void PrintChatbotResponse(std::string response);

    DECLARE_EVENT_TABLE()
};

// Dialog item shown in ChatBotPanelDialog
class ChatBotPanelDialogItem : public wxPanel
{
private:
    // Control elements
    wxStaticText *_chatBotTxt;
    wxStaticBitmap *_chatBotImg;

public:
    // Constructor
    ChatBotPanelDialogItem(wxPanel *parent, wxString text, bool isFromUser);

    // Event handling
    void paintEvent(wxPaintEvent &evt);
    void paintNow();
    void render(wxDC &dc);
};

// Panel that holds the background image
class ChatBotFrameImagePanel : public wxPanel
{
private:
    wxBitmap _image;

public:
    // Constructor
    ChatBotFrameImagePanel(wxFrame *parent);

    // Events
    void paintEvent(wxPaintEvent &evt);
    void paintNow();
    void render(wxDC &dc);
};

// Main window
class ChatBotFrame : public wxFrame
{
private:
    // Control elements
    wxTextCtrl *_userTextCtrl;
    ChatBotPanelDialog *_panelDialog;

public:
    // Constructor
    ChatBotFrame(const wxString &title);

    // Event handling
    void OnEnter(wxCommandEvent &event);
    DECLARE_EVENT_TABLE()
};

#endif

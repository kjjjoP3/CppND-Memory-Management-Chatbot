#ifndef CHATLOGIC_H_
#define CHATLOGIC_H_

#include <vector>
#include <string>
#include "chatgui.h"

// forward declarations
class ChatBot;
class GraphEdge;
class GraphNode;

class ChatLogic
{
private:
    // Data handles (owned) - Nodes are owned by ChatLogic class through unique_ptr
    std::vector<std::unique_ptr<GraphNode>> _nodes;

    // The panel dialog that handles communication between the UI and ChatLogic
    ChatBotPanelDialog *_panelDialog = nullptr;

    // The chatbot instance that communicates with the user
    ChatBot *_chatBot = nullptr;

    // Utility function for adding tokens to graph nodes or edges
    template <typename T>
    void AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element);

public:
    // Constructor and Destructor
    ChatLogic();
    ~ChatLogic();

    // Load the answer graph from a file
    void LoadAnswerGraphFromFile(std::string filename);

    // Set the panel dialog handle (used for UI)
    void SetPanelDialogHandle(ChatBotPanelDialog *panelDialog);

    // Set the chatbot handle
    void SetChatbotHandle(ChatBot *chatbot);

    // Send a message to the chatbot
    void SendMessageToChatbot(std::string message);

    // Send a message to the user (UI)
    void SendMessageToUser(std::string message);

    // Get the image associated with the chatbot
    wxBitmap *GetImageFromChatbot();
};

#endif // CHATLOGIC_H_

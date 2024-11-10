#ifndef CHATLOGIC_H_
#define CHATLOGIC_H_

#include <vector>
#include <string>
#include "chatgui.h"
#include <memory>

class ChatBot;
class GraphEdge;
class GraphNode;

class ChatLogic
{
private:
    std::vector<std::unique_ptr<GraphNode>> _nodes;

    GraphNode *_currentNode;
    ChatBot *_chatBot;
    ChatBotPanelDialog *_panelDialog;

    typedef std::vector<std::pair<std::string, std::string>> tokenlist;

    template <typename T>
    void AddTokensToElement(std::string tokenID, tokenlist &tokens, T &element);

public:
    ChatLogic();
    ~ChatLogic();

    void SetPanelDialogHandle(ChatBotPanelDialog *panelDialog);
    void SetChatbotHandle(ChatBot *chatbot);

    void LoadAnswerGraphFromFile(std::string filename);
    void SendMessageToChatbot(std::string message);
    void SendMessageToUser(std::string message);
    wxBitmap *GetImageFromChatbot();
};

#endif /* CHATLOGIC_H_ */

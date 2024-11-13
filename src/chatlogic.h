#ifndef CHATLOGIC_H_
#define CHATLOGIC_H_

#include <vector>
#include <string>
#include "chatgui.h"
#include <memory> // AC add for use of unique_ptr

// forward declarations
class ChatBot;
class GraphEdge;
class GraphNode;

class ChatLogic
{
private:
    //// STUDENT CODE
    ////

    // data handles (owned)
    std::vector< std::unique_ptr<GraphNode> > _nodes;   // AC Task 3
    // std::vector< GraphEdge * > _edges;      // should we do weak pointers? AC
    // std::vector< std::unique_ptr<GraphEdge> > _edges;      // should we do weak pointers? AC
    // because, there will always be one exclusive owner .. AC -- originates from Task 4
    // why do we need this if this is being maintained in the nodes per Task 4? Referred to leandrofahur github

    ////
    //// EOF STUDENT CODE

    // data handles (not owned)
    GraphNode *_currentNode;
    ChatBot *_chatBot;      // AC Task 5
    // std::unique_ptr<ChatBot> _chatBot;  // AC Task 5    // this is wrong - it's owned by the nodes.. so this must be raw..
                                        // but, how do you know the dangling pointer won't be used?
    ChatBotPanelDialog *_panelDialog;

    // proprietary type definitions
    typedef std::vector<std::pair<std::string, std::string>> tokenlist;

    // proprietary functions
    template <typename T>
    void AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element);

public:
    // constructor / destructor
    ChatLogic();
    ~ChatLogic();

    // getter / setter
    void SetPanelDialogHandle(ChatBotPanelDialog *panelDialog);
    void SetChatbotHandle(ChatBot *chatbot);

    // proprietary functions
    void LoadAnswerGraphFromFile(std::string filename);
    void SendMessageToChatbot(std::string message);
    void SendMessageToUser(std::string message);
    wxBitmap *GetImageFromChatbot();
};

#endif /* CHATLOGIC_H_ */
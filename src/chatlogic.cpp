#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>

#include "graphedge.h"
#include "graphnode.h"
#include "chatbot.h"
#include "chatlogic.h"


ChatLogic::ChatLogic()
{
    std::cout << "ChatLogic Constructor" << std::endl;
}

ChatLogic::~ChatLogic()
{
    std::cout << "ChatLogic Destructor" << std::endl;
}

template <typename T>
void ChatLogic::AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element)
{
    for (auto it = tokens.begin(); it != tokens.end(); ++it)
    {
        if (it->first == tokenID)
        {
            element.AddToken(it->second);
        }
    }
}

void ChatLogic::LoadAnswerGraphFromFile(std::string filename)
{
    std::ifstream file(filename);

    if (!file)
    {
        std::cerr << "Error: Cannot open file!" << std::endl;
        return;
    }

    std::string lineStr;
    while (std::getline(file, lineStr))
    {
        tokenlist tokens;
        std::istringstream tokenStream(lineStr);
        std::string token;

        while (std::getline(tokenStream, token, '>'))
        {
            if (token.size() < 2) continue;

            int posTokenFront = token.find("<");
            std::string tokenStr = token.substr(posTokenFront + 1);
            int posTokenInfo = tokenStr.find(":");

            if (posTokenInfo != std::string::npos)
            {
                std::string tokenType = tokenStr.substr(0, posTokenInfo);
                std::string tokenInfo = tokenStr.substr(posTokenInfo + 1);
                tokens.emplace_back(tokenType, tokenInfo);
            }
        }

        auto typeIt = std::find_if(tokens.begin(), tokens.end(), [](const auto &pair) { return pair.first == "TYPE"; });
        if (typeIt != tokens.end())
        {
            auto idIt = std::find_if(tokens.begin(), tokens.end(), [](const auto &pair) { return pair.first == "ID"; });
            if (idIt != tokens.end())
            {
                int id = std::stoi(idIt->second);
                if (typeIt->second == "NODE")
                {
                    auto nodeIt = std::find_if(_nodes.begin(), _nodes.end(), [&id](const auto &node) { return node->GetID() == id; });
                    if (nodeIt == _nodes.end())
                    {
                        _nodes.emplace_back(std::make_unique<GraphNode>(id));
                        nodeIt = _nodes.end() - 1;
                    }
                    AddAllTokensToElement("ANSWER", tokens, **nodeIt);
                }
                else if (typeIt->second == "EDGE")
                {
                    auto parentIt = std::find_if(tokens.begin(), tokens.end(), [](const auto &pair) { return pair.first == "PARENT"; });
                    auto childIt = std::find_if(tokens.begin(), tokens.end(), [](const auto &pair) { return pair.first == "CHILD"; });

                    if (parentIt != tokens.end() && childIt != tokens.end())
                    {
                        int parentID = std::stoi(parentIt->second);
                        int childID = std::stoi(childIt->second);

                        auto parentNode = std::find_if(_nodes.begin(), _nodes.end(), [&parentID](const auto &node) { return node->GetID() == parentID; });
                        auto childNode = std::find_if(_nodes.begin(), _nodes.end(), [&childID](const auto &node) { return node->GetID() == childID; });

                        auto edge = std::make_unique<GraphEdge>(id);
                        edge->SetParentNode(parentNode->get());
                        edge->SetChildNode(childNode->get());
                        AddAllTokensToElement("KEYWORD", tokens, *edge);
                        (*parentNode)->AddEdgeToChildNode(std::move(edge));
                    }
                }
            }
        }
    }

    GraphNode *rootNode = nullptr;
    for (const auto &node : _nodes)
    {
        if (node->GetNumberOfParents() == 0)
        {
            if (rootNode)
            {
                std::cerr << "ERROR: Multiple root nodes detected." << std::endl;
                return;
            }
            rootNode = node.get();
        }
    }

    auto chatBot = std::make_unique<ChatBot>(CB_IMAGE);
    _chatBot = chatBot.get();
    _chatBot->SetChatLogicHandle(this);
    _chatBot->SetRootNode(rootNode);
    rootNode->MoveChatbotHere(std::move(chatBot));
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog)
{
    _panelDialog = panelDialog;
}

void ChatLogic::SendMessageToChatbot(std::string message)
{
    _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(std::string message)
{
    _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot()
{
    return _chatBot->GetImageHandle();
}

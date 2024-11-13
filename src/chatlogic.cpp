#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

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
    auto tokenIt = tokens.begin();
    while ((tokenIt = std::find_if(tokenIt, tokens.end(),
        [&tokenID](const auto &pair) { return pair.first == tokenID; })) != tokens.end())
    {
        element.AddToken(tokenIt->second);
        ++tokenIt;
    }
}

void ChatLogic::LoadAnswerGraphFromFile(std::string filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Error: File could not be opened!" << std::endl;
        return;
    }

    std::string lineStr;
    while (getline(file, lineStr))
    {
        tokenlist tokens;
        while (!lineStr.empty())
        {
            int posTokenFront = lineStr.find("<");
            int posTokenBack = lineStr.find(">");
            if (posTokenFront == -1 || posTokenBack == -1) break;

            std::string tokenStr = lineStr.substr(posTokenFront + 1, posTokenBack - posTokenFront - 1);
            int posTokenInfo = tokenStr.find(":");
            if (posTokenInfo != std::string::npos)
            {
                tokens.emplace_back(tokenStr.substr(0, posTokenInfo), tokenStr.substr(posTokenInfo + 1));
            }

            lineStr.erase(0, posTokenBack + 1);
        }

        auto typeIt = std::find_if(tokens.begin(), tokens.end(), 
            [](const auto &pair) { return pair.first == "TYPE"; });
        if (typeIt == tokens.end()) continue;

        auto idTokenIt = std::find_if(tokens.begin(), tokens.end(), 
            [](const auto &pair) { return pair.first == "ID"; });
        if (idTokenIt == tokens.end())
        {
            std::cerr << "Error: Missing ID token. Line ignored!" << std::endl;
            continue;
        }

        int id = std::stoi(idTokenIt->second);

        if (typeIt->second == "NODE")
        {
            auto newNode = std::find_if(_nodes.begin(), _nodes.end(),
                [id](const auto &node) { return node->GetID() == id; });
            if (newNode == _nodes.end())
            {
                auto node = std::make_unique<GraphNode>(id);
                AddAllTokensToElement("ANSWER", tokens, *node);
                _nodes.emplace_back(std::move(node));
            }
        }
        else if (typeIt->second == "EDGE")
        {
            auto parentToken = std::find_if(tokens.begin(), tokens.end(), 
                [](const auto &pair) { return pair.first == "PARENT"; });
            auto childToken = std::find_if(tokens.begin(), tokens.end(), 
                [](const auto &pair) { return pair.first == "CHILD"; });

            if (parentToken != tokens.end() && childToken != tokens.end())
            {
                auto parentNode = std::find_if(_nodes.begin(), _nodes.end(),
                    [parentToken](const auto &node) { return node->GetID() == std::stoi(parentToken->second); });
                auto childNode = std::find_if(_nodes.begin(), _nodes.end(),
                    [childToken](const auto &node) { return node->GetID() == std::stoi(childToken->second); });

                if (parentNode != _nodes.end() && childNode != _nodes.end())
                {
                    auto edge = std::make_unique<GraphEdge>(id);
                    edge->SetParentNode(parentNode->get());
                    edge->SetChildNode(childNode->get());
                    AddAllTokensToElement("KEYWORD", tokens, *edge);

                    (*parentNode)->AddEdgeToChildNode(std::move(edge));
                    (*childNode)->AddEdgeToParentNode(edge.get());
                }
            }
        }
    }

    GraphNode *rootNode = nullptr;
    for (const auto &node : _nodes)
    {
        if (node->GetNumberOfParents() == 0)
        {
            if (!rootNode)
            {
                rootNode = node.get();
            }
            else
            {
                std::cerr << "ERROR: Multiple root nodes detected" << std::endl;
            }
        }
    }

    auto chatBot = std::make_unique<ChatBot>(CB_IMAGE);
    _chatBot = chatBot.get();
    _chatBot->SetChatLogicHandle(this);
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

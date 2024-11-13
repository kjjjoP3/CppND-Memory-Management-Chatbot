#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>
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
void ChatLogic::AddAllTokensToElement(const std::string &tokenID, tokenlist &tokens, T &element)
{
    for (auto it = tokens.begin(); it != tokens.end();)
    {
        it = std::find_if(it, tokens.end(), [&tokenID](const auto &token) { return token.first == tokenID; });
        if (it != tokens.end())
        {
            element.AddToken(it->second);
            ++it;
        }
    }
}

void ChatLogic::LoadAnswerGraphFromFile(const std::string &filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "File could not be opened!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        tokenlist tokens;
        std::string token;
        size_t posFront, posBack;

        while ((posFront = line.find("<")) != std::string::npos && (posBack = line.find(">")) != std::string::npos)
        {
            token = line.substr(posFront + 1, posBack - posFront - 1);
            auto posInfo = token.find(":");

            if (posInfo != std::string::npos)
            {
                tokens.emplace_back(token.substr(0, posInfo), token.substr(posInfo + 1));
            }
            line = line.substr(posBack + 1);
        }

        auto typeToken = std::find_if(tokens.begin(), tokens.end(), [](const auto &pair) { return pair.first == "TYPE"; });
        if (typeToken == tokens.end())
            continue;

        auto idToken = std::find_if(tokens.begin(), tokens.end(), [](const auto &pair) { return pair.first == "ID"; });
        if (idToken == tokens.end())
        {
            std::cerr << "Error: ID missing. Line is ignored!" << std::endl;
            continue;
        }

        int id = std::stoi(idToken->second);

        if (typeToken->second == "NODE")
        {
            auto existingNode = std::find_if(_nodes.begin(), _nodes.end(), [&id](const auto &node) { return node->GetID() == id; });

            if (existingNode == _nodes.end())
            {
                auto newNode = std::make_unique<GraphNode>(id);
                AddAllTokensToElement("ANSWER", tokens, *newNode);
                _nodes.push_back(std::move(newNode));
            }
        }
        else if (typeToken->second == "EDGE")
        {
            auto parentToken = std::find_if(tokens.begin(), tokens.end(), [](const auto &pair) { return pair.first == "PARENT"; });
            auto childToken = std::find_if(tokens.begin(), tokens.end(), [](const auto &pair) { return pair.first == "CHILD"; });

            if (parentToken != tokens.end() && childToken != tokens.end())
            {
                int parentID = std::stoi(parentToken->second);
                int childID = std::stoi(childToken->second);

                auto parentNode = std::find_if(_nodes.begin(), _nodes.end(), [&parentID](const auto &node) { return node->GetID() == parentID; });
                auto childNode = std::find_if(_nodes.begin(), _nodes.end(), [&childID](const auto &node) { return node->GetID() == childID; });

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

    file.close();

    GraphNode *rootNode = nullptr;
    for (const auto &node : _nodes)
    {
        if (node->GetNumberOfParents() == 0)
        {
            if (rootNode)
            {
                std::cerr << "ERROR: Multiple root nodes detected" << std::endl;
                return;
            }
            rootNode = node.get();
        }
    }

    auto chatBot = std::make_unique<ChatBot>(CB_IMAGE);
    chatBot->SetChatLogicHandle(this);
    chatBot->SetRootNode(rootNode);
    rootNode->MoveChatbotHere(std::move(chatBot));

    _chatBot = rootNode->GetChatBot();
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog)
{
    _panelDialog = panelDialog;
}

void ChatLogic::SendMessageToChatbot(const std::string &message)
{
    _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(const std::string &message)
{
    _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot()
{
    return _chatBot->GetImageHandle();
}

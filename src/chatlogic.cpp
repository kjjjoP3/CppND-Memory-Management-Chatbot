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
void ChatLogic::AddTokensToElement(std::string tokenID, tokenlist &tokens, T &element)
{
    auto token = tokens.begin();
    while (token != tokens.end())
    {
        token = std::find_if(token, tokens.end(), [&tokenID](const std::pair<std::string, std::string> &pair) { return pair.first == tokenID; });
        if (token != tokens.end())
        {
            element.AddToken(token->second);
            token++; // move to the next token
        }
        else
        {
            break;
        }
    }
}

void ChatLogic::LoadAnswerGraphFromFile(std::string filename)
{
    std::ifstream file(filename);
    if (file)
    {
        std::string line;
        while (getline(file, line))
        {
            tokenlist tokens;
            while (!line.empty())
            {
                int posTokenFront = line.find("<");
                int posTokenBack = line.find(">");
                if (posTokenFront == std::string::npos || posTokenBack == std::string::npos)
                    break;

                std::string tokenStr = line.substr(posTokenFront + 1, posTokenBack - posTokenFront - 1);
                int posTokenInfo = tokenStr.find(":");
                if (posTokenInfo != std::string::npos)
                {
                    std::string tokenType = tokenStr.substr(0, posTokenInfo);
                    std::string tokenInfo = tokenStr.substr(posTokenInfo + 1);
                    tokens.push_back({tokenType, tokenInfo});
                }

                line = line.substr(posTokenBack + 1);
            }

            auto type = std::find_if(tokens.begin(), tokens.end(),
                                      [](const std::pair<std::string, std::string> &pair) { return pair.first == "TYPE"; });

            if (type != tokens.end())
            {
                auto idToken = std::find_if(tokens.begin(), tokens.end(),
                                             [](const std::pair<std::string, std::string> &pair) { return pair.first == "ID"; });

                if (idToken != tokens.end())
                {
                    int id = std::stoi(idToken->second);

                    if (type->second == "NODE")
                    {
                        auto newNode = std::find_if(_nodes.begin(), _nodes.end(),
                                                     [&id](const auto &node) { return node->GetID() == id; });

                        if (newNode == _nodes.end())
                        {
                            _nodes.emplace_back(std::make_unique<GraphNode>(id));
                            newNode = _nodes.end() - 1;
                            AddTokensToElement("ANSWER", tokens, **newNode);
                        }
                    }

                    if (type->second == "EDGE")
                    {
                        auto parentToken = std::find_if(tokens.begin(), tokens.end(),
                                                        [](const std::pair<std::string, std::string> &pair) { return pair.first == "PARENT"; });
                        auto childToken = std::find_if(tokens.begin(), tokens.end(),
                                                       [](const std::pair<std::string, std::string> &pair) { return pair.first == "CHILD"; });

                        if (parentToken != tokens.end() && childToken != tokens.end())
                        {
                            auto parentNode = std::find_if(_nodes.begin(), _nodes.end(),
                                                            [&parentToken](const auto &node) { return node->GetID() == std::stoi(parentToken->second); });
                            auto childNode = std::find_if(_nodes.begin(), _nodes.end(),
                                                           [&childToken](const auto &node) { return node->GetID() == std::stoi(childToken->second); });

                            std::unique_ptr<GraphEdge> edge = std::make_unique<GraphEdge>(id);
                            edge->SetChildNode(childNode->get());
                            edge->SetParentNode(parentNode->get());

                            AddTokensToElement("KEYWORD", tokens, *edge);

                            (*childNode)->AddEdgeToParentNode(edge.get());
                            (*parentNode)->AddEdgeToChildNode(std::move(edge));
                        }
                    }
                }
                else
                {
                    std::cout << "Error: ID missing. Line is ignored!" << std::endl;
                }
            }
        }
        file.close();
    }
    else
    {
        std::cout << "File could not be opened!" << std::endl;
        return;
    }

    GraphNode *rootNode = nullptr;
    for (auto &node : _nodes)
    {
        if (node->GetNumberOfParents() == 0)
        {
            if (rootNode == nullptr)
                rootNode = node.get();
            else
                std::cout << "ERROR : Multiple root nodes detected" << std::endl;
        }
    }

    std::unique_ptr<ChatBot> chatBot = std::make_unique<ChatBot>(CB_IMAGE);
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

#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include "chatlogic.h"
#include "graphnode.h"
#include "graphedge.h"
#include "chatbot.h"

// Constructor WITHOUT memory allocation
ChatBot::ChatBot()
    : _image(nullptr), _chatLogic(nullptr), _rootNode(nullptr), _currentNode(nullptr)
{
}

// Constructor WITH memory allocation
ChatBot::ChatBot(std::string filename)
    : _chatLogic(nullptr), _rootNode(nullptr)
{
    std::cout << "ChatBot Constructor" << std::endl;
    _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG); // Allocate image
}

// Destructor
ChatBot::~ChatBot()
{
    std::cout << "ChatBot Destructor" << std::endl;
    if (_image != NULL) // Release image memory if allocated
    {
        delete _image;
        _image = NULL;
    }
}

//// STUDENT CODE

// Copy Constructor
ChatBot::ChatBot(const ChatBot &source) 
{
    std::cout << "ChatBot Copy Constructor" << std::endl;
    _image = (source._image) ? new wxBitmap(*source._image) : nullptr;
    _chatLogic = source._chatLogic;
    _rootNode = source._rootNode;
    _currentNode = source._currentNode;
}

// Copy Assignment Operator
ChatBot &ChatBot::operator=(const ChatBot &source) 
{
    std::cout << "ChatBot Copy Assignment Operator" << std::endl;
    if (this == &source) // Handle self-assignment
        return *this;

    if (_image != NULL)
    {
        delete _image;
        _image = NULL;
    }
    _image = (source._image) ? new wxBitmap(*source._image) : nullptr;
    _chatLogic = source._chatLogic;
    _rootNode = source._rootNode;
    _currentNode = source._currentNode;
    return *this;
}

// Move Constructor
ChatBot::ChatBot(ChatBot &&source) noexcept 
{
    std::cout << "ChatBot Move Constructor" << std::endl;
    _image = source._image;
    _chatLogic = source._chatLogic;
    _rootNode = source._rootNode;
    _currentNode = source._currentNode;

    source._image = nullptr;
    source._chatLogic = nullptr;
    source._rootNode = nullptr;
    source._currentNode = nullptr;
}

// Move Assignment Operator
ChatBot &ChatBot::operator=(ChatBot &&source) noexcept 
{
    std::cout << "ChatBot Move Assignment Operator" << std::endl;
    if (this == &source)
        return *this;

    if (_image != NULL)
    {
        delete _image;
        _image = NULL;
    }
    _image = source._image;
    _chatLogic = source._chatLogic;
    _rootNode = source._rootNode;
    _currentNode = source._currentNode;

    source._image = nullptr;
    source._chatLogic = nullptr;
    source._rootNode = nullptr;
    source._currentNode = nullptr;
    return *this;
}

//// EOF STUDENT CODE

void ChatBot::ReceiveMessageFromUser(std::string message)
{
    typedef std::pair<GraphEdge *, int> EdgeDist;
    std::vector<EdgeDist> levDists;

    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
    {
        GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
        for (const auto &keyword : edge->GetKeywords())
        {
            levDists.emplace_back(edge, ComputeLevenshteinDistance(keyword, message));
        }
    }

    GraphNode *newNode = _rootNode; // Default to root node
    if (!levDists.empty())
    {
        std::sort(levDists.begin(), levDists.end(),
                  [](const EdgeDist &a, const EdgeDist &b) { return a.second < b.second; });
        newNode = levDists.front().first->GetChildNode();
    }

    _currentNode->MoveChatbotToNewNode(newNode);
}

void ChatBot::SetCurrentNode(GraphNode *node)
{
    _currentNode = node;
    const auto &answers = _currentNode->GetAnswers();
    std::mt19937 generator(static_cast<int>(std::time(0)));
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);
    std::string answer = answers[dis(generator)];
    _chatLogic->SendMessageToUser(answer);
}

int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2)
{
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

    const size_t m(s1.size());
    const size_t n(s2.size());

    if (m == 0) return n;
    if (n == 0) return m;

    std::vector<size_t> costs(n + 1);
    for (size_t k = 0; k <= n; ++k) costs[k] = k;

    for (size_t i = 0; i < m; ++i)
    {
        costs[0] = i + 1;
        size_t corner = i;
        for (size_t j = 0; j < n; ++j)
        {
            size_t upper = costs[j + 1];
            costs[j + 1] = (s1[i] == s2[j]) ? corner : std::min({costs[j], upper, corner}) + 1;
            corner = upper;
        }
    }

    return costs[n];
}

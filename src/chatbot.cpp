#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>

#include "chatlogic.h"
#include "graphnode.h"
#include "graphedge.h"
#include "chatbot.h"

// constructor WITHOUT memory allocation
ChatBot::ChatBot()
{
    _image = nullptr;
    _chatLogic = nullptr;
    _rootNode = nullptr;
}

// constructor WITH memory allocation
ChatBot::ChatBot(std::string filename)
{
    std::cout << "ChatBot Constructor" << std::endl;
    _chatLogic = nullptr;
    _rootNode = nullptr;

    // load image into heap memory
    _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);
}

ChatBot::~ChatBot()
{
    std::cout << "ChatBot Destructor (instance: " << this << ")" << std::endl;

    // deallocate heap memory
    if (_image != nullptr) // Attention: wxWidgets used NULL and not nullptr
    {
        delete _image;
        _image = nullptr;
    }
}

// Copy constructor 
ChatBot::ChatBot(const ChatBot &source)
{
    std::cout << "COPYING content of instance " << &source << " to instance " << this << std::endl;

    _chatLogic = source._chatLogic;
    _rootNode = source._rootNode;
    _image = new wxBitmap(*source._image);
}

// Copy assignment operator 
ChatBot &ChatBot::operator=(const ChatBot &source)
{
    std::cout << "ASSIGNING content of instance " << &source << " to instance " << this << std::endl;

    if (this != &source) {
        delete _image;
        _chatLogic = source._chatLogic;
        _rootNode = source._rootNode;
        _image = new wxBitmap(*source._image);
    }

    return *this;
}

// Move constructor 
ChatBot::ChatBot(ChatBot &&source)
{
    std::cout << "MOVING (constructor) instance " << &source << " to instance " << this << std::endl;
    _chatLogic = source._chatLogic;
    _rootNode = source._rootNode;
    _image = source._image;
    source._chatLogic = nullptr;
    source._rootNode = nullptr;
    source._image = nullptr;
}

// Move assignment operator
ChatBot &ChatBot::operator=(ChatBot &&source)
{
    std::cout << "MOVING (assign) instance " << &source << " to instance " << this << std::endl;
    if (this != &source) {
        delete _image;
        _chatLogic = source._chatLogic;
        _rootNode = source._rootNode;
        _image = source._image;
        source._chatLogic = nullptr;
        source._rootNode = nullptr;
        source._image = nullptr;
    }

    return *this;
}

// Receive message and handle it
void ChatBot::ReceiveMessageFromUser(std::string message)
{
    typedef std::pair<GraphEdge *, int> EdgeDist;
    std::vector<EdgeDist> levDists; // format is <ptr, levDist>

    // Iterate over all edges and keywords to compute Levenshtein distance
    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
    {
        GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
        for (auto keyword : edge->GetKeywords())
        {
            EdgeDist ed{edge, ComputeLevenshteinDistance(keyword, message)};
            levDists.push_back(ed);
        }
    }

    // Sort and choose best matching edge
    GraphNode *newNode = _rootNode;
    if (!levDists.empty())
    {
        std::sort(levDists.begin(), levDists.end(), [](const EdgeDist &a, const EdgeDist &b) { return a.second < b.second; });
        newNode = levDists.at(0).first->GetChildNode(); // Best edge is first after sorting
    }

    // Update chatbot to new node
    _currentNode->MoveChatbotToNewNode(newNode);
}

// Set the current node and reply to user
void ChatBot::SetCurrentNode(GraphNode *node)
{
    _currentNode = node;

    // Randomly select an answer from available answers
    std::vector<std::string> answers = _currentNode->GetAnswers();
    std::mt19937 generator(static_cast<unsigned int>(std::time(0)));
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);
    std::string answer = answers.at(dis(generator));

    _chatLogic->SetChatbotHandle(this); // Update chatbot for chatLogic
    _chatLogic->SendMessageToUser(answer); // Send the selected answer to user
}

// Compute the Levenshtein distance between two strings
int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2)
{
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

    size_t m = s1.size();
    size_t n = s2.size();

    if (m == 0) return n;
    if (n == 0) return m;

    size_t *costs = new size_t[n + 1];
    for (size_t k = 0; k <= n; k++)
        costs[k] = k;

    size_t i = 0;
    for (auto it1 = s1.begin(); it1 != s1.end(); ++it1, ++i)
    {
        costs[0] = i + 1;
        size_t corner = i;

        size_t j = 0;
        for (auto it2 = s2.begin(); it2 != s2.end(); ++it2, ++j)
        {
            size_t upper = costs[j + 1];
            if (*it1 == *it2)
            {
                costs[j + 1] = corner;
            }
            else
            {
                size_t t = std::min(upper, corner);
                costs[j + 1] = std::min(costs[j], t) + 1;
            }

            corner = upper;
        }
    }

    int result = costs[n];
    delete[] costs;

    return result;
}

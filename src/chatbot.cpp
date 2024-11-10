#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>

#include "chatlogic.h"
#include "graphnode.h"
#include "graphedge.h"
#include "chatbot.h"

// Constructor without memory allocation
ChatBot::ChatBot() 
    : _image(nullptr), _chatLogic(nullptr), _rootNode(nullptr), _currentNode(nullptr) {
    // Initializing pointers to null, no memory allocation performed
}

// Constructor with memory allocation
ChatBot::ChatBot(const std::string& filename) {
    std::cout << "ChatBot Constructor" << std::endl;
    
    // Initialize pointers to null
    _chatLogic = nullptr;
    _rootNode = nullptr;

    // Load image into heap memory
    _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);
}

// Destructor: Clean up heap memory
ChatBot::~ChatBot() {
    std::cout << "ChatBot Destructor" << std::endl;

    // Deallocate heap memory for _image, if allocated
    if (_image != nullptr) {
        delete _image;  // Safely delete the image object
        _image = nullptr;
    }
}

// Copy constructor
ChatBot::ChatBot(const ChatBot& source) {
    std::cout << "ChatBot copy constructor" << std::endl;

    // Deep copy for the image, shallow copy for other pointers
    _image = new wxBitmap(*source._image);
    _chatLogic = source._chatLogic;  
    _rootNode = source._rootNode;
    _currentNode = source._currentNode;
}

// Copy assignment operator
ChatBot& ChatBot::operator=(const ChatBot& source) {
    std::cout << "ChatBot copy assignment operator" << std::endl;

    if (this != &source) {
        // Deep copy the image, shallow copy other pointers
        delete _image;  // Release any existing memory
        _image = new wxBitmap(*source._image);
        _chatLogic = source._chatLogic;
        _rootNode = source._rootNode;
        _currentNode = source._currentNode;
    }

    return *this;
}

// Move constructor
ChatBot::ChatBot(ChatBot&& source) noexcept {
    std::cout << "ChatBot move constructor" << std::endl;

    // Transfer ownership of resources from source
    _image = source._image;
    _chatLogic = source._chatLogic;
    _rootNode = source._rootNode;
    _currentNode = source._currentNode;

    // Null out the source's pointers
    source._image = nullptr;
    source._chatLogic = nullptr;
    source._rootNode = nullptr;
    source._currentNode = nullptr;
}

// Move assignment operator
ChatBot& ChatBot::operator=(ChatBot&& source) noexcept {
    std::cout << "ChatBot move assignment operator" << std::endl;

    if (this != &source) {
        // Release any existing resources
        delete _image;

        // Transfer ownership of resources from source
        _image = source._image;
        _chatLogic = source._chatLogic;
        _rootNode = source._rootNode;
        _currentNode = source._currentNode;

        // Null out the source's pointers to prevent double deletion
        source._image = nullptr;
        source._chatLogic = nullptr;
        source._rootNode = nullptr;
        source._currentNode = nullptr;
    }

    return *this;
}

// Handle receiving a message from the user and determining the next action
void ChatBot::ReceiveMessageFromUser(const std::string& message) {
    // Create a vector to hold edges and their corresponding Levenshtein distances
    std::vector<std::pair<GraphEdge*, int>> levDists;

    // Loop through all edges of the current node
    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i) {
        GraphEdge* edge = _currentNode->GetChildEdgeAtIndex(i);
        for (const auto& keyword : edge->GetKeywords()) {
            levDists.emplace_back(edge, ComputeLevenshteinDistance(keyword, message));
        }
    }

    // Select the best matching edge based on Levenshtein distance
    GraphNode* newNode = (_currentNode->GetNumberOfChildEdges() > 0) ? 
                            SelectBestEdge(levDists) : _rootNode;

    // Move the chatbot to the new node
    _currentNode->MoveChatbotToNewNode(newNode);
}

// Select the best matching edge based on Levenshtein distance
GraphNode* ChatBot::SelectBestEdge(const std::vector<std::pair<GraphEdge*, int>>& levDists) {
    // Sort edges based on Levenshtein distance
    auto bestEdge = *std::min_element(levDists.begin(), levDists.end(), 
        [](const std::pair<GraphEdge*, int>& a, const std::pair<GraphEdge*, int>& b) {
            return a.second < b.second;
        });

    // Return the node that corresponds to the best edge
    return bestEdge.first->GetChildNode();
}

// Set the current node of the chatbot and send an appropriate message
void ChatBot::SetCurrentNode(GraphNode* node) {
    _currentNode = node;

    // Select a random answer from the current node
    const auto& answers = _currentNode->GetAnswers();
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);
    std::mt19937 generator(static_cast<unsigned int>(std::time(nullptr)));
    std::string answer = answers.at(dis(generator));

    // Send the selected answer to the user
    _chatLogic->SendMessageToUser(answer);
}

// Compute the Levenshtein distance between two strings
int ChatBot::ComputeLevenshteinDistance(const std::string& s1, const std::string& s2) {
    // Convert both strings to uppercase for comparison
    std::string str1 = s1;
    std::string str2 = s2;
    std::transform(str1.begin(), str1.end(), str1.begin(), ::toupper);
    std::transform(str2.begin(), str2.end(), str2.begin(), ::toupper);

    // Initialize the cost array
    const size_t m = str1.size();
    const size_t n = str2.size();
    if (m == 0) return n;
    if (n == 0) return m;

    size_t* costs = new size_t[n + 1];
    for (size_t i = 0; i <= n; ++i) {
        costs[i] = i;
    }

    for (size_t i = 0; i < m; ++i) {
        size_t corner = costs[0];
        costs[0] = i + 1;
        for (size_t j = 0; j < n; ++j) {
            size_t upper = costs[j + 1];
            costs[j + 1] = (str1[i] == str2[j]) ? corner : std::min({costs[j], upper, corner}) + 1;
            corner = upper;
        }
    }

    int result = costs[n];
    delete[] costs;
    return result;
}

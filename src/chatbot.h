#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <wx/bitmap.h>
#include <string>
#define CB_IMAGE "../images/chatbot.png"

// Forward declarations for other classes
class GraphNode;
class ChatLogic;

class ChatBot {
private:
    wxBitmap* _image; // Chatbot image handle
    GraphNode* _currentNode; // Pointer to the current node in the conversation graph
    GraphNode* _rootNode; // Pointer to the root node of the conversation graph
    ChatLogic* _chatLogic; // Pointer to the ChatLogic class, which handles message exchanges

    int ComputeLevenshteinDistance(const std::string& s1, const std::string& s2); // Levenshtein distance calculation

public:
    // Constructors and destructors
    ChatBot(); // Default constructor (no memory allocation)
    ChatBot(const std::string& filename); // Constructor with image loading
    ~ChatBot(); // Destructor to clean up resources

    // Copy and move constructors and assignment operators
    ChatBot(const ChatBot& source); // Copy constructor
    ChatBot& operator=(const ChatBot& source); // Copy assignment operator
    ChatBot(ChatBot&& source) noexcept; // Move constructor
    ChatBot& operator=(ChatBot&& source) noexcept; // Move assignment operator

    // Setters
    void SetCurrentNode(GraphNode* node); // Set the current conversation node
    void SetRootNode(GraphNode* rootNode) { _rootNode = rootNode; } // Set the root node
    void SetChatLogicHandle(ChatLogic* chatLogic) { _chatLogic = chatLogic; } // Set the chat logic
    wxBitmap* GetImageHandle() { return _image; } // Get the image handle

    // Main functionality
    void ReceiveMessageFromUser(const std::string& message); // Process user input message
    void SendMessageToUser(const std::string& message); // Send a message to the user
    GraphNode* SelectBestEdge(const std::vector<std::pair<GraphEdge*, int>>& levDists); // Select the best matching edge
};

#endif // CHATBOT_H_
#include "graphedge.h"
#include "graphnode.h"

GraphNode::GraphNode(int id) : _id(id), _chatBot(nullptr)
{
    // Initialization handled in initializer list
}

GraphNode::~GraphNode()
{
    // No need for explicit deletion; _chatBot managed externally by ChatLogic
}

void GraphNode::AddToken(std::string token)
{
    _answers.emplace_back(std::move(token)); // More efficient move operation
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
{
    _parentEdges.push_back(edge); // Store incoming edges directly
}

void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge)
{
    _childEdges.emplace_back(std::move(edge)); // Transfer ownership with move semantics
}

void GraphNode::MoveChatbotHere(std::unique_ptr<ChatBot> chatbot)
{
    _chatBot = std::move(chatbot); // Assign ownership of ChatBot
    _chatBot->SetCurrentNode(this); // Update ChatBot's current node
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    if (_chatBot) // Ensure _chatBot is not null before transferring
    {
        newNode->MoveChatbotHere(std::move(_chatBot)); // Transfer ownership to the target node
    }
}

GraphEdge* GraphNode::GetChildEdgeAtIndex(int index)
{
    return _childEdges[index].get(); // Retrieve raw pointer from unique_ptr
}

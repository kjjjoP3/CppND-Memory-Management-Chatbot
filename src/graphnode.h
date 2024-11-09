#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_

#include <vector>
#include <string>
#include "chatbot.h"

// Forward declarations
class GraphEdge;

class GraphNode
{
private:
    // Data members (owned)
    std::vector<std::unique_ptr<GraphEdge>> _childEdges;  // Edges to subsequent nodes

    // Data members (not owned)
    std::vector<GraphEdge *> _parentEdges; // Edges to preceding nodes 
    ChatBot _chatBot; // The associated chatbot for this node

    // Node identifier
    int _id;

    // List of answers for this node
    std::vector<std::string> _answers;

public:
    // Constructor and Destructor
    GraphNode(int nodeId);  // Constructor to initialize node with an ID
    ~GraphNode();           // Destructor to clean up resources

    // Getter functions
    int GetID() const { return _id; }
    int GetNumberOfChildEdges() const { return static_cast<int>(_childEdges.size()); }
    GraphEdge *GetChildEdgeAtIndex(int index) const;
    const std::vector<std::string>& GetAnswers() const { return _answers; }
    int GetNumberOfParents() const { return static_cast<int>(_parentEdges.size()); }

    // Add a token to the answers list
    void AddToken(const std::string& token);

    // Add edge functions
    void AddEdgeToParentNode(GraphEdge *edge);
    void AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge);

    // Move chatbot-related functions
    void MoveChatbotHere(ChatBot chatbot);
    void MoveChatbotToNewNode(GraphNode *newNode);
};

#endif /* GRAPHNODE_H_ */

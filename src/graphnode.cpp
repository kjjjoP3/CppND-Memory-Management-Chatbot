#include "graphedge.h"
#include "graphnode.h"

GraphNode::GraphNode(int id)
{
    _id = id;
}

GraphNode::~GraphNode()
{
    // Destructor không cần phải xử lý việc giải phóng _chatBot 
    // vì nó đã được xử lý trong chatlogic.cpp
}

void GraphNode::AddToken(std::string token)
{
    _answers.push_back(std::move(token));  // Dùng std::move để tránh sao chép không cần thiết
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
{
    _parentEdges.push_back(edge);
}

void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge)
{
    // Sử dụng move semantics để chuyển quyền sở hữu của GraphEdge
    _childEdges.push_back(std::move(edge));
}

void GraphNode::MoveChatbotHere(ChatBot chatbot)
{
    // Dùng std::move để chuyển quyền sở hữu chatbot
    _chatBot = std::move(chatbot);
    _chatBot.SetCurrentNode(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    // Chuyển chatbot đến Node mới bằng move semantics
    newNode->MoveChatbotHere(std::move(_chatBot));
}

GraphEdge *GraphNode::GetChildEdgeAtIndex(int index)
{
    // Trả về con trỏ thô của GraphEdge tại vị trí chỉ định
    return _childEdges[index].get(); 
}

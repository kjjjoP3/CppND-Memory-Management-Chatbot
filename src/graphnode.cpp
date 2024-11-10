#include "graphedge.h"
#include "graphnode.h"

GraphNode::GraphNode(int id) : _id(id), _chatBot(nullptr)
{
    // Khởi tạo ID và chatbot, trong đó chatbot khởi tạo với nullptr
}

GraphNode::~GraphNode()
{
    // Không cần xoá _chatBot vì nó đã được quản lý bởi ChatLogic
}

void GraphNode::AddToken(std::string token)
{
    _answers.push_back(std::move(token)); // Đảm bảo tối ưu hoá khi thêm câu trả lời
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
{
    _parentEdges.push_back(edge); // Thêm cạnh vào danh sách cha
}

void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge)
{
    _childEdges.push_back(std::move(edge)); // Sử dụng std::move để chuyển quyền sở hữu
}

void GraphNode::MoveChatbotHere(std::unique_ptr<ChatBot> chatbot)
{
    _chatBot = std::move(chatbot); // Chuyển quyền sở hữu của chatbot
    _chatBot->SetCurrentNode(this); // Cập nhật nút hiện tại cho chatbot
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    newNode->MoveChatbotHere(std::move(_chatBot)); // Chuyển chatbot đến nút mới
}

GraphEdge *GraphNode::GetChildEdgeAtIndex(int index)
{
    return _childEdges[index].get(); // Trả về con trỏ thô từ unique_ptr
}

#include "graphedge.h"
#include "graphnode.h"

GraphNode::GraphNode(int id)
    : _id(id), _chatBot(nullptr)  // Sử dụng danh sách khởi tạo thay vì gán trong thân hàm
{
}

GraphNode::~GraphNode()
{
    // Destructor để giải phóng tài nguyên nếu cần
}

void GraphNode::AddToken(std::string token)
{
    _answers.push_back(std::move(token));  // Di chuyển token vào vector
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge) // incoming
{
    _parentEdges.push_back(edge);  // Thêm edge vào danh sách parent
}

void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge) // outgoing
{
    _childEdges.push_back(std::move(edge)); // Di chuyển unique_ptr vào vector
}

void GraphNode::MoveChatbotHere(std::unique_ptr<ChatBot> chatbot) // Task 5
{
    _chatBot = std::move(chatbot);  // Di chuyển chatbot vào node
    _chatBot->SetCurrentNode(this); // Đặt node hiện tại cho chatbot
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    newNode->MoveChatbotHere(std::move(_chatBot)); // Di chuyển chatbot sang node mới
    // _chatBot được tự động null sau khi chuyển giao unique_ptr
}

GraphEdge* GraphNode::GetChildEdgeAtIndex(int index)
{
    if (index < 0 || index >= _childEdges.size()) {
        return nullptr; // Trả về nullptr nếu index không hợp lệ
    }
    return _childEdges[index].get();  // Lấy con trỏ raw từ unique_ptr
}

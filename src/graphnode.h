#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_

#include <vector>
#include <string>
#include "chatbot.h"
#include <memory> // Thư viện cần thiết cho unique_ptr

// Forward declarations
class GraphEdge;

class GraphNode
{
private:
    // Data handles (owned)
    std::vector<std::unique_ptr<GraphEdge>> _childEdges;    // Quản lý bằng unique_ptr

    // Data handles (non-owned)
    std::vector<GraphEdge*> _parentEdges;  // Lưu trữ con trỏ raw cho các parent edges

    std::unique_ptr<ChatBot> _chatBot;  // ChatBot được quản lý bởi unique_ptr

    // Proprietary members
    int _id;  // ID của node
    std::vector<std::string> _answers;  // Các câu trả lời liên quan đến node

public:
    // Constructor / Destructor
    GraphNode(int id);
    ~GraphNode();

    // Getter / Setter
    int GetID() const { return _id; }
    int GetNumberOfChildEdges() const { return static_cast<int>(_childEdges.size()); }
    GraphEdge* GetChildEdgeAtIndex(int index);
    std::vector<std::string> GetAnswers() const { return _answers; }
    int GetNumberOfParents() const { return static_cast<int>(_parentEdges.size()); }

    // Proprietary functions
    void AddToken(std::string token);  // Thêm câu trả lời
    void AddEdgeToParentNode(GraphEdge* edge);  // Thêm edge vào parent
    void AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge);  // Thêm edge vào child, sử dụng move

    // Function để chuyển chatbot vào node hiện tại
    void MoveChatbotHere(std::unique_ptr<ChatBot> chatbot);

    // Function để chuyển chatbot sang node mới
    void MoveChatbotToNewNode(GraphNode* newNode);
};

#endif /* GRAPHNODE_H_ */

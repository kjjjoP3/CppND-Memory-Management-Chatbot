#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_

#include <vector>
#include <string>
#include "chatbot.h"
#include <memory>  // Cần thiết cho việc sử dụng smart pointers

// forward declarations
class GraphEdge;

class GraphNode
{
private:
    // Dữ liệu mà GraphNode sở hữu (quản lý bộ nhớ)
    std::vector<std::unique_ptr<GraphEdge>> _childEdges;  // Dùng unique_ptr để sở hữu GraphEdge
    // Dữ liệu không sở hữu (chỉ tham chiếu)
    std::vector<GraphEdge *> _parentEdges;  // Các cạnh tới các node cha

    ChatBot _chatBot;  // Sử dụng ChatBot trong node mà không cần con trỏ

    int _id;  // Mã ID của GraphNode
    std::vector<std::string> _answers;  // Các câu trả lời (token)

public:
    // Constructor / Destructor
    GraphNode(int id);
    ~GraphNode();

    // Getter / Setter
    int GetID() const { return _id; }
    int GetNumberOfChildEdges() const { return _childEdges.size(); }
    GraphEdge *GetChildEdgeAtIndex(int index);
    std::vector<std::string> GetAnswers() const { return _answers; }
    int GetNumberOfParents() const { return _parentEdges.size(); }

    // Hàm thêm dữ liệu vào
    void AddToken(std::string token);
    void AddEdgeToParentNode(GraphEdge *edge);
    void AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge);

    // Hàm di chuyển chatbot
    void MoveChatbotHere(ChatBot chatbot);
    void MoveChatbotToNewNode(GraphNode *newNode);
};

#endif /* GRAPHNODE_H_ */

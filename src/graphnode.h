#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_

#include <vector>
#include <string>
#include <memory>
#include "chatbot.h"

// Forward declaration
class GraphEdge;

class GraphNode
{
private:
    // Danh sách các cạnh con (sở hữu)
    std::vector<std::unique_ptr<GraphEdge>> _childEdges;

    // Danh sách các cạnh cha (không sở hữu)
    std::vector<GraphEdge *> _parentEdges;

    // Con trỏ quản lý chatbot
    std::unique_ptr<ChatBot> _chatBot;

    // ID nút và danh sách câu trả lời
    int _id;
    std::vector<std::string> _answers;

public:
    // Hàm khởi tạo / huỷ
    explicit GraphNode(int id);
    ~GraphNode();

    // Getter / Setter
    int GetID() const { return _id; }
    int GetNumberOfChildEdges() const { return _childEdges.size(); }
    GraphEdge *GetChildEdgeAtIndex(int index);
    std::vector<std::string> GetAnswers() const { return _answers; }
    int GetNumberOfParents() const { return _parentEdges.size(); }

    // Hàm xử lý cạnh và câu trả lời
    void AddToken(std::string token);
    void AddEdgeToParentNode(GraphEdge *edge);
    void AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge);

    // Di chuyển chatbot
    void MoveChatbotHere(std::unique_ptr<ChatBot> chatbot);
    void MoveChatbotToNewNode(GraphNode *newNode);
};

#endif /* GRAPHNODE_H_ */

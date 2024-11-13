#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>

#include "chatlogic.h"
#include "graphnode.h"
#include "graphedge.h"
#include "chatbot.h"

// Constructor không có cấp phát bộ nhớ
ChatBot::ChatBot()
{
    // Thiết lập các con trỏ dữ liệu về NULL
    _image = nullptr;
    _chatLogic = nullptr;
    _rootNode = nullptr;
    // Cần làm gì với _currentNode?
}

// Constructor có cấp phát bộ nhớ
ChatBot::ChatBot(std::string filename)
{
    std::cout << "ChatBot Constructor" << std::endl;
    
    // Thiết lập các con trỏ dữ liệu về NULL
    _chatLogic = nullptr;
    _rootNode = nullptr;

    // Cấp phát bộ nhớ cho hình ảnh
    _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);
}

ChatBot::~ChatBot()
{
    std::cout << "ChatBot Destructor" << std::endl;

    // Giải phóng bộ nhớ đã cấp phát
    if (_image != NULL) // wxWidgets sử dụng NULL thay vì nullptr
    {
        delete _image;  // Có thể gây ra lỗi phân đoạn nếu sử dụng nhiều lần
        _image = NULL;  // Giải phóng bộ nhớ chỉ một lần là đủ
    }
}

// Code của sinh viên

ChatBot::ChatBot(const ChatBot &source) {   // Copy constructor
    std::cout << "ChatBot copy constructor" << std::endl;
    _image = new wxBitmap(*source._image);  // Copy ảnh từ đối tượng nguồn
    _chatLogic = source._chatLogic;         // Con trỏ không sở hữu, nên chỉ sao chép
    _rootNode = source._rootNode;           // Con trỏ không sở hữu, chỉ sao chép
    _currentNode = source._currentNode;     // Con trỏ không sở hữu, chỉ sao chép
}

ChatBot & ChatBot::operator=(const ChatBot &source) {  // Copy assignment operator
    std::cout << "ChatBot copy assignment operator" << std::endl;

    if (this != &source) {  // Kiểm tra gán chính nó
        delete _image;  // Giải phóng bộ nhớ cũ

        // Sao chép từ đối tượng nguồn
        _image = new wxBitmap(*source._image);
        _chatLogic = source._chatLogic;
        _rootNode = source._rootNode;
        _currentNode = source._currentNode;
    }

    return *this;
}

ChatBot::ChatBot(ChatBot &&source) {     // Move constructor
    std::cout << "ChatBot move constructor" << std::endl;
    
    // Giải phóng bộ nhớ cũ
    if (_image != NULL) {
        delete _image;
    }

    // Di chuyển tài nguyên từ đối tượng nguồn
    _image = source._image;
    _chatLogic = source._chatLogic;
    _rootNode = source._rootNode;
    _currentNode = source._currentNode;

    // Đặt lại con trỏ trong đối tượng nguồn
    source._image = nullptr;
    source._chatLogic = nullptr;
    source._rootNode = nullptr;
    source._currentNode = nullptr;
}

ChatBot & ChatBot::operator=(ChatBot &&source) {  // Move assignment operator
    std::cout << "ChatBot move assignment operator" << std::endl;

    if (this != &source) {
        // Giải phóng bộ nhớ cũ
        if (_image != NULL) {
            delete _image;
        }

        // Di chuyển tài nguyên từ đối tượng nguồn
        _image = source._image;
        _chatLogic = source._chatLogic;
        _rootNode = source._rootNode;
        _currentNode = source._currentNode;

        // Đặt lại con trỏ trong đối tượng nguồn
        source._image = nullptr;
        source._chatLogic = nullptr;
        source._rootNode = nullptr;
        source._currentNode = nullptr;
    }

    return *this;
}

// Kết thúc phần code của sinh viên

void ChatBot::ReceiveMessageFromUser(std::string message)
{
    typedef std::pair<GraphEdge *, int> EdgeDist;
    std::vector<EdgeDist> levDists; // danh sách chứa các cặp <con trỏ, khoảng cách Levenshtein>

    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
    {
        GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
        for (auto keyword : edge->GetKeywords())
        {
            EdgeDist ed{edge, ComputeLevenshteinDistance(keyword, message)};
            levDists.push_back(ed);
        }
    }

    // Chọn cạnh phù hợp nhất để tiếp tục
    GraphNode *newNode;
    if (!levDists.empty())
    {
        // Sắp xếp danh sách theo khoảng cách Levenshtein (cạnh tốt nhất nằm ở vị trí đầu)
        std::sort(levDists.begin(), levDists.end(), [](const EdgeDist &a, const EdgeDist &b) { return a.second < b.second; });
        newNode = levDists.at(0).first->GetChildNode();  // Chọn cạnh đầu tiên sau khi sắp xếp
    }
    else
    {
        // Nếu không có kết quả phù hợp, quay lại node gốc
        newNode = _rootNode;
    }

    // Di chuyển chatbot tới node mới
    _currentNode->MoveChatbotToNewNode(newNode);
}

void ChatBot::SetCurrentNode(GraphNode *node)
{
    // Cập nhật con trỏ tới node hiện tại
    _currentNode = node;

    // Chọn một câu trả lời ngẫu nhiên từ các câu trả lời của node hiện tại
    std::vector<std::string> answers = _currentNode->GetAnswers();
    std::mt19937 generator(static_cast<int>(std::time(0)));  // Khởi tạo bộ sinh số ngẫu nhiên
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);
    std::string answer = answers.at(dis(generator));

    // Gửi câu trả lời tới người dùng
    _chatLogic->SendMessageToUser(answer);
}

int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2)
{
    // Chuyển cả hai chuỗi thành chữ hoa trước khi so sánh
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

    // Tính toán khoảng cách Levenshtein giữa hai chuỗi
    const size_t m(s1.size());
    const size_t n(s2.size());

    if (m == 0) return n;
    if (n == 0) return m;

    size_t *costs = new size_t[n + 1];

    for (size_t k = 0; k <= n; k++)
        costs[k] = k;

    size_t i = 0;
    for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i)
    {
        costs[0] = i + 1;
        size_t corner = i;

        size_t j = 0;
        for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j)
        {
            size_t upper = costs[j + 1];
            if (*it1 == *it2)
            {
                costs[j + 1] = corner;
            }
            else
            {
                size_t t(upper < corner ? upper : corner);
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }

            corner = upper;
        }
    }

    int result = costs[n];
    delete[] costs;

    return result;
}

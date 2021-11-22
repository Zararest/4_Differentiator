#define DATA_SIZE 8

enum Types{

    Nothing = 0,
    Op = 1,
    Var = 2,
    Const = 3
};

enum Priorities{//в дереве при обходе вглубину должны убывать приоритеты, иначе скобки

    nothing = 0,
    unary_op = 1,
    const_or_num = 2,
    mul_or_div = 3,
    sub = 4,
    sum = 5
};

typedef int NodeType;
typedef char* NodeData;

class Node{

    NodeType type = Nothing;
    NodeData data = nullptr;

    Node* prev = nullptr;
    Node* left = nullptr;
    Node* right = nullptr;
    
    Node* copy_tree(Node* cur_root);
public:

    Node() = default;
    Node(NodeType new_type, NodeData data, Node* prev_node);
    Node(const Node& old_node);
    Node(Node&& rv_node);
    ~Node();

    Node& operator =(const Node& old_node);
    Node& operator =(Node&& rv_node);

    NodeType get_type();
    int get_priority();
    Node* get_left();
    Node* get_right();
    Node* get_prev();
    bool is_leaf();
    void add_branches(Node* new_left, Node* new_right);
    void unlink_left();
    void unlink_right();
};
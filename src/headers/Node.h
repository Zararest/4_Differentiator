#define DATA_SIZE 8

#include <stdio.h>

/**
 * @brief Типы узлов дерева. Необходимы для покраски узлов графа.
 * 
 */
enum Types{

    Nothing = 0,
    Op = 1,
    Var = 2,
    Const_or_num = 3
};

/**
 * @brief Приоритеты операций. В дереве при обходе вглубину должны убывать , иначе при выводе необходимы скобки.
 * 
 */
enum Priorities{

    nothing = 0,
    unary_op = 1,
    power = 2,      //вроде так
    const_or_num = 3,
    mul_or_div = 4,
    sub = 5,
    sum = 6
};

typedef int NodePriority;
typedef int NodeType;
typedef char* NodeData;

bool find_in_unary_op(NodeData lexem);
bool find_in_binary_op(NodeData lexem);
bool find_in_consts(NodeData lexem);
bool is_number(NodeData lexem);
bool is_var(NodeData lexem);

/**
 * @brief Класс узла дерева. При добавлении инфорцации в узел автоматически определяется тип и приоритет. Реализован вывод через Graphviz.
 * 
 */
class Node{

    NodePriority priority = nothing;
    NodeType type = Nothing;
    NodeData data = nullptr;

    Node* prev = nullptr;
    Node* left = nullptr;
    Node* right = nullptr;
    
    Node* copy_tree(Node* cur_root);
    void print_node_graphviz(Node* cur_node, FILE* graphviz_file);
public:

    Node() = default;
    Node(Node* new_left, NodeData new_data, Node* new_right);
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
    void change_data(NodeData new_data);
    void unlink_left();
    void unlink_right();
    bool check_tree();
    void dump_graphviz(FILE* graph_file);  
};
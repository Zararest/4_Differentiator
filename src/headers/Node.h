#define MAX_DATA_SIZE 8

#include <stdio.h>
#include <string.h>

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
    const_or_var = 3,
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
    
    void print_node_graphviz(Node* cur_node, FILE* graphviz_file);
public:

    Node() = default;
    Node(Node* new_left, NodeData new_data, Node* new_right);
    Node(const Node& old_node);
    Node(Node&& rv_node);
    ~Node();

    static Node* copy_tree(const Node* cur_root);

    Node& operator =(const Node& old_node);
    Node& operator =(Node&& rv_node);

    NodeType get_type();
    int get_priority();

    Node* get_left() const;
    Node* get_right() const;
    Node* get_prev() const;
    bool is_leaf();
    bool is_variable();
    void add_branches(Node* new_left, Node* new_right);
    void change_data(const NodeData new_data);
    void unlink_left();
    void unlink_right();
    void unlink_parent();
    bool check_tree();
    void dump_graphviz(FILE* graph_file);  

    bool operator ==(const Node& right_node){

        if (strcmp(data, right_node.data) == 0){

            return true;
        }

        return false;
    }
};
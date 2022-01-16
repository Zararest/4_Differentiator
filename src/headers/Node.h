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
    Const_or_num = 3,
    Diff = 4
};

/**
 * @brief Приоритеты операций. В дереве при обходе вглубину должны убывать, иначе при выводе необходимы скобки.
 * 
 */
enum Priorities{

    nothing = 0,
    unary_op = 1,
    power = 2,      //вроде так
    const_or_var = 3,
    mul_or_div = 4,
    sub = 5,
    sum = 6,
    differ = 7
};

typedef int NodePriority;
typedef int NodeType;
typedef char* NodeData;
typedef const char* ConstNodeData;

bool find_in_unary_op(ConstNodeData lexem);
bool find_in_binary_op(ConstNodeData lexem);
bool find_in_consts(ConstNodeData lexem);
bool is_number(ConstNodeData lexem);
bool is_var(ConstNodeData lexem);

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
    Node(Node* new_left, ConstNodeData new_data, Node* new_right);
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
    void swap_branches();
    void change_data(ConstNodeData new_data);
    void unlink_left();
    void unlink_right();
    void unlink_parent();
    bool check_tree();
    void dump_graphviz(FILE* graph_file);  

    bool operator ==(const Node& right_node){
        
        if (right_node.data == nullptr){

            if (data == nullptr){ return true; }

            return false;
        }
        
        if (strcmp(data, right_node.data) == 0){

            return true;
        }

        return false;
    }
};
#include "headers/Node.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <assert.h>
#include <algorithm>

#define MAX_FILE_SIZE 200

/**
 * @brief Определяет является ли строка унарным оператором.
 * 
 * @param lexem 
 * @return true 
 * @return false 
 */
bool find_in_unary_op(const NodeData lexem){

    char buffer[MAX_FILE_SIZE] = "";
    FILE* file = fopen("../bin/unary_operators.bin", "r");
    assert(file != nullptr);
    assert(fread(buffer, 1, MAX_FILE_SIZE, file) < MAX_FILE_SIZE);

    if (strstr(buffer, lexem) != nullptr){ return true; }

    return false;
}

/**
 * @brief Определяет является ли строка бинарным опреатором.
 * 
 * @param lexem 
 * @return true 
 * @return false 
 */
bool find_in_binary_op(const NodeData lexem){

    char buffer[MAX_FILE_SIZE] = "";
    FILE* file = fopen("../bin/binary_operators.bin", "r");
    assert(file != nullptr);
    assert(fread(buffer, 1, MAX_FILE_SIZE, file) < MAX_FILE_SIZE);

    if (strstr(buffer, lexem) != nullptr){ return true; }

    return false;
}

/**
 * @brief Определяет является ли строка константой.
 * 
 * @param lexem 
 * @return true 
 * @return false 
 */
bool find_in_consts(const NodeData lexem){

    char buffer[MAX_FILE_SIZE] = "";
    FILE* file = fopen("../bin/consts.bin", "r");
    assert(file != nullptr);
    assert(fread(buffer, 1, MAX_FILE_SIZE, file) < MAX_FILE_SIZE);

    if (strstr(buffer, lexem) != nullptr){ return true; }

    return false;
}   

/**
 * @brief Определяет является ли строка числом.
 * 
 * @param lexem 
 * @return true 
 * @return false 
 */
bool is_number(const NodeData lexem){

    char* endptr = nullptr;
    strtod(lexem, &endptr);

    if (endptr != nullptr){ return true; }

    return false;
}

/**
 * @brief Определяет является ли строка переменной. Переменная - любая последовательность строчных букв, которая не является константой или оператором.
 * 
 * @param lexem 
 * @return true 
 * @return false 
 */
bool is_var(const NodeData lexem){

    if (find_in_unary_op(lexem) || find_in_consts(lexem)){
        return false;
    }

    for (int i = 0; i < strlen(lexem); i++){

        if ((lexem[i] < 'a') || (lexem[i] > 'z')){ 

            return false; 
        }
    }

    return true;
}

/**
 * @brief Проверка связности дерева.
 * 
 * @return true 
 * @return false 
 */
bool Node::check_tree(){

    if (this == nullptr){ return false; }

    if (left != nullptr){ 

        if (left->prev != this){

            return false;
        }

        return left->check_tree();
    } 

    if (right != nullptr){

        if (right->prev != this){

            return false;
        }

        return right->check_tree();
    }

    return true;
}

/**
 * @brief Определение типа узла по его data.
 * 
 * @param node_data 
 * @return NodeType 
 */
NodeType define_type(NodeData node_data){

    if (node_data == nullptr){ return Nothing; }
    if (find_in_unary_op(node_data) || find_in_binary_op(node_data)){ return Op; }
    if (find_in_consts(node_data) || is_number(node_data)){ return Const_or_num; }
    if (is_var(node_data)){ return Var; }

    return -1;
}

/**
 * @brief Определение типа узла по его data.
 * 
 * @param node_data 
 * @return int 
 */
int define_priority(NodeData node_data){

    if (node_data == nullptr){ return nothing; }
    if (find_in_unary_op(node_data)){ return unary_op; }
    if (strcmp(node_data, "^") == 0){ return power; } 
    if ((strcmp(node_data, "*") == 0) || (strcmp(node_data, "/") == 0)){ return mul_or_div; }
    if (strcmp(node_data, "-") == 0){ return sub; }
    if (strcmp(node_data, "+") == 0){ return sum; }

    return const_or_num;
}

/**
 * @brief Создание узла дерева по его данным и двум потомкам. Производится связывание детей с родителем без доп проверок.
 * 
 * @param new_left 
 * @param new_data 
 * @param new_right 
 */
Node::Node(Node* new_left, NodeData new_data, Node* new_right){
    
    type = define_type(new_data);
    priority = define_priority(new_data);

    if (type == -1){

        printf("Syntax error: |%s|\n", new_data);
        exit(0);
    }

    left = new_left;
    right = new_right;

    if (new_left != nullptr){ new_left->prev = this; }
    if (new_right != nullptr){ new_right->prev = this; }

    if (new_data != nullptr){

        data = new char[DATA_SIZE];
        assert(data != nullptr);
        memcpy(data, new_data, DATA_SIZE);  
    } else{

        data = nullptr;
    }
    
}

/**
 * @brief Полное рекурсирвное копирование поддерева.
 * 
 * @param cur_root 
 * @return Node* 
 */
Node* Node::copy_tree(Node* cur_root){

    if (cur_root != nullptr){

        Node* new_root = new Node;
        assert(new_root != nullptr);
        Node* new_left = copy_tree(cur_root->get_left());
        Node* new_right = copy_tree(cur_root->get_right());

        new_root->add_branches(new_left, new_right);

        new_root->type = cur_root->type;
        new_root->priority = cur_root->priority;

        if (cur_root->data != nullptr){

            new_root->data = new char[DATA_SIZE];
            memcpy(new_root->data, cur_root->data, DATA_SIZE);
        } else{

            new_root->data = nullptr;
        }

        return new_root;
    } else{

        return nullptr;
    }
}

/**
 * @brief Конструктор копирования на основе функции copy_tree.
 * 
 * @param old_node 
 */
Node::Node(const Node& old_node){

    Node* new_left = copy_tree(old_node.left);
    Node* new_right = copy_tree(old_node.right);

    add_branches(new_left, new_right);

    type = old_node.type;
    priority = old_node.priority;

    if (old_node.data != nullptr){

        memcpy(data, old_node.data, DATA_SIZE);
    } else{

        delete data;
        data = nullptr;
    }   
}

/**
 * @brief Перемещающий конструктор.
 * 
 * @param rv_node 
 */
Node::Node(Node&& rv_node){

    std::swap(data, rv_node.data);
    std::swap(left, rv_node.left);
    std::swap(right, rv_node.right);

    type = rv_node.type;
    priority = rv_node.priority;
}

/**
 * @brief Деструктор.
 * 
 */
Node::~Node(){

    delete left;
    delete right;

    delete[] data;
}

/**
 * @brief Копирующий оператор присваивания на основе copy_tree.
 * 
 * @param old_node 
 * @return Node& 
 */
Node& Node::operator =(const Node& old_node){

    if (this == &old_node){ return *this; }

    Node* new_left = copy_tree(old_node.left);
    Node* new_right = copy_tree(old_node.right);

    add_branches(new_left, new_right);

    type = old_node.type;
    priority = old_node.priority;
    memcpy(data, old_node.data, DATA_SIZE);

    return *this;
}

/**
 * @brief Перемещающий оператор присваивания.
 * 
 * @param rv_node 
 * @return Node& 
 */
Node& Node::operator =(Node&& rv_node){

    if (this == &rv_node){ return *this; }

    std::swap(data, rv_node.data);
    std::swap(left, rv_node.left);
    std::swap(right, rv_node.right);

    type = rv_node.type;
    priority = rv_node.priority;

    return *this;
}

/**
 * @brief Возврат типа узла.
 * 
 * @return NodeType 
 */
NodeType Node::get_type(){

    return type;
}

/**
 * @brief Возврат приоритета узла.
 * 
 * @return int 
 */
int Node::get_priority(){

    return priority;
}

/**
 * @brief Получить левый узел.
 * 
 * @return Node* 
 */
Node* Node::get_left(){

    return left;
}

/**
 * @brief Получить правый узел.
 * 
 * @return Node* 
 */
Node* Node::get_right(){

    return right;
}

/**
 * @brief Получить прошлый узел.
 * 
 * @return Node* 
 */
Node* Node::get_prev(){

    return prev;
}

/**
 * @brief Является ли узел листом.
 * 
 * @return true 
 * @return false 
 */
bool Node::is_leaf(){

    assert(((right != nullptr) && (left != nullptr)) 
        || ((right == nullptr) && (left == nullptr)));

    if ((left == nullptr) && (right == nullptr)){ return true; }

    return false;
}

/**
 * @brief Добавление новых потомков и удаление старых. Потомки связываются с родителем без дополнительных проверок.
 * 
 * @param new_left 
 * @param new_right 
 */
void Node::add_branches(Node* new_left, Node* new_right){
    
    delete left;
    delete right;

    left = new_left;
    right = new_right;

    if (new_left != nullptr){ new_left->prev = this; }
    if (new_right != nullptr){ new_right->prev = this; }
}

/**
 * @brief Изменение данных с изменением типа и приоритета.
 * 
 * @param new_data 
 */
void Node::change_data(NodeData new_data){

    delete data;
    data = nullptr; //чтобы не сжохнуть от двойного освобождения при исключении
    data = new char[DATA_SIZE];
    memcpy(data, new_data, strlen(new_data));

    type = define_type(new_data);
    priority = define_priority(new_data);
}

/**
 * @brief Разрыв родителя с левым потомком.
 * 
 */
void Node::unlink_left(){

    if (left != nullptr){

        left->prev = nullptr;
        left = nullptr;
    }
}

/**
 * @brief Разрыв родителя с правым потомком.
 * 
 */
void Node::unlink_right(){

    if (right != nullptr){

        right->prev = nullptr;
        right = nullptr;
    }
}

/**
 * @brief Выбор цвета для узла Graphviz.
 * 
 * @param outp_file 
 * @param node_type 
 */
void choose_color(FILE* outp_file, NodeType node_type){

    switch (node_type){

        case Nothing:
            fprintf(outp_file, "fillcolor=""lightgrey""]\n");
            break;
        
        case Op:
            fprintf(outp_file, "fillcolor=""green""]\n");
            break;
        
        case Var:
            fprintf(outp_file, "fillcolor=""yellow""]\n");
            break;
        
        case Const_or_num:
            fprintf(outp_file, "fillcolor=""blue""]\n");
            break;
    }
}

/**
 * @brief Рекурсивный вывод дерева через Graphviz.
 * 
 * @param cur_node 
 * @param outp_file 
 */
void Node::print_node_graphviz(Node* cur_node, FILE* outp_file){

    if (cur_node->data != nullptr){

        fprintf(outp_file, "\"%p\" [label= \"%s\" ", cur_node, cur_node->data);
    } else{

        fprintf(outp_file, "\"%p\" [label= \"null\"", cur_node);   
    }

    choose_color(outp_file, cur_node->type);

    if (cur_node->left != nullptr){

        fprintf(outp_file, "\"%p\" -> \"%p\"\n", cur_node, cur_node->left);
        print_node_graphviz(cur_node->left, outp_file);
    }

    if (cur_node->right != nullptr){

        fprintf(outp_file, "\"%p\" -> \"%p\"\n", cur_node, cur_node->right);
        print_node_graphviz(cur_node->right, outp_file);
    }
    
}

/**
 * @brief Вывод дерева.
 * 
 * @param outp_file 
 */
void Node::dump_graphviz(FILE* outp_file){

    assert(outp_file != nullptr);

    fprintf(outp_file, "digraph Dump{\n");
    fprintf(outp_file, "node[color=""red"",fontsize=14, style=""filled""]\n");
    
    print_node_graphviz(this, outp_file);

    fprintf(outp_file, "}\n");
}
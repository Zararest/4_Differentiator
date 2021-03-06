#include "headers/Node.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <assert.h>
#include <algorithm>
#include <errno.h>

#define MAX_FILE_SIZE 200

/**
 * @brief Определяет является ли строка унарным оператором.
 * 
 * @param lexem 
 * @return true 
 * @return false 
 */
bool find_in_unary_op(ConstNodeData lexem){

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
bool find_in_binary_op(ConstNodeData lexem){

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
bool find_in_consts(ConstNodeData lexem){

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
bool is_number(ConstNodeData lexem){

    char* endptr = nullptr;
    double ret = strtod(lexem, &endptr);
    int err = errno;

    if ((ret == 0) && (*endptr == '\0')){ return true; }
    if (ret != 0){ return true; }

    return false;
}

/**
 * @brief Определяет является ли строка переменной. Переменная - любая последовательность строчных букв, которая не является константой или оператором.
 * 
 * @param lexem 
 * @return true 
 * @return false 
 */
bool is_var(ConstNodeData lexem){

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
NodeType define_type(ConstNodeData node_data){

    if (node_data == nullptr){ return Nothing; }
    if (find_in_unary_op(node_data) || find_in_binary_op(node_data)){ return Op; }
    if (find_in_consts(node_data) || is_number(node_data)){ return Const_or_num; }
    if (is_var(node_data)){ return Var; }
    if (strcmp(node_data, "|") == 0){ return Diff; }

    return -1;
}

/**
 * @brief Определение типа узла по его data.
 * 
 * @param node_data 
 * @return NodePriority 
 */
NodePriority define_priority(ConstNodeData node_data){

    if (node_data == nullptr){ return nothing; }
    if (find_in_unary_op(node_data)){ return unary_op; }
    if (strcmp(node_data, "^") == 0){ return power; } 
    if ((strcmp(node_data, "*") == 0) || (strcmp(node_data, "/") == 0)){ return mul_or_div; }
    if (strcmp(node_data, "-") == 0){ return sub; }
    if (strcmp(node_data, "+") == 0){ return sum; }
    if (strcmp(node_data, "|") == 0){ return differ; }

    return const_or_var;
}

/**
 * @brief Создание узла дерева по его данным и двум потомкам. Производится связывание детей с родителем без доп проверок.
 * 
 * @param new_left 
 * @param new_data 
 * @param new_right 
 */
Node::Node(Node* new_left, ConstNodeData new_data, Node* new_right){
    
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

        data = new char[MAX_DATA_SIZE];
        assert(data != nullptr);
        memcpy(data, new_data, MAX_DATA_SIZE);  
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
Node* Node::copy_tree(const Node* cur_root){

    if (cur_root != nullptr){

        Node* new_root = new Node;
        assert(new_root != nullptr);
        Node* new_left = copy_tree(cur_root->get_left());
        Node* new_right = copy_tree(cur_root->get_right());

        new_root->add_branches(new_left, new_right);

        new_root->type = cur_root->type;
        new_root->priority = cur_root->priority;

        if (cur_root->data != nullptr){

            new_root->data = new char[MAX_DATA_SIZE];
            memcpy(new_root->data, cur_root->data, MAX_DATA_SIZE);
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

        memcpy(data, old_node.data, MAX_DATA_SIZE);
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
    memcpy(data, old_node.data, MAX_DATA_SIZE);

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
Node* Node::get_left() const{

    return left;
}

/**
 * @brief Получить правый узел.
 * 
 * @return Node* 
 */
Node* Node::get_right() const{

    return right;
}

/**
 * @brief Получить прошлый узел.
 * 
 * @return Node* 
 */
Node* Node::get_prev() const{

    return prev;
}

/**
 * @brief Возвращает значение узла, если его тип Const_or_num, и -1, если другой тип. 
 * 
 * @return double 
 */
double Node::get_num() const{

    if (type != Const_or_num){ return -1; }

    if (strcmp(data, "e") == 0){ return 2.72; }
    if (strcmp(data, "pi") == 0){ return 3.14; }
    
    return strtod(data, nullptr);
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
 * @brief Яаляется ли содержимое узла переменной.
 * 
 * @return true 
 * @return false 
 */
bool Node::is_variable(){

    return is_var(data);
}

/**
 * @brief Сравнение данных узла и переданной строки.
 * 
 * @param outer_data 
 * @return true 
 * @return false 
 */
bool Node::cmp_data(ConstNodeData outer_data) const{

    if (outer_data == nullptr){

        if (data == nullptr){ return true; }

        return false;
    }

    if (data == nullptr){ return false; }
    
    if (strcmp(data, outer_data) == 0){ return true; }

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
 * @brief Меняет местами потомков узла.
 * 
 */
void Node::swap_branches(){

    Node* tmp = right;
    right = left;
    left = tmp;
}

/**
 * @brief Изменение данных с изменением типа и приоритета.
 * 
 * @param new_data 
 */
void Node::change_data(ConstNodeData new_data){

    delete data;
    data = nullptr; //чтобы не сжохнуть от двойного освобождения при исключении
    data = new char[MAX_DATA_SIZE];
    assert(strlen(new_data) < MAX_DATA_SIZE);
    memcpy(data, new_data, strlen(new_data) + 1);

    type = define_type(new_data);
    priority = define_priority(new_data);
}

/**
 * @brief Меняет местами указатели на данные и тип с приоритетом. 
 * 
 * @param tmp_node 
 */
void Node::move_data(Node* tmp_node){

    NodeData tmp_data = data;
    NodeType tmp_type = type;
    NodePriority tmp_prior = priority;

    data = tmp_node->data;
    tmp_node->data = tmp_data;

    type = tmp_node->type;
    tmp_node->type = tmp_type;
    
    priority = tmp_node->priority;
    tmp_node->priority = tmp_prior;
}

/**
 * @brief Разрыв данного узла с левым потомком.
 * 
 */
void Node::unlink_left(){

    if (left != nullptr){

        left->prev = nullptr;
        left = nullptr;
    }
}

/**
 * @brief Разрыв данного узла с правым потомком.
 * 
 */
void Node::unlink_right(){

    if (right != nullptr){

        right->prev = nullptr;
        right = nullptr;
    }
}

/**
 * @brief Разрыв данного узла с родителем.
 * 
 */
void Node::unlink_parent(){

    if (prev != nullptr){

        if (prev->left == this){

            prev->left = nullptr;
        } else{

            prev->right = nullptr;
        }

        prev = nullptr;
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
            fprintf(outp_file, "fillcolor=""lightblue""]\n");
            break;
        
        case Diff:
            fprintf(outp_file, "fillcolor=""red""]\n");
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
 * @param graph_file 
 */
void Node::dump_graphviz(FILE* graph_file){

    assert(graph_file != nullptr);

    fprintf(graph_file, "digraph Dump{\n");
    fprintf(graph_file, "node[color=""red"",fontsize=14, style=""filled""]\n");
    
    if (this != nullptr){ print_node_graphviz(this, graph_file); }
    
    fprintf(graph_file, "}\n");
}

/**
 * @brief Рекурсивный вывод выражения в файл Latex.
 * 
 * @param cur_node 
 * @param output_file 
 */
void Node::print_node_latex(Node* cur_node, FILE* outp_file){
    
    assert(outp_file != nullptr);
    assert(cur_node != nullptr);

    fflush(outp_file);

    if (cur_node->is_leaf()){

        cur_node->print_node_data(outp_file);
        return;
    }

    int cur_priority = cur_node->get_priority();
    
    if (cur_node->cmp_data("/")){

        fprintf(outp_file, "\\frac{");
        print_node_latex(cur_node->get_left(), outp_file);
        fprintf(outp_file, "}{");
        print_node_latex(cur_node->get_right(), outp_file);
        fprintf(outp_file, "}");
        return;
    }

    if (cur_node->get_left()->get_priority() <= cur_priority){

        print_node_latex(cur_node->get_left(), outp_file);
        cur_node->print_node_data(outp_file);
    } else{

        fprintf(outp_file, "(");
        print_node_latex(cur_node->get_left(), outp_file);
        fprintf(outp_file, ")");
        cur_node->print_node_data(outp_file);
    }

    if (cur_priority == power){

        fprintf(outp_file, "{");
        print_node_latex(cur_node->get_right(), outp_file);
        fprintf(outp_file, "}");
        return;
    }

    if (cur_priority == unary_op){

        fprintf(outp_file, "(");
        print_node_latex(cur_node->get_right(), outp_file);
        fprintf(outp_file, ")");
        return;
    }

    if (cur_priority == differ){

        print_node_latex(cur_node->get_right(), outp_file);
        fprintf(outp_file, "^2");
        return;
    }

    if (cur_node->get_right()->get_priority() <= cur_priority){

        print_node_latex(cur_node->get_right(), outp_file);
    } else{

        fprintf(outp_file, "(");
        print_node_latex(cur_node->get_right(), outp_file);
        fprintf(outp_file, ")");
    }
}

void Node::print_node_data(FILE* outp_file){

    if (priority == nothing){ return; }

    if (priority == differ){ 
        
        fprintf(outp_file, "|_");
        return;    
    }

    if (priority == unary_op){

        fprintf(outp_file, "\\");
    }

    fprintf(outp_file, "%s", data);
}

/**
 * @brief Вывод выражения в файл Latex.
 * 
 * @param latex_file 
 */
void Node::dump_latex(FILE* latex_file){

    assert(latex_file != nullptr);

    fprintf(latex_file, "\\begin{displaymath}\n");

    if (this != nullptr){ print_node_latex(this, latex_file); }

    fprintf(latex_file, "\n\\end{displaymath}\n");

}
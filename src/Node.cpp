#include "headers/Node.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <assert.h>
#include <algorithm>

#define MAX_FILE_SIZE 200

bool find_in_unary_op(NodeData lexem){

    char buffer[MAX_FILE_SIZE];
    FILE* file = fopen("../bin/unary_operators.bin", "r");
    assert(file != nullptr);
    assert(fread(buffer, 1, MAX_FILE_SIZE, file) < MAX_FILE_SIZE);

    if (strstr(buffer, lexem) != nullptr){ return true; }

    return false;
}

bool find_in_binary_op(NodeData lexem){

    char buffer[MAX_FILE_SIZE];
    FILE* file = fopen("../bin/binary_operators.bin", "r");
    assert(file != nullptr);
    assert(fread(buffer, 1, MAX_FILE_SIZE, file) < MAX_FILE_SIZE);

    if (strstr(buffer, lexem) != nullptr){ return true; }

    return false;
}

bool find_in_consts(NodeData lexem){

    char buffer[MAX_FILE_SIZE];
    FILE* file = fopen("../bin/consts.bin", "r");
    assert(file != nullptr);
    assert(fread(buffer, 1, MAX_FILE_SIZE, file) < MAX_FILE_SIZE);

    if (strstr(buffer, lexem) != nullptr){ return true; }

    return false;
}   

bool is_number(NodeData lexem){

    char* endptr = nullptr;
    strtod(lexem, &endptr);

    if (endptr != nullptr){ return true; }

    return false;
}

bool is_var(NodeData lexem){

    if ((strlen(lexem) == 1)
        && ((lexem[0] < '0')
            || (lexem[0] > '9')))
    {
        return true;
    }

    return false;
}

Node::Node(NodeType new_type, NodeData new_data, Node* prev_node){

    type = new_type;
    data = new char[DATA_SIZE];
    assert(data != nullptr);
    memcpy(data, new_data, DATA_SIZE);
    prev = prev_node;
}

bool Node::check_tree(){

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

NodeType define_type(NodeData node_data){

    if (node_data == nullptr){ return Nothing; }
    if (find_in_unary_op(node_data) || find_in_binary_op(node_data)){ return Op; }
    if (find_in_consts(node_data) || is_number(node_data)){ return Const_or_num; }
    if (is_var(node_data)){ return Var; }

    return -1;
}

Node::Node(Node* new_left, NodeData new_data, Node* new_right){

    type = define_type(new_data);

    if (type == -1){

        printf("Syntax error: |%s|\n", new_data);
        exit(0);
    }

    left = new_left;
    right = new_right;

    if (new_left != nullptr){ new_left->prev = this; }
    if (new_right != nullptr){ new_right->prev = this; }
}

Node* Node::copy_tree(Node* cur_root){

    if (cur_root != nullptr){

        Node* new_root = new Node;
        assert(new_root != nullptr);
        Node* new_left = copy_tree(cur_root->get_left());
        Node* new_right = copy_tree(cur_root->get_right());

        new_root->add_branches(new_left, new_right);

        new_root->type = cur_root->type;
        new_root->data = new char[DATA_SIZE];
        memcpy(new_root->data, cur_root->data, DATA_SIZE);
    
        return new_root;
    } else{

        return nullptr;
    }
}

Node::Node(const Node& old_node){

    Node* new_left = copy_tree(old_node.left);
    Node* new_right = copy_tree(old_node.right);

    add_branches(new_left, new_right);

    type = old_node.type;
    memcpy(data, old_node.data, DATA_SIZE);
}

Node::Node(Node&& rv_node){

    std::swap(data, rv_node.data);
    std::swap(left, rv_node.left);
    std::swap(right, rv_node.right);

    type = rv_node.type;
}

Node::~Node(){

    delete left;
    delete right;

    delete[] data;
}

Node& Node::operator =(const Node& old_node){

    if (this == &old_node){ return *this; }

    Node* new_left = copy_tree(old_node.left);
    Node* new_right = copy_tree(old_node.right);

    add_branches(new_left, new_right);

    type = old_node.type;
    memcpy(data, old_node.data, DATA_SIZE);

    return *this;
}

Node& Node::operator =(Node&& rv_node){

    if (this == &rv_node){ return *this; }

    std::swap(data, rv_node.data);
    std::swap(left, rv_node.left);
    std::swap(right, rv_node.right);

    type = rv_node.type;

    return *this;
}

NodeType Node::get_type(){

    return type;
}

int Node::get_priority(){

    if (data == nullptr){ return nothing; }
    if (find_in_unary_op(data)){ return unary_op; }
    if (strcmp(data, "^") == 0){ return power; } 
    if ((strcmp(data, "*") == 0) || (strcmp(data, "/") == 0)){ return mul_or_div; }
    if (strcmp(data, "-") == 0){ return sub; }
    if (strcmp(data, "+") == 0){ return sum; }

    return const_or_num;
}

Node* Node::get_left(){

    return left;
}

Node* Node::get_right(){

    return right;
}

Node* Node::get_prev(){

    return prev;
}

bool Node::is_leaf(){

    assert(((right != nullptr) && (left != nullptr)) 
        || ((right == nullptr) && (left == nullptr)));

    if ((left == nullptr) && (right == nullptr)){ return true; }

    return false;
}

void Node::add_branches(Node* new_left, Node* new_right){
    
    delete left;
    delete right;

    left = new_left;
    right = new_right;

    if (new_left != nullptr){ new_left->prev = this; }
    if (new_right != nullptr){ new_right->prev = this; }
}

void Node::change_data(NodeData new_data){

    delete data;
    data = nullptr; //чтобы не сжохнуть от двойного освобождения при исключении
    data = new char[DATA_SIZE];
    memcpy(data, new_data, strlen(new_data));

    type = define_type(new_data);
}

void Node::unlink_left(){

    if (left != nullptr){

        left->prev = nullptr;
        left = nullptr;
    }
}

void Node::unlink_right(){

    if (right != nullptr){

        right->prev = nullptr;
        right = nullptr;
    }
}

void Node::print_node_graphviz(Node* cur_node, FILE* outp_file){


    fprintf(outp_file, "%s[fillcolor=""red""]\n", cur_node->data);
    choose_color(outp_file, cur_node->type);

    if (cur_node->left != nullptr){

        fprintf(outp_file, "%s -> %s\n", cur_node->data, cur_node->left->data);
        print_node_graphviz(cur_node->left, outp_file);
    }

    if (cur_node->right != nullptr){

        fprintf(outp_file, "%s -> %s\n", cur_node->data, cur_node->right->data);
        print_node_graphviz(cur_node->right, outp_file);
    }
    
}

void choose_color(FILE* outp_file, NodeType node_type){

    switch (node_type){

        case Nothing:
            fprintf(outp_file, "[fillcolor=""lightgrey""]\n");
            break;
        
        case Op:
            fprintf(outp_file, "[fillcolor=""green""]\n");
            break;
        
        case Var:
            fprintf(outp_file, "[fillcolor=""yellow""]\n");
            break;
        
        case Const_or_num:
            fprintf(outp_file, "[fillcolor=""blue""]\n");
            break;
    }
}

void Node::dump_graphviz(FILE* outp_file){

    assert(outp_file != nullptr);

    fprintf(outp_file, "digraph Dump{\n");
    
    fprintf(outp_file, "node[color=""red"",fontsize=14, style=""filled""]\n");
    fprintf(outp_file, "%s", data);
    choose_color(outp_file, type);

    if (left != nullptr){
        fprintf(outp_file, "%s -> %s\n", data, left->data);
        print_node_graphviz(left, outp_file);
    }
    if (right != nullptr){
        fprintf(outp_file, "%s -> %s\n", data, left->data);
        print_node_graphviz(right, outp_file);
    }
    fprintf(outp_file, "}\n");
}
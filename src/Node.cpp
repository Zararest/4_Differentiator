#include "headers/Node.h"

#include "string.h"
#include "assert.h"
#include <algorithm>

Node::Node(NodeType new_type, NodeData new_data, Node* prev_node){

    type = new_type;
    data = new char[DATA_SIZE];
    assert(data != nullptr);
    memcpy(data, new_data, DATA_SIZE);
    prev = prev_node;
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

bool find_in_unary_op(NodeData lexem){

}

bool find_in_consts(NodeData lexem){

}

bool is_number(NodeData lexem){

}

int Node::get_priority(){

    if (data == nullptr){ return nothing; }
    if (find_in_unary_op(data)){ return unary_op; }
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
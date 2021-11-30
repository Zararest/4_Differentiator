#include "headers/Differ.h"

#include <stdlib.h>
#include <assert.h> 

char check_spaces(FILE* file){

    char symbol = '\0';
    while ((symbol = fgetc(file)) == ' '){

        if ((symbol == EOF) || (symbol == '\n')){

            printf("Incorrect data\n");
            exit(0);
        }
    }

    return symbol;
}

NodeData get_op(FILE* file, NodeData op){

    char symbol = check_spaces(file);

    if (symbol == ')'){

        //printf("There is no operator\n");
        //exit(0);
        return nullptr;
    }    

    op[DATA_SIZE] = '\0';
    int operator_size = 1;

    while (symbol != '('){

        if ((symbol == EOF) || (symbol == '\n')){

            printf("Incorrect data\n");
            exit(0);
        }

        if (operator_size > DATA_SIZE){

            printf("Expression is too big: |%s|\n", op);
            exit(0);
        }

        if (symbol != ' '){

            op[operator_size - 1] = symbol;
            operator_size++;
        }
        
        symbol = fgetc(file);
    }
    op[operator_size - 1] = '\0';

    if (symbol != '('){

        printf("Incorrect operator: |%s|\n", op);
        exit(0);
    } 

    return op;
}

Node* Differ::get_tree(FILE* file){

    char symbol = check_spaces(file);
    char op[DATA_SIZE];

    if (symbol == '('){ 

        return new Node(get_tree(file), get_op(file, op), get_tree(file));
    }

    char lexem[DATA_SIZE + 1];
    lexem[DATA_SIZE] = '\0';
    int lexem_size = 1;

    while (symbol != ')'){

        if ((symbol == EOF) || (symbol == '\n')){

            printf("Incorrect data\n");
            exit(0);
        }

        if (lexem_size > DATA_SIZE){

            printf("Expression is too big: |%s|\n", lexem);
            exit(0);
        }
        
        if (symbol != ' '){

            lexem[lexem_size - 1] = symbol;
            lexem_size++;
        }

        symbol = fgetc(file);
    }

    if (lexem_size > 1){

        lexem[lexem_size - 1] = '\0';
        return new Node(nullptr, lexem, nullptr);
    } else{

        return new Node(nullptr, nullptr, nullptr);
    }
    
}


Differ::Differ(FILE* file){
    
    char op[DATA_SIZE];
    root = get_tree(file);
    //Node* left_brunch = get_tree(file);
    //root->change_data(get_op(file, op));
    //Node* right_brunch = get_tree(file);
}

Differ::~Differ(){

    delete root;
}

void Differ::dump_graphiz(FILE* graph_file){

    assert(graph_file != nullptr);
    assert(root->check_tree());

    root->dump_graphviz(graph_file);
}

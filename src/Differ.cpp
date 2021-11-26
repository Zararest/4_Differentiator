#include "headers/Differ.h"

#include <stdlib.h>
#include <assert.h> 

NodeData get_op(FILE* file){

    char symbol = check_spaces(file);

    if (symbol == ')'){

        printf("There is no operator\n");
        exit(0);
    }    

    char op[DATA_SIZE + 1];
    op[DATA_SIZE] = '\0';
    int operator_size = 1;

    while (((symbol = fgetc(file)) != '(') && (symbol != ' ')){

        if ((symbol == EOF) || (symbol == '\n')){

            printf("Incorrect data\n");
            exit(0);
        }

        if (operator_size > DATA_SIZE){

            printf("Expression is too big: |%s|\n", op);
            exit(0);
        }

        op[operator_size - 1] = symbol;
        operator_size++;
    }
    op[operator_size - 1] = '\0';

    if ((symbol == '(') || (check_spaces(file) == '(')){ return op; }

    printf("Incorrect operator: |%s|\n", op);
    exit(0);

    return nullptr;
}

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

Node* Differ::get_tree(FILE* file){

    char symbol = check_spaces(file);

    if (symbol == '('){ 

        return new Node(get_tree(file), get_op(file), get_tree(file));
    }

    char lexem[DATA_SIZE + 1];
    lexem[DATA_SIZE] = '\0';
    int lexem_size = 1;

    while (((symbol = fgetc(file)) != ')') && (symbol != ' ')){

        if ((symbol == EOF) || (symbol == '\n')){

            printf("Incorrect data\n");
            exit(0);
        }

        if (lexem_size > DATA_SIZE){

            printf("Expression is too big: |%s|\n", lexem);
            exit(0);
        }

        lexem[lexem_size - 1] = symbol;
        lexem_size++;
    }
    lexem[lexem_size - 1] = '\0';

    return new Node(nullptr, lexem, nullptr);
}


Differ::Differ(FILE* file){
    
    Node* left_brunch = get_tree(file);
    root->change_data(get_op(file));
    Node* right_brunch = get_tree(file);
}

void Differ::dump_graphiz(FILE* graph_file){

    assert(graph_file != nullptr);
    assert(root->check_tree());

    root->dump_graphviz(graph_file);
}
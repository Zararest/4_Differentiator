#define PRINT_ERROR_POS fprintf(stderr, "Syntax error:\n");                    \
                        fwrite(line, sizeof(char), cur_pos + 1, stderr);       \
                        putc('\n', stderr);                                    \
                        for(int i = 0; i < cur_pos; i++){ putc('~', stderr); } \
                        putc('^', stderr);

#define IS_REG_EXPR_TRUTHFUL if (!error_has_occured){        \
                                 return fir_arg;             \
                             } else{                         \
                                 error_has_occured = false;  \
                             }

#define SYNTAX_ERROR(expr, action)  if (!(expr) && !error_has_occured){ \
                                        action;                         \
                                        error_has_occured = true;       \
                                    }   


#define REQUIRE(symbol) line[cur_pos] == symbol

#define FUNC_MINUS_ONE_NODE(func_node) new Node(func_node, "-", new Node(nullptr, "1", nullptr))
#define EMPTY_NODE new Node(nullptr, nullptr, nullptr)
#define GET_LL_BRNCH cur_node->get_left()->get_left()
#define GET_LR_BRNCH cur_node->get_left()->get_right()


#define DIFF_POWER  do{                                                                 \
                        tmp_power_root = cur_node;                                      \
                                                                                        \
                        tmp_power_left = Node::copy_tree(tmp_power_root);               \
                        cur_node = tmp_power_left;                                      \
                        DIFF_COMPLEX(DIFF_EXP_FUNC);                                    \
                        tmp_power_left = cur_node;                                      \
                                                                                        \
                        cur_node = Node::copy_tree(tmp_power_root);                     \
                        cur_node->swap_branches();                                      \
                        DIFF_COMPLEX(cur_node->swap_branches();  DIFF_POW_FUNC);        \
                        cur_node->swap_branches();                                      \
                        tmp_power_right = cur_node;                                     \
                                                                                        \
                        tmp_power_root->add_branches(tmp_power_left, tmp_power_right);  \
                        tmp_power_root->change_data("+");                               \
                    } while (0)

#define DIFF_POW_FUNC   do{                                                                     \
                            tmp_left = Node::copy_tree(cur_node->get_right());                  \
                            tmp_diff = cur_node->get_left();                                    \
                            tmp_diff->unlink_parent();                                          \
                                                                                                \
                            tmp_root = cur_node->get_right();                                   \
                            tmp_root->unlink_parent();                                          \
                                                                                                \
                            tmp_right = new Node(tmp_diff, "^", FUNC_MINUS_ONE_NODE(tmp_root)); \
                                                                                                \
                            cur_node->add_branches(tmp_left, tmp_right);                        \
                            cur_node->change_data("*");                                         \
                        } while (0)


#define DIFF_EXP_FUNC   do{                                                                                 \
                            tmp_left = Node::copy_tree(cur_node);                                           \
                            tmp_right = new Node(EMPTY_NODE, "ln", Node::copy_tree(cur_node->get_left()));  \
                                                                                                            \
                            cur_node->add_branches(tmp_left, tmp_right);                                    \
                            cur_node->change_data("*");                                                     \
                        } while (0)

#define DIFF_SIN    do{                                 \
                        cur_node->change_data("cos");   \
                    } while (0)

#define DIFF_COS    do{                                                                     \
                        tmp_right = cur_node->get_right();                                  \
                        tmp_right->unlink_parent();                                         \
                                                                                            \
                        tmp_diff = new Node(EMPTY_NODE, "sin", tmp_right);                  \
                        cur_node->add_branches(new Node(nullptr, "-1", nullptr), tmp_diff); \
                        cur_node->change_data("*");                                         \
                    } while (0)

#define DIFF_TAN    do{                                                                         \
                        tmp_right = cur_node->get_right();                                      \
                        tmp_right->unlink_parent();                                             \
                                                                                                \
                        tmp_diff = new Node(EMPTY_NODE, "cos", tmp_right);                      \
                        tmp_right = new Node(tmp_diff, "^", new Node(nullptr, "2", nullptr));   \
                                                                                                \
                        cur_node->add_branches(new Node(nullptr, "1", nullptr), tmp_right);     \
                        cur_node->change_data("/");                                             \
                    } while (0)

#define DIFF_LN     do{                                                                         \
                        tmp_right = cur_node->get_right();                                      \
                        tmp_right->unlink_parent();                                             \
                                                                                                \
                        cur_node->add_branches(new Node(nullptr, "1", nullptr), tmp_right);     \
                        cur_node->change_data("/");                                             \
                    } while (0)                    

#define DECLARE_DIFF_VARS   Node* tmp_left = nullptr;           \
                            Node* tmp_right = nullptr;          \
                            Node* tmp_diff = nullptr;           \
                            Node* tmp_root = nullptr;           \
                            Node* tmp_power_root = nullptr;     \
                            Node* tmp_power_left = nullptr;     \
                            Node* tmp_power_right = nullptr;    \
                            Node* tmp_complex_left = nullptr;   \
                            Node* tmp_complex_right = nullptr;  \
                            Node* tmp_complex_diff = nullptr;   \
                            Node* tmp_complex_root = nullptr;   \
                            Node* tmp_swap = nullptr;           

/**
 * @brief Тут точно ебанет.
 * 
 */
#define DIFF_COMPLEX(diff_of_func)  do{                                                                     \
                                        tmp_complex_root = Node::copy_tree(cur_node);                       \
                                                                                                            \
                                        tmp_complex_diff = tmp_complex_root;                                \
                                        tmp_complex_root = cur_node;                                        \
                                        cur_node = tmp_complex_diff;                                        \
                                                                                                            \
                                        diff_of_func;                                                       \
                                                                                                            \
                                        tmp_complex_right = Node::copy_tree(tmp_complex_root->get_right()); \
                                        diff_node(tmp_complex_right);                                       \
                                                                                                            \
                                        tmp_complex_root->add_branches(cur_node, tmp_complex_right);        \
                                                                                                            \
                                        tmp_complex_root->change_data("*");                                 \
                                        cur_node = tmp_complex_root;                                        \
                                    } while (0)

#define DIFF_CONST  do{                             \
                        cur_node->change_data("0"); \
                    } while (0)     

#define DIFF_VAR  do{                                                       \
                        if (cur_node->cmp_data(cur_var)){                   \
                                                                            \
                            cur_node->change_data("1");                     \
                        } else{                                             \
                                                                            \
                            cur_node->change_data("0");                     \
                        }                                                   \
                    } while (0)  

#define DIFF_SUB    do{                                     \
                        diff_node(cur_node->get_left());    \
                        diff_node(cur_node->get_right());   \
                    } while (0)     

#define DIFF_SUM    do{                                     \
                        diff_node(cur_node->get_left());    \
                        diff_node(cur_node->get_right());   \
                    } while (0)    

//тут ебануло

#define DIFF_MUL    do{                                                                                          \
                        cur_node->change_data("+");                                                              \
                        tmp_left = cur_node->get_left();                                                         \
                        tmp_right = cur_node->get_right();                                                       \
                                                                                                                 \
                        tmp_left->unlink_parent();                                                               \
                        tmp_right->unlink_parent();                                                              \
                        cur_node->add_branches(new Node(nullptr, "*", nullptr), new Node(nullptr, "*", nullptr));\
                                                                                                                 \
                        tmp_diff = Node::copy_tree(tmp_right);                                                   \
                        diff_node(tmp_diff);                                                                     \
                        cur_node->get_left()->add_branches(tmp_left, tmp_diff);                                  \
                                                                                                                 \
                        tmp_diff = Node::copy_tree(tmp_left);                                                    \
                        diff_node(tmp_diff);                                                                     \
                        cur_node->get_right()->add_branches(tmp_diff, tmp_right);                                \
                    } while (0)    

/**
 * @brief  Если ебанет то на 84 и 89 строчках
 * 
 */

#define DIFF_DIV    do{                                                                                                         \
                        cur_node->change_data("/");                                                                             \
                        tmp_left = cur_node->get_left();                                                                        \
                        tmp_right = cur_node->get_right();                                                                      \
                                                                                                                                \
                        tmp_left->unlink_parent();                                                                              \
                        tmp_right->unlink_parent();                                                                             \
                        cur_node->add_branches(new Node(nullptr, "-", nullptr), new Node(nullptr, "^", nullptr));               \
                                                                                                                                \
                        cur_node->get_left()->add_branches(new Node(nullptr, "*", nullptr), new Node(nullptr, "*", nullptr));   \
                                                                                                                                \
                        tmp_diff = Node::copy_tree(tmp_left);                                                                   \
                        diff_node(tmp_diff);                                                                                    \
                        GET_LL_BRNCH->add_branches(tmp_diff, tmp_right);                                                        \
                        tmp_right = Node::copy_tree(tmp_right);                                                                 \
                                                                                                                                \
                        tmp_diff = Node::copy_tree(tmp_right);                                                                  \
                        diff_node(tmp_diff);                                                                                    \
                        GET_LR_BRNCH->add_branches(tmp_left, tmp_diff);                                                         \
                                                                                                                                \
                        cur_node->get_right()->add_branches(tmp_right, new Node(nullptr, "2", nullptr));                        \
                    } while (0)

#define REMOVE_LEFT_OPERAND do{                                                             \
                                tmp_right = cur_node->get_right()->get_right();             \
                                if (tmp_right != nullptr) { tmp_right->unlink_parent(); }   \
                                                                                            \
                                tmp_left = cur_node->get_right()->get_left();               \
                                if (tmp_left != nullptr){ tmp_left->unlink_parent(); }      \
                                                                                            \
                                cur_node->move_data(cur_node->get_right());                 \
                                cur_node->add_branches(tmp_left, tmp_right);                \
                            } while(0)

    

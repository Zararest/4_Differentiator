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

#define DIFF_EXP_POW_FUNC   do{                                             \
                                tmp_root = cur_node;                        \
                                                                            \
                                tmp_left = copy_tree(tmp_root);             \
                                cur_node = tmp_left;                        \
                                DIFF_COMPLEX(DIFF_EXP_FUNC);                \
                                tmp_left = cur_node;                        \
                                                                            \
                                cur_node = copy_tree(tmp_root);             \
                                DIFF_COMPLEX(DIFF_POW_FUNC);                \
                                tmp_right = cur_node;                       \
                                                                            \
                                tmp_root->add_branches(tmp_left, tmp_right);\
                                tmp_root->change_data("+");                 \
                            } while (0)

#define DIFF_POW_FUNC   do{                                                                     \
                            tmp_left = copy_tree(cur_node->get_right());                        \
                            tmp_diff = cur_node->get_left();                                    \
                                                                                                \
                            tmp_diff->unlink_parent();                                          \
                            tmp_root = cur_node->get_right();                                   \
                            tmp_root->unlink_parent();                                          \
                                                                                                \
                            tmp_right = new Node(tmp_diff, "^", FUNC_MINUS_ONE_NODE(tmp_root)); \
                        } while (0)


#define DIFF_EXP_FUNC   do{                                                                         \
                            tmp_left = copy_tree(cur_node);                                         \
                            tmp_right = new Node(EMPTY_NODE, "ln", copy_tree(cur_node->get_left()));\
                                                                                                    \
                            cur_node->add_brunches(tmp_left, tmp_right);                            \
                            cur_node->change_data("*");                                             \
                        } while (0)

#define DIFF_SIN    do{                                 \
                        cur_node->change_data("cos");   \
                    } while (0)


/**
 * @brief Тут точно ебанет.
 * 
 */
#define DIFF_COMPLEX(diff_of_func)  do{                                                 \
                                        tmp_root = copy_tree(cur_node);                 \
                                                                                        \
                                        tmp_diff = tmp_root;                            \
                                        tmp_root = cur_node;                            \
                                        cur_node = tmp_diff;                            \
                                                                                        \
                                        diff_of_func;                                   \
                                                                                        \
                                        tmp_right = copy_tree(tmp_root->get_right());   \
                                        diff_node(tmp_right);                           \
                                                                                        \
                                        tmp_root->add_branches(cur_node, tmp_right);    \
                                                                                        \
                                        tmp_root->change_data("*");                     \
                                    } while (0)

#define DIFF_POWER  do{             \
                                    \
                    } while (0)     

#define DIFF_CONST  do{                             \
                        cur_node->change_data("0"); \
                    } while (0)     

#define DIFF_VAR  do{                               \
                        cur_node->change_data("1"); \
                    } while (0)  

#define DIFF_SUB    do{                                     \
                        diff_node(cur_node->get_left());    \
                        diff_node(cur_node->get_right());   \
                    } while (0)     

#define DIFF_SUM    do{                                     \
                        diff_node(cur_node->get_left());    \
                        diff_node(cur_node->get_right());   \
                    } while (0)    

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
                        GET_LL_BRNCH->add_branches(tmp_left, tmp_diff);                                                         \
                                                                                                                                \
                        cur_node->get_right()->add_branches(tmp_right, new Node(nullptr, "2", nullptr));                        \
                    } while (0)    
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
#ifndef C_LIBS_CALC_HEADER_H
#define C_LIBS_CALC_HEADER_H

// assumptions
// equation will never be longer than 65535 char long

#include <stdint.h>
#include <string.h>

#ifndef MALLOC
#include <stdlib.h>
#define MALLOC malloc
#endif

#ifndef REALLOC
#include <stdlib.h>
#define REALLOC realloc
#endif

#ifndef ERROR
#include <stdio.h>
#define ERROR(...) fprintf(stderr, __VA_ARGS__)
#endif

#define ROOT_SYM "√"
#define ROOT_LEN (sizeof(ROOT_SYM) - 1)

// custom tpes for cleaner typing
typedef uint16_t expr_index;
typedef uint16_t token_index;
typedef uint16_t string_index;
// binding power type
typedef int8_t bp_t;

// special values an index can hold instead of pointing to something
#define EXPR_INDEX_EOF ((expr_index)0xFFFE)
#define EXPR_INDEX_ERROR ((expr_index)0xFFFF)
#define TOKEN_INDEX_ERROR ((token_index)0xFFFF)

// forward declaration and inline static

// 0 = not same
// 1 = same
// 2 = same but str0 is larger
// 3 = same but str1 is larger
inline static char str_cmp(const char *str0, const char *str1) {
    char out = 1;
    for (;;) {
        if ((*str0 == 0) && (*str1 == 0))
            break;
        else if ((*str0 != 0) && (*str1 == 0))
            out = 2;
        else if ((*str0 == 0) && (*str1 != 0))
            out = 3;
        else if ((*str0 != *str1))
            out = 0;
        else {
            str0++;
            str1++;
            continue;
        }
        break;
    }
    return out;
}

inline static char is_numeric(char c) { return '0' <= c && c <= '9'; }
inline static char is_alphaCap(char c) { return ('A' <= c && c <= 'Z'); }
inline static char is_alphaSmal(char c) { return ('a' <= c && c <= 'z'); }
inline static char is_aplha(char c) {
    return is_alphaSmal(c) || is_alphaCap(c);
}
inline static char is_root(const char *s) {
    return strncmp(s, ROOT_SYM, ROOT_LEN) == 0;
}
inline static char is_operation(const char *op) {
    return *op == '+' || *op == '-' || *op == '*' || *op == '/' || *op == '^' ||
           *op == '%' || is_root(op);
}
inline static unsigned char char_count(const char *s, char d) {
    unsigned char i = 0;
    for (; (s[i] != 0) && (i < 255); ++i) {
        if (s[i] != d)
            break;
    }
    return i;
}

typedef enum EXPRESSION_TYPE : uint8_t {
    EXPRESSION_EOF, // also used as unknown
    //** a and b is both EXPR **//
    EXPRESSION_ADD,  // +
    EXPRESSION_SUB,  // -
    EXPRESSION_MUL,  // *
    EXPRESSION_DIV,  // /
    EXPRESSION_POW,  // ^
    EXPRESSION_ROOT, // √
    //** a is EXPR and b is unused **//
    EXPRESSION_PERCENT, // % percent
    EXPRESSION_NEG,     // -x
    EXPRESSION_SQRT,    // √x
    //** a is token and b is unused **//
    EXPRESSION_VAR,   // only a
    EXPRESSION_VALUE, // only a

    EXPRESSION_ERROR, // error while parsing, can be because of invalid
                      // syntax or invalid token or memory allocation
} EXPRESSION_TYPE;
typedef enum OPERATION_TYPE : bp_t {
    OPERATION_EOF = 0,
    OPERATION_ADD = 1, // +
    OPERATION_SUB,     // -
    OPERATION_MUL,     // *
    OPERATION_DIV,     // /
    OPERATION_POW,     // ^
    OPERATION_ROOT,    // √
    OPERATION_PERCENT, // %
} OPERATION_TYPE;
typedef enum TOKEN_TYPE : uint8_t {
    TOKEN_EOF = 0,
    TOKEN_NUMBER = 1,
    TOKEN_NAME,
    TOKEN_OPERATION, // + - * / ^ % √
    TOKEN_ASSIGN     // =
} TOKEN_TYPE;

// tokenizer
// conert string data into token
typedef struct token_impl {
    string_index pos;     // where it starts
    string_index len;     // how long it is
    enum TOKEN_TYPE type; // what it is
} token_t;

typedef struct tokenizer_impl {
    string_index current;
} tokenizer_t;

// parser

typedef union expr_tok_duality {
    expr_index expr;
    token_index tok;
} expr_tok_duality;
// use tokenizer and use it to form AST
// struct that holds data of single expression
typedef struct expression_impl {
    // lhs
    // used to tell where is the lhs of the expression, or which token it is
    expr_tok_duality a_as;
    // rhs
    // used to tell where is the rhs of the expression
    expr_index b;
    EXPRESSION_TYPE type;
} expression_t;

// parser, also holds the tokens and expressions
typedef struct parser_impl {
    size_t expr_pool_count;
    size_t expr_pool_cap;
    size_t tok_pool_count;
    size_t tok_pool_cap;
    expression_t *expression_pool;
    token_t *token_pool;
} parser_t;
static const bp_t binding_power[][2] = {
    /*   L  R */
    {0, 0},   // EOF
    {10, 11}, // +
    {10, 11}, // -
    {20, 21}, // *
    {20, 21}, // /
    {31, 30}, // ^
    {31, 30}, // √ can be unary
    {40, 41}, // % always unary
};

// forward declaration of functions

// simple routine that will try to tokenize the first token in the string
token_t tokenize(const char *str_view);
// tokenizer, takes a string and returns a token
void tokenizer_start(tokenizer_t *tokenizer, const char *str_view);
// get the current token and move the tokenizer to the next token
token_t tokenizer_next(tokenizer_t *tokenizer, const char *str_view);
// peak the current token without moving the tokenizer
token_t tokenizer_peak(const tokenizer_t *tokenizer, const char *str_view);
void parser_free(parser_t *parser);
// returns the index of the token, or TOKEN_INDEX_ERROR
token_index parser_append_token(parser_t *parser, token_t token);
// returns the index of the expression, or EXPR_INDEX_ERROR
expr_index parser_append_expression(parser_t *parser, expression_t expr);
// figure out which operation the string starts with
bp_t get_operation(const char *s);
char expr_failed(expr_index index);
// manual number parser
float parse_number(const char *str_view, size_t len);
// prat parsing
// returns the index of the expression it made, EXPR_INDEX_EOF if there was
// nothing to parse, or EXPR_INDEX_ERROR
expr_index parse_internal(parser_t *parser, tokenizer_t *tokenizer,
                          const char *str_view, bp_t min_power);
expr_index parse(parser_t *parser, const char *str_view);
// evluator
// takes the parsed AST and evaluate, optionally takes array to a array map
float expression_evaluate(const parser_t *parser, const char *str_view,
                          const expression_t root);

#ifdef CALC_IMPL
#include <math.h>
#include <stddef.h>

// binding power used when the operation is in front of its operand (unary)
static const bp_t prefix_binding_power[] = {
    0,  // EOF
    25, // +
    25, // -
    0,  // *
    0,  // /
    0,  // ^
    32, // √
};
// binary expression type of each operation
static const enum EXPRESSION_TYPE operation_to_expression[] = {
    EXPRESSION_EOF,     // OPERATION_EOF
    EXPRESSION_ADD,     // OPERATION_ADD
    EXPRESSION_SUB,     // OPERATION_SUB
    EXPRESSION_MUL,     // OPERATION_MUL
    EXPRESSION_DIV,     // OPERATION_DIV
    EXPRESSION_POW,     // OPERATION_POW
    EXPRESSION_ROOT,    // OPERATION_ROOT
    EXPRESSION_PERCENT, // OPERATION_PERCENT
};

// simple routine that will try to tokenize the first token in the string
token_t tokenize(const char *str_view) {
    int pos = char_count(str_view, ' '); // skip the white space if any
    int start = pos;
    if (str_view[pos] == '+' || str_view[pos] == '-' || str_view[pos] == '*' ||
        str_view[pos] == '/' || str_view[pos] == '^' ||
        str_view[pos] == '%') { // single char operation
        pos++;
        return (token_t){
            .pos = start, .len = pos - start, .type = TOKEN_OPERATION};
    } else if (is_root(str_view + pos)) {
        pos += ROOT_LEN;
        return (token_t){
            .pos = start, .len = pos - start, .type = TOKEN_OPERATION};
    } else if (is_numeric(str_view[pos]) ||
               str_view[pos] == '.') { // value, we consither dot as a number
                                       // and let the parser handle it
        // count till not numeric
        // count the first half before decimal point
        while (is_numeric(str_view[pos]) || str_view[pos] == '.') {
            pos++;
        }
        return (token_t){
            .pos = start, .len = pos - start, .type = TOKEN_NUMBER};
    } else if (is_aplha(str_view[pos]) ||
               str_view[pos] ==
                   '_') { // name, should start with a letter or underscore
        // count till not name
        while (is_aplha(str_view[pos]) || is_numeric(str_view[pos]) ||
               str_view[pos] == '_') {
            pos++;
        }
        return (token_t){.pos = start, .len = pos - start, .type = TOKEN_NAME};
    } else if (str_view[pos] == '=') { // assignment
        pos++;
        return (token_t){
            .pos = start, .len = pos - start, .type = TOKEN_ASSIGN};
    } else {
        return (token_t){.pos = start, .len = 0, .type = TOKEN_EOF};
    }
}
// tokenizer, takes a string and returns a token
void tokenizer_start(tokenizer_t *tokenizer, const char *str_view) {
    // start the tokenizer, set the current token to the first token
    (void)str_view;
    tokenizer->current = 0;
}
// get the current token and move the tokenizer to the next token
token_t tokenizer_next(tokenizer_t *tokenizer, const char *str_view) {
    // get the current token and save it
    token_t current_token = tokenize(str_view + tokenizer->current);
    current_token.pos += tokenizer->current; // make the position absolute

    tokenizer->current =
        current_token.pos + current_token.len; // move past the token

    return current_token;
}
// peak the current token without moving the tokenizer
token_t tokenizer_peak(const tokenizer_t *tokenizer, const char *str_view) {
    token_t token = tokenize(str_view + tokenizer->current);
    token.pos += tokenizer->current;
    return token;
}

void parser_free(parser_t *parser) {
    free(parser->expression_pool);
    free(parser->token_pool);
    *parser = (parser_t){0, 0, 0, 0, 0, 0};
}

// returns the index of the token, or TOKEN_INDEX_ERROR
token_index parser_append_token(parser_t *parser, token_t token) {
    if (parser->tok_pool_count >= TOKEN_INDEX_ERROR) {
        ERROR("Too many tokens\n");
        return TOKEN_INDEX_ERROR;
    }
    if (parser->tok_pool_count >= parser->tok_pool_cap) {
        size_t new_cap =
            parser->tok_pool_cap == 0 ? 4 : parser->tok_pool_cap * 2;
        token_t *pool =
            (token_t *)REALLOC(parser->token_pool, sizeof(token_t) * new_cap);
        if (pool == NULL) {
            ERROR("Out of memory\n");
            return TOKEN_INDEX_ERROR;
        }
        parser->token_pool = pool;
        parser->tok_pool_cap = new_cap;
    }
    parser->token_pool[parser->tok_pool_count] = token;
    return (token_index)parser->tok_pool_count++;
}
// returns the index of the expression, or EXPR_INDEX_ERROR
expr_index parser_append_expression(parser_t *parser, expression_t expr) {
    if (parser->expr_pool_count >= EXPR_INDEX_EOF) {
        ERROR("Too many expressions\n");
        return EXPR_INDEX_ERROR;
    }
    if (parser->expr_pool_count >= parser->expr_pool_cap) {
        size_t new_cap =
            parser->expr_pool_cap == 0 ? 4 : parser->expr_pool_cap * 2;
        expression_t *pool = (expression_t *)REALLOC(
            parser->expression_pool, sizeof(expression_t) * new_cap);
        if (pool == NULL) {
            ERROR("Out of memory\n");
            return EXPR_INDEX_ERROR;
        }
        parser->expression_pool = pool;
        parser->expr_pool_cap = new_cap;
    }
    parser->expression_pool[parser->expr_pool_count] = expr;
    return (expr_index)parser->expr_pool_count++;
}

// figure out which operation the string starts with
bp_t get_operation(const char *s) {
    if (*s == '+')
        return OPERATION_ADD;
    if (*s == '-')
        return OPERATION_SUB;
    if (*s == '*')
        return OPERATION_MUL;
    if (*s == '/')
        return OPERATION_DIV;
    if (*s == '^')
        return OPERATION_POW;
    if (*s == '%')
        return OPERATION_PERCENT;
    if (is_root(s))
        return OPERATION_ROOT;
    return OPERATION_EOF;
}

// true if the index is not a real expression
char expr_failed(expr_index index) {
    if (index == EXPR_INDEX_EOF)
        ERROR("Unexpected end of expression\n");
    return index >= EXPR_INDEX_EOF;
}

// manual number parser
float parse_number(const char *str_view, size_t len) {
    float whole = 0.f;
    float decimal = 0.f;
    size_t pos = 0;

    // parse the whole part
    while (pos < len && is_numeric(str_view[pos])) {
        whole = whole * 10.f + (str_view[pos] - '0');
        pos++;
    }

    // parse the decimal part
    if (pos < len && str_view[pos] == '.') {
        pos++;
        float decimal_place = 0.1f;
        while (pos < len && is_numeric(str_view[pos])) {
            decimal += (str_view[pos] - '0') * decimal_place;
            decimal_place *= 0.1f;
            pos++;
        }
    }
    // if pos is not equal to len, then the number is invalid, return -1
    if (pos != len) {
        ERROR("Invalid number: %.*s\n", (int)len, str_view);
        return -1.f;
    }

    return whole + decimal;
}

// prat parsing
// returns the index of the expression it made, EXPR_INDEX_EOF if there was
// nothing to parse, or EXPR_INDEX_ERROR
expr_index parse_internal(parser_t *parser, tokenizer_t *tokenizer,
                          const char *str_view, bp_t min_power) {
    expr_index lhs;

    // proces lhs, lhs can be a value, variable, or unary expression
    token_t lhs_token = tokenizer_next(tokenizer, str_view);
    switch (lhs_token.type) {
    case TOKEN_NUMBER: {
        // validate the number, if it is invalid, return error
        float value = parse_number(str_view + lhs_token.pos, lhs_token.len);
        if (value < 0.f) {
            return EXPR_INDEX_ERROR;
        }
        // create a value expression
        token_index lhs_token_index = parser_append_token(parser, lhs_token);
        if (lhs_token_index == TOKEN_INDEX_ERROR)
            return EXPR_INDEX_ERROR;
        lhs = parser_append_expression(parser, (expression_t){
                                                   lhs_token_index,
                                                   0,
                                                   EXPRESSION_VALUE,
                                               });
        if (lhs == EXPR_INDEX_ERROR)
            return EXPR_INDEX_ERROR;
        break;
    }
    case TOKEN_NAME: {
        // create a variable expression
        token_index lhs_token_index = parser_append_token(parser, lhs_token);
        if (lhs_token_index == TOKEN_INDEX_ERROR)
            return EXPR_INDEX_ERROR;
        lhs = parser_append_expression(
            parser, (expression_t){lhs_token_index, 0, EXPRESSION_VAR});
        if (lhs == EXPR_INDEX_ERROR)
            return EXPR_INDEX_ERROR;
        break;
    }
    case TOKEN_OPERATION: { // unary operation
        bp_t op_type = get_operation(str_view + lhs_token.pos);
        if (op_type != OPERATION_ADD && op_type != OPERATION_SUB &&
            op_type != OPERATION_ROOT) {
            ERROR("Invalid token: %.*s\n", (int)lhs_token.len,
                  str_view + lhs_token.pos);
            return EXPR_INDEX_ERROR;
        }
        // fold the expression then create unary expression
        expr_index operand = parse_internal(parser, tokenizer, str_view,
                                            prefix_binding_power[op_type]);
        if (expr_failed(operand))
            return EXPR_INDEX_ERROR;
        // create a unary expression
        if (op_type == OPERATION_ADD) {
            lhs = operand; // +x is just x
        } else {
            lhs = parser_append_expression(
                parser,
                (expression_t){
                    operand,
                    0,
                    op_type == OPERATION_SUB ? EXPRESSION_NEG : EXPRESSION_SQRT,
                });
            if (lhs == EXPR_INDEX_ERROR)
                return EXPR_INDEX_ERROR;
        }
        break;
    }
    case TOKEN_EOF:
        return EXPR_INDEX_EOF;
    default:
        return EXPR_INDEX_ERROR;
    }

    // process rhs
    for (;;) {
        // assume the next token is an operator
        token_t op_token = tokenizer_peak(tokenizer, str_view);
        if (op_token.type != TOKEN_OPERATION) {
            break;
        }

        // check the binding power of the operator
        bp_t op_type = get_operation(str_view + op_token.pos);
        if (op_type == OPERATION_EOF) {
            ERROR("Invalid operator: %.*s\n", (int)op_token.len,
                  str_view + op_token.pos);
            return EXPR_INDEX_ERROR;
        }
        bp_t lbp = binding_power[op_type][0];
        bp_t rbp = binding_power[op_type][1];

        // tells when to break the loop
        if (lbp < min_power)
            break;
        tokenizer_next(tokenizer, str_view);

        // % only has a lhs, so there is nothing to fold
        if (op_type == OPERATION_PERCENT) {
            lhs = parser_append_expression(
                parser, (expression_t){lhs, 0, EXPRESSION_PERCENT});
            if (lhs == EXPR_INDEX_ERROR)
                return EXPR_INDEX_ERROR;
            continue;
        }

        // fold depper expression recursively
        expr_index rhs = parse_internal(parser, tokenizer, str_view, rbp);
        if (expr_failed(rhs))
            return EXPR_INDEX_ERROR;
        // construct new expression
        lhs = parser_append_expression(
            parser, (expression_t){lhs, rhs, operation_to_expression[op_type]});
        if (lhs == EXPR_INDEX_ERROR)
            return EXPR_INDEX_ERROR;
    }

    return lhs;
}

expr_index parse(parser_t *parser, const char *str_view) {
    tokenizer_t tokenizer;
    tokenizer_start(&tokenizer, str_view);
    expr_index root = parse_internal(parser, &tokenizer, str_view, -1);
    if (root >= EXPR_INDEX_EOF)
        return root;

    // everything should be consumed by now
    token_t rest = tokenizer_peak(&tokenizer, str_view);
    if (rest.type != TOKEN_EOF || str_view[rest.pos] != 0) {
        ERROR("Unexpected token: %s\n", str_view + rest.pos);
        return EXPR_INDEX_ERROR;
    }
    return root;
}
// evluator
// takes the parsed AST and evaluate, optionally takes array to a array map
float expression_evaluate(const parser_t *parser, const char *str_view,
                          const expression_t root) {
    switch (root.type) {
    case EXPRESSION_ERROR:
        return 0.f;
    case EXPRESSION_EOF:
        return 0.f;
    case EXPRESSION_ADD: {
        float lhs = expression_evaluate(
            parser, str_view, parser->expression_pool[root.a_as.expr]);
        float rhs = expression_evaluate(parser, str_view,
                                        parser->expression_pool[root.b]);
        return lhs + rhs;
    }
    case EXPRESSION_SUB: {
        float lhs = expression_evaluate(
            parser, str_view, parser->expression_pool[root.a_as.expr]);
        float rhs = expression_evaluate(parser, str_view,
                                        parser->expression_pool[root.b]);
        return lhs - rhs;
    }
    case EXPRESSION_MUL: {
        float lhs = expression_evaluate(
            parser, str_view, parser->expression_pool[root.a_as.expr]);
        float rhs = expression_evaluate(parser, str_view,
                                        parser->expression_pool[root.b]);
        return lhs * rhs;
    }
    case EXPRESSION_DIV: {
        float lhs = expression_evaluate(
            parser, str_view, parser->expression_pool[root.a_as.expr]);
        float rhs = expression_evaluate(parser, str_view,
                                        parser->expression_pool[root.b]);
        return lhs / rhs;
    }
    case EXPRESSION_POW: {
        float lhs = expression_evaluate(
            parser, str_view, parser->expression_pool[root.a_as.expr]);
        float rhs = expression_evaluate(parser, str_view,
                                        parser->expression_pool[root.b]);
        return powf(lhs, rhs);
    }
    case EXPRESSION_ROOT: {
        float lhs = expression_evaluate(
            parser, str_view, parser->expression_pool[root.a_as.expr]);
        float rhs = expression_evaluate(parser, str_view,
                                        parser->expression_pool[root.b]);
        return powf(lhs, rhs);
    }
    case EXPRESSION_PERCENT:
        return expression_evaluate(parser, str_view,
                                   parser->expression_pool[root.a_as.expr]) *
               100.f;
    case EXPRESSION_NEG:
        return -expression_evaluate(parser, str_view,
                                    parser->expression_pool[root.a_as.expr]);
    case EXPRESSION_SQRT: {
        float lhs = expression_evaluate(
            parser, str_view, parser->expression_pool[root.a_as.expr]);
        return powf(lhs, 0.5f);
    }
    case EXPRESSION_VAR: {
        return 0.f; // no support for now
    }
    case EXPRESSION_VALUE: {
        token_t a = parser->token_pool[root.a_as.expr];
        return parse_number(str_view + a.pos, a.len);
    }
    default:
        return 0.f; // error
    }
}

/*/ test
#include <stdio.h>
[[maybe_unused]] static void dump(parser_t *p, const char *s, expr_index i) {
    expression_t e = p->expression_pool[i];
    switch (e.type) {
        case EXPRESSION_VALUE: case EXPRESSION_VAR: { token_t t =
p->token_pool[e.a_as.tok]; printf("%.*s", (int)t.len, s+t.pos); return; } case
EXPRESSION_NEG: printf("(neg "); dump(p,s,e.a_as.expr); printf(")"); return;
        case EXPRESSION_SQRT: printf("(sqrt "); dump(p,s,e.a_as.expr);
printf(")"); return; case EXPRESSION_PERCENT: printf("(pct ");
dump(p,s,e.a_as.expr); printf(")"); return; default: { const char
*n[]={"eof","+","-","*","/","^","root"}; printf("(%s ", n[e.type]);
dump(p,s,e.a_as.expr); printf(" "); dump(p,s,e.b); printf(")"); }
    }
}
int main(void) {
    // const char *tests[] = {"1 + 2 * 3","-2 + 3","-2^2","2^3^2","10 +
50%","√9","2√9 + x_1","1 - 2 - 3","0","1 +","1 $ 2","", "1.2.3","+ 4 * --0.2 +
sus"};
    // for (unsigned k=0;k<sizeof tests/sizeof *tests;k++){
    //     parser_t p={0}; printf("%-12s -> ", tests[k]);
    //     expr_index r = parse(&p, tests[k]);
    //     if (r==EXPR_INDEX_EOF) puts("EOF"); else if (r==EXPR_INDEX_ERROR)
puts("ERROR"); else {dump(&p,tests[k],r); puts("");}
    //     free(p.expression_pool); free(p.token_pool);
    // }
    const char* t = "1+2*5/2.1";
    parser_t parser;
    expr_index root = parse(&parser, t);
    float res = expression_evaluate(&parser, t, parser.expression_pool[root]);

    printf("%f\n", res);

    parser_free(&parser);
}
*/
#endif // #ifdef CALC_IMPL
#endif // ifndef C_LIBS_CALC_HEADER_H

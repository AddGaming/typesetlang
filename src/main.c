#include <stdio.h>
#include <assert.h>
#include "rstd/rstd.h"

// TYPES

typedef enum {
    T_FUN,
    T_STRUCT,
    T_UNSAVE,
    T_WHERE,
    T_PRINT,
    T_NAME,
    T_ARROW,
    T_OPAR,
    T_CPAR,
    T_COL,
    T_SCOL,
    T_ADD,
    T_SUB,
    T_MUL,
    T_DIV,
    T_DOT,
    T_LT,
    T_GT,
    T_EQ,
    T_LTE,
    T_GTE,
    T_OR,
    T_AND,
    T_BOR,
    T_BAND,
    T_OBRACE,
    T_CBRACE,
    T_NL,
    T_OC,
    T_CC
} TokenType;

typedef struct {
    TokenType type;
    size_t start;
    size_t end;
} Token;

DEFINE_RESULT(Token);
IMPL_SWAP(Token);
DEFINE_DA(Token);
IMPL_DA  (Token);
DEFINE_RESULT(TokenType);
IMPL_SWAP(TokenType);
DEFINE_DA(TokenType);
IMPL_DA  (TokenType);

typedef struct {
    charDa buff;
    TokenDa acc;
} TokenState;

// T_FUNCTIONS

Token name_or_keyword(const charDa buff, const size_t i) {
    Token tk = {T_NAME, i - buff.count, i - 1};
    if (       buff.count == 3 
        && buff.ptr[0] == 'f' 
        && buff.ptr[1] == 'u' 
        && buff.ptr[2] == 'n' 
    ) {
        tk.type = T_FUN;
    } else if (buff.count == 6 
        && buff.ptr[0] == 's' 
        && buff.ptr[1] == 't' 
        && buff.ptr[2] == 'r' 
        && buff.ptr[3] == 'u' 
        && buff.ptr[4] == 'c' 
        && buff.ptr[5] == 't' 
    ) {
        tk.type = T_STRUCT;
    } else if (buff.count == 6 
        && buff.ptr[0] == 'u' 
        && buff.ptr[1] == 'n' 
        && buff.ptr[2] == 's' 
        && buff.ptr[3] == 'a' 
        && buff.ptr[4] == 'v' 
        && buff.ptr[5] == 'e' 
    ) {
        tk.type = T_UNSAVE;
    } else if (buff.count == 5 
        && buff.ptr[0] == 'w' 
        && buff.ptr[1] == 'h' 
        && buff.ptr[2] == 'e' 
        && buff.ptr[3] == 'r' 
        && buff.ptr[4] == 'e' 
    ) {
        tk.type = T_WHERE ;
    } else if (buff.count == 5 
        && buff.ptr[0] == 'p' 
        && buff.ptr[1] == 'r' 
        && buff.ptr[2] == 'i' 
        && buff.ptr[3] == 'n' 
        && buff.ptr[4] == 't' 
    ) {
        tk.type = T_PRINT ;
    }
    return tk;
}

TokenState single_char_tk(const TokenType type, TokenState state, const size_t i) {
    if (state.buff.count > 0) {
        Token new_name = name_or_keyword(state.buff, i);
        Token new_cbr  = {type, i, i};
        state.buff.count = 0;
        TokenDa new_acc = insert_TokenDa(&state.acc, new_name).result;
                new_acc = insert_TokenDa(&new_acc,   new_cbr ).result;
        state.acc = new_acc;
    } else {
        Token   new_cbr = {type, i, i};
        TokenDa new_acc = insert_TokenDa(&state.acc, new_cbr).result;
        state.acc = new_acc;
    }
    return state;
}

TokenState double_char_tk(const TokenType type, TokenState state, const size_t i) {
    if (state.buff.count > 0) {
        Token new_name = name_or_keyword(state.buff, i);
        Token new_cbr  = {type, i, i+1};
        state.buff.count = 0;
        TokenDa new_acc = insert_TokenDa(&state.acc, new_name).result;
                new_acc = insert_TokenDa(&new_acc,   new_cbr ).result;
        state.acc = new_acc;
    } else {
        Token   new_cbr = {type, i, i+1};
        TokenDa new_acc = insert_TokenDa(&state.acc, new_cbr ).result;
        state.acc = new_acc;
    }
    return state;
}

TokenDa tokenize(FILE* file, TokenState state, const size_t i) {

    int c = fgetc(file);
    if        (c == EOF) {
        if (state.buff.count > 0) {
            Token new_tk = name_or_keyword(state.buff, i);
            state.buff.count = 0;
            state.acc = insert_TokenDa(&state.acc, new_tk).result;
        } 
        return state.acc;
    } else if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
        if (state.buff.count > 0) {
            Token new_tk = name_or_keyword(state.buff, i);
            state.buff.count = 0;
            state.acc = insert_TokenDa(&state.acc, new_tk).result;
            return tokenize(file, state, i+1);
        } else {
            return tokenize(file, state, i+1);
        }
    } else if (c == ')') {
        state = single_char_tk(T_CPAR, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '(') {
        state = single_char_tk(T_OPAR, state, i);
        return tokenize(file, state, i+1);
    } else if (c == ':') {
        state = single_char_tk(T_COL, state, i);
        return tokenize(file, state, i+1);
    } else if (c == ';') {
        state = single_char_tk(T_SCOL, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '.') {
        state = single_char_tk(T_DOT, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '\n') {
        state = single_char_tk(T_NL, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '{') {
        state = single_char_tk(T_OBRACE, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '}') {
        state = single_char_tk(T_CBRACE, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '+') {
        state = single_char_tk(T_ADD, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '+') {
        state = single_char_tk(T_ADD, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '=') {
        state = single_char_tk(T_EQ, state, i);
        return tokenize(file, state, i+1);
    } else if (
        c == '<' || c == '>' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|'
    ) {
        int d = peak(file);
        // Matches
        if        (c == '<' && d == '=') {
            state = double_char_tk(T_LTE, state, i);
            fseek(file, 1, SEEK_CUR);
            return tokenize(file, state, i+1);
        } else if (c == '>' && d == '=') {
            state = double_char_tk(T_GTE, state, i);
            fseek(file, 1, SEEK_CUR);
            return tokenize(file, state, i+1);
        } else if (c == '-' && d == '>') {
            state = double_char_tk(T_ARROW, state, i);
            fseek(file, 1, SEEK_CUR);
            return tokenize(file, state, i+1);
        } else if (c == '/' && d == '*') {
            state = double_char_tk(T_OC, state, i);
            fseek(file, 1, SEEK_CUR);
            return tokenize(file, state, i+1);
        } else if (c == '&' && d == '&') {
            state = double_char_tk(T_AND, state, i);
            fseek(file, 1, SEEK_CUR);
            return tokenize(file, state, i+1);
        } else if (c == '|' && d == '|') {
            state = double_char_tk(T_OR, state, i);
            fseek(file, 1, SEEK_CUR);
            return tokenize(file, state, i+1);
        } else if (c == '*' && d == '/') {
            state = double_char_tk(T_CC, state, i);
            fseek(file, 1, SEEK_CUR);
            return tokenize(file, state, i+1);
        // Doesnt match
        } else if (c == '<' && d != '=') {
            state = single_char_tk(T_LT, state, i);
            return tokenize(file, state, i);
        } else if (c == '>' && d != '=') {
            state = single_char_tk(T_GT, state, i);
            return tokenize(file, state, i);
        } else if (c == '-' && d != '>') {
            state = single_char_tk(T_SUB, state, i);
            return tokenize(file, state, i);
        } else if (c == '/' && d != '*') {
            state = single_char_tk(T_DIV, state, i);
            return tokenize(file, state, i);
        } else if (c == '*' && d != '/') {
            state = single_char_tk(T_MUL, state, i);
            return tokenize(file, state, i);
        } else if (c == '&' && d != '&') {
            state = single_char_tk(T_BAND, state, i);
            return tokenize(file, state, i);
        } else if (c == '|' && d != '|') {
            state = single_char_tk(T_BOR, state, i);
            return tokenize(file, state, i);
        } else {
            // UNREACHABLE
            err_redln("UNREACHABLE TOKENIZATION STATE!")
            return state.acc;
        }
    } else {
        state.buff = insert_charDa(&state.buff, c).result;
        return tokenize(file, state, i+1);
    }
}

void print_tokens(const TokenDa tokens) {
    for (size_t i = 0; i < tokens.count; i++) {
        switch (tokens.ptr[i].type) {
            case T_FUN:
                yellowln("fun\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_STRUCT:
                yellowln("struct\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_UNSAVE:
                yellowln("unsave\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_WHERE:
                yellowln("where\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_PRINT:
                yellowln("print\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_NAME:
                yellowln("<name>\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_ARROW:
                yellowln("->\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_OPAR:
                yellowln("(\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_CPAR:
                yellowln(")\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_COL:
                yellowln(":\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_SCOL:
                yellowln(";\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_DOT:
                yellowln(".\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_NL:
                yellowln("NL\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_ADD:
                yellowln("+\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_SUB:
                yellowln("-\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_DIV:
                yellowln("/\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_MUL:
                yellowln("*\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_LT:
                yellowln("<\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_GT:
                yellowln(">\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_EQ:
                yellowln("=\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_LTE:
                yellowln("<=\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_GTE:
                yellowln(">=\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_OR:
                yellowln("||\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_AND:
                yellowln("&&\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_BOR:
                yellowln("|\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_BAND:
                yellowln("&\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_OBRACE:
                yellowln("{\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_CBRACE:
                yellowln("}\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_OC:
                yellowln("/*\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case T_CC:
                yellowln("*/\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
        }
    }
}

typedef enum {
    I_ADD_I8,     // | s | s | adds two i8 (index from 0)
    I_ADD_ASS_I8, // | s | s | adds two i8 and assigns result to first one
    I_RADD_I8,    // | s | s | adds two i8 (index from last element)
    I_RADD_ASS_I8,// | s | s | adds two i8 and assigns result to first one
    I_ADD_U8,     // | s | s | adds two u8 (index from 0)
    I_ADD_ASS_U8, // | s | s | adds two u8 and assigns result to first one
    I_RADD_U8,    // | s | s | adds two u8 (index from last element)
    I_RADD_ASS_U8,// | s | s | adds two u8 and assigns result to first one
    I_ASSIGN,     // | s | s | assigns from first arg to second arg
    I_POP_N,      // | s | s | pops n bytes of the stack
    I_JUMP,       // | i |   | jumps n instructions (negative = backwards)
    I_CON_JUMP,   // | s | i | jumps if first element != 0
    I_OUT_S,      // | s | i | prints a signed   value (with nl if i != 0)
    I_OUT_U       // | s | i | prints a unsigned value (with nl if i != 0)
} InstructionType;

typedef struct {
    InstructionType type;
    union {
        struct {
            int iix, iiy;
        };
        struct {
            size_t six;
            int siy;
        };
        struct {
            size_t ssx, ssy;
        };
    };
} Instruction;

DEFINE_RESULT(InstructionType);
IMPL_SWAP    (InstructionType);
DEFINE_DA    (InstructionType);
IMPL_DA      (InstructionType);

DEFINE_RESULT(Instruction);
IMPL_SWAP    (Instruction);
DEFINE_DA    (Instruction);
IMPL_DA      (Instruction);

typedef enum {
    I8,
    U8
} CompType;

typedef struct {
    CompType type;
    union {
        struct {
            ullong ustart, umax, umin, uend;
        };
        struct {
            llong sstart, smax, smin, send;
        };
    };
} ValRange;

const ValRange I8_RANGE = {
    .type = I8,
    .sstart = -128,
    .smin   = -128,
    .smax   =  127,
    .send   =  127,
}; 
const ValRange U8_RANGE = {
    .type = U8,
    .sstart =    0,
    .smin   =    0,
    .smax   =  255,
    .send   =  255,
};

#define MAX_NAME_LEN 255
#define MAX_ARGC      32
// max args for a func == 32
// max name length == 255
typedef struct {
    uchar         argc;
    uchar         unsave;
    char          name[MAX_NAME_LEN + 1];
    InstructionDa ins;
    ValRange      ranges[MAX_ARGC];
} Func;

// max name length == 255
typedef struct {
    char     name[MAX_NAME_LEN + 1];
    ValRange range;
} Var;

DEFINE_RESULT(Func);
IMPL_SWAP    (Func);
DEFINE_DA    (Func);
IMPL_DA      (Func);
DEFINE_RESULT(Var);
IMPL_SWAP    (Var);
DEFINE_DA    (Var);
IMPL_DA      (Var);

InstructionDa parse(const TokenDa* tokens, FILE* file) {
    size_t i = 0;
    InstructionDa ins   = new_InstructionDa(8).result;
    FuncDa        funcs = new_FuncDa       (8).result;
    VarDa         vars  = new_VarDa        (8).result;

    if (tokens->count == 0) {
        redln("ERROR: The input file in empty!");
        return ins;
    }
    while (i <= tokens->count) {
        if (       tokens->ptr[i].type == T_OC) {
            size_t c_start = tokens->ptr[i].start; 
            while (tokens->ptr[i].type != T_CC && i < tokens->count) { i += 1; }
            if (i == tokens->count && tokens->ptr[i].type != T_CC) {
                redln("ERROR: Comment starting @ %zu is not closed!", c_start);
                exit(1);
            } else {
                i += 1;
            }
        } else if (tokens->ptr[i].type == T_FUN) {
            if (tokens->count - i >= 3
                && tokens->ptr[i+1].type == T_NAME
                && tokens->ptr[i+2].type == T_OPAR
            ) {
                // TODO:
                // parse args
                // parse result type
                // parse where
                // parse body 
                i += 1;
            } else { 
                redln("ERROR: syntax error @ %zu.", tokens->ptr[i].start);
                redln("Unexpected Keyword 'fun'!");
                redln("Allowed top level actions are function declaration,");
                redln("function invocation, variable assignment, and comments");
                exit(1);
            }
        } else if (tokens->ptr[i].type == T_NAME) {
            // var assignment
            if (tokens->count - i >= 5
                && tokens->ptr[i+1].type == T_COL
                && tokens->ptr[i+2].type == T_NAME
                && tokens->ptr[i+3].type == T_EQ
            ) {
                // TODO:
                i += 1;
            // func call
            } else if (tokens->count - i >= 3
                && tokens->ptr[i+1].type == T_OPAR
            ) { 
                // TODO:
                i += 1;
            // I dont have side effects, so can be ignored :)
            } else { 
                while (tokens->ptr[i].type != T_NL && i < tokens->count) { i += 1; }
                i += 1; // skip NL
            }
        } else if (tokens->ptr[i].type == T_UNSAVE) {
            // unsave func call
            if (tokens->count - i >= 5
                && tokens->ptr[i+1].type == T_NAME
                && tokens->ptr[i+2].type == T_OPAR
            ) {
                // TODO:
                i += 1;
            // unsave func decl
            } else if (tokens->count - i >= 3
                && tokens->ptr[i+1].type == T_FUN
                && tokens->ptr[i+2].type == T_NAME
                && tokens->ptr[i+3].type == T_OPAR
            ) { 
                // TODO:
                i += 1;
            } else { 
                redln("ERROR: syntax error @ %zu.", tokens->ptr[i].start);
                redln("Unexpected Keyword 'unsave'!");
                redln("'unsave' needs to be followed by either a function declaration");
                redln("or function invocation.");
                exit(1);
            }
        } else if (tokens->ptr[i].type == T_PRINT) {
            if (tokens->count - i >= 5
                && tokens->ptr[i+2].type == T_OPAR
                && tokens->ptr[i+2].type == T_NAME
            ) {
                // TODO:
                i += 1;
            } else { 
                redln("ERROR: syntax error @ %zu.", tokens->ptr[i].start);
                redln("print gets used like a function:");
                redln("\tprint(var)");
                redln("\tprint(a, b, c)");
                exit(1);
            }
        } else {
            size_t c_start = tokens->ptr[i].start; 
            redln("ERROR: Malformed file around %zu!", tokens->ptr[i].start);
            redln("Allowed top level actions are function declaration,");
            redln("function invocation, variable assignment, and comments");
            exit(1);
        }
    }
    return ins;
}

void eval(const InstructionDa* ins) {
    charDa stack = new_charDa(8).result;


}

int main(void) {
    whiteln("---------------------------");
    FILE* file = fopen("test.tp", "r");
    
    TokenState state = { new_charDa(8).result, new_TokenDa(8).result };
    TokenDa tokens = tokenize(
        file, 
        state,
        0
    );
    print_tokens(tokens);
    whiteln("---------------------------");
    
    InstructionDa instructions = parse(&tokens, file);
    fclose(file);
    whiteln("---------------------------");
    
    eval(&instructions);
}

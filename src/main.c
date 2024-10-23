#include <stdio.h>
#include "rstd/rstd.h"
#include "rstd/rstd.c"
#define _CRT_SECURE_NO_WARNINGS

// TYPES

typedef enum {
    FUN,
    STRUCT,
    UNSAVE,
    NAME,
    OPAR,
    CPAR,
    COL,
    ADD,
    LT,
    GT,
    EQ,
    LTE,
    GTE,
    OBRACE,
    CBRACE,
    OC,
    CC
} TokenType;

typedef struct {
    TokenType type;
    size_t start;
    size_t end;
} Token;

DEFINE_DA(Token);
IMPL_DA  (Token);
IMPL_SWAP(Token);
DEFINE_DA(TokenType);
IMPL_DA  (TokenType);
IMPL_SWAP(TokenType);

typedef struct {
    charDa buff;
    TokenDa acc;
} TokenState;

// FUNCTIONS

Token name_or_keyword(const charDa buff, const size_t i) {
    Token tk = {NAME, i - buff.count, i - 1};
    if (       buff.count == 3 
        && buff.ptr[0] == 'f' 
        && buff.ptr[1] == 'u' 
        && buff.ptr[2] == 'n' 
    ) {
        tk.type = FUN;
    } else if (buff.count == 6 
        && buff.ptr[0] == 's' 
        && buff.ptr[1] == 't' 
        && buff.ptr[2] == 'r' 
        && buff.ptr[3] == 'u' 
        && buff.ptr[4] == 'c' 
        && buff.ptr[5] == 't' 
    ) {
        tk.type = STRUCT;
    } else if (buff.count == 6 
        && buff.ptr[0] == 'u' 
        && buff.ptr[1] == 'n' 
        && buff.ptr[2] == 's' 
        && buff.ptr[3] == 'a' 
        && buff.ptr[4] == 'v' 
        && buff.ptr[5] == 'e' 
    ) {
        tk.type = UNSAVE;
    }
    return tk;
}

TokenState single_char_tk(const TokenType type, TokenState state, const size_t i) {
    if (state.buff.count > 0) {
        Token new_name = name_or_keyword(state.buff, i);
        Token new_cbr  = {type, i, i};
        state.buff.count = 0;
        TokenDa new_acc = insert_TokenDa(state.acc, new_name).result;
                new_acc = insert_TokenDa(new_acc,   new_cbr ).result;
        state.acc = new_acc;
    } else {
        Token   new_cbr = {type, i, i};
        TokenDa new_acc = insert_TokenDa(state.acc, new_cbr).result;
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
            state.acc = insert_TokenDa(state.acc, new_tk).result;
        } 
        return state.acc;
    } else if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
        if (state.buff.count > 0) {
            Token new_tk = name_or_keyword(state.buff, i);
            state.buff.count = 0;
            state.acc = insert_TokenDa(state.acc, new_tk).result;
            return tokenize(file, state, i+1);
        } else {
            return tokenize(file, state, i+1);
        }
    } else if (c == ')') {
        state = single_char_tk(CPAR, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '(') {
        state = single_char_tk(OPAR, state, i);
        return tokenize(file, state, i+1);
    } else if (c == ':') {
        state = single_char_tk(COL, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '{') {
        state = single_char_tk(OBRACE, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '}') {
        state = single_char_tk(CBRACE, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '+') {
        state = single_char_tk(ADD, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '+') {
        state = single_char_tk(ADD, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '=') {
        state = single_char_tk(EQ, state, i);
        return tokenize(file, state, i+1);
    } else if (c == '<' || c == '>' || c == '-' || c == '*' || c == '/') {
        int d = peak(file);
        if        (d == EOF) { 
            return state.acc; 
        } else if (c == '<' || d == '=') {
        } else if (c == '>' || d == '=') {
        } else if (c == '-' || d == '>') {
        } else if (c == '/' || d == '*') {
        } else if (c == '*' || d == '/') {
            
        } else {
        }
    } else {
        state.buff = insert_charDa(state.buff, c).result;
        return tokenize(file, state, i+1);
    }
}

void print_tokens(const TokenDa tokens) {
    for (size_t i = 0; i < tokens.count; i++) {
        switch (tokens.ptr[i].type) {
            case FUN:
                yellowln("fun\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case NAME:
                yellowln("<name>\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case OPAR:
                yellowln("(\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case CPAR:
                yellowln(")\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case OBRACE:
                yellowln("{\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case CBRACE:
                yellowln("}\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case COL:
                yellowln(":\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case ADD:
                yellowln("+\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case STRUCT:
                yellowln("struct\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case UNSAVE:
                yellowln("unsave\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case LT:
                yellowln("<\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case GT:
                yellowln(">\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case EQ:
                yellowln("=\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case LTE:
                yellowln("<=\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case GTE:
                yellowln(">=\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case OC:
                yellowln("/*\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
            case CC:
                yellowln("*/\t%zu\t%zu", tokens.ptr[i].start, tokens.ptr[i].end);
                break;
        }
    }
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

    fclose(file);
    whiteln("---------------------------");
}

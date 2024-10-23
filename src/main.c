#include <stdio.h>
#include "rstd/rstd.h"
#include "rstd/rstd.c"

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

// FUNCTIONS

Token name_or_keyword(const charDa buff, const size_t i) {
    Token tk = {NAME, i - buff.count, i - 1};
    if (buff.count == 3 
        && buff.ptr[0] == 'f' 
        && buff.ptr[1] == 'u' 
        && buff.ptr[2] == 'n' 
    ) {
        tk.type = FUN;
    }
    return tk;
}

TokenDa tokenize(FILE* file, charDa buff, const TokenDa acc, const size_t i) {

    int c = fgetc(file);
    if        (c == EOF) {
        if (buff.count > 0) {
            Token new_tk = name_or_keyword(buff, i);
            buff.count = 0;
            TokenDa new_acc = insert_TokenDa(acc, new_tk).result;
            return new_acc;
        } else {
            return acc;
        }
    } else if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
        if (buff.count > 0) {
            Token new_tk = name_or_keyword(buff, i);
            buff.count = 0;
            TokenDa new_acc = insert_TokenDa(acc, new_tk).result;
            return tokenize(file, buff, new_acc, i+1);
        } else {
            return tokenize(file, buff, acc, i+1);
        }
    } else if (c == ')') {
        if (buff.count > 0) {
            Token new_name = name_or_keyword(buff, i);
            Token new_cbr  = {CPAR, i, i};
            buff.count = 0;
            TokenDa new_acc = insert_TokenDa(acc,     new_name).result;
                    new_acc = insert_TokenDa(new_acc, new_cbr ).result;
            return tokenize(file, buff, new_acc, i+1);
        } else {
            Token   new_cbr = {CPAR, i, i};
            TokenDa new_acc = insert_TokenDa(acc, new_cbr).result;
            return tokenize(file, buff, new_acc, i+1);
        }
    } else if (c == '(') {
        if (buff.count > 0) {
            Token new_name = name_or_keyword(buff, i);
            Token new_cbr  = {OPAR, i, i};
            buff.count = 0;
            TokenDa new_acc = insert_TokenDa(acc,     new_name).result;
                    new_acc = insert_TokenDa(new_acc, new_cbr ).result;
            return tokenize(file, buff, new_acc, i+1);
        } else {
            Token   new_cbr = {OPAR, i, i};
            TokenDa new_acc = insert_TokenDa(acc, new_cbr).result;
            return tokenize(file, buff, new_acc, i+1);
        }
    } else if (c == ':') {
        if (buff.count > 0) {
            Token new_name = name_or_keyword(buff, i);
            Token new_cbr  = {COL , i, i};
            buff.count = 0;
            TokenDa new_acc = insert_TokenDa(acc,     new_name).result;
                    new_acc = insert_TokenDa(new_acc, new_cbr ).result;
            return tokenize(file, buff, new_acc, i+1);
        } else {
            Token   new_cbr = {COL, i, i};
            TokenDa new_acc = insert_TokenDa(acc, new_cbr).result;
            return tokenize(file, buff, new_acc, i+1);
        }
    } else if (c == '{') {
        if (buff.count > 0) {
            Token new_name = name_or_keyword(buff, i);
            Token new_cbr  = {OBRACE, i, i};
            buff.count = 0;
            TokenDa new_acc = insert_TokenDa(acc,     new_name).result;
                    new_acc = insert_TokenDa(new_acc, new_cbr ).result;
            return tokenize(file, buff, new_acc, i+1);
        } else {
            Token   new_cbr = {OBRACE, i, i};
            TokenDa new_acc = insert_TokenDa(acc, new_cbr).result;
            return tokenize(file, buff, new_acc, i+1);
        }
    } else if (c == '}') {
        if (buff.count > 0) {
            Token new_name = name_or_keyword(buff, i);
            Token new_cbr  = {CBRACE, i, i};
            buff.count = 0;
            TokenDa new_acc = insert_TokenDa(acc,     new_name).result;
                    new_acc = insert_TokenDa(new_acc, new_cbr ).result;
            return tokenize(file, buff, new_acc, i+1);
        } else {
            Token   new_cbr = {CBRACE, i, i};
            TokenDa new_acc = insert_TokenDa(acc, new_cbr).result;
            return tokenize(file, buff, new_acc, i+1);
        }
    } else {
        charDa new_buff = insert_charDa(buff, c).result;
        return tokenize(file, new_buff, acc, i+1);
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
        }
    }
}

int main(void) {
    whiteln("----------------------");
    FILE* file = fopen("test.tp", "r");
    
    TokenDa tokens = tokenize(
        file, 
        new_charDa(8).result, 
        new_TokenDa(8).result,
        0
    );
    print_tokens(tokens);

    fclose(file);
    whiteln("----------------------");
}

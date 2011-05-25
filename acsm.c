
#include "acsm.h"


/*
* Copyright (C) Weibin Yao
* Adopt the design of Martin Roesch <roesch@sourcefire.com>
*
* DOCUMENT
*
* See wiki: http://en.wikipedia.org/wiki/Aho%E2%80%93Corasick_string_matching_algorithm
*
* This program is an implement of the paper[1].
*
* [1]  Margaret J. Corasick (June 1975). "Efficient string matching: An aid to bibliographic 
* search". Communications of the ACM 18 (6): 333–340. doi:10.1145/360825.360855 
*
* */


acsm_context_t *acsm_alloc(int flag)
{
    int no_case = 0;
    acsm_context_t *ctx;

    if (flag & NO_CASE) {
        no_case = 1;
    }

    ctx = calloc(1, sizeof(acsm_context_t));
    if (ctx == NULL) {
        return NULL;
    }

    ctx->no_case = no_case;
    ctx->max_state = 1;

    return ctx;
}


void acsm_free(acsm_context_t *ctx)
{
    if (ctx) {
        free(ctx);
    }
}


int acsm_add_pattern(acsm_context_t *ctx, u_char *string) 
{
    u_char ch;
    int i, len;
    acsm_pattern_t *p;

    len = acsm_strlen(string);

    p = malloc(sizeof(acsm_pattern_t));
    if (p == NULL) {
        return -1;
    }

    p->string = malloc(len);
    if (p->string == NULL) {
        return -1;
    }

    for(i = 0; i < len; i++) {
        ch = string[i];
        p->string[i] = ctx->no_case ? acsm_tolower(ch) : ch;
    }

    p->next = ctx->patterns;
    ctx->patterns = p;

    ctx->max_state += len;

    return 0;
}


int acsm_complie(acsm_context_t *ctx)
{
    unsigned int i, j;

    ctx->state_table = malloc(ctx->max_state * sizeof(acsm_state_node_t));
    if (ctx->state_table == NULL) {
        return -1;
    }

    for (i = 0; i < ctx->max_state; i++) {

        ctx->state_table[i].fail_state = 0;

        for (j = 0; j < ASCIITABLE_SIZE; j++) {
            ctx->state_table[i].next_state[j] = ACSM_FAIL_STATE;
        }
    }

    /*TODO*/

    return 0;
}


char *test_patterns[] = {"foo", "bar", NULL};
char *input = 
"In the beginning God created the heaven and the earth. \n" \
"And the earth was without form, and void; and darkness was upon the face of the deep. And the Spirit of God moved upon the face of the waters. \n" \
"And God said, Let there be light: and there was light.\n" \
"And God saw the light, that it was good: and God divided the light from the darkness.\n" \
"And God called the light Day, and the darkness he called Night. And the evening and the morning were the first day.\n";


int main() 
{
    u_char *input;
    acsm_context_t *ctx;

    ctx = acsm_alloc(0);
    if (ctx == NULL) {
        fprintf(stderr, "acsm_alloc() error.\n");
        return -1;
    }

    input = (u_char *) test_patterns;
   
    while(input) {
        acsm_add_pattern(ctx, input);
        input++;
    }


    acsm_free(ctx);

    return 0;
}

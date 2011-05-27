
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


typedef struct acsm_queue_s {
    struct acsm_queue_s  *prev;
    struct acsm_queue_s  *next;
} acsm_queue_t;

typedef struct {
    int          state;
    acsm_queue_t queue;
} acsm_state_queue_t;


#define acsm_queue_init(q)                                                    \
    (q)->prev = q;                                                            \
    (q)->next = q


#define acsm_queue_empty(h)                                                   \
    (h == (h)->prev)


#define acsm_queue_insert_head(h, x)                                          \
    (x)->next = (h)->next;                                                    \
    (x)->next->prev = x;                                                      \
    (x)->prev = h;                                                            \
    (h)->next = x


#define acsm_queue_insert_tail(h, x)                                          \
    (x)->prev = (h)->prev;                                                    \
    (x)->prev->next = x;                                                      \
    (x)->next = h;                                                            \
    (h)->prev = x


#define acsm_queue_head(h)                                                    \
    (h)->next


#define acsm_queue_last(h)                                                    \
    (h)->prev


#define acsm_queue_sentinel(h)                                                \
    (h)


#define acsm_queue_next(q)                                                    \
    (q)->next


#define acsm_queue_prev(q)                                                    \
    (q)->prev


#define acsm_queue_remove(x)                                                  \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next


#define acsm_queue_split(h, q, n)                                             \
    (n)->prev = (h)->prev;                                                    \
    (n)->prev->next = n;                                                      \
    (n)->next = q;                                                            \
    (h)->prev = (q)->prev;                                                    \
    (h)->prev->next = h;                                                      \
    (q)->prev = n;


#define acsm_queue_add(h, n)                                                  \
    (h)->prev->next = (n)->next;                                              \
    (n)->next->prev = (h)->prev;                                              \
    (h)->prev = (n)->prev;                                                    \
    (h)->prev->next = h;


#define acsm_queue_data(q, type, link)                                        \
    (type *) ((u_char *) q - offsetof(type, link))


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
    ctx->num_state = 0;

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
    int state, new_state;
    u_char ch;
    unsigned int i, j, k;
    acsm_pattern_t *p, *copy;

    ctx->state_table = malloc(ctx->max_state * sizeof(acsm_state_node_t));
    if (ctx->state_table == NULL) {
        return -1;
    }

    for (i = 1; i < ctx->max_state; i++) {

        ctx->state_table[i].fail_state = 0;
        ctx->state_table[i].match_list = NULL;

        for (j = 0; j < ASCIITABLE_SIZE; j++) {
            ctx->state_table[i].next_state[j] = ACSM_FAIL_STATE;
        }
    }

    for (j = 0; j < ASCIITABLE_SIZE; j++) {
        ctx->state_table[0].next_state[j] = ACSM_FAIL_STATE;
    }

    /* Construction of the goto function */
    p = ctx->patterns;
    while (p) {
        state = 0;
        j = 0;

        while(j < p->len) {

            ch = p->string[j];
            if (ctx->state_table[state].next_state[ch] == ACSM_FAIL_STATE) {
                break;
            }

            state = ctx->state_table[state].next_state[ch];
            j++;
        }

        for (k = j; k < p->len; k++) {
            new_state = ++ctx->num_state;
            ch = p->string[k];

            ctx->state_table[state].next_state[ch] = new_state;
            state = new_state;
        }

        copy = malloc(sizeof(acsm_pattern_t));
        if (copy == NULL) {
            return -1;
        }
        memcpy(copy, p, sizeof(acsm_pattern_t));

        copy->next = ctx->state_table[state].match_list; 
        ctx->state_table[state].match_list = copy; 

        p = p->next;
    }

    /* Construction of the failure function */
    acsm_queue_t        *qu, queue;
    acsm_state_queue_t  free_queue;

    qu = &queue;
    acsm_queue_init(qu);
    qu = &free_queue.queue;
    acsm_queue_init(qu);


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
    u_char **input;
    acsm_context_t *ctx;

    ctx = acsm_alloc(0);
    if (ctx == NULL) {
        fprintf(stderr, "acsm_alloc() error.\n");
        return -1;
    }

    input = (u_char**) test_patterns;
   
    while(*input) {
        acsm_add_pattern(ctx, *input);
        input++;
    }

    acsm_free(ctx);

    return 0;
}

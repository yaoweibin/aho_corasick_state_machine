#ifndef _ACSM_H_
#define _ACSM_H_

#include <stdlib.h>

#define ALPHABET_SIZE    (256)     
#define PATTERN_MAXLEN   (1024) 

#define ACSM_FAIL_STATE  (-1)     


typedef struct acsm_pattern_s {
    unsigned char *string;
    size_t         len;

    struct acsm_pattern_s *next;
} acsm_pattern_t;


typedef struct {
    int next_state[ALPHABET_SIZE];
    int fail_state;

    /* output */
    acsm_pattern_t *match_list;
} acsm_state_node_t;


typedef struct {
    unsigned max_state;
    unsigned num_state;

    acsm_pattern_t    *patterns;
    acsm_state_node_t *state_trie;

    unsigned no_case;
} acsm_context_t;


#endif /* _ACSM_H_ */

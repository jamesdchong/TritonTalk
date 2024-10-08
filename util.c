#include "util.h"

// Linked list functions
int ll_get_length(LLnode* head) {
    LLnode* tmp;
    int count = 1;
    if (head == NULL)
        return 0;
    else {
        tmp = head->next;
        while (tmp != head) {
            count++;
            tmp = tmp->next;
        }
        return count;
    }
}

void ll_append_node(LLnode** head_ptr, void* value) {
    LLnode* prev_last_node;
    LLnode* new_node;
    LLnode* head;

    if (head_ptr == NULL) {
        return;
    }

    // Init the value pntr
    head = (*head_ptr);
    new_node = (LLnode*) malloc(sizeof(LLnode));
    new_node->value = value;

    // The list is empty, no node is currently present
    if (head == NULL) {
        (*head_ptr) = new_node;
        new_node->prev = new_node;
        new_node->next = new_node;
    } else {
        // Node exists by itself
        prev_last_node = head->prev;
        head->prev = new_node;
        prev_last_node->next = new_node;
        new_node->next = head;
        new_node->prev = prev_last_node;
    }
}

LLnode* ll_pop_node(LLnode** head_ptr) {
    LLnode* last_node;
    LLnode* new_head;
    LLnode* prev_head;

    prev_head = (*head_ptr);
    if (prev_head == NULL) {
        return NULL;
    }
    last_node = prev_head->prev;
    new_head = prev_head->next;

    // We are about to set the head ptr to nothing because there is only one
    // thing in list
    if (last_node == prev_head) {
        (*head_ptr) = NULL;
        prev_head->next = NULL;
        prev_head->prev = NULL;
        return prev_head;
    } else {
        (*head_ptr) = new_head;
        last_node->next = new_head;
        new_head->prev = last_node;

        prev_head->next = NULL;
        prev_head->prev = NULL;
        return prev_head;
    }
}

void ll_destroy_node(LLnode* node) {
    if (node->type == llt_string) {
        free((char*) node->value);
    }
    free(node);
}

// Compute the difference in usec for two timeval objects
long timeval_usecdiff(struct timeval* start_time, struct timeval* finish_time) {
    long usec;
    usec = (finish_time->tv_sec - start_time->tv_sec) * 1000000;
    usec += (finish_time->tv_usec - start_time->tv_usec);
    return usec;
}

// Print out messages entered by the user
void print_cmd(Cmd* cmd) {
    fprintf(stderr, "src=%d, dst=%d, message=%s\n", cmd->src_id, cmd->dst_id,
            cmd->message);
}

char* convert_frame_to_char(Frame* frame) {
    char* char_buffer = malloc(sizeof(Frame));
    memcpy(char_buffer, frame, sizeof(Frame));
    return char_buffer;
}

Frame* convert_char_to_frame(char* char_buf) {
    Frame* frame = malloc(sizeof(Frame));
    memcpy(frame, char_buf, sizeof(Frame));
    return frame;
}

uint16_t get_bit(uint16_t next_byte, int j) {
    return ((next_byte>>j) & 0x0001);
}

uint16_t crc16(uint16_t* array, int array_len) {
    uint16_t poly = 0x8005; // 1000 0000 0000 0101
    uint16_t crc = array[0];
    crc = ((crc<<8) & 0xff00) | ((crc>>8) & 0x00ff);
    int i,j;
    for (i = 1; i < array_len / 2; i++) {
        uint16_t next_byte = array[i];
        next_byte = ((next_byte<<8) & 0xff00) | ((next_byte>>8) & 0x00ff);
        // fprintf(stderr, "%x ", next_byte);
        for (j = 15; j >= 0; j--) {
            if (get_bit(crc, 15) == 0x0000) {
                crc = crc<<1;
                crc = crc | get_bit(next_byte, j);
            } else {
                crc = crc<<1;
                crc = crc | get_bit(next_byte, j);
                crc = crc ^ poly;
            }
        }
    }
    crc = ((crc<<8) & 0xff00) | ((crc>>8) & 0x00ff);
    return crc;
}

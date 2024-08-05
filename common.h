#ifndef __COMMON_H__
#define __COMMON_H__

#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_COMMAND_LENGTH 16
#define AUTOMATED_FILENAME 512
typedef unsigned char uchar_t;

// System configuration information
struct SysConfig_t {
    float drop_prob;
    float corrupt_prob;
    unsigned char automated;
    char automated_file[AUTOMATED_FILENAME];
};
typedef struct SysConfig_t SysConfig;

// Command line input information
struct Cmd_t {
    uint16_t src_id;
    uint16_t dst_id;
    char* message;
};
typedef struct Cmd_t Cmd;

// Linked list information
enum LLtype { llt_string, llt_frame, llt_integer, llt_head } LLtype;

struct LLnode_t {
    struct LLnode_t* prev;
    struct LLnode_t* next;
    enum LLtype type;

    void* value;
};
typedef struct LLnode_t LLnode;

#define WINDOW_SIZE 8

#define MAX_FRAME_SIZE 64

// TODO: You should change this! Except, DO NOT CHANGE end_of_message. It SHOULD BE the first field in struct Frame_t
// Remember, your frame can be AT MOST 64 bytes!
#define FRAME_PAYLOAD_SIZE 54
struct __attribute__((__packed__)) Frame_t {
    uchar_t end_of_message;
    // Add other header fields
    char data[FRAME_PAYLOAD_SIZE+1]; // +1 for the null terminator

    uint16_t src_id; // sender of frame
    uint16_t dst_id; // receiver of frame
    uint16_t seqNum; // sequence number of frame
    uint16_t crc; // 2 bytes
};
typedef struct Frame_t Frame;

struct recvQ_slot {
    Frame* frame;
};
typedef struct recvQ_slot recvQ[WINDOW_SIZE];

// Receiver and sender data structures
struct Receiver_t {
    // DO NOT CHANGE:
    // 1) buffer_mutex
    // 2) buffer_cv
    // 3) input_framelist_head
    // 4) recv_id
    pthread_mutex_t buffer_mutex;
    pthread_cond_t buffer_cv;
    LLnode* input_framelist_head;
    int recv_id;

    LLnode* final_frames; // Queue to store frames until all a command's frames are processed and ready to print
    int send_id; // The id of the sender sending a message
    int lastSeqNums[256]; // Array containing the last seq Num received from each of the 256 senders to avoid printing duplicates
    LLnode* senders_buffer[256]; // Array of queues that hold incoming frames for each sender to avoid printing multiple senders' messages to the same receiver
    char* print_strings[256]; // Array of strings holding the the frames' messages until ready to print

    uchar_t RWS;
    uchar_t NFE[256];
    uchar_t LFR[256];

    recvQ windowsArray[256];
};

struct sendQ_slot {
    Frame* frame;
    struct timeval* timeout;
};
typedef struct sendQ_slot sendQ[WINDOW_SIZE];

struct Sender_t {
    // DO NOT CHANGE:
    // 1) buffer_mutex
    // 2) buffer_cv
    // 3) input_cmdlist_head
    // 4) input_framelist_head
    // 5) send_id
    pthread_mutex_t buffer_mutex;
    pthread_cond_t buffer_cv;
    LLnode* input_cmdlist_head;
    LLnode* input_framelist_head;
    int send_id;

    // Frame* lastFrame; // Last frame sent to a receiver. Used to resent expired frames
    LLnode* frames; // Queue of frames to be sent to the receiver
    // struct timeval* tv; // The next expiring time value for the current frame
    // uchar_t nextSeqNum; // The next sequence number for the next frame sent
    // uchar_t emptyQueue; // Checks if the sender is currently sending frames
    int broadcast_counter; // Must receiver 255 acks before sending new frame

    uchar_t SWS;
    uchar_t LAR[256];
    uchar_t LFS[256];
    // Frame* slidingWindow[256];
    // struct sendQ_slot {
    //     struct timeval* timeout;
    //     Frame* frame;
    // } sendQ[WINDOW_SIZE];

    // struct frame_buffer {
    //     Frame* frame;
    //     struct timeval* timeout;
    // } frame_buffer[256];

    sendQ frame_buffer[256];

    struct ack_buffer {
        Frame* ack;
    } ack_buffer[256];

    uchar_t outstanding_frames;

    uchar_t nextSeqNums[256];
};

enum SendFrame_DstType { ReceiverDst, SenderDst } SendFrame_DstType;

typedef struct Sender_t Sender;
typedef struct Receiver_t Receiver;

// Declare global variables here
// DO NOT CHANGE:
//   1) glb_senders_array
//   2) glb_receivers_array
//   3) glb_senders_array_length
//   4) glb_receivers_array_length
//   5) glb_sysconfig
//   6) CORRUPTION_BITS
Sender* glb_senders_array;
Receiver* glb_receivers_array;
int glb_senders_array_length;
int glb_receivers_array_length;
SysConfig glb_sysconfig;
int CORRUPTION_BITS;

#endif

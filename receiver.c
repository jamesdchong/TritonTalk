#include <string.h>
#include "receiver.h"

void init_receiver(Receiver* receiver, int id) {
    pthread_cond_init(&receiver->buffer_cv, NULL);
    pthread_mutex_init(&receiver->buffer_mutex, NULL);
    receiver->input_framelist_head = NULL;
    receiver->recv_id = id;

    receiver->final_frames = NULL;
    receiver->send_id = -1;
    for (int i = 0; i < glb_senders_array_length; i++) {
        receiver->lastSeqNums[i] = -1;

        receiver->senders_buffer[i] = NULL;

        receiver->print_strings[i] = malloc(100000 * sizeof(char));
        char temp[100000];
        temp[0] = '\0';
        memcpy(receiver->print_strings[i], temp, 100000);
    }

    receiver->RWS = 8;

    for (int i = 0; i < 256; i++) {
        receiver->NFE[i] = 0;
        receiver->LFR[i] = 255;
    }

    // for (int i = 0; i < 256; i++) {
    //     for (int j = 0; j < WINDOW_SIZE; j++) {
    //         receiver->windowsArray[i][j].frame = malloc(sizeof(Frame));
    //     }
    // }
}

void handle_incoming_msgs(Receiver* receiver,
                          LLnode** outgoing_frames_head_ptr) {
    // TODO: Suggested steps for handling incoming frames
    //    1) Dequeue the Frame from the sender->input_framelist_head
    //    2) Convert the char * buffer to a Frame data type
    //    3) Check whether the frame is for this receiver
    //    4) Acknowledge that this frame was received
    int incoming_msgs_length = ll_get_length(receiver->input_framelist_head);
    while (incoming_msgs_length > 0) {
        // 1) Dequeue the Frame from the sender->input_framelist_head
        LLnode* ll_inmsg_node = ll_pop_node(&receiver->input_framelist_head);
        incoming_msgs_length = ll_get_length(receiver->input_framelist_head);

        // DUMMY CODE: Print the raw_char_buf
        // NOTE: You should not blindly print messages!
        //      Ask yourself: Is this message really for me?

        // 2) Convert the char * buffer to a Frame data type
        char* raw_char_buf = ll_inmsg_node->value;
        free(ll_inmsg_node);

        uint16_t* crc_buffer = (uint16_t*) raw_char_buf;
        uint16_t crc = crc16(crc_buffer, MAX_FRAME_SIZE);

        // fprintf(stderr, "received crc is %x\n", crc);

        if (crc != 0x0000) {
            // fprintf(stderr, "message corrupted %x\n", crc);
            continue;
        }

        Frame* frame = convert_char_to_frame(raw_char_buf);

        ll_append_node(&(receiver->senders_buffer)[frame->src_id], raw_char_buf);

        // Loop through each sender
        for (int i = 0; i < glb_senders_array_length; i++) {
            int sender_buffer_length = ll_get_length(receiver->senders_buffer[i]);

            // Process frames for each sender
            while(sender_buffer_length > 0) {
                LLnode* ll_frame_node = ll_pop_node(&receiver->senders_buffer[i]);
                sender_buffer_length = ll_get_length(receiver->senders_buffer[i]);

                // Convert to frame
                char* sender_raw_char_buf = ll_frame_node->value;
                Frame* inframe = convert_char_to_frame(sender_raw_char_buf);

                // 3) Check whether the frame is for this receiver
                // If the receiver id is 255, print the message to all receivers
                if ((inframe->dst_id == 255 || inframe->dst_id == receiver->recv_id)) {

                    if((receiver->NFE[inframe->src_id] <= (receiver->NFE[inframe->src_id] + receiver->RWS - 1) % 256 && inframe->seqNum >= receiver->NFE[inframe->src_id] && inframe->seqNum <= (receiver->NFE[inframe->src_id] + receiver-> RWS - 1) % 256) || (receiver->NFE[inframe->src_id] > (receiver->NFE[inframe->src_id] + receiver->RWS - 1) % 256 && (inframe->seqNum >= receiver->NFE[inframe->src_id] || inframe->seqNum <= (receiver->NFE[inframe->src_id] + receiver->RWS - 1) % 256))) {
                        fprintf(stderr, "Within receiver window\n");
                        receiver->windowsArray[inframe->src_id][inframe->seqNum % WINDOW_SIZE].frame = inframe;

                        fprintf(stderr, "NFE %u\n", receiver->NFE[inframe->src_id]);
                        fprintf(stderr, "end %u\n", (receiver->NFE[inframe->src_id] + receiver->RWS - 1) % 256);

                        // slides window
                        while(receiver->NFE[inframe->src_id] <= (receiver->NFE[inframe->src_id] + receiver->RWS - 1) % 256 && receiver->windowsArray[inframe->src_id][receiver->NFE[inframe->src_id] % WINDOW_SIZE].frame != NULL && receiver->NFE[inframe->src_id] == receiver->windowsArray[inframe->src_id][receiver->NFE[inframe->src_id] % WINDOW_SIZE].frame->seqNum) {

                            
                            fprintf(stderr, "Received message\n");
                            // fprintf(stderr, "message is %s and inframe->seqNum is %d and receiver->last_seqNum is %d\n", inframe->data, inframe->seqNum, receiver->lastSeqNums[receiver->send_id]);

                            receiver->send_id = inframe->src_id; // Set the current sender's id

                            fprintf(stderr, "frame seqNum %d\n", inframe->seqNum);
                            fprintf(stderr, "lastseqnum %d\n", receiver->lastSeqNums[receiver->send_id]);

                            fprintf(stderr, "eom is %u\n", inframe->end_of_message);

                            // Concat the exisiting print_string with this message if it is not a duplicate
                            if (inframe->seqNum != receiver->lastSeqNums[receiver->send_id]) {        //         NEED TO KEEP WITH THE ELSE STATEMENT FOR DROPS???? AHHHHHHHHHHHHHHHHHHHHHHHH
                                strcat(receiver->print_strings[i], inframe->data);

                                receiver->lastSeqNums[receiver->send_id]++;
                                // Executes when the frame is the last frame of the message
                                if (inframe->end_of_message == 0xFF) {
                                    // Print combined string to stdout
                                    fprintf(stdout, "<RECV_%d>:[%s]\n", receiver->recv_id, receiver->print_strings[i]);
                                    receiver->print_strings[i][0] = '\0';
                                }

                                receiver->NFE[inframe->src_id]++;
                            } else {
                                // receiver->lastSeqNums[receiver->send_id]--;
                            }




                            
                        }

                        fprintf(stderr, "NFE after %u\n", receiver->NFE[inframe->src_id]);
                        fprintf(stderr, "end after %u\n", (receiver->NFE[inframe->src_id] + receiver->RWS - 1) % 256);
                    } else {
                        fprintf(stderr, "Not in receiver window\n");
                    }

                                            //send ack
                        Frame* ackframe = malloc(sizeof(Frame));

                        // Set sender id, sequence num
                        ackframe->src_id = inframe->src_id;
                        ackframe->dst_id = inframe->dst_id;
                        ackframe->seqNum = inframe->seqNum;

                        ackframe->crc = 0;

                        uint16_t* converted_outgoing_charbuf = (uint16_t*) convert_frame_to_char(ackframe);

                        uint16_t crc = crc16(converted_outgoing_charbuf, MAX_FRAME_SIZE);

                        ackframe->crc = crc;

                        // Add ack frame to outgoing frames
                        char* ack_charbuf = convert_frame_to_char(ackframe);
                        ll_append_node(outgoing_frames_head_ptr, ack_charbuf);

                        fprintf(stderr, "Successfully sent Ack %d\n", ackframe->seqNum);

                }
            }
        }
    }
}

void* run_receiver(void* input_receiver) {
    struct timespec time_spec;
    struct timeval curr_timeval;
    const int WAIT_SEC_TIME = 0;
    const long WAIT_USEC_TIME = 100000;
    Receiver* receiver = (Receiver*) input_receiver;
    LLnode* outgoing_frames_head;

    // This incomplete receiver thread, at a high level, loops as follows:
    // 1. Determine the next time the thread should wake up if there is nothing
    // in the incoming queue(s)
    // 2. Grab the mutex protecting the input_msg queue
    // 3. Dequeues messages from the input_msg queue and prints them
    // 4. Releases the lock
    // 5. Sends out any outgoing messages

    while (1) {
        // NOTE: Add outgoing messages to the outgoing_frames_head pointer
        outgoing_frames_head = NULL;
        gettimeofday(&curr_timeval, NULL);

        // Either timeout or get woken up because you've received a datagram
        // NOTE: You don't really need to do anything here, but it might be
        // useful for debugging purposes to have the receivers periodically
        // wakeup and print info
        time_spec.tv_sec = curr_timeval.tv_sec;
        time_spec.tv_nsec = curr_timeval.tv_usec * 1000;
        time_spec.tv_sec += WAIT_SEC_TIME;
        time_spec.tv_nsec += WAIT_USEC_TIME * 1000;
        if (time_spec.tv_nsec >= 1000000000) {
            time_spec.tv_sec++;
            time_spec.tv_nsec -= 1000000000;
        }

        //*****************************************************************************************
        // NOTE: Anything that involves dequeing from the input frames should go
        //      between the mutex lock and unlock, because other threads
        //      CAN/WILL access these structures
        //*****************************************************************************************
        pthread_mutex_lock(&receiver->buffer_mutex);

        // Check whether anything arrived
        int incoming_msgs_length =
            ll_get_length(receiver->input_framelist_head);
        if (incoming_msgs_length == 0) {
            // Nothing has arrived, do a timed wait on the condition variable
            // (which releases the mutex). Again, you don't really need to do
            // the timed wait. A signal on the condition variable will wake up
            // the thread and reacquire the lock
            pthread_cond_timedwait(&receiver->buffer_cv,
                                   &receiver->buffer_mutex, &time_spec);
        }

        handle_incoming_msgs(receiver, &outgoing_frames_head);

        pthread_mutex_unlock(&receiver->buffer_mutex);

        // CHANGE THIS AT YOUR OWN RISK!
        // Send out all the frames user has appended to the outgoing_frames list
        int ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
        while (ll_outgoing_frame_length > 0) {
            LLnode* ll_outframe_node = ll_pop_node(&outgoing_frames_head);
            char* char_buf = (char*) ll_outframe_node->value;

            // The following function frees the memory for the char_buf object
            send_msg_to_senders(char_buf);

            // Free up the ll_outframe_node
            free(ll_outframe_node);

            ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
        }
    }
    pthread_exit(NULL);
}

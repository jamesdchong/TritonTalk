#include "sender.h"

#include <assert.h>

void init_sender(Sender* sender, int id) {
    pthread_cond_init(&sender->buffer_cv, NULL);
    pthread_mutex_init(&sender->buffer_mutex, NULL);
    sender->send_id = id;
    sender->input_cmdlist_head = NULL;
    sender->input_framelist_head = NULL;

    // TODO: You should fill in this function as necessary
    // sender->lastFrame = NULL;
    sender->frames = NULL;
    // sender->tv = NULL;
    // sender->nextSeqNum = 0;
    // sender->emptyQueue = 0x00; // 0x00 for empty queue, 0xFF for non-empty queue
    sender->broadcast_counter = 0;

    sender->SWS = WINDOW_SIZE;
    for (int i = 0; i < 256; i++) {
        sender->LAR[i] = 255;
        sender->LFS[i] = 255;
    }

    // for (int i = 0; i < 256; i++) {
    //     sender->frame_buffer[i].frame = malloc(sizeof(Frame));
    //     sender->frame_buffer[i].timeout = NULL;
    // }

    for (int i = 0; i < 256; i++) {
        sender->ack_buffer[i].ack = malloc(sizeof(Frame));
    }

    for (int i = 0; i < 256; i++) {
        sender->nextSeqNums[i] = 0;
    }

    // for (int i = 0; i < WINDOW_SIZE; i++) {
    //     sender->sendQ[i].frame = malloc(sizeof(Frame));
    //     sender->sendQ[i].timeout = NULL;
    // }

    sender->outstanding_frames = 0;
}

struct timeval* sender_get_next_expiring_timeval(Sender* sender) {
    // TODO: You should fill in this function so that it returns the next
    // timeout that should occur

    // Create new timeval struct in memory, return sender's tv struct
    // struct timeval* new_tv = malloc(sizeof(struct timeval));
    // new_tv = sender->tv;
    // return new_tv;

    if(sender) {
        //dummy code to remove warning DELETEEEEEE
    }

    return NULL;
}

void handle_incoming_acks(Sender* sender, LLnode** outgoing_frames_head_ptr) {
    // TODO: Suggested steps for handling incoming ACKs
    //    1) Dequeue the ACK from the sender->input_framelist_head
    //    2) Convert the char * buffer to a Frame data type
    //    3) Check whether the frame is for this sender
    //    4) Do stop-and-wait for sender/receiver pair
    int input_frame_length = ll_get_length(sender->input_framelist_head);

    // Loop while there are ack frames available
    while (input_frame_length > 0) {
        // 1) Dequeue the ACK from the sender->input_framelist_head
        LLnode* ll_input_frame_node = ll_pop_node(&sender->input_framelist_head);
        input_frame_length = ll_get_length(sender->input_framelist_head);

        // 2) Convert the char * buffer to a Frame data type
        char* raw_char_buf = ll_input_frame_node->value;

        uint16_t* crc_buffer = (uint16_t*) raw_char_buf;
        uint16_t crc = crc16(crc_buffer, MAX_FRAME_SIZE);

        if (crc != 0x0000) {
            // fprintf(stderr, "ack corrupted\n");
            continue;
        }

        Frame* ackframe = convert_char_to_frame(raw_char_buf);

        free(raw_char_buf);
        
        // 3) Check whether the frame is for this sender
        if (ackframe->src_id == sender->send_id) {

            sender->ack_buffer[ackframe->seqNum % WINDOW_SIZE].ack = ackframe;

            sender->frame_buffer[ackframe->dst_id][sender->LAR[ackframe->dst_id]].timeout = NULL;


            while ((sender->LAR[ackframe->dst_id] + 1) % 256 <= (sender->LAR[ackframe->dst_id] + 1) % 256 + sender->SWS - 1 && sender->ack_buffer[ackframe->seqNum % WINDOW_SIZE].ack != NULL && (sender->LAR[ackframe->dst_id] + 1) % 256 == sender->ack_buffer[ackframe->seqNum % WINDOW_SIZE].ack->seqNum) {
                sender->LAR[ackframe->dst_id]++;
                sender->frame_buffer[ackframe->dst_id][sender->LAR[ackframe->dst_id]].timeout = NULL;
            }
            fprintf(stderr, "LAR is %u\n", sender->LAR[ackframe->dst_id]);



            // When still sending broadcasted frames, do not send new frames
            if (sender->broadcast_counter != 0) {
                // sender->emptyQueue = 0xFF;
                sender->broadcast_counter--;
            }

            // fprintf(stderr, "seq num is %u\n", ackframe->seqNum % WINDOW_SIZE);



// NEEEDDD




            // if(sender->broadcast_counter == 0) {
            //     // ack needs to be for next in order frame
            //     // if (++sender->LAR == ackframe->seqNum) {

            //         // sender->sendQ[sender->LAR % WINDOW_SIZE].timeout = NULL;
            //         // sender->frame_buffer[sender->LAR].timeout = NULL;

            //         sender->emptyQueue = 0xFF;

            //         fprintf(stderr, "outstanding frames %d\n", sender->outstanding_frames);

            //         if (sender->outstanding_frames > 0) {
            //             sender->outstanding_frames--;
            //             sender->LFS++;

            //             Frame* frame = sender->frame_buffer[][sender->LFS].frame;


            //             char* outgoing_charbuf = convert_frame_to_char(frame);
            //             ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);

            //             // Sets the timeout time for this message
            //             struct timeval* new_tv = malloc(sizeof(struct timeval));
            //             gettimeofday(new_tv, NULL);
            //             new_tv->tv_usec += 90000;

            //             fprintf(stderr, "LFS is %u\n", sender->LFS);
            //             sender->frame_buffer[sender->LFS].timeout = new_tv;
            //         }




// NEEDDDD






                // } else {
                    // sender->LAR--;
                    // char* ack_buffer = convert_frame_to_char(ackframe);
                    // ll_append_node(&sender->input_framelist_head, ack_buffer);
                // }
            // }

            if (ll_get_length(sender->frames) != 0 && sender->broadcast_counter == 0) {
                // fprintf(stderr, "Sending new frame message: %s, seq num: %d\n", sender->lastFrame->data, frame->seqNum);

                // sender->emptyQueue = 0xFF;

                // // Get next frame
                // LLnode* ll_frames_node = ll_pop_node(&sender->frames);
                // Frame* frame = (Frame*) ll_frames_node->value;
                // free(ll_frames_node);
                // // Send next frame to receiver
                // char* outgoing_charbuf = convert_frame_to_char(frame);
                // // Send to receiver
                // ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);

                // // Sets the timeout time for this message
                // struct timeval* new_tv = malloc(sizeof(struct timeval));
                // gettimeofday(new_tv, NULL);
                // new_tv->tv_usec += 90000;
                // // sender->tv = new_tv;

                // sender->LFS = frame->seqNum;

                // fprintf(stderr, "LFS is %u\n", sender->LFS);
                // sender->sendQ[sender->LFS % WINDOW_SIZE].frame = frame;
                // sender->sendQ[sender->LFS % WINDOW_SIZE].timeout = new_tv;

            } else {
                // If there are no more frames to send, there is no frame to resend
                // sender->lastFrame = NULL;
                // sender->emptyQueue = 0x00;
            }
        }
        // free(ackframe);
        // free(ll_input_frame_node);
    }
}

void handle_input_cmds(Sender* sender, LLnode** outgoing_frames_head_ptr) {
    // TODO: Suggested steps for handling input cmd
    //    1) Dequeue the Cmd from sender->input_cmdlist_head
    //    2) Convert to Frame
    //    3) Set up the frame according to the protocol
    int input_cmd_length = ll_get_length(sender->input_cmdlist_head);

    // Recheck the command queue length to see if stdin_thread dumped a command
    // on us
    input_cmd_length = ll_get_length(sender->input_cmdlist_head);
    while (input_cmd_length > 0) {
        // 1) Dequeue the Cmd from sender->input_cmdlist_head
        LLnode* ll_input_cmd_node = ll_pop_node(&sender->input_cmdlist_head);
        input_cmd_length = ll_get_length(sender->input_cmdlist_head);

        // Cast to Cmd type and free up the memory for the node
        Cmd* outgoing_cmd = (Cmd*) ll_input_cmd_node->value;
        free(ll_input_cmd_node);

        // DUMMY CODE: Add the raw char buf to the outgoing_frames list
        // NOTE: You should not blindly send this message out!
        //      Ask yourself: Is this message actually going to the right
        //      receiver (recall that default behavior of send is to broadcast
        //      to all receivers)?
        //                    Were the previous messages sent to this receiver
        //                    ACTUALLY delivered to the receiver?
        int msg_length = strlen(outgoing_cmd->message);
        if (msg_length > FRAME_PAYLOAD_SIZE) {
            // Do something about messages that exceed the frame size

            // Loop for each frame in the message
            for (int i = 0; i < msg_length / FRAME_PAYLOAD_SIZE+1; i++) {
                // 2) Convert to Frame
                Frame* outgoing_frame = malloc(sizeof(Frame));
                assert(outgoing_frame);
                // Copy the selected message
                memcpy(outgoing_frame->data, outgoing_cmd->message + (i * FRAME_PAYLOAD_SIZE), FRAME_PAYLOAD_SIZE);
                outgoing_frame->data[FRAME_PAYLOAD_SIZE] = '\0'; // Null-terminate the message

                // 3) Set up the frame according to the protocol
                outgoing_frame->src_id = outgoing_cmd->src_id;
                outgoing_frame->dst_id = outgoing_cmd->dst_id;

                outgoing_frame->seqNum = sender->nextSeqNums[outgoing_frame->dst_id];
                // if (sender->nextSeqNum == 255) {
                //     sender->nextSeqNum = 0;
                // } else {
                sender->nextSeqNums[outgoing_frame->dst_id]++;
                // }

                // Set end_of_message char if last frame in message
                if (i == msg_length / FRAME_PAYLOAD_SIZE) {
                    outgoing_frame->end_of_message = 0xFF;
                } else {
                    outgoing_frame->end_of_message = 0x00;
                }

                // fprintf(stderr, "seqNum is %u\n", sender->nextSeqNum);

                outgoing_frame->crc = 0;

                uint16_t* converted_outgoing_charbuf = (uint16_t*) convert_frame_to_char(outgoing_frame);

                uint16_t crc = crc16(converted_outgoing_charbuf, MAX_FRAME_SIZE);

                free(converted_outgoing_charbuf);

                outgoing_frame->crc = crc;

                // If the frame being sent is broadcasted, must resend the frame to all available receivers
                if(outgoing_frame->dst_id == 255) {
                    sender->broadcast_counter += glb_receivers_array_length;
                }

                sender->frame_buffer[outgoing_frame->dst_id][outgoing_frame->seqNum].frame = outgoing_frame;
                // memcpy(sender->frame_buffer[outgoing_frame->dst_id][outgoing_frame->seqNum].frame, outgoing_frame, sizeof(Frame));

                if((sender->LAR[outgoing_frame->dst_id] <= sender->LFS[outgoing_frame->dst_id] && sender->LFS[outgoing_frame->dst_id] - sender->LAR[outgoing_frame->dst_id] < WINDOW_SIZE) || (sender->LAR[outgoing_frame->dst_id] > sender->LFS[outgoing_frame->dst_id] && ((256 - sender->LAR[outgoing_frame->dst_id]) + sender->LFS[outgoing_frame->dst_id] < WINDOW_SIZE))) {
                    //send to receiver
                    sender->LFS[outgoing_frame->dst_id] = outgoing_frame->seqNum;


                    // Sets the timeout time for this message
                    struct timeval* new_tv = malloc(sizeof(struct timeval));
                    gettimeofday(new_tv, NULL);
                    new_tv->tv_usec += 90000;

                    fprintf(stderr, "LFS is %u\n", sender->LFS[outgoing_frame->dst_id]);
                    // sender->frame_buffer[sender->LFS].frame = outgoing_frame;

                    sender->frame_buffer[outgoing_frame->dst_id][sender->LFS[outgoing_frame->dst_id]].timeout = new_tv;

                    // sender->sendQ[sender->LFS % WINDOW_SIZE].frame = outgoing_frame;
                    // sender->sendQ[sender->LFS % WINDOW_SIZE].timeout = new_tv;

                    char* outgoing_charbuf = convert_frame_to_char(outgoing_frame);
                    ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);
                } else {
                    // Convert the message to the outgoing_charbuf
                    // char* outgoing_charbuf = convert_frame_to_char(outgoing_frame);
                    // ll_append_node(&sender->frames, outgoing_charbuf);
                    sender->outstanding_frames++;
                }
                // fprintf(stderr, "New command with multiple frames: id %d is %s\n", sender->send_id, outgoing_frame->data);

            }

            // At this point, we don't need the outgoing_cmd
            free(outgoing_cmd->message);
            free(outgoing_cmd);

        } else {
            // 2) Convert to Frame
            Frame* outgoing_frame = malloc(sizeof(Frame));
            assert(outgoing_frame);
            // Copy the message
            memcpy(outgoing_frame->data, outgoing_cmd->message, strlen(outgoing_cmd->message));
            outgoing_frame->data[strlen(outgoing_cmd->message)] = '\0'; // Null-terminate the message

            // 3) Set up the frame according to the protocol
            outgoing_frame->src_id = outgoing_cmd->src_id; // Set the sender id
            outgoing_frame->dst_id = outgoing_cmd->dst_id; // Set the receiver id
            outgoing_frame->end_of_message = 0xFF; // when only sending one frame, this frame is the last frame

            outgoing_frame->seqNum = sender->nextSeqNums[outgoing_frame->dst_id];
            // if (sender->nextSeqNum == 255) {
            //     sender->nextSeqNum = 0;
            // } else {
                // sender->nextSeqNum++;
            sender->nextSeqNums[outgoing_frame->dst_id]++;
            // }
                // fprintf(stderr, "seqNum is %u\n", sender->nextSeqNum);

            outgoing_frame->crc = 0;

            uint16_t* converted_outgoing_charbuf = (uint16_t*) convert_frame_to_char(outgoing_frame);

            uint16_t crc = crc16(converted_outgoing_charbuf, MAX_FRAME_SIZE);

            free(converted_outgoing_charbuf);

            outgoing_frame->crc = crc;

            // If the frame being sent is broadcasted, must resend the frame to all available receivers
            if(outgoing_frame->dst_id == 255) {
                sender->broadcast_counter += glb_receivers_array_length;
            }

            // At this point, we don't need the outgoing_cmd
            free(outgoing_cmd->message);
            free(outgoing_cmd);

            sender->frame_buffer[outgoing_frame->dst_id][outgoing_frame->seqNum].frame = outgoing_frame;
            // memcpy(sender->frame_buffer[outgoing_frame->dst_id][outgoing_frame->seqNum].frame, outgoing_frame, sizeof(Frame));

            if((sender->LAR[outgoing_frame->dst_id] <= sender->LFS[outgoing_frame->dst_id] && sender->LFS[outgoing_frame->dst_id] - sender->LAR[outgoing_frame->dst_id] < WINDOW_SIZE) || (sender->LAR[outgoing_frame->dst_id] > sender->LFS[outgoing_frame->dst_id] && ((256 - sender->LAR[outgoing_frame->dst_id]) + sender->LFS[outgoing_frame->dst_id] < WINDOW_SIZE))) {
                //send to receiver
                sender->LFS[outgoing_frame->dst_id] = outgoing_frame->seqNum;


                // Sets the timeout time for this message
                struct timeval* new_tv = malloc(sizeof(struct timeval));
                gettimeofday(new_tv, NULL);
                new_tv->tv_usec += 90000;

                fprintf(stderr, "LFS is %u\n", sender->LFS[outgoing_frame->dst_id]);

                sender->frame_buffer[outgoing_frame->dst_id][sender->LFS[outgoing_frame->dst_id]].timeout = new_tv;

                // sender->sendQ[sender->LFS % WINDOW_SIZE].frame = outgoing_frame;
                // sender->sendQ[sender->LFS % WINDOW_SIZE].timeout = new_tv;

                char* outgoing_charbuf = convert_frame_to_char(outgoing_frame);
                ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);
            } else {
                // Convert the message to the outgoing_charbuf
                // char* outgoing_charbuf = convert_frame_to_char(outgoing_frame);
                // ll_append_node(&sender->frames, outgoing_charbuf);
                sender->outstanding_frames++;
            }
            
            // fprintf(stderr, "New command with single frame: %s\n", outgoing_frame->data);

            free(outgoing_frame);
        }

        // if (sender->emptyQueue == 0x00) {
        //     sender->emptyQueue = 0xFF; // No longer an empty queue

        //     // Get next frame
        //     LLnode* ll_frames_node = ll_pop_node(&sender->frames);
        //     Frame* frame = (Frame*) ll_frames_node->value;
        //     free(ll_frames_node);



        //     char* outgoing_charbuf = convert_frame_to_char(frame);

        //     // This frame gets resent if dropped
        //     // sender->lastFrame = frame;
        //     // memcpy(sender->lastFrame->data, frame->data, FRAME_PAYLOAD_SIZE);


        //     // Send to receiver
        //     ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);

        //     // fprintf(stderr, "Sending very first new frame message: %s, seq num: %d\n", sender->lastFrame->data, frame->seqNum);
        // }
    }
}

void handle_timedout_frames(Sender* sender, LLnode** outgoing_frames_head_ptr) {
//     // TODO: Handle timeout by resending the appropriate message

//     // Execute only if a frame has already been sent
//     // if (sender->lastFrame != NULL) {
//     // fprintf(stderr, "here %u and %u\n", sender->LFS, sender->LAR);
//     // if (sender->LAR < sender->LFS) {

//         // uchar_t index;
//         // if (sender->LAR % WINDOW_SIZE == WINDOW_SIZE) {
//         //     index = 0;
//         // } else {
//         //     index = sender->LAR + 1;
//         // }

//         // Frame* resentFrame = sender->sendQ[index % WINDOW_SIZE].frame;
//         // struct timeval* timeout = sender->sendQ[index % WINDOW_SIZE].timeout;

// for (int i = 0; i < glb_receivers_array_length; i++) {



//         uchar_t index = ++sender->LAR[i];
//         struct timeval* timeout = sender->frame_buffer[i][index].timeout;
//         if (timeout != NULL) {

//             // Get the current time
//             struct timeval* tv = malloc(sizeof(struct timeval));
//             gettimeofday(tv, NULL);

//             // Get the difference between the current time and the set timeout time of a frame
//             long time_elapsed = timeval_usecdiff(tv, timeout);

//             // Executes when the current time has elapsed the timeout time
//             if(time_elapsed < 0) {
//                 // Frame* resentFrame = malloc(sizeof(Frame));
//                 // memcpy(resentFrame, sender->frame_buffer[i][index].frame, sizeof(Frame));

//                 Frame* resentFrame = NULL;
//                 memcpy(resentFrame, sender->frame_buffer[i][index].frame, sizeof(Frame));


//                 resentFrame->crc = 0;

//                 uint16_t* converted_outgoing_charbuf = (uint16_t*) convert_frame_to_char(resentFrame);

//                 uint16_t crc = crc16(converted_outgoing_charbuf, MAX_FRAME_SIZE);

//                 free(converted_outgoing_charbuf);

//                 resentFrame->crc = crc;


//                 // Convert to charbuf
//                 char* outgoing_charbuf = convert_frame_to_char(resentFrame);
//                 // Resend to receiver
//                 ll_append_node(outgoing_frames_head_ptr, outgoing_charbuf);

//                 // Set timeout time
//                 struct timeval* new_tv = malloc(sizeof(struct timeval));
//                 gettimeofday(new_tv, NULL);
//                 new_tv->tv_usec += 90000;
//                 // sender->tv = new_tv;
//                 // sender->sendQ[index % WINDOW_SIZE].timeout = new_tv;
//                 // sender->frame_buffer[i][sender->LAR[i]].frame = resentFrame;
//                 memcpy(sender->frame_buffer[i][index].frame, resentFrame, sizeof(Frame));
//                 sender->frame_buffer[i][index].timeout = new_tv;

//                 fprintf(stderr, "dropped. resending message\n");
//             }
//         }
//         sender->LAR[i]--;
// }







}

void* run_sender(void* input_sender) {
    struct timespec time_spec;
    struct timeval curr_timeval;
    const int WAIT_SEC_TIME = 0;
    const long WAIT_USEC_TIME = 100000;
    Sender* sender = (Sender*) input_sender;
    LLnode* outgoing_frames_head;
    struct timeval* expiring_timeval;
    long sleep_usec_time, sleep_sec_time;

    // This incomplete sender thread, at a high level, loops as follows:
    // 1. Determine the next time the thread should wake up
    // 2. Grab the mutex protecting the input_cmd/inframe queues
    // 3. Dequeues messages from the input queue and adds them to the
    // outgoing_frames list
    // 4. Releases the lock
    // 5. Sends out the messages

    while (1) {
        outgoing_frames_head = NULL;

        // Get the current time
        gettimeofday(&curr_timeval, NULL);

        // time_spec is a data structure used to specify when the thread should
        // wake up The time is specified as an ABSOLUTE (meaning, conceptually,
        // you specify 9/23/2010 @ 1pm, wakeup)
        time_spec.tv_sec = curr_timeval.tv_sec;
        time_spec.tv_nsec = curr_timeval.tv_usec * 1000;

        // Check for the next event we should handle
        expiring_timeval = sender_get_next_expiring_timeval(sender);

        // Perform full on timeout
        if (expiring_timeval == NULL) {
            time_spec.tv_sec += WAIT_SEC_TIME;
            time_spec.tv_nsec += WAIT_USEC_TIME * 1000;
        } else {
            // Take the difference between the next event and the current time
            sleep_usec_time = timeval_usecdiff(&curr_timeval, expiring_timeval);

            // Sleep if the difference is positive
            if (sleep_usec_time > 0) {
                sleep_sec_time = sleep_usec_time / 1000000;
                sleep_usec_time = sleep_usec_time % 1000000;
                time_spec.tv_sec += sleep_sec_time;
                time_spec.tv_nsec += sleep_usec_time * 1000;
            }
        }

        // Check to make sure we didn't "overflow" the nanosecond field
        if (time_spec.tv_nsec >= 1000000000) {
            time_spec.tv_sec++;
            time_spec.tv_nsec -= 1000000000;
        }

        //*****************************************************************************************
        // NOTE: Anything that involves dequeing from the input frames or input
        // commands should go
        //      between the mutex lock and unlock, because other threads
        //      CAN/WILL access these structures
        //*****************************************************************************************
        pthread_mutex_lock(&sender->buffer_mutex);

        // Check whether anything has arrived
        int input_cmd_length = ll_get_length(sender->input_cmdlist_head);
        int inframe_queue_length = ll_get_length(sender->input_framelist_head);

        // Nothing (cmd nor incoming frame) has arrived, so do a timed wait on
        // the sender's condition variable (releases lock) A signal on the
        // condition variable will wakeup the thread and reaquire the lock
        if (input_cmd_length == 0 && inframe_queue_length == 0) {
            pthread_cond_timedwait(&sender->buffer_cv, &sender->buffer_mutex,
                                   &time_spec);
        }
        // Implement this
        handle_incoming_acks(sender, &outgoing_frames_head);

        // Implement this
        handle_input_cmds(sender, &outgoing_frames_head);

        pthread_mutex_unlock(&sender->buffer_mutex);

        // Implement this
        handle_timedout_frames(sender, &outgoing_frames_head);

        // CHANGE THIS AT YOUR OWN RISK!
        // Send out all the frames
        int ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);

        while (ll_outgoing_frame_length > 0) {
            LLnode* ll_outframe_node = ll_pop_node(&outgoing_frames_head);
            char* char_buf = (char*) ll_outframe_node->value;

            // Don't worry about freeing the char_buf, the following function
            // does that
            send_msg_to_receivers(char_buf);

            // Free up the ll_outframe_node
            free(ll_outframe_node);

            ll_outgoing_frame_length = ll_get_length(outgoing_frames_head);
        }
    }
    pthread_exit(NULL);
    return 0;
}

// msg 0 0 0123456789 0 0123456789 1 0123456789 2 0123456789 3 0123456789 4 0123456789 5 0123456789 6 0123456789 7 0123456789 8 0123456789 9 0123456789 10 0123456789 11 0123456789 12 0123456789 13 0123456789 14 0123456789 15 0123456789 16 0123456789 17 0123456789 18 0123456789 19 0123456789 20 0123456789 21 0123456789 22 0123456789 23 0123456789 24 0123456789 25 0123456789 26 0123456789 27 0123456789 28 0123456789 29 0123456789 30 0123456789 31 0123456789 32 0123456789 33 0123456789 34 0123456789 35 0123456789 36 0123456789 37 0123456789 38 0123456789 39 0123456789 40

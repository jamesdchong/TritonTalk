# TritonTalk

TritonTalk is a networked communication program that enables message exchange between multiple clients in a real-time environment. It employs a client-server architecture, with separate modules handling distinct aspects of communication, including sending, receiving, and processing messages.

## Project Overview

TritonTalk is designed to facilitate real-time message exchange between multiple clients using a client-server model. The project includes various modules to handle different aspects of communication, such as sending and receiving messages, processing inputs, and managing communication protocols.

## Usage

- Starting the Server: The server listens for incoming client connections and manages message exchanges. Run the server executable to start it.

- Connecting Clients: Clients connect to the server and can send or receive messages. Use the client executable with the server's IP address and port number.

- Sending Messages: Once connected, clients can send messages to other clients through the server. Messages are handled in real-time.

- Receiving Messages: Clients receive messages from other connected clients, which are processed and displayed as per the application's functionality.

## File Descriptions
- common.h: Header file defining common data structures and constants used across modules.
- communicate.c: Contains functions related to network communication, including sending and receiving - - messages.
- input.c: Handles user input processing.
- main.c: Entry point of the application; initializes the server or client based on command-line arguments.
- receiver.c: Implements functions for receiving messages from clients.
- sender.c: Implements functions for sending messages to clients.
- util.c: Contains utility functions used across the project.

## Contact

For any questions or support, please contact:
- Name: James Chong
- Email: jamesdchong@gmail.com
- GitHub: https://github.com/jamesdchong

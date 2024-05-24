
# ERO: Escape Room Online

This project tries to emulate the experience of a fantasy dungeon themed Escape Room through a Client-Server application. The goal of the game is to collect three items and use them to exit the virtual room.

## Installation and Run Locally
1. Clone the repository:
    ```sh
    git clone https://github.com/jonnyfratta/Socket-based_Escape_Room.git
    ```

2. Open a terminal on your Linux Os, enter the project's directory and execute:
    ```sh
    make all
    ```
        

## Usage/Examples

### Client

#### First Phase: Initial Screen

Upon startup, the client is prompted to choose one of the available scenarios (only one in this case).

#### Second Phase: Access

The next step involves registration or login. No other actions can be performed until an account is accessed. If registration is chosen, the login to the newly created account happens automatically.

#### Third Phase: Commands

From this point on, commands can be executed (the list of possible commands is not immediately shown on the screen but can be viewed by typing 'options'). Command recognition is entirely handled by the client, which then simply informs the server.

### Server

#### First Phase: Initial Screen

At startup, the server requests a start or stop command. The "start" command initiates the server by exiting the loop within the 'start()' function and beginning to accept requests from various clients. The "stop" command, on the other hand, terminates the server with an exit.

#### Second Phase: Request Management

At this point, the server waits for client connections (up to 5 simultaneously), handling their requests. At the end of each execution, it calculates the elapsed time, prints it, and if time has expired, sets the outcome and sends it to the client.

#### Third Phase: Player Termination

This phase may occur due to time expiration, in-game objects or dynamics, or player abandonment. In such cases, the server removes the involved player from the game and checks if they were the last. Upon a positive outcome, it terminates with an exit().

### Victory and Defeat

Victory can be achieved in two ways: by obtaining the three token objects and using them in the correct order once the door is unlocked, or by finding one of the two Easter eggs and then using it once the door is open.

As for defeat, it can occur due to the use of the second Easter egg, the use of offerHelp (sacrifice), voluntary abandonment via end, or due to the expiration of time.


## Development
I developed this project and the corresponding makefile in Linux OS so if you try to run it in a different OS it probably give you errors. It's the result of a project assignment I had to do for my "Reti Informatiche" course at my bachelor's degree in Computer Engineering

From an implementation standpoint, I used I/O multiplexing to manage the sockets of the various players participating simultaneously in the same game. Data exchange occurs through the use of send and receive primitives with blocking sockets. I made this decision because I consider I/O multiplexing to be more intuitive and believe that the workload is not so high as to make this approach disadvantageous compared to a concurrent server implementation. Nonetheless, I acknowledge that this approach is more demanding due to the need to actively monitor the various sets, but I believe this is a negligible drawback under these conditions. Regarding the blocking sockets, the choice was made because all the functions in my code would not have other operations to perform in the absence of the exchanged data.

I always used buffers with a maximum size set to 1024, but at the moment of data exchange, I used double send and double receive (one for size and one for buffer) to send only the strictly necessary amount of data. Any variable that needed to contain the size of a buffer and thus be exchanged between client and server was allocated on 16 bits (u_int16_t or int16_t) rather than on 32 bits if I had made them all int. This decision was also made with the aim of sending the smallest possible data sizes.

In some functions, I tried to avoid unnecessary exchanges between client and server, which is why, when possible, I assigned particular values (usually <= 0) to the length of the buffer to be sent (since it is the first send/recv of the pair) that are then interpreted by the receiving counterpart.

The outcome of the game was entrusted to a variable 'outcome' that is modified by the server depending on what happens in the game. This variable is checked at the end of the execution of each command, and based on its value, the scenario that led to the possible termination is discriminated.

Finally, I tried to shift as much computational load as possible towards the client, so that the latter would only need to request data from the server that it could not otherwise access.

### Optional Features

The feature I chose to implement is based on the fantasy setting of my scenario. It involves a player who needs help solving a puzzle being able to write its text in a shared space (global variable) using the askForHelp function, and another player being able to decide to sacrifice themselves, through the offerHelp function, to provide the solution to the player in need.

AskForHelp: If there is no help request awaiting response when called, the help request is loaded via the 'help' command. Upon invoking the command again after a certain interval, the function checks if anyone has responded to the question, and if so, returns the answer.

OfferHelp: If no player has requested help when called, it returns; otherwise, it prompts the current player to provide the answer and then removes them.

### Time Management:

In my implementation, time management is separated between the client and server to reduce the number of communications between them. For example, if only the server were to manage time, the client would need to request the remaining time every time it needed to display it. Time is initialized when the first player logs in, and from that moment on, management is separate.

The only additional transmission occurs when subsequent players log in, where the client requests the current remaining time from the server. The time managed by the client is important for display purposes and for verification, but in reality, it is the server's time that is essential. The server checks it to set 'outcome' in case the time has expired. The outcome is then transmitted to the client, which, upon verifying that its timer has also expired, terminates the game.



## Authors

- [@jonnyfratta](https://github.com/jonnyfratta)
- structure collaborator: [@msegreto](https://github.com/msegreto)

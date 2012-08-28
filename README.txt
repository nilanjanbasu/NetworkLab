usage:
./network inputfile outputfile {probability}

Problem Statement:
Simulation Stop and Wait Protocol at Data Link Layer

Write a program to transmit the contents of an ASCII data file from a transmitter process to a receiver
process communicating through pipes. The transmitter reads data from the file, creates the data frame,
calculates the checksum, applies a "damage" function, and then calls the receiver. The receiver checks the
checksum, writes the correct frame to out file, and returns the acknowledgement. Use the simplex stop
and wait protocol with positive acknowledgement and retransmission (PAR) (you may see pp 197-202 of
Tanenbaum's Computer Networks (third edition) or consult any other book).

The program should read the input stream and split into frames of eight bytes. If the total length is not a
multiple of eight, pad the last frame with 1s. If the total length is multiple of eight, make the last data frame
all 1s. This should be recognized at the receiving side as the end of file. Each frame will also have a 16 bit
checksum attached, using CRC coding. Also add a one-byte field to the frame structure containing one-
bit sequence number that flips between 0 and 1 for the frame sequence. The frame will first be passed to
a "damage" function, which occasionally damage the frame (this simulates Transmission errors) and then
transmitted to the receiver. The sender then waits for ACK or NAK from the receiver. ACK means, "the
frame was received OK, so send the next". NAK means "The frame was damaged, please send again".
ACK/NAKS are never damaged or lost.

Frames will be transmitted one by one according to protocol. The receiver will read each frame. It must
first check for sequence number and checksum errors. If no error occurs, it will return an ACK to the
sender. Otherwise, it will return a NAK retransmission. Use any values to represent ACK/NAK. It is the
receiver's responsibility to reassemble frames into output stream. The receiver will reconstruct the data
streams one by one according to the protocol and write it to the file.

There is a "damage" function which uses a random-number generator to determine whether to damage a
frame, and, if so how many and which bits to flip. The probability that a given frame will be damaged (d)
will be entered by the user at runtime. If the frame is to be damaged, the probability of flipping one bit is
0.5, the probability of flipping two bits is 0.3, and the probability of flipping three bits is 0.2. Every bit in
the frame is equally likely to be damaged.

Include trace option for both the sender and the receiver. If the trace is on, the sender will output the
message:

Frame: xxx transmitted --yyy

Where xxx is the frame number (just maintain a counter of frames successfully transmitted), and yyy is the
string either "intact" or "damaged" depending on whether the gremlin damages it or not. The receiver will
likewise output the message:

Frame: xxx received --yyy
with similar meaning. The trace option (on or off) is specified at run time.

PROGRAM INPUT FROM USER:
1. Name of the input file.
2. Name of the output file.
3. Probability that a frame will be damaged
4. Whether or not frame tracing should be enabled

PROGRAM OUTPUT SCREEN (AFTER TRANSMISSION COMPLETE)
1. Total number of packets read from sender's network layer (the data file).
2. Total number of frames transmitted (correct plus damaged).
3. Total number of frames damaged.
4. Maximum number of retransmissions for any single frame.
5. Total number of packets delivered to receiver's network layer (output file).
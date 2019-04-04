
THREE STRUCTS:

    EVM:Has idx of EVM,thread id of thread corresponding to that evm,corresponding booth,corresponding no of slots and a boolean flag for its working(1 -> working).
    VOTER: Has idx of Voter,thread id of thread corresponding to that voter,corresponding booth,corresponding evm ans status(new,waiting,assigned or completed).
    BOOTH: Has idx of Booth,thread id of thread corresponding to that booth,no of voters, no of slots, no voters who are done, all the voters, mutex to assign voter and corresponding conditions.

MAIN FLOW:
It initializes,creates thread and joins Booth threads.

BOOTH THREAD:
It initializes,creates thread and joins EVM threads.
It initializes,creates thread and joins Voter threads.

VOTER THREAD:
Voter waits for a EVM slot to be free for it.
On gaining it it votes.
The it signals that is is free so someone else can occupy it.

EVM THREAD:
EVMs waits for booth to start.
Then slots are free and waits for voter to take it.
If voter is done it declares itself to be free.
If all the voters are done then its shuts down.

TO run:
	gcc -pthread gopichand.c
	./a.out
	Enter the no. of refree


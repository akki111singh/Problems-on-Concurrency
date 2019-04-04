STRUCTS:
    REFREE:Has idx of REFREE,thread id of thread corresponding to that REFREE,corresponding ACADEMY,corresponding no of slots and a boolean flag for its working(1 -> working).
    PLAYER: Has idx of PLAYER,thread id of thread corresponding to that PLAYER,corresponding ACADEMY,corresponding refree and status(new,waiting,assigned or completed).
    ACADEMY: Has idx of Academy,thread id of thread corresponding to that academy,no of players, max_slots, number of players who are done, all the players, mutex to assign player and corresponding conditions.
MAIN FLOW:
    It initializes,creates thread and joins ACADEMY threads.
ACADEMY THREAD:
    It initializes,creates thread and joins REFREE threads.
    It initializes,creates thread and joins PLAYERS threads.

PLAYER THREAD:
    PLAYER waits for a REFREE slot to be free for it.
    On gaining it it MET with organiser.
    
REFREE THREAD:
    REFREE waits for 2  players .
    If he has done with match hen declares himself to be free.

To run:
	gcc -pthread file_name.c
	./a.out
	Enter the no of booth,no of evm,max_slotsOFevm

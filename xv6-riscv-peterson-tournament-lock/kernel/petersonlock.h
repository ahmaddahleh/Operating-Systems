// the petersonlock for synchronization and mutual exclusion 
struct petersonlock
{
    uint used;       // is the lock is being used ?
    uint flag[2];    // flag[0] = does process 0 want to enter?
                    // flag[1] = does process 1 want to enter?

    uint turn;       // whose turn is it (0 or 1)?=
};

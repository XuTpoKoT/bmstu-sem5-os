struct QUEUE
{
    int num;
    int pid;
    int result;
};

program RPC_PROG
{
    version RPC_VER
    {
        struct QUEUE GET_NUMBER() = 1;
        struct QUEUE WAIT_RESULT(struct QUEUE) = 2;
    } = 1;
} = 0x20000021;


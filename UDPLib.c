#include "UDPLib.h"

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void handleSigInt(int sig)
{
    if (sig != SIGINT)
    {
        return;
    }
    printf("\n\nFINISH USING SIGINT\n\n");
    closeAll();
    exit(0);
}
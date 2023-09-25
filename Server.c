#include <pthread.h>   /* for POSIX threads */
#include <semaphore.h> /* for unnamed "mutexes" */
#include <signal.h>    /* for handling SIGINT */
#include "UDPLib.h"

#define MAX_BOLTUNS 5

void printCallsInfo(); /* Print status info for logging */

struct call calls[MAX_BOLTUNS];
int calls_count = MAX_BOLTUNS, complete_count = 5;
int sock; /* Socket descriptor for server */

void closeAll()
{
    printf("\n\nFINISH USING SIGINT\n\n");
    printCallsInfo();
    close(sock);
}

void initPulls()
{
    // initialize calls
    for (int i = 0; i < calls_count; ++i)
    {
        struct call call = {.id = i, .caller_id = -1, .receiver_id = -1};
        calls[i] = call;
    }
}

//////  TASK LOGIC  //////

void printCallsInfo()
{
    for (int j = 0; j < calls_count; ++j)
    {
        printf("call with id = %d must be called by boltun #%d, must be received by boltun %d\n", calls[j].id, calls[j].caller_id, calls[j].receiver_id);
    }
}

void findCallers(struct response *response, struct call *call, int requestAuthor)
{
    calls[call->id] = *call;
    response->call = *call;
    response->response_code = NO_CALL;
    for (int i = 0; i < calls_count; ++i)
    {
        if (i != call->id)
        {
            if (calls[i].receiver_id == requestAuthor)
            {
                printf("Boltun #%d received call from boltun %d\n", requestAuthor, i);
                response->response_code = CALL_RECEIVED;
                response->call = calls[i];
                response->call.receiver_id = requestAuthor;
                response->call.caller_id = i;
            }
        }
    }
}

void findWaiters(struct response *response, struct call *call, int requestAuthor)
{
    calls[call->id] = *call;
    response->call = *call;
    response->response_code = NO_ANSWER;

    for (int i = 0; i < calls_count; ++i)
    {
        if (i != call->id)
        {
            if (call->receiver_id == calls[i].receiver_id && calls[i].caller_id == -1)
            {
                printf("Boltun #%d accepted call from boltun %d\n", i + 1, requestAuthor + 1);
                calls[i].caller_id = requestAuthor;
                // response->call = *call;
                response->response_code = CALL_ACCEPTED;
            }
            if (call->receiver_id == calls[i].caller_id && calls[i].receiver_id == call->caller_id)
            {
                printf("Boltun #%d accepted call from boltun %d\n", i + 1, requestAuthor + 1);
                // response->call = calls[i];
                response->response_code = CALL_ACCEPTED;
            }
            // add when its just waits;
        }
    }
}

void endCall(struct response *response, struct call *call, int requestAuthor)
{
    calls[call->id] = *call;
    response->response_code = END;
}

//////  END OF TASK LOGIC  //////

int main(int argc, char *argv[])
{
    (void)signal(SIGINT, handleSigInt);

    struct sockaddr_in servaddr, cliaddr;
    unsigned short port;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <Broadcast Port> [Tasks count]\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);

    // create a datagram socket using UDP
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1)
    {
        DieWithError("socket() failed");
    }

    // setup local address structure
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // bind to the broadcast port
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        DieWithError("bind() failed");
    }

    struct request request;
    struct response response;

    initPulls();
    printf("pulls have been initialized\n");

    while (complete_count == calls_count)
    {
        printf("Server listening...\n");

        // receive request from client
        socklen_t len = sizeof(cliaddr);
        recvfrom(sock, &request, sizeof(struct request), 0, (struct sockaddr *)&cliaddr, &len);

        printf("Request code: %d\n", request.request_code);
        printf("Boltun NUM: %d\n", request.boltun_id);
        printf("\n");

        int boltun_id = request.boltun_id;

        int requestAuthor = request.boltun_id;
        struct call call = request.call;

        switch (request.request_code)
        {
        case 0: // wait for calls
            findCallers(&response, &call, requestAuthor);
            break;
        case 1: // make call
            findWaiters(&response, &call, requestAuthor);
            break;
        case 2: // end call
            endCall(&response, &call, requestAuthor);
            break;
        default: // ub
            break;
        }

        printf("\n");

        // send response to client
        sendto(sock, &response, sizeof(struct response), 0, (struct sockaddr *)&cliaddr, len);
    }

    // finish socket work
    closeAll();
    return 0;
}
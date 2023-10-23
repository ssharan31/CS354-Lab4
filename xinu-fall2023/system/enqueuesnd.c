#include <xinu.h>

int32 enqueuesndb(pid32 preceiver, pid32 psender, umsg32 pmessage) {
    intmask mask = disable();  // Disable interrupts

    struct procent *recvptr = &proctab[preceiver];  // Pointer to receiver's proc entry

    // Allocate memory for a new blockedsenders struct
    // Will also be the node we enqueue
    struct blockedsenders *newNode = (struct blockedsenders *)getmem(sizeof(struct blockedsenders));
    if (newNode == NULL) {
        restore(mask);
        return -1;  // Return -1 if memory allocation fails
    }

    // Populate the new node
    newNode->senderpid = psender;
    newNode->sendermsg = pmessage;
    newNode->next = NULL;

    // Insert the new node into the FIFO queue
    if (recvptr->prsendbqueue1 == NULL) {  // If the queue is empty
        recvptr->prsendbqueue1 = newNode;
        recvptr->prsendbqueue2 = newNode;
    } else {  // If the queue is not empty
        recvptr->prsendbqueue2->next = newNode;
        recvptr->prsendbqueue2 = newNode;
    }

    restore(mask);  // Restore interrupts
    return 0;  // Return 0 upon success
}

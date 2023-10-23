/* send.c - send */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  send  -  Pass a message to a process and start recipient if waiting
 *------------------------------------------------------------------------
 */
syscall	sendb(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg		/* Contents of message		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid]; //Getting process from the process table

	/* Check for existing message by checking if the reciever's  message buffer is full*/
	if (prptr->prhasmsg) {
		// The reciever's buffer is full so we block the sender
		if (enqueuesndb(pid, currpid, msg) == -1) {
            restore(mask);
            return SYSERR;  // enqueuesndb failed
        }
		proctab[currpid].prstate = PR_SNDB;  // Set the sender's state to blocked
        resched();  // Reschedule the processes
		//restore(mask);
		//return SYSERR;
	}
	/* If the buffer is not full or once it becomes empty, send the message*/
	prptr->prmsg = msg;   /* Deliver message		*/
	prptr->prhasmsg = TRUE;		/* Indicate message is waiting	*/

	/* Unblock reciever if waiting*/
	/* If recipient waiting or in timed-wait make it ready */

	if (prptr->prstate == PR_RECV) {
		ready(pid);
	} else if (prptr->prstate == PR_RECTIM) {
		unsleep(pid);
		ready(pid);
	}
	restore(mask);		/* Restore interrupts */
	return OK;
}

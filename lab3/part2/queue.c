//queue.c//
int enqueue(PROC **queue, PROC *p)
{
    if ((*queue) == 0)
    {
        *queue = p;
        (*queue)->next = 0;
    }
    else if (p->priority > (*queue)->priority)
    {
        p->next = (*queue);
        (*queue) = p;
    }
    else
    {
        while ((*queue)->next != 0 && p->priority <= (*queue)->next->priority)
        {
		(*queue) = (*queue)->next;
        }
        
        p->next = (*queue)->next;
        (*queue)->next = p;
    }
}

PROC *dequeue(PROC **queue)
{
    PROC *p = *queue;
    if (*queue != 0) *queue = (*queue)->next;
    return p;
}

int printList(char name, PROC *queue)
{
	printf("%s = ", name);
	while(queue)
	{
		printf("[%d %d]->", queue->pid, queue->priority);
		queue = queue->next;
	}
	printf("Null\n\n");
}

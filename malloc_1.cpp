#include <unistd.h>
#define MAXLIMIT 100000000
#define ERROR -1

void* smalloc(size_t size)
{
    if(!size || size > MAXLIMIT)
    {
        return NULL;
    }
    //This is literally just going tobe using the sbrk to allocate some extra memory:
    //THis doesn't have a pool or anytthing..!!!

    void* allocatedBuffer = sbrk(size);
    if(allocatedBuffer != (void *) ERROR) // Taken from man 2 
    {
        return allocatedBuffer;
    }
    return NULL;
}
//Done 

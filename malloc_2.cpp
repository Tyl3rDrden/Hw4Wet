#include <unistd.h>
#include <assert.h>
#include <cstring>
#define MAXLIMIT 100000000
#define ERROR -1
#define SIZEMETADATA sizeof(memoryBlock)   //Size of the metadata

//I'll provide the H file later

class memoryBlock
{
    private:
        static int numFreeBlocks; 
        static int numFreeBytes;
        static int numAllocatedBlocks;
        static int numAllocatedBytes;
        //SizeMetaData ISCONSTANT AND DEFINED IN A MACRO



        static memoryBlock* firstElement; //Points to the first elment in the linkedList
        static memoryBlock* lastElement; //Points to the last element in the linkedList
        memoryBlock* next;
        memoryBlock* prev;
        size_t size;
        bool isFree;

        // These functions are not part of the interface 
        static memoryBlock* getFirstMemoryBlock()
        {
            return firstElement;
        }
        static memoryBlock* allocateBlock(size_t size)
        {
            memoryBlock* newBlock = (memoryBlock*)sbrk(size * sizeof(memoryBlock));

            numAllocatedBytes += size;
            numAllocatedBlocks++;

            //Stats added ++++++++++++++++++++++++++Jonathan!


            if(newBlock == (void*)ERROR)
            {
                return NULL;
            }
            newBlock->size = size;
            newBlock->isFree = false;


            //First element Allocation
            if(!firstElement)
            {
                firstElement = newBlock;
                lastElement = newBlock;
                newBlock->next = NULL;
                newBlock->prev = NULL;
                return newBlock;
            }
            
            newBlock->next = NULL;
            //End of the linked List Becasue it is allocated Linearly

            //Update the last element
            lastElement->next = newBlock;
            //Update the new block to point to the last element int the linked list
            newBlock->prev = lastElement;
            return newBlock;

        }
        //These are private because thwe blocks are integrated with class inside
        static memoryBlock* getFreeBlock(size_t size)
        {
            //I assume the size is valid
            memoryBlock* freeBlock = findFreeBlock(size);
            if(freeBlock)
            {
                numFreeBlocks--;
                numFreeBytes -= freeBlock->size;
                //Stats record ++++++++++++++++++++++++++Jonathan!
                freeBlock->isFree = false;
                return freeBlock;
            }
            //No block found now we need to use srbk to allocate a ne block
            return allocateBlock(size);
        }
        static memoryBlock* findFreeBlock(size_t size)
        {
            memoryBlock* currentBlock = getFirstMemoryBlock();
            while(currentBlock)
            {
                if(currentBlock->isFree && currentBlock->size >= size)
                {
                    return currentBlock;
                }
                currentBlock = currentBlock->next;
            }
            //No free block found
            return NULL;
        }
        void* getBuffer()
        {
            return this + 1;
        }

    public: 
        void freeBlock()
        {
            this->isFree = true;

            //Stats record ++++++++++++++++++++++++++Jonathan!
            numFreeBlocks++;
            numFreeBytes += this->size;
        }
        static void* getMemoryBuffer(size_t size)
        {
            memoryBlock* result = getFreeBlock(size);
            return result->getBuffer();
            //result->size = size;
        }
        int getSize()
        {
            return size;
        }
        bool getIsFree()
        {
            return isFree;
        }
        
};


//Initializing the static global variables
memoryBlock* memoryBlock::firstElement = NULL;
memoryBlock* memoryBlock::lastElement = NULL;
static int numFreeBlocks = 0; 
static int numFreeBytes = 0;
static int numAllocatedBlocks = 0;
static int numAllocatedBytes = 0;
static int metaDataBytes = 0;


void* smalloc(size_t size)
{
    if(!size || size > MAXLIMIT)
    {
        return NULL;
    }
    return memoryBlock::getMemoryBuffer(size);
}


void* scalloc(size_t num, size_t size)
{
    if(!size || size > MAXLIMIT)
    {
        return NULL;
    }

    //Need to check num as well TODO

    size_t totalSize = num * size;
    void* result = smalloc(totalSize);
    if(result)
    {
        std::memset(result, 0, totalSize);
    }
    return result;
}


void sfree(void* p)
{
    //No need to check for null
    //See part 4 of the question
    memoryBlock* block = (memoryBlock*)((char*)p - sizeof(memoryBlock));
    //Again copiliot literally took all the fun away
    block->freeBlock();
}

void* srealloc(void* oldp, size_t size)
{
    //No need to check Null popinter. 

    //See part 4 for more details


    memoryBlock* block = (memoryBlock*)((char*)oldp - sizeof(memoryBlock));
    if(block->getSize() >= size)
    {
        return oldp;
    }
    //WE need to allocate a new block
    void* newBlock = smalloc(size);
    if(newBlock)
    {
        //I have to use std::memmove because the memory might overlap
        std::memmove(newBlock, oldp, block->getSize());
        sfree(oldp);
    }
}
size_t _num_free_blocks()
{
    return numFreeBlocks;
}
size_t _num_free_bytes()
{
    return numFreeBytes;
}
size_t _num_allocated_blocks()
{
    return numAllocatedBlocks;
}
size_t _num_allocated_bytes()
{
    return numAllocatedBytes;
}
size_t _size_meta_data()
{
    return SIZEMETADATA;
}
size_t _num_meta_data_bytes()
{
    return numAllocatedBlocks * SIZEMETADATA;
}



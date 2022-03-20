#include "constants_libraries.h"

struct MemoryBlock
{
    byte* heapPointer; // Start of block in HEAP!
    uint size; // All size!
};

class MemoryListForBorderDescriptorDeallocer
{
    friend class MemoryPool;

    private:
        byte* head = nullptr;
        uint size = 0;
    
    public:
        void initList(byte* head)
        {
            this->head = head;
            size = 1;
        }
    
    public:
        void deleteBlockFromList(byte* blockPtrToDelete)
        {
            if (size == 0)
                throw std::overflow_error("List is empty! Can't do delete!");
            
            byte* nextBlock  = returnLinkOnNextElement(blockPtrToDelete);
            byte* previousBlock = returnLinkOnPreviousElement(blockPtrToDelete);

            unitePtrsOfTwoBlocks(previousBlock, nextBlock);

            if (blockPtrToDelete == head)
            {
                nextBlock == nullptr ? head = previousBlock : head = nextBlock;
            }
            size--;
        }
    
    public:
        void initHeadOfList()
        {
            setFirstSignOfElement(head, false);

            setFirstSizeOfElement(head, HEAP_SIZE - SIGN_SIZE*2 - HEADER*2 - LINK_SIZE*2);

            setLinkOnNextElement(head, nullptr);

            setLinkOnPreviousElement(head, nullptr);

            setSecondSizeOfElement(head, HEAP_SIZE - SIGN_SIZE*2 - HEADER*2 - LINK_SIZE*2);

            setSecondSignOfElement(head, false);
        }

    public:
        void unitePtrsOfTwoBlocks(byte* previousBlockPtr, byte* nextBlockPtr)
        {
            if (previousBlockPtr != nullptr)
            {
                setLinkOnNextElement(previousBlockPtr, nextBlockPtr);
            }
            if (nextBlockPtr != nullptr)
            {
                setLinkOnPreviousElement(nextBlockPtr, previousBlockPtr);
            }
        }
    
    public:
        void addBlockToList(byte* newBlockPtr)
        {
            setFirstSignOfElement(newBlockPtr, false);
            setSecondSignOfElement(newBlockPtr, false);
            setSecondSizeOfElement(newBlockPtr, returnFirstSizeOfElement(newBlockPtr));

            if (head == nullptr)
            {
                setLinkOnNextElement(newBlockPtr, nullptr);
                setLinkOnNextElement(newBlockPtr, nullptr);
                size = 1;
                head = newBlockPtr;
                return;
            }

            if (newBlockPtr < head)
            {
                byte* previousElementOFHead = returnLinkOnPreviousElement(head);
                
                setLinkOnPreviousElement(newBlockPtr, previousElementOFHead);
                setLinkOnNextElement(newBlockPtr, head);

                setLinkOnPreviousElement(head, newBlockPtr);
                setLinkOnNextElement(previousElementOFHead, newBlockPtr);

                head = newBlockPtr;
            }
            else // newBlockPtr > head
            {
                byte* nextElementOfHead = returnLinkOnNextElement(head);

                setLinkOnPreviousElement(newBlockPtr, head);
                setLinkOnNextElement(newBlockPtr, nextElementOfHead);

                setLinkOnPreviousElement(nextElementOfHead, newBlockPtr);
                setLinkOnNextElement(head, newBlockPtr);
            }

            size++;
        }

    public:
        byte* findNeededBlock(uint sizeOfBlockData)
        {
            byte* ptr = head;
            while (ptr)
            {
                if (returnFirstSizeOfElement(ptr) >= sizeOfBlockData)
                {
                    return ptr;
                }
                ptr = returnLinkOnNextElement(ptr);
            }
            return nullptr;
        }
    
    public:
        void moveStartPtrOfMemoryBlock(byte* blockPtr, uint bytesToMove)
        {
            uint dataSizeOfBlock = returnFirstSizeOfElement(blockPtr);
            uint newDataSizeOfBlock = dataSizeOfBlock - bytesToMove - 2*HEADER - 2*SIGN_SIZE - 2*LINK_SIZE;

            byte* nextBlock = returnLinkOnNextElement(blockPtr);
            byte* previousBlock = returnLinkOnPreviousElement(blockPtr);

            uint toMove = bytesToMove + 2*HEADER + 2*SIGN_SIZE + 2*LINK_SIZE;
            byte* newPtrOfBlock = blockPtr + toMove;

            // change block  
            setFirstSignOfElement(newPtrOfBlock, false);
            setFirstSizeOfElement(newPtrOfBlock, newDataSizeOfBlock);
            setSecondSignOfElement(newPtrOfBlock, false);
            setSecondSizeOfElement(newPtrOfBlock, newDataSizeOfBlock);
            setLinkOnNextElement(newPtrOfBlock, nextBlock);
            setLinkOnPreviousElement(newPtrOfBlock, previousBlock);

            // change nearby blocks
            setLinkOnNextElement(previousBlock, newPtrOfBlock);
            setLinkOnPreviousElement(nextBlock, newPtrOfBlock);
            
            // change head
            if (blockPtr == head)
                head = newPtrOfBlock;
        }
    
    public:
        void setFirstSignOfElement(byte* startPtrOfBlock, bool signOfBlock)
        {
            sign* sign1 = (sign*)(startPtrOfBlock);
            *sign1 = signOfBlock;
        }
        void setSecondSignOfElement(byte* startPtrOfBlock, bool signOfBlock)
        {
            uint sizeOfData = *(uint*)(startPtrOfBlock + SIGN_SIZE);
            sign* sign2 = (sign*)(startPtrOfBlock + SIGN_SIZE + HEADER + 2*LINK_SIZE + sizeOfData + HEADER);
            *sign2 = signOfBlock;
        }
        void setFirstSizeOfElement(byte* startPtrOfBlock, uint sizeOfBlock)
        {
            uint* size1 = (uint*)(startPtrOfBlock + SIGN_SIZE);
            *size1 = sizeOfBlock;
        }
        void setSecondSizeOfElement(byte* startPtrOfBlock, uint sizeOfBlock)
        {
            uint sizeOfData = *(uint*)(startPtrOfBlock + SIGN_SIZE);
            uint* size2 = (uint*)(startPtrOfBlock + SIGN_SIZE + HEADER + 2*LINK_SIZE + sizeOfData);
            *size2 = sizeOfBlock;
        }
        void setLinkOnNextElement(byte* startPtrOfBlock, link nextBlock)
        {
            if (startPtrOfBlock == nullptr)
                return;

            link* linkOnNextBlock = (link*)(startPtrOfBlock + SIGN_SIZE + HEADER);
            *linkOnNextBlock = nextBlock;
        }
        void setLinkOnPreviousElement(byte* startPtrOfBlock, link previousBlock)
        {
            if (startPtrOfBlock == nullptr)
                return;

            link* linkOnPreviousBlock = (link*)(startPtrOfBlock + SIGN_SIZE + HEADER + LINK_SIZE);
            *linkOnPreviousBlock = previousBlock;
        }
        bool returnFirstSignOfElement(byte* startPtrOfBlock)
        {
            return *(sign*)(startPtrOfBlock);
        }
        bool returnSecondSignOfElement(byte *startPtrOfBlock)
        {
            uint sizeOfBlock = *(uint*)(startPtrOfBlock + SIGN_SIZE);
            return *(sign*)(startPtrOfBlock + SIGN_SIZE + HEADER + 2*LINK_SIZE + sizeOfBlock + HEADER);
        }
        uint returnFirstSizeOfElement(byte* startPtrOfBlock)
        {
            return *(uint*)(startPtrOfBlock + SIGN_SIZE);
        }
        uint returnSecondSizeOfElement(byte* startPtrOfBlock)
        {
            uint sizeOfBlock = *(uint*)(startPtrOfBlock + SIGN_SIZE);
            return *(uint*)(startPtrOfBlock + SIGN_SIZE + HEADER + 2*LINK_SIZE + sizeOfBlock);
        }
        byte* returnLinkOnNextElement(byte* startPtrOfBlock)
        {
            return *(link*)(startPtrOfBlock + SIGN_SIZE + HEADER);
        }
        byte* returnLinkOnPreviousElement(byte* startPtrOfBlock)
        {
            return *(link*)(startPtrOfBlock + SIGN_SIZE + HEADER + LINK_SIZE);
        }
        void printElement(byte* startPtrOfBlock)
        {
            std::cout << returnFirstSignOfElement(startPtrOfBlock) << std::endl;
            std::cout << returnFirstSizeOfElement(startPtrOfBlock) << std::endl;
            std::cout << returnSecondSignOfElement(startPtrOfBlock) << std::endl;
        }
};

class MemoryPool
{
    friend class Allocer;
    friend class Deallocer;

    private:
        byte Heap[HEAP_SIZE];
        MemoryListForBorderDescriptorDeallocer* listForBorderDescriptor = nullptr;

    public: 
        MemoryPool(MemoryListForBorderDescriptorDeallocer* listForBorderDescriptor)
        {
            this->listForBorderDescriptor = listForBorderDescriptor;
            this->listForBorderDescriptor->initList(Heap);
            this->listForBorderDescriptor->initHeadOfList();
        }

    private:
        void initFirstMemoryBlockForSortedListBlockJoinner(MemoryBlock& firstMemoryBlock)
        {
            firstMemoryBlock.heapPointer = Heap;
            firstMemoryBlock.size = HEAP_SIZE;
        }
    
    public:
        MemoryPool(const MemoryPool& memoryPool) = delete;
        MemoryPool& operator= (const MemoryPool& memoryPool) = delete;
};
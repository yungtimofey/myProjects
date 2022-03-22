class Deallocer
{
    public:
        virtual void tryToFree(void*) const = 0;
        virtual ~Deallocer(){};
    
    protected:
        MemoryListForBorderDescriptorDeallocer* returnMemoryListForBorderDescriptorDeallocer(MemoryPool& _memoryPool)
        {
            return _memoryPool.listForBorderDescriptor;
        }
        byte* returnMinimalPointerInHeap(MemoryPool& _memoryPool)
        {
            return _memoryPool.Heap;
        }
};


class BorderDescriptorDeallocer : public Deallocer
{
    private:
        MemoryListForBorderDescriptorDeallocer* listForBorderDescriptor;
        byte *minimalPtr, *maximumPtr;
    
    public: 
        BorderDescriptorDeallocer(MemoryPool& _memoryPool);
    
    public:
        void tryToFree(void* userPtr) const override;
    private:
        byte* findNeededBlock(void* userPtr) const;
        bool thePreviousMemoryBlockIsTheLowerBound(byte* startPtrOfCurrentBlock) const;
        void unionMemoryBlockWithThePreviousBlock(byte* startPtrOfCurrentBlock) const;
        bool theNextMemoryBlockIsTheUpperBound(byte* startPtrOfCurrentBlock) const;
    private:
        byte* returnPtrOfLeftElementInHeap(byte* currentPtrOfElement) const;
        byte* returnPtrOfRightElementInHeap(byte* currentPtrOfElement) const;
        void unionTwoBlocks(byte* leftBlock, byte* rightBlock) const;
};


BorderDescriptorDeallocer::BorderDescriptorDeallocer(MemoryPool& _memoryPool)
{
    this->listForBorderDescriptor = returnMemoryListForBorderDescriptorDeallocer(_memoryPool);

    this->minimalPtr = returnMinimalPointerInHeap(_memoryPool);
    this->maximumPtr = minimalPtr + HEAP_SIZE-1; 
    // [minimalPtr, maximumPtr] - range of pointers
}


void BorderDescriptorDeallocer::tryToFree(void* userPtr) const
{
    byte* startBlockPtr = (byte*)userPtr - 2*LINK_SIZE - HEADER - SIGN_SIZE;

    if (thePreviousMemoryBlockIsTheLowerBound(startBlockPtr))
    {
        //std::cout << "need to union!" << std::endl;
        byte* previousElementInHeap = returnPtrOfLeftElementInHeap(startBlockPtr);
        listForBorderDescriptor->deleteBlockFromList(previousElementInHeap);
        unionTwoBlocks(previousElementInHeap, startBlockPtr);

        startBlockPtr = previousElementInHeap;
    }

    if (theNextMemoryBlockIsTheUpperBound(startBlockPtr))
    {
        //std::cout << "need to union!" << std::endl;
        byte* nextElementInHeap = returnPtrOfRightElementInHeap(startBlockPtr);
        listForBorderDescriptor->deleteBlockFromList(nextElementInHeap);
        unionTwoBlocks(startBlockPtr, nextElementInHeap);
    }

    listForBorderDescriptor->addBlockToList(startBlockPtr);
}

bool BorderDescriptorDeallocer::thePreviousMemoryBlockIsTheLowerBound(byte* startPtrOfCurrentBlock) const
{
    byte* previousElementSignPtr = startPtrOfCurrentBlock - SIGN_SIZE;
    if (previousElementSignPtr < minimalPtr)
    {
        //std::cout << "No previous element!" << std::endl;
        return false;
    }
    
    sign signOfPreviousElement = *(sign*)previousElementSignPtr;
    if (signOfPreviousElement)
    {
        //std::cout << "Previous element has +" << std::endl;
    }
    return (signOfPreviousElement == false);
}
bool BorderDescriptorDeallocer::theNextMemoryBlockIsTheUpperBound(byte* startPtrOfCurrentBlock) const
{
    byte* nextElementSignPtr = startPtrOfCurrentBlock + 2*SIGN_SIZE + 2*HEADER + 2*LINK_SIZE + 
        listForBorderDescriptor->returnFirstSizeOfElement(startPtrOfCurrentBlock);
    
    if (nextElementSignPtr > maximumPtr)
    {
        //std::cout << "No next element!" << std::endl;
        return false;
    }
    
    sign signOfNextBlock = *(sign*)(nextElementSignPtr);
    return signOfNextBlock == false;
}
void BorderDescriptorDeallocer::unionMemoryBlockWithThePreviousBlock(byte* startPtrOfCurrentBlock) const
{
    this->listForBorderDescriptor->unitePtrsOfTwoBlocks(
        this->listForBorderDescriptor->returnLinkOnPreviousElement(startPtrOfCurrentBlock),
        this->listForBorderDescriptor->returnLinkOnNextElement(startPtrOfCurrentBlock)
    );
}

byte* BorderDescriptorDeallocer::returnPtrOfLeftElementInHeap(byte* currentPtrOfElement) const
{
    uint dataSizeOfLeftElement = *(uint*)(currentPtrOfElement - SIGN_SIZE - HEADER);
    return currentPtrOfElement - 2*SIGN_SIZE - 2*LINK_SIZE - 2*HEADER - dataSizeOfLeftElement;
}
byte* BorderDescriptorDeallocer::returnPtrOfRightElementInHeap(byte* currentPtrOfElement) const
{
    uint sizeOfElement = listForBorderDescriptor->returnFirstSizeOfElement(currentPtrOfElement);
    return currentPtrOfElement + 2*SIGN_SIZE + 2*HEADER + 2*LINK_SIZE + sizeOfElement;
}
void BorderDescriptorDeallocer::unionTwoBlocks(byte* ptrOfLeftBlock, byte* ptrOfRightBlock) const
{
    uint newSizeOfBlock = listForBorderDescriptor->returnFirstSizeOfElement(ptrOfLeftBlock) + 
        listForBorderDescriptor->returnFirstSizeOfElement(ptrOfRightBlock) + 2*SIGN_SIZE + 2*HEADER + 2*LINK_SIZE;
    
    listForBorderDescriptor->setFirstSizeOfElement(ptrOfLeftBlock, newSizeOfBlock);
    listForBorderDescriptor->setSecondSizeOfElement(ptrOfRightBlock, newSizeOfBlock);
}

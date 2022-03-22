class Allocer
{
    public:
        virtual void* tryToMalloc(size_t) const = 0;
        virtual ~Allocer(){};
    
    protected:
        MemoryListForBorderDescriptorDeallocer* returnMemoryListForBorderDescriptorDeallocer(MemoryPool& _memoryPool)
        {
            return _memoryPool.listForBorderDescriptor;
        }
};

class FirstSuitableMethodAllocerForBouboundaryDescriptor : public Allocer
{
    private:
        MemoryListForBorderDescriptorDeallocer* listForBorderDescriptor;

    public:
        FirstSuitableMethodAllocerForBouboundaryDescriptor(MemoryPool& _memoryPool);
    
    public:
        void* tryToMalloc(size_t size) const;
    private:
        bool notEnoughMemoryInHeapToAllocate(size_t size) const;
        byte* returnPtrOfFirstSuitableMemoryBlockOrThrowError(uint neededSize) const;
        byte* initMemoryBlockAndReturnUserPointer(byte* ptrOfMemoryBlock, uint sizeOfDataInBytes) const;
    private:
        bool needToAllocAllBlockMemoryToAvaidFragmentation(byte* startPtrOfBlock, uint neededSize) const;
};


FirstSuitableMethodAllocerForBouboundaryDescriptor::FirstSuitableMethodAllocerForBouboundaryDescriptor(MemoryPool& _memoryPool)
{
    this->listForBorderDescriptor = returnMemoryListForBorderDescriptorDeallocer(_memoryPool);
}


void* FirstSuitableMethodAllocerForBouboundaryDescriptor::tryToMalloc(size_t size) const
{
    if (notEnoughMemoryInHeapToAllocate(size))
    {
        throw std::overflow_error("Not enough memory in heap!");
    }

    byte* ptrOfMemoryBlock = returnPtrOfFirstSuitableMemoryBlockOrThrowError(size);
    byte* ptrToReturn = initMemoryBlockAndReturnUserPointer(ptrOfMemoryBlock, (uint)size);

    return ptrToReturn;
}

bool FirstSuitableMethodAllocerForBouboundaryDescriptor::notEnoughMemoryInHeapToAllocate(size_t size) const 
{
    return (size + 2*HEADER + 2*SIGN_SIZE + 2*LINK_SIZE) >= HEAP_SIZE;
}
byte* FirstSuitableMethodAllocerForBouboundaryDescriptor::returnPtrOfFirstSuitableMemoryBlockOrThrowError(uint neededDataSize) const
{
    byte* ptrToReturn = this->listForBorderDescriptor->findNeededBlock(neededDataSize);

    if (ptrToReturn == nullptr)
        throw std::invalid_argument("Can't find needed block to make alloc!");

    return ptrToReturn;
}
byte* FirstSuitableMethodAllocerForBouboundaryDescriptor::initMemoryBlockAndReturnUserPointer(
    byte* ptrOfMemoryBlock, 
    uint sizeOfDataInBytes) const
{
    bool needToDeleteBlockFromList = false;

    if (needToAllocAllBlockMemoryToAvaidFragmentation(ptrOfMemoryBlock, sizeOfDataInBytes))
    {
        std::cout << "Need to allocate all block" << std::endl;
        sizeOfDataInBytes = *(uint*)(ptrOfMemoryBlock + SIGN_SIZE);
        needToDeleteBlockFromList = true;
    }

    byte* userPtr;
    // change block parameters (size, ptr) or delete block
    if (needToDeleteBlockFromList)
    {
        listForBorderDescriptor->deleteBlockFromList(ptrOfMemoryBlock);
    }
    else
    {
        listForBorderDescriptor->moveStartPtrOfMemoryBlock(ptrOfMemoryBlock, sizeOfDataInBytes);
    }

    // put first sign
    listForBorderDescriptor->setFirstSignOfElement(ptrOfMemoryBlock, true);

    // put size
    listForBorderDescriptor->setFirstSizeOfElement(ptrOfMemoryBlock, sizeOfDataInBytes);

    // put second sign
    listForBorderDescriptor->setSecondSignOfElement(ptrOfMemoryBlock, true);

    userPtr = ptrOfMemoryBlock + SIGN_SIZE + HEADER + 2*LINK_SIZE;
    return userPtr;
}

bool FirstSuitableMethodAllocerForBouboundaryDescriptor::needToAllocAllBlockMemoryToAvaidFragmentation(
    byte* startPtrOfBlock, 
    uint neededSizeForData) const 
{
    uint sizeOfBlockData = *(uint*)(startPtrOfBlock + SIGN_SIZE);
    uint allAvailableSize = sizeOfBlockData + 2*SIGN_SIZE + 2*HEADER + 2*LINK_SIZE;
    uint allNeededSize = 2*SIGN_SIZE + 2*HEADER + 2*LINK_SIZE + neededSizeForData;

    return (allAvailableSize - allNeededSize) <= (2*SIGN_SIZE + 2*HEADER + 2*LINK_SIZE);
}

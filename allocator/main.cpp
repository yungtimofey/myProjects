#include "Allocator.h"


int main(int argc, char** argv)
{
    MemoryListForBorderDescriptorDeallocer _list;
    MemoryPool _memoryPool(&_list);

    FirstSuitableMethodAllocerForBouboundaryDescriptor _allocer1(_memoryPool);
    BorderDescriptorDeallocer _deallocer1(_memoryPool);
    Allocator _allocator(&_allocer1, &_deallocer1);

    int* a = (int*)_allocator.malloc(sizeof(int));
    *a = 10;
    std::cout << *a << std::endl;

    char* b = (char*)_allocator.malloc(sizeof(char));
    *b = 'z';
    std::cout << *b << std::endl;

    _allocator.free(a);
    _allocator.free(b);

    system("pause");
    return 0;
}
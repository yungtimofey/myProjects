#include "Heap.h"
#include "Allocer.h"
#include "Deallocer.h"


class Allocator
{
    protected:
        Allocer* _allocer;
        Deallocer* _deallocer;
        
    public:
        Allocator(
            Allocer* _allocer, 
            Deallocer* _deallocer)
        {
            this->_allocer = _allocer;
            this->_deallocer = _deallocer;
        }
    
    public:
        void* malloc(unsigned int size)
        {
            try
            {
                void* savePointerToReturn = _allocer->tryToMalloc(size);
                return savePointerToReturn;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            return nullptr;
        }
    
    public:
        void free(void* ptr)
        {
            try
            {
                _deallocer->tryToFree(ptr);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }
};
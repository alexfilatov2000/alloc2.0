<h1>
    Page allocator
</h1>
<h4 align="center">What is Page allocator</h4>

<div>
    This allocator serves as an interface for physically contiguous allocations in multiples of page size. Allocation operations are carried out by looking into appropriate zones for free pages. Physical pages in each zone are managed by Buddy System, which serves as the backend algorithm for the page frame allocator.
</div>

<br>

<div>
The struct allocator which can select data in heap, with size that you say.
This allocator use malloc to allocate memory. Each block of memory == size leveling by 4 + Header size. In this realization header size == 24 bytes 
</div>
<br>
Functions:

void* mem_alloc(struct allocator* _allocator,size_t size) – When the mem_alloc function has called, it searches for the page with a suitable block size type or an empty page. If it found such page - it searches for the free block and occupies it. If free block wasn't found - page search process continues. If empty page was found - it's assigned to a suitable block size type.

void mem_free(struct allocator* _allocator, void* addr) – This function calls first of all decides, if it has to make the block smaller or bigger. Block is marked as 'free', and new block is allocated (called alloc(size)).

void* mem_realloc(struct allocator* _allocator, void* addr, size_t size) – try to find new block. If success copy data to new block and call mem_free for old block

Examples

Creation of allocator.

   Code:
  
    myAlloc myAlloc(2, 100);
    
Allocate memory.

   Code:

    myAlloc myAlloc(2, 100);
    int *temp = (int*)myAlloc.m_alloc(50);
    *temp = 555;
    
Reallocate memory.
    Code:

     myAlloc myAlloc(2, 100);
     int *temp = (int*)myAlloc.m_alloc(50);
     *temp = 555;
     myAlloc.m_alloc(50);
     int *newA = (int*)myAlloc.m_realoc(temp, 50);
   
Free of memory.
   Code:

    myAlloc myAlloc(2, 100);
    int *temp = (int*)myAlloc.m_alloc(50);
    *temp = 555;
    mem_free(temp)
    

#ifndef allocator_hpp
#define allocator_hpp

struct Block {
    Block* nextBlock;
    Block* prevBlock;
    void* addr;
    unsigned long size;
    bool isFree;
};

struct UsedBlock {
    UsedBlock* nextBlock;
    UsedBlock* prevBlock;
    Block* originalBlock;
};

struct List {
    int blockAmount;
    unsigned long size;
    char mode;
    void* buff;
    List* nextList;
    Block* block;
};

class myAlloc {
    bool m_allocationUsed;
    unsigned long m_pageSize;
    UsedBlock * m_usedBlocks;
    List* m_emptyPage;
    List* m_dividedPages;
    List* m_fullSizePage;
    
    void *helper(List* list);
    void divList(List *list, unsigned long blockSize);
    unsigned long getAlignSize(unsigned long n);
    List* findPage(unsigned long memSize);
public:
    myAlloc(int numPages, unsigned long size);
    void* m_alloc(unsigned long size);
    void m_free(void* addr);
    void* m_realoc(void* addr, unsigned long size);
};
#endif
//

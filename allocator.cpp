#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.hpp"

#define FREE 0
#define DIVIDED 1
#define PAGED 2

unsigned long myAlloc::getAlignSize(unsigned long n) {
    int align = 8;
    if (n % 4 == 0 && n != 0) return n;
    if (n < 4) return 4;
    while (n > align) align = align + 4;
    return align;
}

List* myAlloc::findPage(unsigned long memSize){
    List* dividedPages = m_dividedPages;
    List* emptyPages = m_emptyPage;
    List* page = dividedPages;
    while (page != NULL){
        if(page->size >= memSize && page->blockAmount > 0){
            return page;
        }
        page = page->nextList;
    }
    return emptyPages;
}

void* myAlloc::helper(List* list){
    Block* block = list->block;
    UsedBlock * usedBlocks = m_usedBlocks;
    while (block != NULL) {
        if (block->isFree) {
            if (usedBlocks == NULL) {
                UsedBlock * usedBlock = (UsedBlock *) malloc(sizeof(Block));
                usedBlock->nextBlock = NULL;
                usedBlock->prevBlock = NULL;
                usedBlock->originalBlock = block;
                usedBlocks = usedBlock;
            } else {
                while (usedBlocks->nextBlock != NULL) usedBlocks = usedBlocks->nextBlock;
                UsedBlock* newBlock = (UsedBlock *) malloc(sizeof(Block));
                newBlock->prevBlock = usedBlocks;
                newBlock->nextBlock = NULL;
                newBlock->originalBlock = block;
            }
            list->blockAmount--;
            block->isFree = false;
            return block->addr;
        }
        block = block->nextBlock;
    }
    return NULL;
}

void myAlloc::divList(List *list, unsigned long blockSize) {
    Block* block = (Block*) malloc(sizeof(Block));
    int blockAmount = (int) (list->size / blockSize);
    block->size = blockSize;
    block->addr = list->buff;
    block->isFree = true;
    list->block = block;
    list->blockAmount++;
    list->size = blockSize;

    for (int i = 1; i < blockAmount; ++i) {
        Block* newBlock = (Block*) malloc(sizeof(Block));
        newBlock->size = blockSize;
        newBlock->addr = (char*) list->buff + (i * blockSize);
        newBlock->nextBlock = NULL;
        newBlock->prevBlock = block;
        newBlock->isFree = true;

        block->nextBlock = newBlock;
        block = newBlock;

        list->blockAmount++;
    }
}

myAlloc::myAlloc(int numPages, unsigned long size){
    if (numPages < 1) return;
    size = getAlignSize(size);
    List* list = (List*) malloc(sizeof(List));
    list->size = size;
    list->mode = FREE;
    list->nextList = NULL;
    list->buff = malloc(sizeof(char) * size);
    list->blockAmount = 0;
    list->block = NULL;

    m_emptyPage = list;
    m_allocationUsed = false;
    m_pageSize = size;
    m_fullSizePage = NULL;
    m_dividedPages = NULL;
    m_usedBlocks = NULL;

    for (int i = 1; i < numPages; ++i) {
        List *nextList = (List*) malloc(sizeof(List));
        nextList->nextList = NULL;
        nextList->blockAmount = 0;
        nextList->block = NULL;
        nextList->size = size;
        nextList->buff = malloc(sizeof(char) * size);
        nextList->mode = FREE;
        list->nextList = nextList;
        list = nextList;
    }
}

void* myAlloc::m_alloc(unsigned long size){
    List * list;
    size = getAlignSize(size);
    if (!m_allocationUsed) {
        m_allocationUsed = true;
        list = m_emptyPage;
        if (size > (m_pageSize / 2)) {
            list->mode = PAGED;
            m_emptyPage = m_emptyPage->nextList;
            m_fullSizePage = list;
            m_fullSizePage->nextList = NULL;
            return list->buff;
        }
        divList(list, size);

        list->mode = DIVIDED;

        m_emptyPage = m_emptyPage->nextList;
        m_dividedPages = list;
        m_dividedPages->nextList = NULL;
        return helper(list);
    }
    list = findPage(size);
    if (list == NULL) {
        return NULL;
    } else if (list->mode == DIVIDED) {
        return helper(list);
    } else if (list->mode == FREE && size > (list->size / 2)) {
        list->mode = PAGED;
        list->nextList = m_fullSizePage;
        m_emptyPage = m_emptyPage->nextList;
        return list->buff;
    } else {
        divList(list, size);

        list->nextList = m_dividedPages;
        list->mode = DIVIDED;

        m_dividedPages = list;
        m_emptyPage = m_emptyPage->nextList;
        return helper(list);
    }
}

void myAlloc::m_free(void* addr) {
    UsedBlock * block = m_usedBlocks;
    List* page = m_fullSizePage;
    while (block != NULL) {
        if (block->originalBlock->addr == addr && !block->originalBlock->isFree) {
            block->originalBlock->isFree = true;
            UsedBlock * prevBlock = block->prevBlock;
            UsedBlock* nextBlock = block->nextBlock;
            if (prevBlock != NULL) { prevBlock->nextBlock = nextBlock; }
            if (nextBlock != NULL) { nextBlock->prevBlock = prevBlock; }
            return;
        }
        block = block->nextBlock;
    }
    while (page != NULL) {
        if(page->buff == addr) {
            page->mode = FREE;
            return;
        }
        page = page->nextList;
    }
}

void* myAlloc::m_realoc(void* addr, unsigned long size) {
    void* newAddr = m_alloc(size);
    if (newAddr == NULL) return NULL;
    memcpy(newAddr, addr, size);
    m_free(addr);
    return newAddr;
}
//

#include "BufferManager.hpp"
// #define DEBUG
BufferManager::BufferManager(){
    blocks = new Block[BLOCK_SIZE];
    #ifdef DEBUG
    printf("BufferManager Init Done!\n");
    #endif
}

BufferManager::~BufferManager(){
    delete[] blocks;
}

void BufferManager::Bpanic(const char* s)const {
    printf("[BufferManager Panic] %s", s);
    while(1);
}

BID BufferManager::getblk(std::string &str, int off){
    BID empty=-1;
    Block* bp = blocks;
    for(int i=0; i<BLOCK_NUM;i++, bp++){
        if(bp->filename == str && bp->offset == off){
            bp->refcnt++;
            return i;
        }
        if(empty == -1 && bp->refcnt == 0)
            empty = i;
    }
    if(empty == -1)
        Bpanic("No free blocks!");
    bp = blocks + empty;
    bp->filename = str;
    bp->isDirty = bp->isPinned = 0;
    bp->offset = off;
    bp->refcnt = 1;
    return empty;
}

void BufferManager::pinBlock(BID bid){
    blocks[bid].isPinned = true;
}

void BufferManager::dirtBlock(BID bid){
    blocks[bid].isDirty = true;
}

void BufferManager::unpinBlock(BID bid){
    blocks[bid].isPinned = false;
}

void BufferManager::undirtBlock(BID bid){
    blocks[bid].isDirty = false;
}

BID BufferManager::bread(std::string &str, int off){
    BID bid = getblk(str, off);
    Block * bp = blocks + bid;
    FILE *fp = fopen((str + ".data").c_str(), "rb+");

    if (fp == NULL) // If not exist, create one
        fp = fopen((str + ".data").c_str(), "wb+");

    fseek(fp, off * BLOCK_SIZE, 0);    // Realocate location
    int t = fread(bp->data, BLOCK_SIZE, 1, fp);
    if (t != 1)
    {
        if(ferror(fp)){
            printf("bread error, %s read fails!\n", str.c_str());
            return -1;
        }
        else if(feof(fp)) fwrite("\0", 1, 1, fp);
    }

    fclose(fp);
    return bid;
}

bool BufferManager::bwrite(BID bid){
    Block * bp = blocks + bid;
    FILE *fp = fopen((bp->filename + ".data").c_str(), "rb+");
    if (fp == nullptr)
        return false;

    fseek(fp, bp->offset * BLOCK_SIZE, 0);
    fwrite(bp->data, BLOCK_SIZE, 1, fp);
    fclose(fp);
    return true;
}

void BufferManager::brelease(BID bid){
    Block * bp = blocks + bid;
    /* Already released or pinned */
    if(bp->refcnt<=0 || bp->isPinned) return;

    if(bp->isDirty){ // Dirty, so write it into the disk.
        bwrite(bid);
        bp->isDirty = false;
    }

    if(--bp->refcnt == 0){
        bp->filename = "";
        bp->offset = 0;
    }
    return;
}

char *BufferManager::baddr(BID bid){
    return blocks[bid].data;
}
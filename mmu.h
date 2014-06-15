//
//  mmu.h
//  mmu_class
//
//  Created by 逸婷 李 on 14-6-9.
//  Copyright (c) 2014年 逸婷 李. All rights reserved.
//

#ifndef mmu_class_mmu_h
#define mmu_class_mmu_h
#define MAXMEM 0x20000
#define MAXVIR 0x40000

/*=======================数据结构声明=========================*/

typedef unsigned long word;

typedef struct tlb_line{//tlb一项
	word tag;
	word pfn;
	int v;
	int dirty;
}*tlb_line_p;

typedef struct tlb{//tlb表
	tlb_line_p lines[256];
}*tlb_p;

typedef struct cache_block
{
    word tag;
	int	valid;
	word data[4];
}*cache_block_p;

typedef struct cache_line{//cache一个block
	cache_block_p blocks[4];
}*cache_line_p;

typedef struct cache{//整个cache
	cache_line_p lines[256];
}*cache_p;

typedef struct page_line{
    word phy_addr;//+offset组成真正的物理地址;
}*page_line_p;

typedef struct page_table{//由offset（12位）决定放在
    page_line_p lines[0x1000];
}*page_table_p;

typedef struct page{//放在哪里由vir_addr来决定放在哪张页表
    page_table_p tables[128];
}*page_p;

/*=======================数据结构声明=========================*/


class mmu_class{
private:
    word real_memory[MAXMEM];
    page_p nowpage;
	tlb_p nowtlb;
	cache_p nowcache;
public:
    mmu_class();
    word mmu(word vir_addr,int write,word wdata);
    word* disparray(word vir_addr);
    void initial_memory(word real_memory[MAXMEM]);
    void initial_page(page_p p);
    void initial_tlb(tlb_p p,page_p page);
    void inital_cache(cache_p p,word real_memory[]);
    int mapping_type(word vir_addr);
    word match_tlb(word vir_addr,tlb_p tlb,page_p page);
    //void update_tlb(word vir_addr,tlb_p p,page_p page);
    word match_cache(word phy_addr,cache_p p,word real_memory[MAXMEM]);
    void update_cache(word phy_addr,cache_p p,word real_memory[MAXMEM]);
    int write_cache(word phy_addr,word wdata,cache_p p,word real_memory[MAXMEM]);
};
#endif

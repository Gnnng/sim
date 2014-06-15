//
//  main.cpp
//  mmu_class
//
//  Created by 逸婷 李 on 14-6-9.
//  Copyright (c) 2014年 逸婷 李. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "math.h"
#include "mmu.h"

using namespace std;

mmu_class::mmu_class()
{
    nowcache=(cache_p)malloc(sizeof(struct cache));
    nowtlb=(tlb_p)malloc(sizeof(struct tlb));
    nowpage=(page_p)malloc(sizeof(struct page));

    initial_memory(real_memory);
    //printf("real_memory[0x1000]=%lu\n",real_memory[0x1000]);


    initial_page(nowpage);
    initial_tlb(nowtlb, nowpage);
    inital_cache(nowcache, real_memory);
    //printf("cache->index0->blocks1=%lu\n",nowcache->lines[0]->blocks[1]->data[0]);

}

/*==================各种内存，虚拟内存，内存页表，tlb，cache初始化====================*/
void mmu_class::initial_memory(word real_memory[MAXMEM])//主存，2^18*4B=1MB
{
    int i;
    for(i=0;i<(MAXMEM);i++)
    {
        real_memory[i]=i;
    }
}

//虚拟内存大小是2MB
//虚拟内存从0x00000000~00040000映射到物理地址0x00000000~00020000

//内存页表，关联虚拟地址与物理地址

//内存页表结构，页表采用页大小（2^12*4B=16KB,2^12条）*页数（2^7）

void mmu_class::initial_page(page_p p)
{
    int i=0,j=0;
    word vir_addr;
    word offset,vpn;
    word frame=0;
    word phy_addr;

    for (i=0; i<128; i++)
    {
        p->tables[i]=(page_table_p)malloc(sizeof(struct page_table));
        for (j=0; j<0x1000; j++)
        {
            p->tables[i]->lines[j]=(page_line_p)malloc(sizeof(struct page_line));
        }
    }


    for(vir_addr=0;vir_addr<0x00040000;vir_addr++)
    {
        vpn=vir_addr>>12&0xfffff;
        offset=vir_addr&0xfff;
        frame=vpn%0x20;//映射方法就是pfn=vpn%0x20，从而将0x00000000-0x00040000的虚拟地址映射到0x00000000-0x00020000
        phy_addr=frame<<12|offset;
        p->tables[vpn]->lines[offset]->phy_addr=phy_addr;
        frame++;
    }
}

//tlb初始化,tlb采取大小为128条大小的tlb表，初始化放入最前面的pfn不同128条数据
void mmu_class::initial_tlb(tlb_p p,page_p page)
{
    word vir_addr=0;
    word vpn=0;
    word nowvpn=0;
    word offset;
    word pfn;
    int count=0;
    int i,j;

    for (i=0; i<256; i++)
    {
        p->lines[i]=(tlb_line_p)malloc(sizeof(struct tlb_line));
    }


    while (count!=128)
    {
        vpn=vir_addr>>12&0xfffff;
        nowvpn=vpn;
        offset=vir_addr&0xfff;
        p->lines[count]->tag=vpn;
        //pfn=page->tables[vpn]->lines[offset]->phy_addr;
        p->lines[count]->pfn=page->tables[vpn]->lines[offset]->phy_addr>>12&0xfffff;
        vir_addr++;
        count++;

        while (vpn==nowvpn)
        {
            vpn=vir_addr>>12&0xfffff;
            vir_addr++;
        }
        vir_addr--;
    }
}

void mmu_class::inital_cache(cache_p p,word real_memory[])
{

    int i,j;
    for (i=0; i<256; i++)
    {
        p->lines[i]=(cache_line_p)malloc(sizeof(struct cache_line));
        for (j=0; j<4; j++)
        {
            p->lines[i]->blocks[j]=(cache_block_p)malloc(sizeof(struct cache_block));
        }
    }

    int index=0;
    int offset=0;
    int num=0;
    int count=0;
    word phy_addr=0;
    word tag=0;

    while(phy_addr<0x1000)
    {
        i=0;
        index=phy_addr>>2&0xff;//8
        tag=phy_addr>>10&0x3fffff;//22

        if (count==256)
        {
            count=0;
            num++;
        }
        p->lines[index]->blocks[num]->tag=tag;
        while (i<4)
        {
            offset=phy_addr&0x3;
            p->lines[index]->blocks[num]->data[offset]=real_memory[phy_addr];
            phy_addr++;
            i++;
        }
        count++;
    }
}

/*==================各种内存，虚拟内存，内存页表，tlb，cache初始化====================*/

word* mmu_class::disparray(word vir_addr)
{
    word phy_addr=0;
    word *p;

    phy_addr=match_tlb(vir_addr,nowtlb,nowpage);//tlb快速查表

    p=real_memory+phy_addr;

    return p;
}



word mmu_class::mmu(word vir_addr,int write,word wdata)//要有写信号，和写入的数据，1:write;0:unwrite
{
    int type;//映射类型
    int tlb_access=0,cache_access=0;//,write_buffer_access=0;

    word phy_addr=0,data=0;

    type=mapping_type(vir_addr);//检查映射类型

    switch(type)
    {
        case -1:printf("illegal address error!");
        case 0:tlb_access=1;break;//kuseg需要tlb处理
        case 1:phy_addr=vir_addr&0x7fffffff;cache_access=1;break;//cached kseg0 固定映射方法 需要cache处理
        case 2:phy_addr=vir_addr&0x1fffffff;break;//uncached kseg1 固定映射方法 没有cache
        case 3:tlb_access=1;break;//kseg2需要tlb处理
    }

    //tlb处理
    if(tlb_access)//如果有tlb处理信号
    {
        phy_addr=match_tlb(vir_addr,nowtlb,nowpage);//tlb快速查表
        //printf("tlb -- phy_addr=%lu -- content=%lu\n",phy_addr,real_memory[phy_addr]);
        cache_access=1;//进行完tlb处理后要进行cache处理，给予cache处理信号
    }

    //data=real_memory[phy_addr];

    if(cache_access)//如果有cache处理信号
    {
        if(!write)//只读取
        {
            //printf("phy_addr=%lu\n",phy_addr);
            data=match_cache(phy_addr,nowcache,real_memory);//cache高速缓存查找数据
            // printf("cache->index0->blocks1=%lu\n",nowcache->lines[0]->blocks[1]->data[0]);
            //printf("cache -- data=%lu\n",data);
        }
        else//写入
        {
            if(write_cache(phy_addr, wdata, nowcache,real_memory))
            {
                printf("Write Succeed!\n");
                //printf("cache->index0->blocks1=%lu\n",nowcache->lines[0]->blocks[1]->data[0]);
                //printf("real_memory[0x1000]=%lu\n",real_memory[0x1000]);
                data=wdata;
            }
            else
            {
                printf("Write Failed\n");
                return 0;
            }

        }
    }
    return data;
}

int mmu_class::mapping_type(word vir_addr)//检查映射类型，属于mips地址空间的哪一个类型，分别有不同的处理方法
{
    int type;//映射类型
    //kuseg: 0x000 0000 - 0x7FFF FFFF (低端2G)-- -1
    //kseg0: 0x8000 0000 - 0x9FFF FFFF(512M)-- 0
    //kseg1: 0xA000 0000 - 0xBFFF FFFF(512M)-- 1
    //kseg2: 0xC000 0000 - 0xFFFF FFFF--2

    if(vir_addr>=0x0000000)
    {
        if(vir_addr<=0x7fffffff)
        {
            type=0;
        }
        else
        {
            if(vir_addr<=0x9fffffff)
            {
                type=1;

            }
            else
            {
                if(vir_addr<=0xBfffffff)
                {
                    type=2;
                }
                else
                {
                    if(vir_addr<=0xffffffff)
                    {
                        type=3;
                    }
                    else
                    {
                        return -1;//illegal address
                    }
                }
            }
        }
    }
    return type;
}


word mmu_class::match_tlb(word vir_addr,tlb_p tlb,page_p page)
{
    int i;

    word vpn,offset,phy_addr;

vpn=vir_addr>>12&0xfffff;//计算该虚拟地址vpn
offset=vir_addr&0xfff;

    for(i=0;i<128;i++)//遍历tlb（index）
    {
        if (tlb->lines[i]->tag==vpn)
        {
            phy_addr=tlb->lines[i]->pfn<<12|offset;
            return phy_addr;
        }
    }
    printf("There is a tlb miss!\n");
    //update_tlb(vir_addr,tlb,page);
    return page->tables[vpn]->lines[offset]->phy_addr;
}

word mmu_class::match_cache(word phy_addr,cache_p p,word real_memory[MAXMEM])//查找高速缓存cache，这里采用4-way组相联cache，每个block16个字节，可以存放4个32位数据
{
    int index;
    int dataindex;
    int offset;
    word tag;
    int i;

    index=(phy_addr>>2)&0xff;//cache 索引
    tag=phy_addr>>10;//tag 物理地址高二十位 为标记
    offset=phy_addr&0x3;

    for (i=0;i<4;i++)//循环遍历查找
    {
        if(p->lines[index]->blocks[i]->tag==tag)
        {
            return p->lines[index]->blocks[i]->data[offset];//返回对应数据
        }
    }
    printf("There is a cache miss!\n");
    update_cache(phy_addr,p,real_memory);
    return real_memory[phy_addr];
}


void mmu_class::update_cache(word phy_addr,cache_p p,word real_memory[MAXMEM])//if there is a cache miss, we should update it.
{
    word index;
    word tag;
    word offset;
    int num;

    index=(phy_addr>>2)&0xff;
    tag=phy_addr>>10&0x3fffff;
    offset=phy_addr&0x3;

    srand((int)time(0));
    num=rand()%4;

    p->lines[index]->blocks[num]->tag=tag;


    switch(offset)
    {
        case 0:p->lines[index]->blocks[num]->data[0]=real_memory[phy_addr];
            p->lines[index]->blocks[num]->data[1]=real_memory[phy_addr+1];
            p->lines[index]->blocks[num]->data[2]=real_memory[phy_addr+2];
            p->lines[index]->blocks[num]->data[3]=real_memory[phy_addr+3];
            break;
        case 1:p->lines[index]->blocks[num]->data[0]=real_memory[phy_addr-1];
            p->lines[index]->blocks[num]->data[1]=real_memory[phy_addr];
            p->lines[index]->blocks[num]->data[2]=real_memory[phy_addr+1];
            p->lines[index]->blocks[num]->data[3]=real_memory[phy_addr+2];
            break;
        case 2:p->lines[index]->blocks[num]->data[0]=real_memory[phy_addr-2];
            p->lines[index]->blocks[num]->data[1]=real_memory[phy_addr-1];
            p->lines[index]->blocks[num]->data[2]=real_memory[phy_addr];
            p->lines[index]->blocks[num]->data[3]=real_memory[phy_addr+1];
            break;
        case 3:p->lines[index]->blocks[num]->data[0]=real_memory[phy_addr-3];
            p->lines[index]->blocks[num]->data[1]=real_memory[phy_addr-2];
            p->lines[index]->blocks[num]->data[2]=real_memory[phy_addr-1];
            p->lines[index]->blocks[num]->data[3]=real_memory[phy_addr];
            break;
    }
}


int mmu_class::write_cache(word phy_addr,word wdata,cache_p p,word real_memory[MAXMEM])
{
    word index;
    word tag;
    word offset;
    int num;

    index=(phy_addr>>2)&0xff;
    tag=phy_addr>>10&0x3fffff;
    offset=phy_addr&0x3;

    srand((int)time(0));
    num=rand()%4;

    p->lines[index]->blocks[num]->tag=tag;
    real_memory[phy_addr]=wdata;

    switch(offset)
    {
        case 0:p->lines[index]->blocks[num]->data[0]=wdata;
            p->lines[index]->blocks[num]->data[1]=real_memory[phy_addr+1];
            p->lines[index]->blocks[num]->data[2]=real_memory[phy_addr+2];
            p->lines[index]->blocks[num]->data[3]=real_memory[phy_addr+3];
            return 1;
            break;
        case 1:p->lines[index]->blocks[num]->data[0]=real_memory[phy_addr-1];
            p->lines[index]->blocks[num]->data[1]=wdata;
            p->lines[index]->blocks[num]->data[2]=real_memory[phy_addr+1];
            p->lines[index]->blocks[num]->data[3]=real_memory[phy_addr+2];
            return 1;
            break;
        case 2:p->lines[index]->blocks[num]->data[0]=real_memory[phy_addr-2];
            p->lines[index]->blocks[num]->data[1]=real_memory[phy_addr-1];
            p->lines[index]->blocks[num]->data[2]=wdata;
            p->lines[index]->blocks[num]->data[3]=real_memory[phy_addr+1];
            return 1;
            break;
        case 3:p->lines[index]->blocks[num]->data[0]=real_memory[phy_addr-3];
            p->lines[index]->blocks[num]->data[1]=real_memory[phy_addr-2];
            p->lines[index]->blocks[num]->data[2]=real_memory[phy_addr-1];
            p->lines[index]->blocks[num]->data[3]=wdata;
            return 1;
            break;
        default:return 0;
    }
}


//int main()
//{
//    mmu_class go;
//    printf("%lu\n",*go.disparray(0x1000));
//}


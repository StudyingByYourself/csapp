#include <stdio.h>
#include "instruction.h"
#include "../cpu/mmu.h"
#include "../cpu/register.h"
#include "dram.h"

uint64_t decode_od(od_t od)
{
    if (od.type == IMM)
    {
        return *((uint64_t *)&od.imm);
    }
    else if (od.type == REG)
    {
        return (uint64_t)od.reg1;
    }
    else
    {
        // mm
        uint64_t vaddr = 0;
        if (od.type == MM_IMM)
        {
            vaddr = od.imm;
        }
        if (od.type == MM_REG)
        {
            vaddr = *(od.reg1);
        }
        else if (od.type == MM_IMM_REG)
        {
            vaddr = od.imm + *(od.reg1);
        }
        else if (od.type == MM_REG1_REG2)
        {
            vaddr = *(od.reg1) + *(od.reg2);
        }
        else if (od.type == MM_IMM_REG1_REG2)
        {
            vaddr = od.imm + *(od.reg1) + *(od.reg2);
        }
        else if (od.type == MM_REG2_S)
        {
            vaddr = (*(od.reg2)) * od.sca1;
        }
        else if (od.type == MM_IMM_REG2_S)
        {
            vaddr = od.imm + (*(od.reg2)) * od.sca1;
        }
        else if (od.type == MM_REG1_REG2_S)
        {
            vaddr = *(od.reg1) + (*(od.reg2)) * od.sca1;
        }
        else if (od.type == MM_IMM_REG1_REG2_S)
        {
            vaddr = od.imm + *(od.reg1) + (*(od.reg2)) * od.sca1;
        }
        return vaddr;
    }
}

void init_handler_table()
{
    handler_table[mov_reg_reg] = &mov_reg_reg_handler;
    handler_table[mov_reg_mem] = &mov_reg_mem_handler;
    handler_table[mov_mem_reg] = &mov_mem_reg_handler;
    handler_table[push_reg] = &push_reg_handler;
    handler_table[pop_reg] = &pop_reg_handler;
    handler_table[call] = &call_handler;
    handler_table[ret] = &ret_handler;
    handler_table[add_reg_reg] = &add_reg_reg_handler;
}

void instruction_cycle()
{
    inst_t *instr = (inst_t *)reg.rip;
    uint64_t src = decode_od(instr->src);
    uint64_t dst = decode_od(instr->dst);
    handler_t handler = handler_table[instr->op];
    handler(src, dst);
    printf("%s\n", instr->code);
}

void mov_reg_reg_handler(uint64_t src, uint64_t dst)
{
    *(uint64_t *)dst = *(uint64_t *)src;
    reg.rip =  reg.rip + sizeof(inst_t);
}

void mov_reg_mem_handler(uint64_t src, uint64_t dst)
{
    write64bits_dram(va2pa(dst), *(unsigned int*)src);
    reg.rip =  reg.rip + sizeof(inst_t);
}

void mov_mem_reg_handler(uint64_t src, uint64_t dst)
{
    *(uint64_t *)dst = read64bits_dram(va2pa(src));
    reg.rip =  reg.rip + sizeof(inst_t);
}

void push_reg_handler(uint64_t src, uint64_t dst)
{
    reg.rsp = reg.rsp - 8;
    write64bits_dram(va2pa(reg.rsp), *(unsigned int*)src);
    reg.rip =  reg.rip + sizeof(inst_t);
}

void pop_reg_handler(uint64_t src, uint64_t dst)
{
    *(uint64_t*)src = read64bits_dram(va2pa(reg.rsp));
    reg.rsp = reg.rsp + 8;
    reg.rip =  reg.rip + sizeof(inst_t);
}

void call_handler(uint64_t src, uint64_t dst)
{
    // src: imm address of called function
    reg.rsp = reg.rsp - 8;
    // write return address to rsp memory
    write64bits_dram(va2pa(reg.rsp), reg.rip + sizeof(inst_t));
    reg.rip = src;
}

void ret_handler(uint64_t src, uint64_t dst)
{
    uint64_t ret_addr = read64bits_dram(va2pa(reg.rsp));
    reg.rsp += 8;
    reg.rip = ret_addr;
    return;
}

void add_reg_reg_handler(uint64_t src, uint64_t dst)
{
    // add_reg_reg_handler(src = &rax, dst = &rbx)
    /*
    rax pmm[0x1234] = 0x11110000
    rbx pmm[0x1238] = 0xabcd
    src: 0x1234
    dst: 0x1238
    *(uint64_t *)src = 0x11110000
    *(uint64_t *)dst = 0xabcd
    */
    *(uint64_t *)dst = *(uint64_t *)src + *(uint64_t *)dst;
    reg.rip =  reg.rip + sizeof(inst_t);
}
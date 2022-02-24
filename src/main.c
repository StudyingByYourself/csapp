#include <stdio.h>
#include "cpu/register.h"
#include "cpu/mmu.h"
#include "memory/instruction.h"
#include "memory/dram.h"
#include "disk/elf.h"

int main()
{
    init_handler_table();
    
    // init register
    reg.rax = 0x12340000;
    reg.rbx = 0;
    reg.rcx = 0x555555554660;
    reg.rdx = 0xabcd;
    reg.rsi = 0x7fffffffe4b8;
    reg.rdi = 0x1;
    reg.rbp = 0x7fffffffe3d0;
    reg.rsp = 0x7fffffffe3b0;

    reg.rip = (uint64_t)&program[11];

    write64bits_dram(va2pa(0x7fffffffe3d0), 0x555555554660);  // rbp
    write64bits_dram(va2pa(0x7fffffffe3c8), 0x0);
    write64bits_dram(va2pa(0x7fffffffe3c0), 0xabcd);
    write64bits_dram(va2pa(0x7fffffffe3b8), 0x12340000);
    write64bits_dram(va2pa(0x7fffffffe3b0), 0x555555554660);  // rsp

    print_register();
    print_stack();

    // run inst
    for (int i = 0; i < 3; i++)
    {
        instruction_cycle();
        print_register();
        print_stack();
    }
    return 0;
}

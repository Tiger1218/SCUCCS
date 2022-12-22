/*
 * A security lab for beginners to help them familar with the technology which will be used in the following labs.
 * By Tiger1218, 2022-12-22, released under the AGPL license[https://opensource.org/licenses/AGPL-3.0].
 * Code for School of Cyber Science and Engineering, Sichuang University.
 */ 

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

void hack_me();
void linux_setting();
void try_gdb();
void endianness();
void stack();
void hijack_rip();


int data_int[] = {0xd25f2f67, 0xaad200ad, 0xa505c8ac, 0x6d295f2, 0x5c1e76d2, 0x426cba27, 0x38363bb2, 0xd930942f, 0x3b4fe71c, 0x23b0eaae};
char data_chr[] = "Wubba Lubba Dub Dub!";
void * data_pointer[] = {printf, puts, scanf, hack_me, linux_setting, try_gdb, endianness, stack, hijack_rip};
char shellcode[0x50];
uint64_t pointer, end;


void hack_me(){
    puts("Good job!");
    puts("");
}

void linux_setting(){
    /* 
     * In this section you need to compile this .c source code under Linux and run it in terminal.
     * You need to complete learning [NJU PA0](https://nju-projectn.github.io/ics-pa-gitbook/ics2022/PA0.html) first.
     * I suggest you use gcc as your default compiler with C.
     * Many Linux distributions have built-in development tools, and they usually contain gcc.
     * (Checkpoint I) *try to type `gcc --version` in your terminal!
     * (trouble shooting) If `gcc: command not found` displayed, run `sudo apt install build-essential` if your distribution is Ubuntu / Kali Linux; for other distribution, google how to install gcc in your computer.
     * (Checkpoint II) run `gcc lab0.c -o lab0` in you terminal!
     * After you run this command, gcc will compile lab0.c into an executabe file `lab0`, which equals to `lab0.exe` on Windows.
     * (Interlude) Executable file format on modern Windows is PE, on Linux is ELF, on MacOS/IOS is Mach-O.
     * Now you can run the `lab0` file by type `./lab0` in your terminal!
     * (Checkpoint III) Run this program !
     * (trouble shooting) If you can't compile it due to many reasons, you can get it from my website.
     * (trouble shooting) Just run `curl https://tiger1218.com/files/lab0 --output lab0 && chmod +x lab0`.  
     */
    puts("As you see this, I'm sure you run this program on Linux/WSL!\nSo, here's a gift for you: ");
    puts("496620796f75207365652074686973206d65");
    puts("------------------------------------------------------------------------------------");
}

void try_gdb(){
    /*
     * In this section you need to learn how to use the strongest debugger, gdb.
     * This a gdb tutorial : https://www.cs.umd.edu/~srhuang/teaching/cmsc212/gdb-tutorial-handout.pdf . You should learn it before the following part start."
     * (Checkpoint IV) Debug this program using gdb. First you need to recompile this program by `gcc lab0.c -o lab0 -g`, which will add debug info to the binary.
     * (Checkpoint V) Break the program on line XX or addr displayed, than print the variable randnum. Continue and input that num.
     */
    uint64_t rip;
    srand(time(0));
    int randnum = rand(), guessnum;
    puts("Guess the number ?");
    asm volatile("1: lea 1b(%%rip), %0;": "=a"(rip)); // get current rip value
    printf("Current IP = 0x%lx\n", rip); 
    scanf("%d", &guessnum);
    if(guessnum == randnum){
        puts("Wow...You are correct.");
        puts("7373616765206e6f772c20796f75206d7573");
        puts("------------------------------------------------------------------------------------");
    }
    else{
        puts("Seems that you got wrong!");
        exit(0);
    }
}

void endianness(){
    /*
     * In this section, you need to learn about [endianness](https://en.wikipedia.org/wiki/Endianness) first.
     * View the memory where data_int, data_chr, data_pointer stored and see how little endian works.
     * (Checkpoint VI) Input numbers and see the value of *(int *)fina using gdb
     * (Checkpoint VII) Pass this level.
     */
    int data = 0xdeadbeef;
    char fina[4];
    for(int i = 0 ; i < 4 ; i ++){
        int td;
        scanf("%d", &td);
        fina[i] = td & 0xff;
    }
    if(data == *(int *)fina){
        puts("You solved it!");
        puts("742068617665207061737365642074686520");
    }
    else{
        puts("Zannen desu...");
        exit(0);
    }
}

uint64_t substack(){
    uint64_t varies3 = 0xdeadbeefadcaffee;
    return (uint64_t)&varies3;
}

void stack(){
    printf("%08lx\n", (uint64_t)stack);
    // asm volatile("1: lea 1b(%%rbp), %0;": "=a"(rbp));
    char varies0[8] = "ABCDEFGH";
    int varies1 = 0x12345678;
    int varies2 = 0x90abcdef;
    register int* a = &varies1;
    register int* b = &varies2;
    end = substack();
    // printf("%p %p\n", (uint64_t)varies0, end);
    // printf("%p\n", stack);
    for(pointer = end; pointer <= (uint64_t)varies0 ; pointer += 8){
        // int td;
        printf("0x%lx : %8lx\n", pointer, *(uint64_t *)(pointer));
        // scanf("%d", &td);
        // if(td != *(int *)(i)){
        //     printf("%p wrong. \n", i);
        //     exit(0);
        // }
    }

}

void hijack_rip(){
    // pass
}

int main(){
    /* Hey, welcome to lab0!
     * This Lab aims to provide a step-by-step guide help you get familar with basic concepts & basic technology rapidly.
     * You need to follow the instructions in each sections to solve them without modifing the source code. Also, I will arrange some checkpoints in the instructions. You'd better not go directly to next checkpoint till you finish last. 
     * I strongly recommand you read [提问的智慧](https://lug.ustc.edu.cn/wiki/doc/smart-questions/) and [别像弱智一样提问](https://github.com/tangx/Stop-Ask-Questions-The-Stupid-Ways/blob/master/README.md) first.
     * Remember, Google is your friend. 
     * If you have any problem solving this lab and you are sure that I'm the screw-up, plz email me at tiger1218 [at] foxmail.com!
     * Enjoy your journey!
     */
    // linux_setting();
    // // try_gdb();
    // endianness();
    stack();
    // hijack_rip();


    return 0;
}

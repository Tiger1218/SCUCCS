/*
 * A security lab for beginners to help them familar with the technology which will be used in the following labs.
 * By Tiger1218 & dying084, 2022-12-22, released under the AGPL license[https://opensource.org/licenses/AGPL-3.0].
 * Code for School of Cyber Science and Engineering, Sichuang University.
 */ 

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

void linux_setting();
void try_gdb();
void endianness();
uint64_t substack();
void stack();

int data_int[] = {0xd25f2f67, 0xaad200ad, 0xa505c8ac, 0x6d295f2, 0x5c1e76d2, 0x426cba27, 0x38363bb2, 0xd930942f, 0x3b4fe71c, 0x23b0eaae};
char data_chr[] = "Wubba Lubba Dub Dub!";
void * data_pointer[] = {printf, puts, scanf, linux_setting, try_gdb, endianness, substack, stack};
char shellcode[0x50];

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
    puts("As you see this, I'm sure you run this program on Linux/WSL!");
}

void try_gdb(){
    /*
     * In this section you will learn how to use the strongest debugger, gdb.
     * This a gdb tutorial : https://www.cs.umd.edu/~srhuang/teaching/cmsc212/gdb-tutorial-handout.pdf . You should learn it before the following part start."
     * (Checkpoint IV) Debug this program using gdb. First you need to recompile this program by `gcc lab0.c -o lab0 -g`, which will add debug info to the binary.
     * (Checkpoint V) Break the program on line XX or addr displayed, than print the variable randnum. Continue and input that num.
     * (Interlude) Random number
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
    }
    else{
        puts("Seems that you got wrong!");
        exit(0);
    }
}

void endianness(){
    /*
     * In this section, you will learn about endianness.
     * First, you need to read the [wikipedia page](https://en.wikipedia.org/wiki/Endianness). 
     * View the memory where data_int, data_chr, data_pointer stored and see how little endian works.
     * (Checkpoint VI) Input numbers and see the value of ans using gdb.
     * (Checkpoint VII) Pass this level.
     */
    uint64_t a;
    uint32_t b = 0xdeadbeef;
    uint16_t c = 0xface;
    uint8_t d[2] = {0xae,0xaf};
    * (uint8_t *)&a = d[0];
    * (uint16_t *)((uint8_t *)&a + 1) = c;
    * (uint32_t *)((uint8_t *)&a + 3) = b;
    * (uint8_t *)((uint8_t *)&a + 7) = d[1];
    char ans[8];
    for(int i = 0; i < 8 ; i++){
        int p;
        scanf("%d", &p);
        ans[i] = p & 0xff;
    }
    if(* (uint64_t *)ans == a){
        puts("pass!");
    }
    else puts("try again");
}

uint64_t substack(){
    uint64_t varies3 = 0xdeadbeefadcaffee;
    return (uint64_t)&varies3;
}

void stack(){
    /* 
     * This section you will learn C Function Call Stack.
     * Well, I must warn you, this level is **very** difficult. But daijoubu! With perseverance and willingness to learn, I believe you can make it.
     * You need to install `binutils` in order to gain `objdump` command.
     * Recompile this source code with `gcc lab0.c -o lab0 -g -fno-stack-protector`
     * (Interlude) [What's canary & why we ban it?](https://en.wikipedia.org/wiki/Buffer_overflow_protection#Canaries)
     * (Checkpoint VIII) Use `objdump -d lab0` ,find the assembly code of function `stack` and `substack`, than **skim** it. Focus on the stack operations.
     * (trouble shooting) Use `objdump -d lab0 -M intel` if you think the Intel style is better the AT&T style.
     * You must read [this article](https://www.cnblogs.com/clover-toeic/p/3755401.html) before the following tasks begin.
     * (Checkpoint IX) Finish 
     * 4 hints are given: 1. the structure of the stack is like [local varieable] + [stored rbp] + [stored rip] + [local varieable]
     *                    2. ignore the local varieable starts with `register`, they are assigned storage method.
     *                    3. consider the offset between the start addr of function `stack` and the [stored rip]
     *                    4. consider the offset between the addr of varies0 and the [stored rbp]
     * Why ?
     * Well if you find it's too difficult, you can run `export SECURITY_DEBUG=1` and restart the program. Than it will show you the stack. 
     */
    char varies0[8] = "ABCDEFGH";
    int varies1 = 0x12345678;
    int varies2 = 0x90abcdef;
    register int* a = &varies1;
    register int* b = &varies2;
    register int count = 0;
    register uint64_t end = substack();
    printf("%08lx\n", (uint64_t)stack);
    printf("%08lx\n", (uint64_t)varies0);
    for(register uint64_t pointer = end; pointer <= (uint64_t)varies0 ; pointer += 8){
        char *s = getenv("SECURITY_DEBUG");
        if(s){
            printf("0x%lx : %8lx\n", pointer, *(uint64_t *)(pointer));
            continue;
        }
        uint64_t input;
        scanf("%lu", &input);
        if(input != *(uint64_t *)(pointer)){
            puts("Fail.");
            exit(0);
        }
        count ++;
    }
    puts("Congrations!");
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
    linux_setting();
    try_gdb();
    endianness();
    stack();
    return 0;
}

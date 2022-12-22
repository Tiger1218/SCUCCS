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

void hack_me(){
    puts()
}

void linux_setting(){
    /* 
     * So first you need to compile this .c source code under Linux.
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
    usleep(1000);
    puts("496620796f75207365652074686973206d65");
    usleep(1000);
    puts("------------------------------------------------------------------------------------");
}

void try_gdb(){
    /* 
    puts("Now, you need to try gdb, the most wide-used debugger on *nix system.");
    puts("This is a gdb tutorial : https://www.cs.cmu.edu/~gilpin/tutorial/ . You should learn it before the following part start.");
    usleep(1000);
    puts("Than a challenge comes: ");
    puts("You need to guess the random number!");
    puts("And, break after 36 so that you can view the randnum by gdb!")

    __asm__(
        movq %rip, %rsi
    )
    printf("Current IP = %P\n");
    srand(time(0));
    int randnum = rand(), guessnum;
    puts("Guess the number ?");
    scanf("%d", &guessnum);
    if(guessnum == randnum){
        puts("Wow...You are correct.")
        puts("")
    }
    else{
        puts("Seems that you got wrong!");
        exit(0);
    }
    */
}

int main(){
    /* Hey, welcome to lab0!
     * This Lab aims to help you get familar with basic concepts & basic technology rapidly.
     * You need to follow the instructions in each sections. Also, I will arrange some checkpoints in the instructions. You'd better not go directly to next checkpoint till you finish last. 
     * I strongly recommand you read [提问的智慧](https://lug.ustc.edu.cn/wiki/doc/smart-questions/) and [别像弱智一样提问](https://github.com/tangx/Stop-Ask-Questions-The-Stupid-Ways/blob/master/README.md) first.
     * Remember, Google is your friend. 
     * If you have any problem solving this lab and you are sure that I'm the screw-up, plz email me at tiger1218 [at] foxmail.com!
     * Enjoy your journey!
     */
    linux_setting();
    try_gdb();

    return 0;
}
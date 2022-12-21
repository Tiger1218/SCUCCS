/*
 * A security lab for beginners to help them familar with the technology which will be used in the following labs.
 * By Tiger1218, 2022-12-22, released under the GPL license.
 * Code for School of Cyber Science and Engineering, Sichuang University.
 */ 

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void hack_me(){
    puts("Oh you hack me?");
    puts("Than here's the reward!");
}

void linux_setting(){
    puts("As you see this, I'm sure you run this program on Linux/WSL!\nSo, here's a gift for you: ");
    usleep(1000);
    puts("496620796f75207365652074686973206d65");
    usleep(1000);
    puts("If you have any problem solving this lab, plz email me at tiger1218 [at] foxmail.com");
    puts("Enjoy it!");
    puts("------------------------------------------------------------------------------------");
}

void try_gdb(){
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
}

int main(){
    linux_setting();
    try_gdb();

    return 0;
}
# Writeup for Decoding Lab, iCarnegie

## overview

All materials needed in this lab are
stored in [my github repo](https://github.com/Tiger1218/SCUCCS/tree/main/C-Programming/Security%20Labs/lab1).

* my gcc version: `gcc (Ubuntu 11.2.0-19ubuntu1) 11.2.0`
* compile command: `cc lab1.c -o lab1 -g`
* linux kernel: `5.15.0-56-generic`

So let's begin.

## key1 & key2

根据`guide.html`，我们首先只需要考虑`key1`和`key2`；而我们应该要得到一串解密后的字符串，以`From:`开头。审计函数`extract_message1`，经过黑盒 & 白盒测试，我们可以发现该函数会从`start+1`处开始，每`stride`个字符，drop一个字符。考虑到小端序，我们整个字符数组应该为：

```python
real_data =  ['63', '63', '63', '63', '63', '63', '63', '63', '63', '46', '46', '72', '72', '6f', '6d', '6f', '3a', '20', '6d', '46', '72', '3a', '69', '65', '20', '6e', '64', '43', 'a ', '54', '54', '6f', '3a', '45', '20', '59', 'a ', '6f', '75', '54', 'a ', '47', '6f', '6f', '6f', '3a', '64', '21', '20', '20', '4e', '59', '6f', '77', '6f', '20', '74', '75', '72', '79', 'a ', '20', '63', '45', '68', '6f', '78', '6f', '73', '63', '69', '6e', '65', '67', '20', '6c', '6b', '65', '6c', '79', '73', '65', '33', '2c', '6e', '34', '20', '74', '74', '6f', '21', '20', '66', '59', '6f', '72', '6f', '63', '65', '75', '20', '61', '20', '20', '63', '67', '61', '6c', '6f', '6c', '20', '74', '74', '6f', '20', '20', '65', '65', '78', '74', '76', '72', '61', '65', '63', '74', '72', '32', '20', '79', '61', '6e', '74', '64', 'a ', '68', '61', '76', '69', '6f', '69', '6e', '64', '20', '67', '74', '68', '21', '65', '20', '0 ', '63', '61', '78', '6c', '6c', '78', '20', '74', '78', '6f', '20', '78', '65', '78', '78', '74', '72', '78', '61', '63', '78', '74', '31', '78', '0 ']
real_string = 'cccccccccFFrromo: mFr:ie ndC\nTTo:E Y\nouT\nGooo:d!  NYowo tury\n cEhoxoscineg lkelyse3,n4 tto! fYoroceu a  cgalol tto  eextvraectr2 yantd\nhavioind gth!e caxllx txo xexxtrxacxt1x'
```

输出脚本位于`solve.py`。

定位F，r，o，m。最后得出start=9 & stride=3。

`start`为dummy（在内存中）的第一个byte, `stride`为dummy（在内存中）的第二个byte。

然而Linux和Windows都是小端序，所以正确的解释是：`start`是dummy的Least Significant Byte，`stride`则是次低位。

也就是`dummy`应该为`0x????0309`。`?`可以取任何值。

看到`process_keys12`，该函数可以视为一个任意内存写：将`key1`赋值为`&key1`相对于指定修改内存的偏移，key2为指定修改的值。

一种可能的解法就是修改`dummy`。我们可以用反汇编软件或gdb知道`&dummy`与`&key1`的偏移。

```bash
pwndbg> set args 0 0x0102
pwndbg> b lab1.c:76
Breakpoint 1 at 0x1479: file lab1.c, line 77.
pwndbg> r
Starting program: /root/repos/SCUCCS/C-Programming/Security Labs/lab1/lab1 0 0x0102
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Breakpoint 1, main (argc=3, argv=0x7fffffffe378) at lab1.c:77
warning: Source file is more recent than executable.
77	    process_keys12(&key1, &key2);
LEGEND: STACK | HEAP | CODE | DATA | RWX | RODATA
──────────────────────────────────────────────────────────────────────[ REGISTERS / show-flags off / show-compact-regs off ]───────────────────────────────────────────────────────────────────────
*RAX  0x102
 RBX  0x0
*RCX  0x7fffffffe65e ◂— 0x2f3d4c4c45485300
 RDX  0x0
*RDI  0x10
*RSI  0x102
*R8   0xfffffffffffffff
 R9   0x0
*R10  0x7ffff7f4aac0 (_nl_C_LC_CTYPE_toupper+512) ◂— 0x100000000
*R11  0x7ffff7f4b3c0 (_nl_C_LC_CTYPE_class+256) ◂— 0x2000200020002
*R12  0x7fffffffe378 —▸ 0x7fffffffe61d ◂— '/root/repos/SCUCCS/C-Programming/Security Labs/lab1/lab1'
*R13  0x5555555553a2 (main) ◂— endbr64 
*R14  0x555555557da0 (__do_global_dtors_aux_fini_array_entry) —▸ 0x555555555180 (__do_global_dtors_aux) ◂— endbr64 
*R15  0x7ffff7ffd040 (_rtld_global) —▸ 0x7ffff7ffe2e0 —▸ 0x555555554000 ◂— 0x10102464c457f
*RBP  0x7fffffffe260 ◂— 0x3
*RSP  0x7fffffffe210 —▸ 0x7fffffffe378 —▸ 0x7fffffffe61d ◂— '/root/repos/SCUCCS/C-Programming/Security Labs/lab1/lab1'
*RIP  0x555555555479 (main+215) ◂— lea rdx, [rbp - 0x2c]
───────────────────────────────────────────────────────────────────────────────[ DISASM / x86-64 / set emulate on ]────────────────────────────────────────────────────────────────────────────────
 ► 0x555555555479 <main+215>    lea    rdx, [rbp - 0x2c]
   0x55555555547d <main+219>    lea    rax, [rbp - 0x30]
   0x555555555481 <main+223>    mov    rsi, rdx
   0x555555555484 <main+226>    mov    rdi, rax
   0x555555555487 <main+229>    call   process_keys12                <process_keys12>
 
   0x55555555548c <main+234>    lea    rax, [rbp - 0x34]
   0x555555555490 <main+238>    movzx  eax, byte ptr [rax]
   0x555555555493 <main+241>    movsx  eax, al
   0x555555555496 <main+244>    mov    dword ptr [rbp - 0x20], eax
   0x555555555499 <main+247>    lea    rax, [rbp - 0x34]
   0x55555555549d <main+251>    add    rax, 1
─────────────────────────────────────────────────────────────────────────────────────────[ SOURCE (CODE) ]─────────────────────────────────────────────────────────────────────────────────────────
In file: /root/repos/SCUCCS/C-Programming/Security Labs/lab1/lab1.c
   72     key1 = strtol(argv[1], NULL, 0);
   73     key2 = strtol(argv[2], NULL, 0);
   74     if (argc > 3) key3 = strtol(argv[3], NULL, 0);
   75     if (argc > 4) key4 = strtol(argv[4], NULL, 0);
   76 
 ► 77     process_keys12(&key1, &key2);
   78 
   79     start = (int)(*(((char *) &dummy)));
   80     stride = (int)(*(((char *) &dummy) + 1));
   81 
   82     if (key3 != 0 && key4 != 0) {
─────────────────────────────────────────────────────────────────────────────────────────────[ STACK ]─────────────────────────────────────────────────────────────────────────────────────────────
00:0000│ rsp 0x7fffffffe210 —▸ 0x7fffffffe378 —▸ 0x7fffffffe61d ◂— '/root/repos/SCUCCS/C-Programming/Security Labs/lab1/lab1'
01:0008│     0x7fffffffe218 ◂— 0x300000000
02:0010│     0x7fffffffe220 ◂— 0x0
03:0018│     0x7fffffffe228 ◂— 0x100000000
04:0020│     0x7fffffffe230 ◂— 0x10200000000
05:0028│     0x7fffffffe238 ◂— 0x0
... ↓        2 skipped
───────────────────────────────────────────────────────────────────────────────────────────[ BACKTRACE ]───────────────────────────────────────────────────────────────────────────────────────────
 ► f 0   0x555555555479 main+215
   f 1   0x7ffff7db5d90 __libc_start_call_main+128
   f 2   0x7ffff7db5e40 __libc_start_main+128
   f 3   0x555555555105 _start+37
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
pwndbg> p &dummy
$1 = (int *) 0x7fffffffe22c
pwndbg> p &dummy-&key1
$2 = -1
```

所以第一阶段的`payload`为`./lab1 -1 0x0309`。

```bash
root@workshop:~/repos/SCUCCS/C-Programming/Security Labs/lab1# ./lab1 -1 0x0309
From: Friend
To: You
Good! Now try choosing keys3,4 to force a call to extract2 and
avoid the call to extract1
```

## key3 & key4

观察得知：

1. 与`process_keys12`一样，`process_keys34`也是一个任意内存写。
2. 除非修改了`data`、`start`或者`stride`，`extract_message1`执行后一定会使得`msg1`不等于空。
3. `start`和`stride`应该不变；不然`extract_message2`的返回值，`msg2`就会被修改
4. `process_keys34`会执行两次；`process_keys34`的修改又是偏移性质，所以两次对内存的修改结果可能不一样。

由此可以得出两个解题思路：

1. 在第一处`process_keys34`时修改返回地址，在`ret`的时候跳转到`msg2 = extract_message2(start, stride);`所在的地址。
2. 在第一处`process_keys34`时将`data`的第10位修改为0。这样我们就可以进入if；进入if后的第二处`process_keys34`又可以通过offset式的修改方法把第10位置为不为0的数，因此可以输出`msg2`。

在这里仅简述第一种做法的方法。

```bash
pwndbg> set args -1 0x0309 4 49
pwndbg> b lab1.c:30
Breakpoint 1 at 0x1247: file lab1.c, line 30.
pwndbg> r
Starting program: /root/repos/SCUCCS/C-Programming/Security Labs/lab1/lab1 -1 0x0309 4 49
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Breakpoint 1, process_keys34 (key3=0x7fffffffe228, key4=0x7fffffffe22c) at lab1.c:30
warning: Source file is more recent than executable.
30	    *(((int *)&key3) + *key3) += *key4;
LEGEND: STACK | HEAP | CODE | DATA | RWX | RODATA
──────────────────────────────────────────────────────────────────────[ REGISTERS / show-flags off / show-compact-regs off ]───────────────────────────────────────────────────────────────────────
*RAX  0x7fffffffe228 ◂— 0x3100000004
 RBX  0x0
*RCX  0x7fffffffe65e ◂— 0x2f3d4c4c45485300
*RDX  0x7fffffffe22c ◂— 0x900000031 /* '1' */
*RDI  0x7fffffffe228 ◂— 0x3100000004
*RSI  0x7fffffffe22c ◂— 0x900000031 /* '1' */
*R8   0x1999999999999999
 R9   0x0
*R10  0x7ffff7f4aac0 (_nl_C_LC_CTYPE_toupper+512) ◂— 0x100000000
*R11  0x7ffff7f4b3c0 (_nl_C_LC_CTYPE_class+256) ◂— 0x2000200020002
*R12  0x7fffffffe368 —▸ 0x7fffffffe617 ◂— '/root/repos/SCUCCS/C-Programming/Security Labs/lab1/lab1'
*R13  0x5555555553a2 (main) ◂— endbr64 
*R14  0x555555557da0 (__do_global_dtors_aux_fini_array_entry) —▸ 0x555555555180 (__do_global_dtors_aux) ◂— endbr64 
*R15  0x7ffff7ffd040 (_rtld_global) —▸ 0x7ffff7ffe2e0 —▸ 0x555555554000 ◂— 0x10102464c457f
*RBP  0x7fffffffe1f0 —▸ 0x7fffffffe250 ◂— 0x5
*RSP  0x7fffffffe1f0 —▸ 0x7fffffffe250 ◂— 0x5
*RIP  0x555555555247 (process_keys34+16) ◂— mov rax, qword ptr [rbp - 8]
───────────────────────────────────────────────────────────────────────────────[ DISASM / x86-64 / set emulate on ]────────────────────────────────────────────────────────────────────────────────
 ► 0x555555555247 <process_keys34+16>    mov    rax, qword ptr [rbp - 8]
   0x55555555524b <process_keys34+20>    mov    eax, dword ptr [rax]
   0x55555555524d <process_keys34+22>    cdqe   
   0x55555555524f <process_keys34+24>    lea    rdx, [rax*4]
   0x555555555257 <process_keys34+32>    lea    rax, [rbp - 8]
   0x55555555525b <process_keys34+36>    add    rax, rdx
   0x55555555525e <process_keys34+39>    mov    ecx, dword ptr [rax]
   0x555555555260 <process_keys34+41>    mov    rax, qword ptr [rbp - 0x10]
   0x555555555264 <process_keys34+45>    mov    edx, dword ptr [rax]
   0x555555555266 <process_keys34+47>    mov    rax, qword ptr [rbp - 8]
   0x55555555526a <process_keys34+51>    mov    eax, dword ptr [rax]
─────────────────────────────────────────────────────────────────────────────────────────[ SOURCE (CODE) ]─────────────────────────────────────────────────────────────────────────────────────────
In file: /root/repos/SCUCCS/C-Programming/Security Labs/lab1/lab1.c
   25 void process_keys12 (int * key1, int * key2) {
   26     *((int *) (key1 + *key1)) = *key2;
   27 }
   28 
   29 void process_keys34 (int * key3, int * key4) {
 ► 30     *(((int *)&key3) + *key3) += *key4;
   31 }
   32 
   33 char * extract_message1(int start, int stride) {
   34     int i, j, k;
   35     int done = 0;
─────────────────────────────────────────────────────────────────────────────────────────────[ STACK ]─────────────────────────────────────────────────────────────────────────────────────────────
00:0000│ rbp rsp             0x7fffffffe1f0 —▸ 0x7fffffffe250 ◂— 0x5
01:0008│                     0x7fffffffe1f8 —▸ 0x5555555554cb (main+297) ◂— mov edx, dword ptr [rbp - 0x1c]
02:0010│                     0x7fffffffe200 —▸ 0x7fffffffe368 —▸ 0x7fffffffe617 ◂— '/root/repos/SCUCCS/C-Programming/Security Labs/lab1/lab1'
03:0018│                     0x7fffffffe208 ◂— 0x500000000
04:0020│                     0x7fffffffe210 ◂— 0x0
05:0028│                     0x7fffffffe218 ◂— 0x30900000000
06:0030│                     0x7fffffffe220 ◂— 0x309ffffffff
07:0038│ rax rdi rdx-4 rsi-4 0x7fffffffe228 ◂— 0x3100000004
───────────────────────────────────────────────────────────────────────────────────────────[ BACKTRACE ]───────────────────────────────────────────────────────────────────────────────────────────
 ► f 0   0x555555555247 process_keys34+16
   f 1   0x5555555554cb main+297
   f 2   0x7ffff7db5d90 __libc_start_call_main+128
   f 3   0x7ffff7db5e40 __libc_start_main+128
   f 4   0x555555555105 _start+37
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
pwndbg> p &key3-$rbp
First argument of `-' is a pointer and second argument is neither
an integer nor a pointer of the same type.
pwndbg> p 0x7fffffffe1f8 - (long)&key3
$1 = 16
```

一个`int`是4个bytes，所以key3应该为4，才能修改栈上的返回值。

```bash
root@workshop:~/repos/SCUCCS/C-Programming/Security Labs/lab1# objdump -d lab1 -M intel| grep process_keys34
0000000000001237 <process_keys34>:
    14c6:	e8 6c fd ff ff       	call   1237 <process_keys34>
    14f7:	e8 3b fd ff ff       	call   1237 <process_keys34>
```

$ \mathrm{0x14f7}-\mathrm{0x14c6}=49 $

因此key4就是49。

稍微解释一下为什么可以这么做：原返回值一定是第一个`call process_keys`的下一位汇编的地址；要修改成的返回值也要是第二个`call process_keys`的地址。这两条指令的长短相等，故返回值之差一定也为49。

所以第二阶段的`payload`为`./lab1 -1 0x0309 4 49`。

```bash
root@workshop:~/repos/SCUCCS/C-Programming/Security Labs/lab1# ./lab1 -1 0x0309 4 49
From: CTE
To: You
Excellent!You got everything!
```

## Summary

首先，总地来说，这个`Decoding Lab`的水平明显不如CSAPP Labs。第一个问题在于该题在不同平台甚至不同编译优化环境下的`payload`不一样，这就让调试非常头疼：如果目标解题者，这种调试是很难的————尤其是在初学者尚未用明白gdb的时候，静态分析源程序基本分析不出来；比如`dummy`和`key1`之间的差，如果看源代码很容易误以为偏移是`-3`，但是编译器在优化的时候其实会把`dummy`+`key1`与`start`+`stride`错开，哪怕调到`-Og`都是一样。第二个问题在于第二问有一些脑洞的成分，在解决第一问后，很容易将思路放在修改特定的数据上面，但是仔细分析后才能发现，修改`dummy`/`start`/`stride`都不行，要修改`data`或者栈上存储的返回值。

其次，顺利+能够形成激励机制地完成这个lab所需要的技术栈太大了。我（相对算比较轻松地）完成这个lab用到的技术栈有：

* 熟悉汇编语言
* 熟悉指针、Linux内存机制
* 丰富的Linux使用经验
* 丰富的gdb经验

对其中一项或者几项不熟悉都会使得完成该Lab变成痛苦的事情。在教学质量、学习时间、助教指导都不能得到保证的情况下，布置这个作业与鼓励抄袭无异。

最后，我想稍微谈几句与教学相关的问题。

我非常同意[上海交通大学学生生存手册](https://survivesjtu.gitbook.io/survivesjtumanual/li-zhi-pian/huan-ying-lai-dao-shang-hai-jiao-tong-da-xue)中的一段话：

> 各位同学们，在本书的开始，我不得不遗憾地告诉大家一个消息。国内绝大部分大学的本科教学，不是濒临崩溃，而是早已崩溃。在此，我无意争论是否复旦、中科大、或者清华、北大是否比我们崩溃的更少一些——这种争论是没有意义的。我只是看到了无数充满求知欲、激情、与年轻梦想的同学们，将要把自己的四年青春，充满希望与信任地交给大学来塑造。这使我心中非常不安。

在略显传统传统的"C语言程序设计"这一门课程中引入一些优秀大学课程中的Labs自然是有利无弊的事情，但是我们也要看到，在CSAPP开设的CMU，[CMU 15-213](https://www.cs.cmu.edu/~213/)有着前置课程[CMU 15-122](https://www.cs.cmu.edu/~15122/)——而这门课基本上相当于我们的"C语言程序设计" & "数据结构与算法" & 基本Linux命令 & 基本Linux系统机制 & 一点点计算机导论。MIT在计算机相关课程开始之前有[The Missing Semester of Your CS Education](https://missing.csail.mit.edu/)，介绍了Linux下的Shell与基本Shell操作、Vim、命令行配置、Git、tmux、ssh、基本的安全/密码学意识、Debugging调试等。哪怕在计算机科学本科生教学水平普遍很差的中国高校中，NJU都有[PA(计算机系统基础 课程实验)](https://nju-projectn.github.io/ics-pa-gitbook/ics2022/index.html)这样非常优秀的基础课程。

## References

[Decoding Lab: Understanding a Secret Message | Yieldnull](https://yieldnull.com/blog/d18eebb3d5460a86d652924b233e9ab30daafe6f/)
[安全项目第一题解答及思路分析 | junyu33](https://blog.junyu33.me/2021/10/29/security01.html)，第二问的第二种做法实现
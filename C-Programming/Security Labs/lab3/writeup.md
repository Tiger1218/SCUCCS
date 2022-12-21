# Writeup for Bomb Lab, CSAPP

## Overview

Above all, I need to declare that I used the newest version of [Bomb Lab](http://csapp.cs.cmu.edu/3e/bomblab.pdf), which means it differs in many ways from the 200X version of this lab. The most notable difference is that is uses x64 architecture.

```bash
curl http://csapp.cs.cmu.edu/3e/bomb.tar --output bomb.tar
tar xvf bomb.tar
```

All files needed are stored in [my github repo](https://github.com/Tiger1218/SCUCCS/tree/main/C-Programming/Security%20Labs/lab3).

## Initialize

### FileStream

```c
if ( argc == 1 )
{
    infile = (FILE *)stdin;
}
else
{
    v3 = argv;
    if ( argc != 2 )
    {
        __printf_chk(1LL, "Usage: %s [<input_file>]\n", *argv);
        exit(8);
    }
    *(_QWORD *)&argc = argv[1];
    argv = (const char **)"r";
    infile = fopen(*(const char **)&argc, "r");
    if ( !infile )
    {
        __printf_chk(1LL, "%s: Error: Couldn't open %s\n", *v3, v3[1]);
        exit(8);
    }
}
```

如果运行程序的时候有参数，那么将参数视为文件地址，然后把输入流重定向至文件读入流。

这意味着我们可以使用`./bomb payload`来检验我们的payload了。（虽然原来也可以`./bomb < payload`就是了）

### bind

`initialize_bomb`函数把`SIGINT`信号（一般来自于`Ctrl+C`）绑定到了`signal_handler`函数上面。[^1]

## Phase1

```c
__int64 __fastcall phase_1(__int64 a1)
{
  __int64 result; // rax

  result = strings_not_equal(a1, "Border relations with Canada have never been better.");
  if ( (_DWORD)result )
    explode_bomb();
  return result;
}
```

因为源文件并没有去除符号表，因此我们可以猜测 `string_not_equal`函数在 `arg1`和 `arg2`相等时返回1。

所以Phase1的Payload就是 `Border relations with Canada have never been better.`。

## Phase2

```c
__int64 __fastcall read_six_numbers(__int64 a1, __int64 a2)
{
    __int64 result; // rax
    result = __isoc99_sscanf(a1, &unk_4025C3, a2, a2 + 4, a2 + 8, a2 + 12, a2 + 16, a2 + 20);
    if ( (int)result <= 5 )
        explode_bomb();
    return result;
}
__int64 __fastcall phase_2(__int64 a1)
{
    __int64 result; // rax
    char *v2; // rbx
    int v3; // [rsp+0h] [rbp-38h] BYREF
    char v4; // [rsp+4h] [rbp-34h] BYREF
    char v5; // [rsp+18h] [rbp-20h] BYREF

    read_six_numbers(a1, &v3);
    if ( v3 != 1 )
        explode_bomb();
    v2 = &v4;
    do
    {
        result = (unsigned int)(2 * *((_DWORD *)v2 - 1));
        if ( *(_DWORD *)v2 != (_DWORD)result )
            explode_bomb();
        v2 += 4;
    }
    while ( v2 != &v5 );
    return result;
}
```

观察后发现：

1. `unk_4025C3`处内存布局为 `25 64 20 25 64 20 25 64 20 25 64 20 25 64 20 25 64 00`。考虑到字符串的 `\x00`截断，`unk_4025C3`为 `%d %d %d %d %d %d`。
2. 翻阅[cppreference](https://en.cppreference.com/w/c/io/fscanf)可以发现 `sscanf`会将给定的第一个参数视为缓冲区，从此处读取数据。
3. 只有把 `a2`视为一个长度为6的int数组才能解释这两段代码。

这是修改后的代码：

```c
__int64 __fastcall phase_2(__int64 a1)
{
    __int64 result; // rax
    int *v2; // rbx
    int v3[6]; // [rsp+0h] [rbp-38h] BYREF
    char v4; // [rsp+18h] [rbp-20h] BYREF

    read_six_numbers(a1, v3);
    if ( v3[0] != 1 )
        explode_bomb();
    v2 = &v3[1];
    do
    {
        result = (unsigned int)(2 * *(v2 - 1));
        if ( *v2 != (_DWORD)result )
            explode_bomb();
        ++v2;
    }
    while ( v2 != (int *)&v4 );
    return result;
}
```

1 DWORD = 4 BYTE[^2]

解释一句，DWORD基本上可以看作unsigned int。

条件就是必须满足$ v_2[i] = v_2[i-1] $。

显然，在v2指针遍历完v3后就会移动至v4，循环结束。

所以Phase2的Payload就是 `1 2 4 8 16 32`。

## Phase3

```c
__int64 __fastcall phase_3(__int64 a1)
{
    __int64 result; // rax
    int v2; // [rsp+8h] [rbp-10h] BYREF
    int v3; // [rsp+Ch] [rbp-Ch] BYREF

    if ( (int)__isoc99_sscanf(a1, "%d %d", &v2, &v3) <= 1 )
        explode_bomb();
    switch ( v2 )
    {
        case 0:
            result = 207LL;
            break;
        case 1:
            result = 311LL;
            break;
        case 2:
            result = 707LL;
            break;
        case 3:
            result = 256LL;
            break;
        case 4:
            result = 389LL;
            break;
        case 5:
            result = 206LL;
            break;
        case 6:
            result = 682LL;
            break;
        case 7:
            result = 327LL;
            break;
        default:
            explode_bomb();
            return result;
    }
    if ( (_DWORD)result != v3 )
        explode_bomb();
    return result;
}
```

这个Phase的本意是让我们逆向Assembly。而在汇编代码中 `switch ... case`语句由跳转表实现，需要一定基础才能逆向出来。

Payload任选一个：`0 207`。

## Phase4

```c
__int64 __fastcall phase_4(__int64 a1)
{
    __int64 result; // rax
    unsigned int v2; // [rsp+8h] [rbp-10h] BYREF
    int v3; // [rsp+Ch] [rbp-Ch] BYREF

    if ( (unsigned int)__isoc99_sscanf(a1, "%d %d", &v2, &v3) != 2 || v2 > 14 )
        explode_bomb();
    result = func4(v2, 0LL, 14LL);
    if ( (_DWORD)result || v3 )
        explode_bomb();
    return result;
}
```

显然我们要满足：

1. $ v_2 \leq 14$
2. $ \operatorname{func4}(v_2, 0, 14) = 0 $ & $ v_3=0 $

接下来让我们分析 `func4`。

```c
__int64 __fastcall func4(int a1, int a2, int a3)
{
    int v3; // ecx
    __int64 result; // rax

    v3 = (a3 - a2) / 2 + a2;
    if ( v3 > a1 )
        return 2 * (unsigned int)func4(a1, a2, v3 - 1);
    result = 0LL;
    if ( v3 < a1 )
        result = 2 * (unsigned int)func4(a1, v3 + 1, a3) + 1;
    return result;
}
```

显然在$ v_3=a_1 $ 的情况下函数 `func4`可以直接返回0。

此时$ a_1 = v_3 = \lfloor{\frac{(a_3 + a_2)}{2}}\rfloor=\frac{0+14}{2}=7 $。

因此这个Phase的Payload就是 `7 0`

另外一种解法是，考虑到v2可能的取值空间很小，我们可以用爆破的方法解出这道题。

## Phase5

```c
unsigned __int64 __fastcall phase_5(__int64 a1)
{
    __int64 i; // rax
    char v3[8]; // [rsp+10h] [rbp-18h] BYREF
    unsigned __int64 v4; // [rsp+18h] [rbp-10h]

    v4 = __readfsqword(0x28u);
    if ( (unsigned int)string_length((_BYTE *)a1) != 6 )
        explode_bomb();
    for ( i = 0LL; i != 6; ++i )
        v3[i] = array_3449[*(_BYTE *)(a1 + i) & 0xF];
    v3[6] = 0;
    if ( (unsigned int)strings_not_equal(v3, "flyers") )
        explode_bomb();
    return __readfsqword(0x28u) ^ v4;
}
```

我们能够分析出两点：

1. a1的长度应该是6
2. 对于每个a1中的字符，取其ASCII码的最低四位作为索引，取 `array_3449`数组的对应位数组成新的字符串，该字符串必须为 `flyers`。

导出 `array_3449`数组的值后写出exp。

```python
from string import ascii_letters, digits
array_3449 = [
  0x6D, 0x61, 0x64, 0x75, 0x69, 0x65, 0x72, 0x73, 0x6E, 0x66, 
  0x6F, 0x74, 0x76, 0x62, 0x79, 0x6C
]
target_str = "flyers"

for chrs in target_str:
    for num in array_3449:
        if ord(chrs) != num:
            continue;
        for i in range(10):
            if chr(array_3449.index(num) + i * (0xf + 1)) in ascii_letters + digits:
                print(chr(array_3449.index(num) + i * (0xf + 1)), end="")
                break
```

Phase5的payload为 `9ON567`。

## Phase6

我们可以将函数 `phase_6`分为三个sections。

### Section1

```c
v1 = v15;
read_six_numbers(a1, v15);
v2 = 0;
while ( 1 )
{
    if ( (unsigned int)(*v1 - 1) > 5 )
        explode_bomb();
    if ( ++v2 == 6 )
        break;
    v3 = v2;
    do
    {
        if ( *v1 == v15[v3] )
        explode_bomb();
        ++v3;
    }
    while ( v3 <= 5 );
    ++v1;
}
```

分析出以下点：

* 输入为6个数组，存储在 `v15`数组中。
* `v2`为循环的标定参数，在达到6时结束循环
* `v15`数组中每个数字不能大于6，也就是必须小于等于6
* 对于 `v15`数组中每个数，不能与后面的数相等

### Section2

```c
v4 = (char *)v15;
do
{
    *(_DWORD *)v4 = 7 - *(_DWORD *)v4;
    v4 += 4;
}
while ( v4 != &v16 );
```

将 `v15`数组中每个数变成7减去它自身。

### Section3

```c
for ( i = 0LL; i != 24; i += 4LL )
{
    v8 = v15[i / 4];
    if ( v8 <= 1 )
    {
        v6 = &node1;
    }
    else
    {
        v7 = 1;
        v6 = &node1;
        do
        {
        v6 = (_QWORD *)v6[1];
        ++v7;
        }
        while ( v7 != v8 );
    }
    *(__int64 *)((char *)&v17 + 2 * i) = (__int64)v6;
}
```

```c
.data:00000000006032D0 node1           dd 14Ch                 ; DATA XREF: phase_6:loc_401183↑o
.data:00000000006032D0                                         ; phase_6+B0↑o
.data:00000000006032D4                 dd 1
.data:00000000006032D8                 dd 6032E0h
.data:00000000006032DC                 dd 0
.data:00000000006032E0                 public node2
.data:00000000006032E0 node2           dd 0A8h
.data:00000000006032E4                 dd 2
.data:00000000006032E8                 dd 6032F0h
.data:00000000006032EC                 dd 0
.data:00000000006032F0                 public node3
.data:00000000006032F0 node3           dd 39Ch
.data:00000000006032F4                 dd 3
.data:00000000006032F8                 dd 603300h
.data:00000000006032FC                 dd 0
.data:0000000000603300                 public node4
.data:0000000000603300 node4           dd 2B3h
.data:0000000000603304                 dd 4
.data:0000000000603308                 dd 603310h
.data:000000000060330C                 dd 0
.data:0000000000603310                 public node5
.data:0000000000603310 node5           dd 1DDh
.data:0000000000603314                 dd 5
.data:0000000000603318                 dd 603320h
.data:000000000060331C                 dd 0
.data:0000000000603320                 public node6
.data:0000000000603320 node6           dd 1BBh
.data:0000000000603324                 dd 6
.data:0000000000603328                 dd 0
.data:000000000060332C                 dd 0
```

我们可以看到，node的结构很像一个结构体，遵循着下面的结构：

``data + id + next_node + 0``

接下来把 `node[v8[i]]`的地址保存在 `v17`中。

### Section4

```c
for ( j = v17; ; j = v12 )
{
    v12 = *(_QWORD *)v10;
    *(_QWORD *)(j + 8) = *(_QWORD *)v10;
    v10 += 8;
    if ( v10 == &v19 )
        break;
}
*(_QWORD *)(v12 + 8) = 0LL;
```

将后一个 `node`的指针指向前一个 `node`，也就是将 `node`的顺序倒过来了。

```c
v13 = 5;
do
{
    result = **(unsigned int **)(v9 + 8);
    if ( *(_DWORD *)v9 < (int)result )
        explode_bomb();
    v9 = *(_QWORD *)(v9 + 8);
    --v13;
}
while ( v13 );
```

如果（按照 `node`的顺序），前者比后者的 `data`小，那么炸弹爆炸。

我们将 `node`按照 `data`做一个排序：

``0x0A8 < 0x14C < 0x1BB < 0x1DD < 0x2B3 < 0x39C``
``2 < 1 < 6 < 5 < 4 < 3``

这是正确的顺序；逆转之后变成了

``3 4 5 6 1 2``

与7减去自身后变成了：

``4 3 2 1 5 6``

这就是Phase6的Payload。

## Summary

All-In-One:

```bash
root@workshop:~/repos/SCUCCS/C-Programming/Security Labs/lab3/bomb# cat payload 
Border relations with Canada have never been better.
1 2 4 8 16 32
0 207
7 0
9ON567
4 3 2 1 6 5
root@workshop:~/repos/SCUCCS/C-Programming/Security Labs/lab3/bomb# ./bomb payload
Welcome to my fiendish little bomb. You have 6 phases with
which to blow yourself up. Have a nice day!
Phase 1 defused. How about the next one?
That's number 2.  Keep going!
Halfway there!
So you got that one.  Try this one.
Good work!  On to the next...
Congratulations! You've defused the bomb!
```

首先，这道题最有利于锻炼自己逆向能力的解题方法是对着汇编代码硬磕。比如Phase3的switch跳转表：CSAPP中专门花了~1页来讲跳转表的机制以及为什么可以起到优化的效果。而IDA Pro[^3]无疑让这个过程缺少了不少乐趣。

其次，逆向的过程中也用到了很多tricks————比如指针乱跳、内存操作等。CSAPP Labs作为CSAPP稳坐CS神课第一把交椅的重要原因，其中的这个Bomb Lab更是让人印象深刻。可惜作为self-learning使用的bomb版本没有autoGrader，没有爆炸一次就会减二分之一分数的惩罚，让我们在拆弹的过程中少了很多惊险 & 刺激。

<!--最后吐槽一句你院：都2202年了还在用老版本的bomblab————人家CSAPP主书都在2010年从x86 architecture转向x86-64了，怎么到了你这里安全项目还开倒班车了呢。-->

## References

[^1]: [Linux SIGINT Manual Page](https://man7.org/linux/man-pages/man3/siginterrupt.3.html)
[^2]: [Bits, Bytes, Words](https://www.cs.uaf.edu/2004/fall/cs301/notes/node15.html)
[^3]: [IDA Pro - Hex Rays](https://www.hex-rays.com/ida-pro)
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

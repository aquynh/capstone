# CS_ARCH_BPF, CS_MODE_LITTLE_ENDIAN+CS_MODE_BPF_EXTENDED, None
0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00 = ldb [0x0]
0x28,0x00,0x00,0x00,0xfa,0x00,0x00,0xff = ldh [0xff0000fa]
0x40,0x10,0x00,0x00,0xcc,0x00,0x00,0x00 = ldw [r1+0xcc]
0x18,0x00,0x00,0x00,0x0c,0xb0,0xce,0xfa,0x00,0x00,0x00,0x00,0xef,0xbe,0xad,0xde = lddw 0xdeadbeeffaceb00c
0x71,0x13,0x11,0x00,0x00,0x00,0x00,0x00 = ldxb r3, [r1+0x11]
0x94,0x09,0x00,0x00,0x37,0x13,0x03,0x00 = mod r9, 0x31337
0x84,0x03,0x00,0x00,0x00,0x00,0x00,0x00 = neg r3
0x87,0x00,0x00,0x00,0x00,0x00,0x00,0x00 = neg64 r0
0xdc,0x02,0x00,0x00,0x20,0x00,0x00,0x00 = be32 r2
0x05,0x00,0x08,0x00,0x00,0x00,0x00,0x00 = jmp +0x8
0xdd,0x35,0x30,0x00,0x00,0x00,0x00,0x00 = jsle r5, r3, +0x30
0xa5,0x35,0x30,0x00,0x10,0x00,0x00,0x00 = jlt r5, 0x10, +0x30
0xc3,0x12,0x00,0x10,0x00,0x00,0x00,0x00 = xaddw [r2+0x1000], r1
0xdb,0xa9,0x00,0x01,0x00,0x00,0x00,0x00 = xadddw [r9+0x100], r10

/* Capstone Disassembler Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2013> */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <capstone.h>

struct platform {
	cs_arch arch;
	cs_mode mode;
	unsigned char *code;
	size_t size;
	char *comment;
};

static void print_string_hex(unsigned char *str, int len)
{
	unsigned char *c;

	printf("Code: ");
	for (c = str; c < str + len; c++) {
		printf("0x%02x ", *c & 0xff);
	}
	printf("\n");
}

static void test()
{
#define X86_CODE16 "\x8d\x4c\x32\x08\x01\xd8\x81\xc6\x34\x12\x00\x00"
#define X86_CODE32 "\x8d\x4c\x32\x08\x01\xd8\x81\xc6\x34\x12\x00\x00"
#define X86_CODE64 "\x55\x48\x8b\x05\xb8\x13\x00\x00"
//#define ARM_CODE "\x04\xe0\x2d\xe5"
#define ARM_CODE "\xED\xFF\xFF\xEB\x04\xe0\x2d\xe5\x00\x00\x00\x00\xe0\x83\x22\xe5\xf1\x02\x03\x0e\x00\x00\xa0\xe3\x02\x30\xc1\xe7\x00\x00\x53\xe3"
#define ARM_CODE2 "\x10\xf1\x10\xe7\x11\xf2\x31\xe7\xdc\xa1\x2e\xf3\xe8\x4e\x62\xf3"
#define THUMB_CODE "\x70\x47\xeb\x46\x83\xb0\xc9\x68"
#define THUMB_CODE2 "\x4f\xf0\x00\x01\xbd\xe8\x00\x88\xd1\xe8\x00\xf0"
#define MIPS_CODE "\x0C\x10\x00\x97\x00\x00\x00\x00\x24\x02\x00\x0c\x8f\xa2\x00\x00\x34\x21\x34\x56"
//#define MIPS_CODE "\x21\x38\x00\x01"
//#define MIPS_CODE "\x21\x30\xe6\x70"
#define MIPS_CODE2 "\x56\x34\x21\x34\xc2\x17\x01\x00"
//#define ARM64_CODE "\xe1\x0b\x40\xb9"	// ldr		w1, [sp, #0x8]
//#define ARM64_CODE "\x00\x40\x21\x4b"	// 	sub		w0, w0, w1, uxtw
//#define ARM64_CODE "\x21\x7c\x02\x9b"	// mul	x1, x1, x2
//#define ARM64_CODE "\x20\x74\x0b\xd5"	// dc	zva, x0
#define ARM64_CODE "\x21\x7c\x02\x9b\x21\x7c\x00\x53\x00\x40\x21\x4b\xe1\x0b\x40\xb9\x10\x20\x21\x1e"

	struct platform platforms[] = {
		{
			.arch = CS_ARCH_X86,
			.mode = CS_MODE_16,
			.code = (unsigned char *)X86_CODE16,
			.size = sizeof(X86_CODE32) - 1,
			.comment = "X86 16bit (Intel syntax)"
		},
		{
			.arch = CS_ARCH_X86,
			.mode = CS_MODE_32 + CS_MODE_SYNTAX_ATT,
			.code = (unsigned char *)X86_CODE32,
			.size = sizeof(X86_CODE32) - 1,
			.comment = "X86 32bit (ATT syntax)"
		},
		{
			.arch = CS_ARCH_X86,
			.mode = CS_MODE_32,
			.code = (unsigned char *)X86_CODE32,
			.size = sizeof(X86_CODE32) - 1,
			.comment = "X86 32 (Intel syntax)"
		},
		{
			.arch = CS_ARCH_X86,
			.mode = CS_MODE_64,
			.code = (unsigned char *)X86_CODE64,
			.size = sizeof(X86_CODE64) - 1,
			.comment = "X86 64 (Intel syntax)"
		},
		{
			.arch = CS_ARCH_ARM,
			.mode = CS_MODE_ARM,
			.code = (unsigned char *)ARM_CODE,
			.size = sizeof(ARM_CODE) - 1,
			.comment = "ARM"
		},
		{
			.arch = CS_ARCH_ARM,
			.mode = CS_MODE_THUMB,
			.code = (unsigned char *)THUMB_CODE2,
			.size = sizeof(THUMB_CODE2) - 1,
			.comment = "THUMB-2"
		},
		{
			.arch = CS_ARCH_ARM,
			.mode = CS_MODE_ARM,
			.code = (unsigned char *)ARM_CODE2,
			.size = sizeof(ARM_CODE2) - 1,
			.comment = "ARM: Cortex-A15 + NEON"
		},
		{
			.arch = CS_ARCH_ARM,
			.mode = CS_MODE_THUMB,
			.code = (unsigned char *)THUMB_CODE,
			.size = sizeof(THUMB_CODE) - 1,
			.comment = "THUMB"
		},
		{
			.arch = CS_ARCH_MIPS,
			.mode = CS_MODE_32 + CS_MODE_BIG_ENDIAN,
			.code = (unsigned char *)MIPS_CODE,
			.size = sizeof(MIPS_CODE) - 1,
			.comment = "MIPS-32 (Big-endian)"
		},
		{
			.arch = CS_ARCH_MIPS,
			.mode = CS_MODE_64+ CS_MODE_LITTLE_ENDIAN,
			.code = (unsigned char *)MIPS_CODE2,
			.size = sizeof(MIPS_CODE2) - 1,
			.comment = "MIPS-64-EL (Little-endian)"
		},
		{
			.arch = CS_ARCH_ARM64,
			.mode = CS_MODE_ARM,
			.code = (unsigned char *)ARM64_CODE,
			.size = sizeof(ARM64_CODE) - 1,
			.comment = "ARM-64"
		},
	};

	csh handle;
	size_t address = 0x1000;
	//cs_insn all_insn[16];
	cs_insn *all_insn;
	int i;

	for (i = 0; i < sizeof(platforms)/sizeof(platforms[0]); i++) {
		if (cs_open(platforms[i].arch, platforms[i].mode, &handle))
			return;

		//size_t count = cs_disasm(handle, platforms[i].code, platforms[i].size, address, 0, all_insn);
		size_t count = cs_disasm_dyn(handle, platforms[i].code, platforms[i].size, address, 0, &all_insn);
		if (count) {
			printf("****************\n");
			printf("Platform: %s\n", platforms[i].comment);
			print_string_hex(platforms[i].code, platforms[i].size);
			printf("Disasm:\n");

			size_t j;
			int n;
			for (j = 0; j < count; j++) {
				cs_insn *i = &(all_insn[j]);
				printf("0x%" PRIu64 ":\t%s\t\t%s // insn-ID: %u, insn-mnem: %s\n",
						(uint64_t) i->address, i->mnemonic, i->op_str,
						i->id, cs_insn_name(handle, i->id));

				// print implicit registers used by this instruction
				if (i->regs_read[0] != 0) {
					printf("\tImplicit registers read: ");
					for (n = 0; n < 32; n++) {
						if (i->regs_read[n] == 0)
							break;
						printf("%s ", cs_reg_name(handle, i->regs_read[n]));
					}
					printf("\n");
				}

				// print implicit registers modified by this instruction
				if (i->regs_write[0] != 0) {
					printf("\tImplicit registers modified: ");
					for (n = 0; n < 32; n++) {
						if (i->regs_write[n] == 0)
							break;
						printf("%s ", cs_reg_name(handle, i->regs_write[n]));
					}
					printf("\n");
				}

				// print the groups this instruction belong to
				if (i->groups[0] != 0) {
					printf("\tThis instruction belongs to groups: ");
					for (n = 0; n < 8; n++) {
						if (i->groups[n] == 0)
							break;
						printf("%u ", i->groups[n]);
					}
					printf("\n");
				}
			}

			// print out the next offset, after the last insn
			printf("0x%" PRIu64 ":\n", (uint64_t) all_insn[j-1].address + all_insn[j-1].size);

			// free memory allocated by cs_disasm_dyn()
			cs_free(all_insn);
		} else {
			printf("****************\n");
			printf("Platform: %s\n", platforms[i].comment);
			print_string_hex(platforms[i].code, platforms[i].size);
			printf("ERROR: Failed to disasm given code!\n");
		}

		printf("\n");

		cs_close(handle);
	}
}

int main()
{
	test();

	return 0;
}

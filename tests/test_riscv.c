

#include <stdio.h>
#include <stdlib.h>

#include <platform.h>
#include <capstone.h>

struct platform {
	cs_arch arch;
	cs_mode mode;
	unsigned char *code;
	size_t size;
	char *comment;
};

static csh handle;

static void print_string_hex(char *comment, unsigned char *str, size_t len)
{
	unsigned char *c;

	printf("%s", comment);
	for (c = str; c < str + len; c++) {
		printf("0x%02x ", *c & 0xff);
	}

	printf("\n");
}

//TODO_rod: after initial testing, print with details.
static void print_insn_detail(cs_insn *ins)
{
	int i;
	int n;
	cs_riscv *riscv;
	cs_detail *detail;

	// detail can be NULL on "data" instruction if SKIPDATA option is turned ON
	if (ins->detail == NULL)
		return;

	riscv = &(ins->detail->riscv);
	detail = ins->detail;
	if (riscv->op_count)
		printf("\top_count: %u\n", riscv->op_count);

	for (i = 0; i < riscv->op_count; i++) {
		cs_riscv_op *op = &(riscv->operands[i]);
		switch((int)op->type) {
			default:
				break;
			case RISCV_OP_REG:
				printf("\t\toperands[%u].type: REG = %s\n", i, cs_reg_name(handle, op->reg));
				break;
			case RISCV_OP_IMM:
				printf("\t\toperands[%u].type: IMM = 0x%" PRIx64 "\n", i, op->imm);
				break;
			case RISCV_OP_MEM:
				printf("\t\toperands[%u].type: MEM\n", i);
				if (op->mem.base != RISCV_REG_INVALID)
					printf("\t\t\toperands[%u].mem.base: REG = %s\n",
							i, cs_reg_name(handle, op->mem.base));
				if (op->mem.disp != 0)
					printf("\t\t\toperands[%u].mem.disp: 0x%" PRIx64 "\n", i, op->mem.disp);

				break;
		}

	}
	
	//print the groups this instruction belongs to
	if (detail->groups_count > 0) {
		printf("\tThis instruction belongs to groups: ");
		for (n = 0; n < detail->groups_count; n++) {
			printf("%s ", cs_group_name(handle, detail->groups[n]));
		}
		printf("\n");
	}

	printf("\n");
}

static void test()
{
	//   \xb3\x00\x00\x00 for add ra, zero, zero
	//	\x93\x00\x21\x00
	//  \xb3\x70\x31\x00
#define RISCV_CODE "\xb3\x00\x00\x00\x93\x00\x21\x00\xb3\x70\x31\x00"

	struct platform platforms[] = {
		{
			CS_ARCH_RISCV,
			CS_MODE_LITTLE_ENDIAN,
			(unsigned char *)RISCV_CODE,
			sizeof(RISCV_CODE) - 1,
			"RISCV"
		},
	};
	
	uint64_t address = 0x1000;
	cs_insn *insn;
	int i;
	size_t count;

	for (i = 0; i < sizeof(platforms)/sizeof(platforms[0]); i++) {
		cs_err err = cs_open(platforms[i].arch, platforms[i].mode, &handle);
		if (err) {
			printf("Failed on cs_open() with error returned: %u\n", err);
			continue;
		}
		
		//To turn on or off the Print Details option
		//cs_option(handle, CS_OPT_DETAIL, CS_OPT_OFF); 
		cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);

		count = cs_disasm(handle, platforms[i].code, platforms[i].size, address, 0, &insn);
		if (count) {
			size_t j;

			printf("****************\n");
			printf("Platform: %s\n", platforms[i].comment);
			print_string_hex("Code:", platforms[i].code, platforms[i].size);
			printf("Disasm:\n");

			for (j = 0; j < count; j++) {
				printf("0x%" PRIx64 ":\t%s\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
				print_insn_detail(&insn[j]);
			}
			printf("0x%" PRIx64 ":\n", insn[j-1].address + insn[j-1].size);

			// free memory allocated by cs_disasm()
			cs_free(insn, count);
		} else {
			printf("****************\n");
			printf("Platform: %s\n", platforms[i].comment);
			print_string_hex("Code:", platforms[i].code, platforms[i].size);
			printf("ERROR: Failed to disasm given code!\n");
		}

		printf("\n");

		cs_close(&handle);
	}
}

int main()
{
	test();

	return 0;
}

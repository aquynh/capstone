/* Capstone Disassembler Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2013> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <capstone.h>

#include "cs_priv.h"

#include "MCRegisterInfo.h"

#ifdef CS_SUPPORT_X86
#include "arch/X86/X86Disassembler.h"
#include "arch/X86/X86InstPrinter.h"
#include "arch/X86/mapping.h"
#endif /* CS_SUPPORT_X86 */

#ifdef CS_SUPPORT_ARM
#include "arch/ARM/ARMDisassembler.h"
#include "arch/ARM/ARMInstPrinter.h"
#include "arch/ARM/mapping.h"
#endif /* CS_SUPPORT_ARM */

#ifdef CS_SUPPORT_MIPS
#include "arch/Mips/MipsDisassembler.h"
#include "arch/Mips/MipsInstPrinter.h"
#include "arch/Mips/mapping.h"
#endif /* CS_SUPPORT_MIPS */

#ifdef CS_SUPPORT_AARCH64
#include "arch/AArch64/AArch64Disassembler.h"
#include "arch/AArch64/AArch64InstPrinter.h"
#include "arch/AArch64/mapping.h"
#endif /* CS_SUPPORT_AARCH64 */

#include "utils.h"

// Package version
#define PKG_MAJOR 1
#define PKG_MINOR 0

// API version
#define API_MAJOR 1
#define API_MINOR 7

cs_err cs_errno(csh handle)
{
	if (!handle)
		return CS_ERR_CSH;

	cs_struct *ud = (cs_struct *)(uintptr_t)handle;

	return ud->errnum;
}

void cs_version(int *major, int *minor)
{
	*major = API_MAJOR;
	*minor = API_MINOR;
}

cs_err cs_open(cs_arch arch, cs_mode mode, csh *handle)
{
	cs_struct *ud;

	ud = calloc(1, sizeof(*ud));
	if (!ud) {
		// memory insufficient
		return CS_ERR_MEM;
	}

	ud->errnum = CS_ERR_OK;
	ud->arch = arch;
	ud->mode = mode;
	ud->big_endian = mode & CS_MODE_BIG_ENDIAN;
	ud->reg_name = NULL;

	switch (ud->arch) {
#ifdef CS_SUPPORT_X86
		case CS_ARCH_X86:
			// by default, we use Intel syntax
			ud->printer = X86_Intel_printInst;
			ud->printer_info = NULL;
			ud->disasm = X86_getInstruction;
			ud->reg_name = X86_reg_name;
			ud->insn_id = X86_get_insn_id;
			ud->insn_name = X86_insn_name;
			break;
#endif /* CS_SUPPORT_X86 */
#ifdef CS_SUPPORT_ARM
		case CS_ARCH_ARM: {
					MCRegisterInfo *mri = malloc(sizeof(*mri));

					ARM_init(mri);

					ud->printer = ARM_printInst;
					ud->printer_info = mri;
					ud->reg_name = ARM_reg_name;
					ud->insn_id = ARM_get_insn_id;
					ud->insn_name = ARM_insn_name;
					ud->post_printer = ARM_post_printer;

					if (ud->mode & CS_MODE_THUMB)
						ud->disasm = Thumb_getInstruction;
					else
						ud->disasm = ARM_getInstruction;
					break;
				}
#endif /* CS_SUPPORT_ARM */
#ifdef CS_SUPPORT_MIPS
		case CS_ARCH_MIPS: {
				   MCRegisterInfo *mri = malloc(sizeof(*mri));

				   Mips_init(mri);
				   ud->printer = Mips_printInst;
				   ud->printer_info = mri;
				   ud->getinsn_info = mri;
				   ud->reg_name = Mips_reg_name;
				   ud->insn_id = Mips_get_insn_id;
				   ud->insn_name = Mips_insn_name;

				   if (ud->mode & CS_MODE_32)
					   ud->disasm = Mips_getInstruction;
				   else
					   ud->disasm = Mips64_getInstruction;

				   if (ud->mode & CS_MODE_MICRO)
					   ud->micro_mips = true;

				   break;
			}
#endif /* CS_SUPPORT_MIPS */
#ifdef CS_SUPPORT_AARCH64
		case CS_ARCH_ARM64: {
					MCRegisterInfo *mri = malloc(sizeof(*mri));

					AArch64_init(mri);
					ud->printer = AArch64_printInst;
					ud->printer_info = mri;
					ud->getinsn_info = mri;
					ud->disasm = AArch64_getInstruction;
					ud->reg_name = AArch64_reg_name;
					ud->insn_id = AArch64_get_insn_id;
					ud->insn_name = AArch64_insn_name;
					ud->post_printer = AArch64_post_printer;
					break;
			}
#endif /* CS_SUPPORT_AARCH64 */
		default:	// unsupported architecture
			free(ud);
			return CS_ERR_ARCH;
	}

	*handle = (uintptr_t)ud;

	return CS_ERR_OK;
}

cs_err cs_close(csh handle)
{
	if (!handle)
		return CS_ERR_CSH;

	cs_struct *ud = (cs_struct *)(uintptr_t)handle;

	switch (ud->arch) {
		case CS_ARCH_X86:
			break;
		case CS_ARCH_ARM:
		case CS_ARCH_MIPS:
		case CS_ARCH_ARM64:
			free(ud->printer_info);
			break;
		default:	// unsupported architecture
			return CS_ERR_HANDLE;
	}

	memset(ud, 0, sizeof(*ud));
	free(ud);

	return CS_ERR_OK;
}

#define MIN(x, y) ((x) < (y) ? (x) : (y))

// fill insn with mnemonic & operands info
static void fill_insn(cs_struct *handle, cs_insn *insn, char *buffer, MCInst *mci,
		PostPrinter_t printer, unsigned char *code)
{
	memcpy(insn, &mci->pub_insn, sizeof(*insn));

	// map internal instruction opcode to public insn ID
	if (handle->insn_id)
		handle->insn_id(insn, MCInst_getOpcode(mci));

	// alias instruction might have ID saved in OpcodePub
	if (MCInst_getOpcodePub(mci))
		insn->id = MCInst_getOpcodePub(mci);

	if (printer)
		printer(insn->id, insn, buffer);

	// fill in mnemonic & operands
	char *tab = strchr(buffer, '\t');
	if (tab) {
		*tab = '\0';
		strncpy(insn->op_str, tab + 1, sizeof(insn->op_str) - 1);
		insn->op_str[sizeof(insn->op_str) - 1] = '\0';
	} else
		insn->op_str[0] = '\0';

	strncpy(insn->mnemonic, buffer, sizeof(insn->mnemonic) - 1);
	insn->mnemonic[sizeof(insn->mnemonic) - 1] = '\0';

	// fill the instruction bytes
	memcpy(insn->bytes, code, MIN(sizeof(insn->bytes), insn->size));
}

cs_err cs_option(csh ud, cs_opt_type type, size_t value)
{
	cs_struct *handle = (cs_struct *)(uintptr_t)ud;
	if (!handle)
		return CS_ERR_CSH;

	switch (handle->arch) {
		default:
			break;
#ifdef CS_SUPPORT_X86
		case CS_ARCH_X86:
			if (type & CS_OPT_SYNTAX) {
				switch(value) {
					default:
						break;
					case CS_OPT_SYNTAX_INTEL:
						handle->printer = X86_Intel_printInst;
						break;
					case CS_OPT_SYNTAX_ATT:
						handle->printer = X86_ATT_printInst;
						break;
				}
			}
			break;
#endif /* CS_SUPPORT_X86 */
	}

	return CS_ERR_OK;
}

size_t cs_disasm(csh ud, unsigned char *buffer, size_t size, uint64_t offset, size_t count, cs_insn *insn)
{
	cs_struct *handle = (cs_struct *)(uintptr_t)ud;
	MCInst mci;
	uint16_t insn_size;
	size_t c = 0;

	if (!handle) {
		// FIXME: handle this case?
		// handle->errnum = CS_ERR_HANDLE;
		return 0;
	}

	handle->errnum = CS_ERR_OK;

	while (size > 0) {
		MCInst_Init(&mci);	

		bool r = handle->disasm(ud, buffer, size, &mci, &insn_size, offset, handle->getinsn_info);
		if (r) {
			SStream ss;
			SStream_Init(&ss);

			mci.pub_insn.size = insn_size;
			mci.pub_insn.address = offset;
			mci.mode = handle->mode;
			handle->printer(&mci, &ss, handle->printer_info);

			fill_insn(handle, insn, ss.buffer, &mci, handle->post_printer, buffer);

			c++;
			insn++;
			buffer += insn_size;
			size -= insn_size;
			offset += insn_size;

			if (count > 0) {
				if (c == count)
					return c;
			}
		} else
			// face a broken instruction?
			return c;
	}

	return c;
}

// dynamicly allocate memory to contain disasm insn
// NOTE: caller must free() the allocated memory itself to avoid memory leaking
size_t cs_disasm_dyn(csh ud, unsigned char *buffer, size_t size, uint64_t offset, size_t count, cs_insn **insn)
{
	cs_struct *handle = (cs_struct *)(uintptr_t)ud;
	MCInst mci;
	uint16_t insn_size;
	size_t c = 0, f = 0;
	cs_insn insn_cache[64];
	void *total = NULL;
	size_t total_size = 0;

	if (!handle) {
		// FIXME: how to handle this case:
		// handle->errnum = CS_ERR_HANDLE;
		return 0;
	}

	handle->errnum = CS_ERR_OK;

	while (size > 0) {
		MCInst_Init(&mci);	

		bool r = handle->disasm(ud, buffer, size, &mci, &insn_size, offset, handle->getinsn_info);
		if (r) {
			SStream ss;
			SStream_Init(&ss);

			mci.pub_insn.size = insn_size;
			mci.pub_insn.address = offset;
			mci.mode = handle->mode;
			handle->printer(&mci, &ss, handle->printer_info);

			fill_insn(handle, &insn_cache[f], ss.buffer, &mci, handle->post_printer, buffer);
			f++;

			if (f == ARR_SIZE(insn_cache)) {
				// resize total to contain newly disasm insns
				total_size += sizeof(insn_cache);
				void *tmp = realloc(total, total_size);
				if (tmp == NULL) {	// insufficient memory
					free(total);
					handle->errnum = CS_ERR_MEM;
					return 0;
				}

				total = tmp;
				memcpy(total + total_size - sizeof(insn_cache), insn_cache, sizeof(insn_cache));
				// reset f back to 0
				f = 0;
			}

			c++;
			buffer += insn_size;
			size -= insn_size;
			offset += insn_size;

			if (count > 0 && c == count)
				break;
		} else	{
			// encounter a broken instruction
			// XXX: TODO: JOXEAN continue here
			break;
		}
	}

	if (f) {
		// resize total to contain newly disasm insns
		void *tmp = realloc(total, total_size + f * sizeof(insn_cache[0]));
		if (tmp == NULL) {	// insufficient memory
			free(total);
			handle->errnum = CS_ERR_MEM;
			return 0;
		}

		total = tmp;
		memcpy(total + total_size, insn_cache, f * sizeof(insn_cache[0]));
	}

	*insn = total;

	return c;
}

void cs_free(void *m)
{
	free(m);
}

// return friendly name of regiser in a string
char *cs_reg_name(csh ud, unsigned int reg)
{
	cs_struct *handle = (cs_struct *)(uintptr_t)ud;

	if (!handle || handle->reg_name == NULL) {
		return NULL;
	}

	return handle->reg_name(reg);
}

char *cs_insn_name(csh ud, unsigned int insn)
{
	cs_struct *handle = (cs_struct *)(uintptr_t)ud;

	if (!handle || handle->insn_name == NULL) {
		return NULL;
	}

	return handle->insn_name(insn);
}

static bool arr_exist(unsigned int *arr, int max, unsigned int id)
{
	int i;

	for (i = 0; i < max; i++) {
		if (arr[i] == id)
			return true;
	}

	return false;
}

bool cs_insn_group(csh handle, cs_insn *insn, unsigned int group_id)
{
	if (!handle)
		return false;

	return arr_exist(insn->groups, insn->groups_count, group_id);
}

bool cs_reg_read(csh handle, cs_insn *insn, unsigned int reg_id)
{
	if (!handle)
		return false;

	return arr_exist(insn->regs_read, insn->regs_read_count, reg_id);
}

bool cs_reg_write(csh handle, cs_insn *insn, unsigned int reg_id)
{
	if (!handle)
		return false;

	return arr_exist(insn->regs_write, insn->regs_write_count, reg_id);
}

int cs_op_count(csh ud, cs_insn *insn, unsigned int op_type)
{
	if (!ud)
		return -1;

	cs_struct *handle = (cs_struct *)(uintptr_t)ud;
	unsigned int count = 0, i;

	handle->errnum = CS_ERR_OK;

	switch (handle->arch) {
		default:
			handle->errnum = CS_ERR_HANDLE;
			return -1;
#ifdef CS_SUPPORT_ARM 
		case CS_ARCH_ARM:
			for (i = 0; i < insn->arm.op_count; i++)
				if (insn->arm.operands[i].type == op_type)
					count++;
			break;
#endif /* CS_SUPPORT_ARM */
#ifdef CS_SUPPORT_AARCH64
		case CS_ARCH_ARM64:
			for (i = 0; i < insn->arm64.op_count; i++)
				if (insn->arm64.operands[i].type == op_type)
					count++;
			break;
#endif /* CS_SUPPORT_AARCH64 */
#ifdef CS_SUPPORT_X86
		case CS_ARCH_X86:
			for (i = 0; i < insn->x86.op_count; i++)
				if (insn->x86.operands[i].type == op_type)
					count++;
			break;
#endif /* CS_SUPPORT_X86 */
#ifdef CS_SUPPORT_MIPS
		case CS_ARCH_MIPS:
			for (i = 0; i < insn->mips.op_count; i++)
				if (insn->mips.operands[i].type == op_type)
					count++;
			break;
#endif /* CS_SUPPORT_MIPS */
	}

	return count;
}

int cs_op_index(csh ud, cs_insn *insn, unsigned int op_type,
		unsigned int post)
{
	if (!ud)
		return -1;

	cs_struct *handle = (cs_struct *)(uintptr_t)ud;
	unsigned int count = 0, i;

	handle->errnum = CS_ERR_OK;

	switch (handle->arch) {
		default:
			handle->errnum = CS_ERR_HANDLE;
			return -1;
#ifdef CS_SUPPORT_ARM 
		case CS_ARCH_ARM:
			for (i = 0; i < insn->arm.op_count; i++) {
				if (insn->arm.operands[i].type == op_type)
					count++;
				if (count == post)
					return i;
			}
			break;
#endif /* CS_SUPPORT_ARM */
#ifdef CS_SUPPORT_AARCH64
		case CS_ARCH_ARM64:
			for (i = 0; i < insn->arm64.op_count; i++) {
				if (insn->arm64.operands[i].type == op_type)
					count++;
				if (count == post)
					return i;
			}
			break;
#endif /* CS_SUPPORT_AARCH64 */
#ifdef CS_SUPPORT_X86
		case CS_ARCH_X86:
			for (i = 0; i < insn->x86.op_count; i++) {
				if (insn->x86.operands[i].type == op_type)
					count++;
				if (count == post)
					return i;
			}
			break;
#endif /* CS_SUPPORT_X86 */
#ifdef CS_SUPPORT_MIPS
		case CS_ARCH_MIPS:
			for (i = 0; i < insn->mips.op_count; i++) {
				if (insn->mips.operands[i].type == op_type)
					count++;
				if (count == post)
					return i;
			}
			break;
#endif /* CS_SUPPORT_MIPS */
	}

	return -1;
}

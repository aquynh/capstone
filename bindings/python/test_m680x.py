#!/usr/bin/env python

# Capstone Python bindings, by Nguyen Anh Quynnh <aquynh@gmail.com>

from __future__ import print_function
import sys
from capstone import *
from capstone.m680x import *
_python3 = sys.version_info.major == 3


address_modes = (
	"M680X_AM_NONE",
	"M680X_AM_INHERENT",
	"M680X_AM_REGISTER",
	"M680X_AM_IMMEDIATE",
	"M680X_AM_INDEXED",
	"M680X_AM_EXTENDED",
	"M680X_AM_DIRECT",
	"M680X_AM_RELATIVE",
	"M680X_AM_IMM_DIRECT",
	"M680X_AM_IMM_INDEXED",
	"M680X_AM_IMM_EXTENDED",
	"M680X_AM_BIT_MOVE",
	"M680X_AM_INDEXED2",
	"M680X_AM_DIR_IMM_REL",
	"M680X_AM_IDX_IMM_REL",
	"M680X_AM_DIRECT_IMM",
	"M680X_AM_INDEXED_IMM",
	"M680X_AM_IDX_DIR_REL",
	"M680X_AM_IDX_DIRECT",
	"M680X_AM_IMM_REL",
        "M680X_AM_DIRECT_REL",
        "M680X_AM_INDEXED_REL",
        "M680X_AM_DIRECT_IDX",
        "M680X_AM_DIRECT2",
        "M680X_AM_INDEXED_DIR",
        "M680X_AM_EXTENDED_IMM",
        "M680X_AM_EXT_IMM_REL",
        "M680X_AM_EXT_PAGE",
        "M680X_AM_IDX_PAGE",
        "M680X_AM_REG_RELATIVE",
        "M680X_AM_EXT_EXT",
        "M680X_AM_INDEXED_EXT",
        "M680X_AM_EXT_INDEXED",
	)

s_access = (
	"UNCHANGED", "READ", "WRITE", "READ | WRITE",
	)

M6800_CODE = b"\x01\x09\x36\x64\x7f\x74\x10\x00\x90\x10\xA4\x10\xb6\x10\x00\x39"

M6801_CODE = b"\x04\x05\x3c\x3d\x38\x93\x10\xec\x10\xed\x10\x39"
M6805_CODE = b"\x04\x7f\x00\x17\x22\x28\x00\x2e\x00\x40\x42\x5a\x70\x8e\x97\x9c\xa0\x15\xad\x00\xc3\x10\x00\xda\x12\x34\xe5\x7f\xfe"
M6808_CODE = b"\x31\x22\x00\x35\x22\x45\x10\x00\x4b\x00\x51\x10\x52\x5e\x22\x62\x65\x12\x34\x72\x84\x85\x86\x87\x8a\x8b\x8c\x94\x95\xa7\x10\xaf\x10\x9e\x60\x7f\x9e\x6b\x7f\x00\x9e\xd6\x10\x00\x9e\xe6\x7f"
HCS08_CODE = b"\x32\x10\x00\x9e\xae\x9e\xce\x7f\x9e\xbe\x10\x00\x9e\xfe\x7f\x3e\x10\x00\x9e\xf3\x7f\x96\x10\x00\x9e\xff\x7f\x82"
HD6301_CODE = b"\x6b\x10\x00\x71\x10\x00\x72\x10\x10\x39"
M6809_CODE = b"\x06\x10\x19\x1a\x55\x1e\x01\x23\xe9\x31\x06\x34\x55\xa6\x81\xa7\x89\x7f\xff\xa6\x9d\x10\x00\xa7\x91\xa6\x9f\x10\x00\x11\xac\x99\x10\x00\x39\xA6\x07\xA6\x27\xA6\x47\xA6\x67\xA6\x0F\xA6\x10\xA6\x80\xA6\x81\xA6\x82\xA6\x83\xA6\x84\xA6\x85\xA6\x86\xA6\x88\x7F\xA6\x88\x80\xA6\x89\x7F\xFF\xA6\x89\x80\x00\xA6\x8B\xA6\x8C\x10\xA6\x8D\x10\x00\xA6\x91\xA6\x93\xA6\x94\xA6\x95\xA6\x96\xA6\x98\x7F\xA6\x98\x80\xA6\x99\x7F\xFF\xA6\x99\x80\x00\xA6\x9B\xA6\x9C\x10\xA6\x9D\x10\x00\xA6\x9F\x10\x00"
M6811_CODE = b"\x02\x03\x12\x7f\x10\x00\x13\x99\x08\x00\x14\x7f\x02\x15\x7f\x01\x1e\x7f\x20\x00\x8f\xcf\x18\x08\x18\x30\x18\x3c\x18\x67\x18\x8c\x10\x00\x18\x8f\x18\xce\x10\x00\x18\xff\x10\x00\x1a\xa3\x7f\x1a\xac\x1a\xee\x7f\x1a\xef\x7f\xcd\xac\x7f"
CPU12_CODE = b"\x00\x04\x01\x00\x0c\x00\x80\x0e\x00\x80\x00\x11\x1e\x10\x00\x80\x00\x3b\x4a\x10\x00\x04\x4b\x01\x04\x4f\x7f\x80\x00\x8f\x10\x00\xb7\x52\xb7\xb1\xa6\x67\xa6\xfe\xa6\xf7\x18\x02\xe2\x30\x39\xe2\x10\x00\x18\x0c\x30\x39\x10\x00\x18\x11\x18\x12\x10\x00\x18\x19\x00\x18\x1e\x00\x18\x3e\x18\x3f\x00"
HD6309_CODE = b"\x01\x10\x10\x62\x10\x10\x7b\x10\x10\x00\xcd\x49\x96\x02\xd2\x10\x30\x23\x10\x38\x10\x3b\x10\x53\x10\x5d\x11\x30\x43\x10\x11\x37\x25\x10\x11\x38\x12\x11\x39\x23\x11\x3b\x34\x11\x8e\x10\x00\x11\xaf\x10\x11\xab\x10\x11\xf6\x80\x00"

all_tests = (
        (CS_ARCH_M680X, CS_MODE_M680X_6301, HD6301_CODE, "M680X_HD6301", None),
        (CS_ARCH_M680X, CS_MODE_M680X_6309, HD6309_CODE, "M680X_HD6309", None),
        (CS_ARCH_M680X, CS_MODE_M680X_6800, M6800_CODE, "M680X_M6800", None),
        (CS_ARCH_M680X, CS_MODE_M680X_6801, M6801_CODE, "M680X_M6801", None),
        (CS_ARCH_M680X, CS_MODE_M680X_6805, M6805_CODE, "M680X_M68HC05", None),
        (CS_ARCH_M680X, CS_MODE_M680X_6808, M6808_CODE, "M680X_M68HC08", None),
        (CS_ARCH_M680X, CS_MODE_M680X_6809, M6809_CODE, "M680X_M6809", None),
        (CS_ARCH_M680X, CS_MODE_M680X_6811, M6811_CODE, "M680X_M68HC11", None),
        (CS_ARCH_M680X, CS_MODE_M680X_CPU12, CPU12_CODE, "M680X_CPU12", None),
        (CS_ARCH_M680X, CS_MODE_M680X_HCS08, HCS08_CODE, "M680X_HCS08", None),
        )

# print hex dump from string all upper case
def to_hex_uc(string):
    if _python3:
        return " ".join("0x%02X" % c for c in string)
    else:
        return " ".join("0x%02X" % ord(c) for c in string)

# print short hex dump from byte array all upper case
def to_hex_short_uc(byte_array):
    return "".join("%02X" % b for b in byte_array)

def print_insn_detail(insn):
    # print address, mnemonic and operands
    #print("0x%x:\t%s\t%s\t%s" % (insn.address, binascii.hexlify(bytearray(insn.bytes)), \
    print("0x%04X: %s\t%s\t%s" % (insn.address, to_hex_short_uc(insn.bytes), \
	insn.mnemonic, insn.op_str))

    # "data" instruction generated by SKIPDATA option has no detail
    if insn.id == 0:
        return

    print("\taddress_mode: %s" % address_modes[insn.address_mode])

    if len(insn.operands) > 0:
        print("\top_count: %u" % len(insn.operands))
        c = 0
        for i in insn.operands:
            if i.type == M680X_OP_REGISTER:
                comment = "";
                if (((c == 0) and (insn.flags & M680X_FIRST_OP_IN_MNEM)) or
                    ((c == 1) and (insn.flags & M680X_SECOND_OP_IN_MNEM))):
                    comment = " (in mnemonic)";
                print("\t\toperands[%u].type: REGISTER = %s%s" % (c,
                    insn.reg_name(i.reg), comment))
            if i.type == M680X_OP_INDEX:
                print("\t\toperands[%u].type: INDEX = %u" % (c, i.index))
            if i.type == M680X_OP_IMMEDIATE:
                print("\t\toperands[%u].type: IMMEDIATE = #%d" % (c, i.imm))
            if i.type == M680X_OP_DIRECT:
                print("\t\toperands[%u].type: DIRECT = 0x%02X" % (c, i.direct_addr))
            if i.type == M680X_OP_EXTENDED:
                if i.ext.indirect:
                    indirect = "INDIRECT"
                else:
                    indirect = ""
                print("\t\toperands[%u].type: EXTENDED %s = 0x%04X" % (c, indirect, i.ext.address))
            if i.type == M680X_OP_RELATIVE:
                print("\t\toperands[%u].type: RELATIVE = 0x%04X" % (c, i.rel.address))
            if i.type == M680X_OP_INDEXED:
                if (i.idx.flags & M680X_IDX_INDIRECT):
                    indirect = " INDIRECT"
                else:
                    indirect = ""
                print("\t\toperands[%u].type: INDEXED%s" % (c, indirect))
                if i.idx.base_reg != M680X_REG_INVALID:
                    print("\t\t\tbase register: %s" % insn.reg_name(i.idx.base_reg))
                if i.idx.offset_reg != M680X_REG_INVALID:
                    print("\t\t\toffset register: %s" % insn.reg_name(i.idx.offset_reg))
                if (i.idx.offset_bits != 0) and (i.idx.offset_reg == M680X_REG_INVALID) and (i.idx.inc_dec == 0):
                    print("\t\t\toffset: %u" % i.idx.offset)
                    if i.idx.base_reg == M680X_REG_PC:
                        print("\t\t\toffset address: 0x%04X" % i.idx.offset_addr)
                    print("\t\t\toffset bits: %u" % i.idx.offset_bits)
                if i.idx.inc_dec != 0:
                    if i.idx.flags & M680X_IDX_POST_INC_DEC:
                       s_post_pre = "post"
                    else:
                       s_post_pre = "pre"
                    if i.idx.inc_dec > 0:
                        s_inc_dec = "increment"
                    else:
                        s_inc_dec = "decrement"
                    print("\t\t\t%s %s: %d" %
			(s_post_pre, s_inc_dec, abs(i.idx.inc_dec)))
            if (i.size != 0):
                print("\t\t\tsize: %d" % i.size)
            if (i.access != CS_AC_INVALID):
                print("\t\t\taccess: %s" % s_access[i.access])

            c += 1

    (regs_read, regs_write) = insn.regs_access()

    if len(regs_read) > 0:
        print("\tRegisters read:", end="")
        for r in regs_read:
            print(" %s" %(insn.reg_name(r)), end="")
        print("")

    if len(regs_write) > 0:
        print("\tRegisters modified:", end="")
        for r in regs_write:
            print(" %s" %(insn.reg_name(r)), end="")
        print("")

    if len(insn.groups) > 0:
         print("\tgroups_count: %u" % len(insn.groups))

# ## Test class Cs
def test_class():

    for (arch, mode, code, comment, syntax) in all_tests:
        print("*" * 20)
        print("Platform: %s" % comment)
        print("Code: %s" % to_hex_uc(code))
        print("Disasm:")

        try:
            md = Cs(arch, mode)
            if syntax is not None:
                md.syntax = syntax
            md.detail = True
            for insn in md.disasm(code, 0x1000):
                print_insn_detail(insn)
                print ()
        except CsError as e:
            print("ERROR: %s" % e)


if __name__ == '__main__':
    test_class()

(* For Capstone Engine. AUTO-GENERATED FILE, DO NOT EDIT [m680x_const.ml] *)
let _M680X_OPERAND_COUNT = 9;;

(* M680X registers and special registers *)

let _M680X_REG_INVALID = 0;;
let _M680X_REG_A = 1;;
let _M680X_REG_B = 2;;
let _M680X_REG_E = 3;;
let _M680X_REG_F = 4;;
let _M680X_REG_0 = 5;;
let _M680X_REG_D = 6;;
let _M680X_REG_W = 7;;
let _M680X_REG_CC = 8;;
let _M680X_REG_DP = 9;;
let _M680X_REG_MD = 10;;
let _M680X_REG_X = 11;;
let _M680X_REG_Y = 12;;
let _M680X_REG_S = 13;;
let _M680X_REG_U = 14;;
let _M680X_REG_V = 15;;
let _M680X_REG_Q = 16;;
let _M680X_REG_PC = 17;;
let _M680X_REG_ENDING = 18;;

(* M680X Addressing Modes *)

let _M680X_AM_NONE = 0;;
let _M680X_AM_INHERENT = 1;;
let _M680X_AM_REGISTER = 2;;
let _M680X_AM_IMMEDIATE = 3;;
let _M680X_AM_INDEXED = 4;;
let _M680X_AM_EXTENDED = 5;;
let _M680X_AM_DIRECT = 6;;
let _M680X_AM_RELATIVE = 7;;
let _M680X_AM_IMM_DIRECT = 8;;
let _M680X_AM_IMM_INDEXED = 9;;
let _M680X_AM_IMM_EXTENDED = 10;;
let _M680X_AM_BIT_MOVE = 11;;
let _M680X_AM_INDEXED2 = 12;;
let _M680X_AM_DIR_IMM_REL = 13;;
let _M680X_AM_IDX_IMM_REL = 14;;
let _M680X_AM_DIRECT_IMM = 15;;
let _M680X_AM_INDEXED_IMM = 16;;
let _M680X_AM_IDX_DIR_REL = 17;;
let _M680X_AM_IDX_DIRECT = 18;;
let _M680X_AM_ENDING = 19;;

(* Operand type for instruction's operands *)

let _M680X_OP_INVALID = 0;;
let _M680X_OP_REGISTER = 1;;
let _M680X_OP_IMMEDIATE = 2;;
let _M680X_OP_INDEXED = 3;;
let _M680X_OP_EXTENDED = 4;;
let _M680X_OP_DIRECT = 5;;
let _M680X_OP_RELATIVE = 6;;
let _M680X_OP_INDEX = 7;;

(* Supported values for mem.idx.inc_dec *)

let _M680X_NO_INC_DEC = 0;;
let _M680X_PRE_DEC_1 = 1;;
let _M680X_PRE_DEC_2 = 2;;
let _M680X_POST_INC_1 = 3;;
let _M680X_POST_INC_2 = 4;;
let _M680X_POST_DEC_1 = 5;;

(* Supported bit values for mem.idx.offset_bits *)

let _M680X_OFFSET_NONE = 0;;
let _M680X_OFFSET_BITS_5 = 5;;
let _M680X_OFFSET_BITS_8 = 8;;
let _M680X_OFFSET_BITS_16 = 16;;

(* Supported bit flags for mem.idx.flags *)

(* These flags can be comined *)
let _M680X_IDX_INDIRECT = 1;;
let _M680X_IDX_NO_COMMA = 2;;

(* Group of M680X instructions *)

let _M680X_GRP_INVALID = 0;;

(* Generic groups *)
let _M680X_GRP_JUMP = 1;;
let _M680X_GRP_CALL = 2;;
let _M680X_GRP_RET = 3;;
let _M680X_GRP_INT = 4;;
let _M680X_GRP_IRET = 5;;
let _M680X_GRP_PRIV = 6;;
let _M680X_GRP_BRAREL = 7;;

(* Architecture-specific groups *)
let _M680X_GRP_ENDING = 8;;

(* M680X instruction flags: *)
let _M680X_FIRST_OP_IN_MNEM = 1;;
let _M680X_SECOND_OP_IN_MNEM = 2;;

(* M680X instruction IDs *)

let _M680X_INS_INVLD = 0;;
let _M680X_INS_ABA = 1;;
let _M680X_INS_ABX = 2;;
let _M680X_INS_ABY = 3;;
let _M680X_INS_ADC = 4;;
let _M680X_INS_ADCA = 5;;
let _M680X_INS_ADCB = 6;;
let _M680X_INS_ADCD = 7;;
let _M680X_INS_ADCR = 8;;
let _M680X_INS_ADD = 9;;
let _M680X_INS_ADDA = 10;;
let _M680X_INS_ADDB = 11;;
let _M680X_INS_ADDD = 12;;
let _M680X_INS_ADDE = 13;;
let _M680X_INS_ADDF = 14;;
let _M680X_INS_ADDR = 15;;
let _M680X_INS_ADDW = 16;;
let _M680X_INS_AIM = 17;;
let _M680X_INS_AND = 18;;
let _M680X_INS_ANDA = 19;;
let _M680X_INS_ANDB = 20;;
let _M680X_INS_ANDCC = 21;;
let _M680X_INS_ANDD = 22;;
let _M680X_INS_ANDR = 23;;
let _M680X_INS_ASL = 24;;
let _M680X_INS_ASLA = 25;;
let _M680X_INS_ASLB = 26;;
let _M680X_INS_ASLD = 27;;
let _M680X_INS_ASR = 28;;
let _M680X_INS_ASRA = 29;;
let _M680X_INS_ASRB = 30;;
let _M680X_INS_ASRD = 31;;
let _M680X_INS_ASRX = 32;;
let _M680X_INS_BAND = 33;;
let _M680X_INS_BCC = 34;;
let _M680X_INS_BCLR = 35;;
let _M680X_INS_BCS = 36;;
let _M680X_INS_BEOR = 37;;
let _M680X_INS_BEQ = 38;;
let _M680X_INS_BGE = 39;;
let _M680X_INS_BGT = 40;;
let _M680X_INS_BHCC = 41;;
let _M680X_INS_BHCS = 42;;
let _M680X_INS_BHI = 43;;
let _M680X_INS_BIAND = 44;;
let _M680X_INS_BIEOR = 45;;
let _M680X_INS_BIH = 46;;
let _M680X_INS_BIL = 47;;
let _M680X_INS_BIOR = 48;;
let _M680X_INS_BIT = 49;;
let _M680X_INS_BITA = 50;;
let _M680X_INS_BITB = 51;;
let _M680X_INS_BITD = 52;;
let _M680X_INS_BITMD = 53;;
let _M680X_INS_BLE = 54;;
let _M680X_INS_BLS = 55;;
let _M680X_INS_BLT = 56;;
let _M680X_INS_BMC = 57;;
let _M680X_INS_BMI = 58;;
let _M680X_INS_BMS = 59;;
let _M680X_INS_BNE = 60;;
let _M680X_INS_BOR = 61;;
let _M680X_INS_BPL = 62;;
let _M680X_INS_BRCLR = 63;;
let _M680X_INS_BRSET = 64;;
let _M680X_INS_BRA = 65;;
let _M680X_INS_BRN = 66;;
let _M680X_INS_BSET = 67;;
let _M680X_INS_BSR = 68;;
let _M680X_INS_BVC = 69;;
let _M680X_INS_BVS = 70;;
let _M680X_INS_CBA = 71;;
let _M680X_INS_CLC = 72;;
let _M680X_INS_CLI = 73;;
let _M680X_INS_CLR = 74;;
let _M680X_INS_CLRA = 75;;
let _M680X_INS_CLRB = 76;;
let _M680X_INS_CLRD = 77;;
let _M680X_INS_CLRE = 78;;
let _M680X_INS_CLRF = 79;;
let _M680X_INS_CLRW = 80;;
let _M680X_INS_CLRX = 81;;
let _M680X_INS_CLV = 82;;
let _M680X_INS_CMP = 83;;
let _M680X_INS_CMPA = 84;;
let _M680X_INS_CMPB = 85;;
let _M680X_INS_CMPD = 86;;
let _M680X_INS_CMPE = 87;;
let _M680X_INS_CMPF = 88;;
let _M680X_INS_CMPR = 89;;
let _M680X_INS_CMPS = 90;;
let _M680X_INS_CMPU = 91;;
let _M680X_INS_CMPW = 92;;
let _M680X_INS_CMPX = 93;;
let _M680X_INS_CMPY = 94;;
let _M680X_INS_COM = 95;;
let _M680X_INS_COMA = 96;;
let _M680X_INS_COMB = 97;;
let _M680X_INS_COMD = 98;;
let _M680X_INS_COME = 99;;
let _M680X_INS_COMF = 100;;
let _M680X_INS_COMW = 101;;
let _M680X_INS_COMX = 102;;
let _M680X_INS_CPD = 103;;
let _M680X_INS_CPX = 104;;
let _M680X_INS_CPY = 105;;
let _M680X_INS_CWAI = 106;;
let _M680X_INS_DAA = 107;;
let _M680X_INS_DEC = 108;;
let _M680X_INS_DECA = 109;;
let _M680X_INS_DECB = 110;;
let _M680X_INS_DECD = 111;;
let _M680X_INS_DECE = 112;;
let _M680X_INS_DECF = 113;;
let _M680X_INS_DECW = 114;;
let _M680X_INS_DECX = 115;;
let _M680X_INS_DES = 116;;
let _M680X_INS_DEX = 117;;
let _M680X_INS_DEY = 118;;
let _M680X_INS_DIVD = 119;;
let _M680X_INS_DIVQ = 120;;
let _M680X_INS_EIM = 121;;
let _M680X_INS_EOR = 122;;
let _M680X_INS_EORA = 123;;
let _M680X_INS_EORB = 124;;
let _M680X_INS_EORD = 125;;
let _M680X_INS_EORR = 126;;
let _M680X_INS_EXG = 127;;
let _M680X_INS_FDIV = 128;;
let _M680X_INS_IDIV = 129;;
let _M680X_INS_ILLGL = 130;;
let _M680X_INS_INC = 131;;
let _M680X_INS_INCA = 132;;
let _M680X_INS_INCB = 133;;
let _M680X_INS_INCD = 134;;
let _M680X_INS_INCE = 135;;
let _M680X_INS_INCF = 136;;
let _M680X_INS_INCW = 137;;
let _M680X_INS_INCX = 138;;
let _M680X_INS_INS = 139;;
let _M680X_INS_INX = 140;;
let _M680X_INS_INY = 141;;
let _M680X_INS_JMP = 142;;
let _M680X_INS_JSR = 143;;
let _M680X_INS_LBCC = 144;;
let _M680X_INS_LBCS = 145;;
let _M680X_INS_LBEQ = 146;;
let _M680X_INS_LBGE = 147;;
let _M680X_INS_LBGT = 148;;
let _M680X_INS_LBHI = 149;;
let _M680X_INS_LBLE = 150;;
let _M680X_INS_LBLS = 151;;
let _M680X_INS_LBLT = 152;;
let _M680X_INS_LBMI = 153;;
let _M680X_INS_LBNE = 154;;
let _M680X_INS_LBPL = 155;;
let _M680X_INS_LBRA = 156;;
let _M680X_INS_LBRN = 157;;
let _M680X_INS_LBSR = 158;;
let _M680X_INS_LBVC = 159;;
let _M680X_INS_LBVS = 160;;
let _M680X_INS_LDA = 161;;
let _M680X_INS_LDAA = 162;;
let _M680X_INS_LDAB = 163;;
let _M680X_INS_LDB = 164;;
let _M680X_INS_LDBT = 165;;
let _M680X_INS_LDD = 166;;
let _M680X_INS_LDE = 167;;
let _M680X_INS_LDF = 168;;
let _M680X_INS_LDMD = 169;;
let _M680X_INS_LDQ = 170;;
let _M680X_INS_LDS = 171;;
let _M680X_INS_LDU = 172;;
let _M680X_INS_LDW = 173;;
let _M680X_INS_LDX = 174;;
let _M680X_INS_LDY = 175;;
let _M680X_INS_LEAS = 176;;
let _M680X_INS_LEAU = 177;;
let _M680X_INS_LEAX = 178;;
let _M680X_INS_LEAY = 179;;
let _M680X_INS_LSL = 180;;
let _M680X_INS_LSLA = 181;;
let _M680X_INS_LSLB = 182;;
let _M680X_INS_LSLD = 183;;
let _M680X_INS_LSLX = 184;;
let _M680X_INS_LSR = 185;;
let _M680X_INS_LSRA = 186;;
let _M680X_INS_LSRB = 187;;
let _M680X_INS_LSRD = 188;;
let _M680X_INS_LSRW = 189;;
let _M680X_INS_LSRX = 190;;
let _M680X_INS_MUL = 191;;
let _M680X_INS_MULD = 192;;
let _M680X_INS_NEG = 193;;
let _M680X_INS_NEGA = 194;;
let _M680X_INS_NEGB = 195;;
let _M680X_INS_NEGD = 196;;
let _M680X_INS_NEGX = 197;;
let _M680X_INS_NOP = 198;;
let _M680X_INS_OIM = 199;;
let _M680X_INS_ORA = 200;;
let _M680X_INS_ORAA = 201;;
let _M680X_INS_ORAB = 202;;
let _M680X_INS_ORB = 203;;
let _M680X_INS_ORCC = 204;;
let _M680X_INS_ORD = 205;;
let _M680X_INS_ORR = 206;;
let _M680X_INS_PSHA = 207;;
let _M680X_INS_PSHB = 208;;
let _M680X_INS_PSHS = 209;;
let _M680X_INS_PSHSW = 210;;
let _M680X_INS_PSHU = 211;;
let _M680X_INS_PSHUW = 212;;
let _M680X_INS_PSHX = 213;;
let _M680X_INS_PSHY = 214;;
let _M680X_INS_PULA = 215;;
let _M680X_INS_PULB = 216;;
let _M680X_INS_PULS = 217;;
let _M680X_INS_PULSW = 218;;
let _M680X_INS_PULU = 219;;
let _M680X_INS_PULUW = 220;;
let _M680X_INS_PULX = 221;;
let _M680X_INS_PULY = 222;;
let _M680X_INS_ROL = 223;;
let _M680X_INS_ROLA = 224;;
let _M680X_INS_ROLB = 225;;
let _M680X_INS_ROLD = 226;;
let _M680X_INS_ROLW = 227;;
let _M680X_INS_ROLX = 228;;
let _M680X_INS_ROR = 229;;
let _M680X_INS_RORA = 230;;
let _M680X_INS_RORB = 231;;
let _M680X_INS_RORD = 232;;
let _M680X_INS_RORW = 233;;
let _M680X_INS_RORX = 234;;
let _M680X_INS_RSP = 235;;
let _M680X_INS_RTI = 236;;
let _M680X_INS_RTS = 237;;
let _M680X_INS_SBA = 238;;
let _M680X_INS_SBC = 239;;
let _M680X_INS_SBCA = 240;;
let _M680X_INS_SBCB = 241;;
let _M680X_INS_SBCD = 242;;
let _M680X_INS_SBCR = 243;;
let _M680X_INS_SEC = 244;;
let _M680X_INS_SEI = 245;;
let _M680X_INS_SEV = 246;;
let _M680X_INS_SEX = 247;;
let _M680X_INS_SEXW = 248;;
let _M680X_INS_STA = 249;;
let _M680X_INS_STAA = 250;;
let _M680X_INS_STAB = 251;;
let _M680X_INS_STB = 252;;
let _M680X_INS_STBT = 253;;
let _M680X_INS_STD = 254;;
let _M680X_INS_STE = 255;;
let _M680X_INS_STF = 256;;
let _M680X_INS_STOP = 257;;
let _M680X_INS_STQ = 258;;
let _M680X_INS_STS = 259;;
let _M680X_INS_STU = 260;;
let _M680X_INS_STW = 261;;
let _M680X_INS_STX = 262;;
let _M680X_INS_STY = 263;;
let _M680X_INS_SUB = 264;;
let _M680X_INS_SUBA = 265;;
let _M680X_INS_SUBB = 266;;
let _M680X_INS_SUBD = 267;;
let _M680X_INS_SUBE = 268;;
let _M680X_INS_SUBF = 269;;
let _M680X_INS_SUBR = 270;;
let _M680X_INS_SUBW = 271;;
let _M680X_INS_SWI = 272;;
let _M680X_INS_SWI2 = 273;;
let _M680X_INS_SWI3 = 274;;
let _M680X_INS_SYNC = 275;;
let _M680X_INS_TAB = 276;;
let _M680X_INS_TAP = 277;;
let _M680X_INS_TAX = 278;;
let _M680X_INS_TBA = 279;;
let _M680X_INS_TEST = 280;;
let _M680X_INS_TFM = 281;;
let _M680X_INS_TFR = 282;;
let _M680X_INS_TIM = 283;;
let _M680X_INS_TPA = 284;;
let _M680X_INS_TST = 285;;
let _M680X_INS_TSTA = 286;;
let _M680X_INS_TSTB = 287;;
let _M680X_INS_TSTD = 288;;
let _M680X_INS_TSTE = 289;;
let _M680X_INS_TSTF = 290;;
let _M680X_INS_TSTW = 291;;
let _M680X_INS_TSTX = 292;;
let _M680X_INS_TSX = 293;;
let _M680X_INS_TSY = 294;;
let _M680X_INS_TXA = 295;;
let _M680X_INS_TXS = 296;;
let _M680X_INS_TYS = 297;;
let _M680X_INS_WAI = 298;;
let _M680X_INS_WAIT = 299;;
let _M680X_INS_XGDX = 300;;
let _M680X_INS_XGDY = 301;;
let _M680X_INS_ENDING = 302;;

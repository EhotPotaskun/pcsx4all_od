/* NOTE: There's no need to do zero register optimizatins since we have
         native zero reg on mips. */
#define REC_ITYPE_RT_RS_I16(insn, _rt_, _rs_, _imm_) \
do { \
	u32 rt  = _rt_; \
	u32 rs  = _rs_; \
	s32 imm = _imm_; \
	if (!rt) break; \
	iRegs[_rt_] = -1; \
	u32 r1, r2; \
	if (rs == rt) { \
		r1 = regMipsToArm(rt, REG_LOAD, REG_REGISTER); \
		r2 = r1; \
	} else { \
		r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER); \
		r2 = regMipsToArm(rs, REG_LOAD, REG_REGISTER); \
	} \
	gen(insn, r1, r2, imm); \
	regMipsChanged(rt); \
	regBranchUnlock(r1); \
	regBranchUnlock(r2); \
} while (0)

static void recADDI()
{
	u32 x = iRegs[_Rs_];
	REC_ITYPE_RT_RS_I16(ADDI,  _Rt_, _Rs_, ((s16)(_Imm_)));
	if (x != -1)
		iRegs[_Rt_] = x + (s16)(_Imm_);
}

static void recADDIU() { recADDI(); }
static void recSLTI()  { REC_ITYPE_RT_RS_I16(SLTI,  _Rt_, _Rs_, ((s16)(_Imm_))); }
static void recSLTIU() { REC_ITYPE_RT_RS_I16(SLTIU, _Rt_, _Rs_, ((s16)(_Imm_))); }

#define REC_ITYPE_RT_RS_U16(insn, _rt_, _rs_, _imm_) \
do { \
	u32 rt  = _rt_; \
	u32 rs  = _rs_; \
	u32 imm = _imm_; \
	if (!rt) break; \
	iRegs[_rt_] = -1; \
	u32 r1, r2; \
	if (rs == rt) { \
		r1 = regMipsToArm(rt, REG_LOAD, REG_REGISTER); \
		r2 = r1; \
	} else { \
		r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER); \
		r2 = regMipsToArm(rs, REG_LOAD, REG_REGISTER); \
	} \
	gen(insn, r1, r2, imm); \
	regMipsChanged(rt); \
	regBranchUnlock(r1); \
	regBranchUnlock(r2); \
} while (0)


static void recANDI()  { REC_ITYPE_RT_RS_U16(ANDI, _Rt_, _Rs_, ((u16)(_ImmU_))); }
static void recORI()
{
	u32 x = iRegs[_Rs_];
	REC_ITYPE_RT_RS_U16(ORI,  _Rt_, _Rs_, ((u16)(_ImmU_)));
	if (x != -1)
		iRegs[_Rt_] = x | ((u16)(_Imm_));
}
static void recXORI()  { REC_ITYPE_RT_RS_U16(XORI, _Rt_, _Rs_, ((u16)(_ImmU_))); }

#define REC_ITYPE_RT_U16(insn, _rt_, _imm_) \
do { \
	u32 rt  = _rt_; \
	u32 imm = _imm_; \
	if (!rt) break; \
	u32 r1 = regMipsToArm(rt, REG_FIND, REG_REGISTER); \
	gen(insn, r1, imm); \
	regMipsChanged(rt); \
	regBranchUnlock(r1); \
} while (0)

static void recLUI()   { iRegs[_Rt_] = ((u16)_ImmU_) << 16; REC_ITYPE_RT_U16(LUI, _Rt_, ((u16)(_ImmU_))); }

#define REC_RTYPE_RD_RS_RT(insn, _rd_, _rs_, _rt_) \
do { \
	u32 rd  = _rd_; \
	u32 rt  = _rt_; \
	u32 rs  = _rs_; \
	if (!rd) break; \
	u32 r1, r2, r3; \
	iRegs[_rd_] = -1; \
	if (rs == rd) { \
		r1 = regMipsToArm(rd, REG_LOAD, REG_REGISTER); \
		r2 = r1; \
		r3 = (rd == rt ? r1 : regMipsToArm(rt, REG_LOAD, REG_REGISTER)); \
	} else if (rt == rd) { \
		r1 = regMipsToArm(rd, REG_LOAD, REG_REGISTER); \
		r3 = r1; \
		r2 = regMipsToArm(rs, REG_LOAD, REG_REGISTER); \
	} else { \
		r1 = regMipsToArm(rd, REG_FIND, REG_REGISTER); \
		r2 = regMipsToArm(rs, REG_LOAD, REG_REGISTER); \
		r3 = (rs == rt ? r2 : regMipsToArm(rt, REG_LOAD, REG_REGISTER)); \
	} \
	gen(insn, r1, r2, r3); \
	regMipsChanged(rd); \
	regBranchUnlock(r1); \
	regBranchUnlock(r2); \
	regBranchUnlock(r3); \
} while (0)

static void recADD()   { REC_RTYPE_RD_RS_RT(ADD, _Rd_, _Rs_, _Rt_); }
static void recADDU()  { recADD(); }
static void recSUB()   { REC_RTYPE_RD_RS_RT(SUB, _Rd_, _Rs_, _Rt_); }
static void recSUBU()  { recSUB(); }

static void recAND()   { REC_RTYPE_RD_RS_RT(AND, _Rd_, _Rs_, _Rt_); }
static void recOR()    { REC_RTYPE_RD_RS_RT(OR,  _Rd_, _Rs_, _Rt_); }
static void recXOR()   { REC_RTYPE_RD_RS_RT(XOR, _Rd_, _Rs_, _Rt_); }
static void recNOR()   { REC_RTYPE_RD_RS_RT(NOR, _Rd_, _Rs_, _Rt_); }

static void recSLT()   { REC_RTYPE_RD_RS_RT(SLT,  _Rd_, _Rs_, _Rt_); }
static void recSLTU()  { REC_RTYPE_RD_RS_RT(SLTU, _Rd_, _Rs_, _Rt_); }

#define REC_RTYPE_RD_RT_SA(insn, _rd_, _rt_, _sa_) \
do { \
	u32 rd = _rd_; \
	u32 rt = _rt_; \
	u32 sa = _sa_; \
	if (!rd) break; \
	u32 r1, r2; \
	if (rd == rt) { \
		if (!sa) break; \
		r1 = regMipsToArm(rd, REG_LOAD, REG_REGISTER); \
		r2 = r1; \
	} else { \
		r1 = regMipsToArm(rd, REG_FIND, REG_REGISTER); \
		r2 = regMipsToArm(rt, REG_LOAD, REG_REGISTER); \
	} \
	gen(insn, r1, r2, sa); \
	regMipsChanged(rd); \
	regBranchUnlock(r1); \
	regBranchUnlock(r2); \
} while (0)

static void recSLL()   { REC_RTYPE_RD_RT_SA(SLL, _Rd_, _Rt_, _Sa_); }
static void recSRL()   { REC_RTYPE_RD_RT_SA(SRL, _Rd_, _Rt_, _Sa_); }
static void recSRA()   { REC_RTYPE_RD_RT_SA(SRA, _Rd_, _Rt_, _Sa_); }

#define REC_RTYPE_RD_RT_RS(insn, _rd_, _rt_, _rs_) \
do { \
	u32 rd = _rd_; \
	u32 rt = _rt_; \
	u32 rs = _rs_; \
	if (!rd) break; \
	u32 r1, r2, r3; \
	if (rd == rt) { \
		if (!rs) break; \
		r1 = regMipsToArm(rd, REG_LOAD, REG_REGISTER); \
		r2 = r1; \
		r3 = (rs == rd ? r1 : regMipsToArm(rs, REG_LOAD, REG_REGISTER)); \
	} else if (rd == rs) { \
		r1 = regMipsToArm(rd, REG_LOAD, REG_REGISTER); \
		r3 = r1; \
		r2 = regMipsToArm(rt, REG_LOAD, REG_REGISTER); \
	} else { \
		r1 = regMipsToArm(rd, REG_FIND, REG_REGISTER); \
		r2 = regMipsToArm(rt, REG_LOAD, REG_REGISTER); \
		r3 = (rs == rt) ? r2 : regMipsToArm(rs, REG_LOAD, REG_REGISTER); \
	} \
	gen(insn, r1, r2, r3); \
	regMipsChanged(rd); \
	regBranchUnlock(r1); \
	regBranchUnlock(r2); \
	regBranchUnlock(r3); \
} while (0)

static void recSLLV()  { REC_RTYPE_RD_RT_RS(SLLV, _Rd_, _Rt_, _Rs_); }
static void recSRLV()  { REC_RTYPE_RD_RT_RS(SRLV, _Rd_, _Rt_, _Rs_); }
static void recSRAV()  { REC_RTYPE_RD_RT_RS(SRAV, _Rd_, _Rt_, _Rs_); }

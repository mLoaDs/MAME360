

#define if2    if12
#define f2Op1  f12Op1
#define f2Op2  f12Op2
#define f2Flag1 f12Flag1
#define f2Flag2 f12Flag2

#define F2END() \
	return 2+amLength1+amLength2;

#define F2LOADOPFLOAT(num) \
	if (f2Flag##num)								\
		appf = u2f(v60.reg[f2Op##num]);  \
	else	\
		appf = u2f(MemRead32(f2Op##num));

#define F2STOREOPFLOAT(num) \
	if (f2Flag##num)		\
		v60.reg[f2Op##num] = f2u(appf);  \
	else	\
		MemWrite32(f2Op##num, f2u(appf));

void F2DecodeFirstOperand(UINT32 (*DecodeOp1)(void), UINT8 dim1)
{
	modDim = dim1;
	modM = if2 & 0x40;
	modAdd = PC + 2;
	amLength1 = DecodeOp1();
	f2Op1 = amOut;
	f2Flag1 = amFlag;
}

void F2DecodeSecondOperand(UINT32 (*DecodeOp2)(void), UINT8 dim2)
{
	modDim = dim2;
	modM = if2 & 0x20;
	modAdd = PC + 2 + amLength1;
	amLength2 = DecodeOp2();
	f2Op2 = amOut;
	f2Flag2 = amFlag;
}

void F2WriteSecondOperand(UINT8 dim2)
{
	modDim = dim2;
	modM = if2 & 0x20;
	modAdd = PC + 2 + amLength1;
	amLength2 = WriteAM();
}

int opCVTWS(void)
{
	float val;

	F2DecodeFirstOperand(ReadAM,2);

	// Convert to float
	val = (float)(INT32)f2Op1;
	modWriteValW = f2u(val);

	_OV=0;
	_CY=(val < 0.0f);
	_S=((modWriteValW & 0x80000000)!=0);
	_Z=(val == 0.0f);

	F2WriteSecondOperand(2);
	F2END();
}

int opCVTSW(void)
{
	float val;

	F2DecodeFirstOperand(ReadAM,2);

	// Convert to UINT32
	val = u2f(f2Op1);
	modWriteValW = (UINT32)val;

	_OV=0;
	_CY=(val < 0.0f);
	_S=((modWriteValW & 0x80000000)!=0);
	_Z=(val == 0.0f);

	F2WriteSecondOperand(2);
	F2END();
}

int opMOVFS(void)
{
	F2DecodeFirstOperand(ReadAM,2);
	modWriteValW = f2Op1;
	F2WriteSecondOperand(2);
	F2END();
}

int opNEGFS(void)
{
	float appf;

	F2DecodeFirstOperand(ReadAM, 2);
	F2DecodeSecondOperand(ReadAMAddress, 2);

	F2LOADOPFLOAT(2);

	appf = -appf;

	_OV=0;
	_CY=(appf < 0.0f);
	_S=((f2u(appf) & 0x80000000)!=0);
	_Z=(appf == 0.0f);

	F2STOREOPFLOAT(2);
	F2END()
}

int opADDFS(void)
{
	UINT32 appw;
	float appf;

	F2DecodeFirstOperand(ReadAM, 2);
	F2DecodeSecondOperand(ReadAMAddress, 2);

	F2LOADOPFLOAT(2);

	appf += u2f(f2Op1);

	appw = f2u(appf);
	_OV = _CY = 0;
	_S = ((appw & 0x80000000)!=0);
	_Z = (appw == 0);

	F2STOREOPFLOAT(2);
	F2END()
}

int opSUBFS(void)
{
	UINT32 appw;
	float appf;

	F2DecodeFirstOperand(ReadAM, 2);
	F2DecodeSecondOperand(ReadAMAddress, 2);

	F2LOADOPFLOAT(2);

	appf -= u2f(f2Op1);

	appw = f2u(appf);
	_OV = _CY = 0;
	_S = ((appw & 0x80000000)!=0);
	_Z = (appw == 0);

	F2STOREOPFLOAT(2);
	F2END()
}

int opMULFS(void)
{
	UINT32 appw;
	float appf;

	F2DecodeFirstOperand(ReadAM, 2);
	F2DecodeSecondOperand(ReadAMAddress, 2);

	F2LOADOPFLOAT(2);

	appf *= u2f(f2Op1);

	appw = f2u(appf);
	_OV = _CY = 0;
	_S = ((appw & 0x80000000)!=0);
	_Z = (appw == 0);

	F2STOREOPFLOAT(2);
	F2END()
}

int opDIVFS(void)
{
	UINT32 appw;
	float appf;

	F2DecodeFirstOperand(ReadAM, 2);
	F2DecodeSecondOperand(ReadAMAddress, 2);

	F2LOADOPFLOAT(2);

	appf *= u2f(f2Op1);

	appw = f2u(appf);
	_OV = _CY = 0;
	_S = ((appw & 0x80000000)!=0);
	_Z = (appw == 0);

	F2STOREOPFLOAT(2);
	F2END()
}

int opSCLFS(void)
{
	UINT32 appw;
	float appf;

	F2DecodeFirstOperand(ReadAM, 1);
	F2DecodeSecondOperand(ReadAMAddress, 2);

	F2LOADOPFLOAT(2);

	if ((INT32)f2Op1 < 0)
		appf /= 1 << -(INT32)f2Op1;
	else
		appf *= 1 << f2Op1;

	appw = f2u(appf);
	_OV = _CY = 0;
	_S = ((appw & 0x80000000)!=0);
	_Z = (appw == 0);

	F2STOREOPFLOAT(2);
	F2END()
}

int opCMPF(void)
{
	float appf;

	F2DecodeFirstOperand(ReadAM, 2);
	F2DecodeSecondOperand(ReadAM, 2);

	appf = u2f(f2Op2) - u2f(f2Op1);

	_Z = (appf == 0);
	_S = (appf < 0);
	_OV = 0;
	_CY = 0;

	F2END();
}

int op5FUNHANDLED(void)
{
	logerror("Unhandled 5F opcode at %08x\n", PC);
	abort();
}

int op5CUNHANDLED(void)
{
	logerror("Unhandled 5C opcode at %08x\n", PC);
	abort();
}

int (*Op5FTable[32])(void) =
{
	opCVTWS,
	opCVTSW,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED,
	op5FUNHANDLED
};

int (*Op5CTable[32])(void) =
{
	opCMPF,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	opMOVFS,
	opNEGFS,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,

	opSCLFS,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	opADDFS,
	opSUBFS,
	opMULFS,
	opDIVFS,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED,
	op5CUNHANDLED
};


UINT32 op5F(void)
{
	if2 = OpRead8(PC + 1);
	return Op5FTable[if2&0x1F]();
}


UINT32 op5C(void)
{
	if2 = OpRead8(PC + 1);
	return Op5CTable[if2&0x1F]();
}

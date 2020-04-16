#include <stdio.h>
#include <stdlib.h>

unsigned char memory[1024+3]=
{
    // 0x22,0x10,0x00,0x01,
    // 0x02,0x31,0x88,0x26,
    // 0x02,0x20,0x00,0x08
    0x01,0x00,0x10,0x22,
    0x26,0x88,0x31,0x02,
    0x08,0x00,0x20,0x02,
};
// running data is stored in memory as little-endian
unsigned int mips_reg[68];
enum register_name
{
    zero,                                                               // always 0
    at,                                                                 // reserved for assembly program
    v0,v1,                                                              // result value
    a0,a1,a2,a3,                                                        // argument
    t0,t1,t2,t3,t4,t5,t6,t7,                                            // temporary value
    s0,s1,s2,s3,s4,s5,s6,s7,                                            // stored value
    t8,t9,                                                              // other temporary value
    k0,k1,                                                              // reserved for OS
    gp,                                                                 // global pointer
    sp,                                                                 // stack pointer
    fp,                                                                 // (also s8)frame pointer
    ra,                                                                 // returned address
    f0,f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,
    f15,f16,f17,f18,f19,f20,f21,f22,f23,f24,f25,f26,f27,f28,f29,f30,f31,// float register
    Hi,Lo,                                                              // used by multiplier
    PC,                                                                 // program counter
    IR                                                                  // instruction register
};
const char* regname[]=
{
    "zero",
    "at",
    "v0","v1",
    "a0","a1","a2","a3",
    "t0","t1","t2","t3","t4","t5","t6","t7",
    "s0","s1","s2","s3","s4","s5","s6","s7",
    "t8","t9",
    "k0","k1",
    "gp",
    "sp",
    "fp",
    "ra",
    "f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","f10",
    "f11","f12","f13","f14","f15","f16","f17","f18","f19","f20",
    "f21","f22","f23","f24","f25","f26","f27","f28","f29","f30","f31",
    "Hi","Lo",
    "PC","IR"
};
enum instruction_set
{
    END,
    //R-format
    ADD,ADDU,SUB,SUBU,SLT,SLTU,
    AND,OR,XOR,NOR,
    SLL,SRL,SRA,
    SLLV,SRLV,SRAV,
    JR,
    //I-format
    ADDI,ADDIU,
    SLTI,SLTIU,
    ANDI,ORI,XORI,
    LUI,LW,SW,
    //J-format
    BEQ,BNE,J,JAL
};
int get_ins()
{
    int op=mips_reg[IR]&0xfc000000;
    int ret_ins=0;
    if(!op)
        switch(mips_reg[IR]&0x0000003f)
        {
            case 0x20:ret_ins=ADD;break;
            case 0x21:ret_ins=ADDU;break;
            case 0x22:ret_ins=SUB;break;
            case 0x23:ret_ins=SUBU;break;
            case 0x2a:ret_ins=SLT;break;
            case 0x2b:ret_ins=SLTU;break;
            case 0x24:ret_ins=AND;break;
            case 0x25:ret_ins=OR;break;
            case 0x26:ret_ins=XOR;break;
            case 0x27:ret_ins=NOR;break;
            case 0x00:ret_ins=SLL;break;
            case 0x02:ret_ins=SRL;break;
            case 0x03:ret_ins=SRA;break;
            case 0x04:ret_ins=SLLV;break;
            case 0x06:ret_ins=SRLV;break;
            case 0x07:ret_ins=SRAV;break;
            case 0x08:ret_ins=JR;break;
            default:break;
        }
    switch(op)
    {
        case 0x00000000:break;
        case 0x20000000:ret_ins=ADDI;break;
        case 0x24000000:ret_ins=ADDIU;break;
        case 0x28000000:ret_ins=SLTI;break;
        case 0x2c000000:ret_ins=SLTIU;break;
        case 0x30000000:ret_ins=ANDI;break;
        case 0x34000000:ret_ins=ORI;break;
        case 0x38000000:ret_ins=XORI;break;
        case 0x3c000000:ret_ins=LUI;break;
        case 0x8c000000:ret_ins=LW;break;
        case 0xac000000:ret_ins=SW;break;
        case 0x10000000:ret_ins=BEQ;break;
        case 0x14000000:ret_ins=BNE;break;
        default:break;
    }
    return ret_ins;
}
void RFORMAT(unsigned int opr)
{
    unsigned int rs,rt,rd,shamt,rt_flag;
    rs=   (mips_reg[IR]&0x03e00000)>>21;
    rt=   (mips_reg[IR]&0x001f0000)>>16;
    rd=   (mips_reg[IR]&0x0000f800)>>11;
    shamt=(mips_reg[IR]&0x000007c0)>>6;
    rt_flag=0x80000000&mips_reg[rt];
    switch(opr)
    {
        case ADD:  mips_reg[rd] = (unsigned int)((int)mips_reg[rs]+(int)mips_reg[rt]); break;
        case ADDU: mips_reg[rd] = mips_reg[rs]+mips_reg[rt];                           break;
        case SUB:  mips_reg[rd] = (unsigned int)((int)mips_reg[rs]-(int)mips_reg[rt]); break;
        case SUBU: mips_reg[rd] = mips_reg[rs]-mips_reg[rt];                           break;
        case SLT:  mips_reg[rd] = ((int)mips_reg[rs]<(int)mips_reg[rt])?1:0;           break;
        case SLTU: mips_reg[rd] = (mips_reg[rs]<mips_reg[rt])?1:0;                     break;
        case AND:  mips_reg[rd] = mips_reg[rs]&mips_reg[rt];                           break;
        case OR:   mips_reg[rd] = mips_reg[rs]|mips_reg[rt];                           break;
        case XOR:  mips_reg[rd] = mips_reg[rs]^mips_reg[rt];                           break;
        case NOR:  mips_reg[rd] = !(mips_reg[rs]|mips_reg[rt]);                        break;
        case SLL:  mips_reg[rd] = mips_reg[rt]<<shamt;                                 break;
        case SRL:  mips_reg[rd] = mips_reg[rt]>>shamt;                                 break;
        case SRA:  mips_reg[rd] = rt_flag|((mips_reg[rt]^rt_flag)>>shamt);             break;
        case SLLV: mips_reg[rd] = mips_reg[rt]<<mips_reg[rs];                          break;
        case SRLV: mips_reg[rd] = mips_reg[rt]>>mips_reg[rs];                          break;
        case SRAV: mips_reg[rd] = rt_flag|((mips_reg[rt]^rt_flag)>>mips_reg[rs]);      break;
        case JR:   mips_reg[PC] = mips_reg[rs];                                        break;
    }
    mips_reg[PC] += opr==JR?0:4;
    mips_reg[zero] = 0;
    return;
}
void IFORMAT(unsigned int opr)
{
    unsigned int rs,rd,im;
    rs=(mips_reg[IR]&0x03e00000)>>21;
    rd=(mips_reg[IR]&0x001f0000)>>16;
    im=mips_reg[IR]&0x0000ffff;
    switch(opr)
    {
        case ADDI:  mips_reg[rd] = (int)mips_reg[rs]+(int)im;                                        break;
        case ADDIU: mips_reg[rd] = mips_reg[rs]+im;                                                  break;
        case SLTI:  mips_reg[rd] = ((int)mips_reg[rs]<(int)im)?1:0;                                  break;
        case SLTIU: mips_reg[rd] = (mips_reg[rs]<im)?1:0;                                            break;
        case ANDI:  mips_reg[rd] = mips_reg[rs]&im;                                                  break;
        case ORI:   mips_reg[rd] = mips_reg[rs]|im;                                                  break;
        case XORI:  mips_reg[rd] = mips_reg[rs]^im;                                                  break;
        case LUI:   mips_reg[rd] = im<<16;                                                           break;
        case LW:    mips_reg[rd] = *(unsigned int*)((unsigned char*)memory+mips_reg[rs]+im);         break;
        case SW:    *(unsigned int*)((unsigned char*)memory+mips_reg[rs]+im) = mips_reg[rd];         break;
        case BEQ:   mips_reg[PC] = (mips_reg[rs]==mips_reg[rd])?mips_reg[PC]+4+(im<<2):mips_reg[PC]; break;
        case BNE:   mips_reg[PC] = (mips_reg[rs]!=mips_reg[rd])?mips_reg[PC]+4+(im<<2):mips_reg[PC]; break;
    }
    mips_reg[PC] += (opr==BEQ||opr==BNE)?0:4;
    mips_reg[zero] = 0;
    return;
}
void JFORMAT(unsigned int opr)
{
    int addr;
    addr=mips_reg[IR]&0x03ffffff;
    switch(opr)
    {
        case J:   mips_reg[PC] = (mips_reg[PC]+4)&0xf0000000+(addr<<2);                            break;
        case JAL: mips_reg[ra] = mips_reg[PC];mips_reg[PC] = (mips_reg[PC]+4)&0xf0000000+(addr<<2);break;
    }
    mips_reg[zero] = 0;
    return;
}
void print_reginfo()
{
    for(int i=zero;i<=IR;++i)
        printf("%s: 0x%.8x %c",regname[i],mips_reg[i],(i+1)%4? ' ':'\n');
    printf("\n");
    return;
}
void proc()
{
    int opr;
    mips_reg[PC]=0;
    while(mips_reg[PC]<1024)
    {
        mips_reg[IR]=*(unsigned int*)((unsigned char*)memory+mips_reg[PC]);
        opr=get_ins();
        if(ADD<=opr && opr<=JR)
            RFORMAT(opr);
        else if(ADDI<=opr && opr<=SW)
            IFORMAT(opr);
        else
            JFORMAT(opr);
        print_reginfo();
    }
    return;
}
int main()
{
    proc();
    return 0;
}
/*
example:
    while(true)
        s0++;

    addi $s0 $s0 1
    xor  $s1 $s1 $s1
    jr   $s1

    0010 0010 0001 0000 0000 0000 0000 0001
    0000 0010 0011 0001 1000 1000 0010 0110
    0000 0010 0010 0000 0000 0000 0000 1000

    0x22 0x10 0x00 0x01
    0x02 0x31 0x88 0x26
    0x02 0x20 0x00 0x08
    =>
    0x01 0x00 0x10 0x22
    0x02 0x31 0x88 0x26
    0x08 0x00 0x20 0x02
*/

/*
MIPS16 instruction set

R
     op     rs    rt    rd    shamt funct
bit  31-26  25-21 20-16 15-11 10-6  5-0

     op     rs    rt    rd    shamt funct  function
add  000000 rs    rt    rd    00000 100000 rd=rs+rt
addu 000000 rs    rt    rd    00000 100001 rd=rs+rt(unsigned)
sub  000000 rs    rt    rd    00000 100010 rd=rs-rt
subu 000000 rs    rt    rd    00000 100011 rd=rs-rt(unsigned)
slt  000000 rs    rt    rd    00000 101010 rd=(rs<rt)?1:0
sltu 000000 rs    rt    rd    00000 101011 rd=(rs<rt)?1:0(unsigned)

     op     rs    rt    rd    shamt funct  function
and  000000 rs    rt    rd    00000 100100 rd=rs&rt
or   000000 rs    rt    rd    00000 100101 rd=rs|rt
xor  000000 rs    rt    rd    00000 100110 rd=rs^rt
nor  000000 rs    rt    rd    00000 100111 rd=!(rs|rt)

     op     rs    rt    rd    shamt funct  function
sll  000000 00000 rt    rd    shamt 000000 rd=rt<<shamt
srl  000000 00000 rt    rd    shamt 000010 rd=rt>>shamt
sra  000000 00000 rt    rd    shamt 000011 rd=rt>>shamt(sign flag reserved)
sllv 000000 rs    rt    rd    00000 000100 rd=rt<<rs
srlv 000000 rs    rt    rd    00000 000110 rd=rt>>rs
srav 000000 rs    rt    rd    00000 000111 rd=rt>>rs(sign flag reserved)

     op     rs    rt    rd    shamt funct  function
jr   000000 rs    00000 00000 00000 001000 PC=rs

I
      op     rs    rd    im
bit   31-26  25-21 20-16 15-0

      op     rs    rd    im
addi  001000 rs    rd    im rd=rs+im
addiu 001001 rs    rd    im rd=rs+im(unsigned)
slti  001010 rs    rd    im rd=(rs<im)?1:0
sltiu 001011 rs    rd    im rd=(rs<im)?1:0(unsigned)

      op     rs    rd    im
andi  001100 rs    rd    im rd=rs&im
ori   001101 rs    rd    im rd=rs|im
xori  001110 rs    rd    im rd=rs^im

      op     rs    rd    im
lui   001111 00000 rd    im rd=im*65536(rd=im<<16) (fill the register's high 16 bit)
lw    100011 rs    rd    im rd=memory[rs+im]
sw    101011 rs    rd    im memory[rs+im]=rd

      op     rs    rd    im
beq   000100 rs    rd    im PC=(rs==rd)?PC+4+im<<2:PC
bne   000101 rs    rd    im PC=(rs!=rd)?PC+4+im<<2:PC

J
      op     address
bit   31-26  25-0

      op     address
j     000010 addr    PC={(PC+4)[31,28],addr,00}
jal   000011 addr    $31=PC; PC={(PC+4)[31,28],addr,00}
$31 is ra register
*/
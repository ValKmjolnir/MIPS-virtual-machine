#include <iostream>
#include <fstream>
#include <vector>

const char* regname[]=
{
    "zero",                                 // always 0
    "at",                                   // reserved for assembly program
    "v0","v1",                              // result value
    "a0","a1","a2","a3",                    // argument
    "t0","t1","t2","t3","t4","t5","t6","t7",// temporary value
    "s0","s1","s2","s3","s4","s5","s6","s7",// stored value
    "t8","t9",                              // other temporary value
    "k0","k1",                              // reserved for OS
    "gp",                                   // global pointer
    "sp",                                   // stack pointer
    "fp",                                   // (also s8)frame pointer
    "ra",                                   // returned address
};
int get_regname(std::string str)
{
    for(int i=0;i<32;++i)
        if(str==regname[i])
            return i;
    return 0;
}
enum instruction_set
{
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
    LUI,LW,SW,BEQ,BNE,
    //J-format
    J,JAL
};
const char* oprname[]=
{
    //R-format
    "add","addu","sub","subu","slt","sltu",
    "and","or","xor","nor",
    "sll","srl","sra",
    "sllv","srlv","srav",
    "jr",
    //I-format
    "addi","addiu",
    "slti","sltiu",
    "andi","ori","xori",
    "lui","lw","sw","beq","bne",
    //J-format
    "j","jal"
};
int get_oprname(std::string str)
{
    for(int i=0;i<31;++i)
        if(str==oprname[i])
            return i;
    return -1;
}
enum token_type
{
    tok_identifier=1,
    tok_operator,
    tok_number
};
struct token
{
    int type;
    std::string context;
};
std::vector<char> resource;
std::vector<token> tokens;
std::vector<unsigned int> codes;

void scanner()
{
    int res_size=resource.size();
    std::string str="";
    for(int i=0;i<res_size;++i)
    {
        if(('a'<=resource[i] && resource[i]<='z') 
        || ('A'<=resource[i] && resource[i]<='Z') 
        || resource[i]=='_' || resource[i]=='$')
        {
            while(i<res_size
            && (('a'<=resource[i] && resource[i]<='z') 
            || ('A'<=resource[i] && resource[i]<='Z') 
            || ('0'<=resource[i] && resource[i]<='9')
            || resource[i]=='_' || resource[i]=='$'))
            {
                str+=resource[i];
                ++i;
            }
            if(str.length())
            {
                token tmp;
                tmp.type=tok_identifier;
                tmp.context=str;
                tokens.push_back(tmp);
                str="";
            }
            if(i>=res_size)
                break;
        }
        if(resource[i]==':' || resource[i]==',' || resource[i]=='(' || resource[i]==')')
        {
            str="";
            str+=resource[i];
            token tmp;
            tmp.type=tok_operator;
            tmp.context=str;
            tokens.push_back(tmp);
            str="";
        }
        if('0'<=resource[i] && resource[i]<='9')
        {
            str+=resource[i];
            ++i;
            if(i<res_size && resource[i]=='x')
                str+='x';
            ++i;
            while(i<res_size
            && (('0'<=resource[i] && resource[i]<='9')
            || ('a'<=resource[i] && resource[i]<='f')))
            {
                str+=resource[i];
                ++i;
            }
            --i;
            if(str.length())
            {
                token tmp;
                tmp.type=tok_number;
                tmp.context=str;
                tokens.push_back(tmp);
                str="";
            }
        }
        if(resource[i]=='#')
        {
            while(i<res_size && resource[i]!='\n') ++i;
            --i;
        }
    }
    return;
}
void generator()
{
    return;
}
int main(int argc,char* argv[])
{
    if(argc<=1)
    {
        std::cout<<"lack arguments."<<std::endl;
        return 0;
    }
    std::ifstream fin(argv[2]);
    std::ofstream fout(argv[1]);
    if(fin.fail())
    {
        std::cout<<"cannot open "<<argv[2]<<std::endl;
        return 0;
    }
    if(fout.fail())
    {
        std::cout<<"cannot open "<<argv[1]<<std::endl;
        return 0;
    }
    while(!fin.eof())
    {
        char c=fin.get();
        if(fin.eof()) break;
        resource.push_back(c);
    }
    scanner();
    fin.close();
    for(int i=0;i<tokens.size();++i)
    {
        fout<<"{ type:";
        switch (tokens[i].type)
        {
            case tok_identifier:fout<<"identifier,";break;
            case tok_operator:  fout<<"operator  ,";break;
            case tok_number:    fout<<"number    ,";break;
            default:break;
        }
        fout<<"str:"<<tokens[i].context<<"}\n";
    }
    fout.close();
    return 0;
}
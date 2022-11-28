#include <iostream>
#include <stdlib.h>
#include <string>
#include <bitset>
#include <fstream>
#include <vector>

using namespace std;

//instruction_type:
// 0 for R-Format
// 1 for I-Format
// 2 for I-Format (load)
// 3 for I-Format (jalr)
// 4 for S-Format
// 5 for B-Format
// 6 for J-Format
// 7 for U-Format (lui)





struct instruction
{
    int index;
    string bstring;
    int instruction_type;
    int labelled; // label is Ln: if label = n>=0, -1 otherwise
    int label; // label is Ln if label = n>0, -1 if no label in instruction, -2 if label present but outside bounds
};

typedef struct instruction Instruction;


//Global variables:

vector<Instruction> v_inst; 
int label_number = 0;


//functions:

int instruction_type_finder(string bstring)
{
    string opcode = bstring.substr(31-6, 7);
    if(opcode == "0110011") return 0;
    else if(opcode == "0010011") return 1;
    else if(opcode == "0000011") return 2;
    else if(opcode == "1100111") return 3;
    else if(opcode == "0100011") return 4;
    else if(opcode == "1100011") return 5;
    else if(opcode == "1101111") return 6;
    else if(opcode == "0110111") return 7;
    else return -1;
}

int imm_finder_b(string bstring)
{
    string imm = "";
    imm += bstring[31-7];
    imm += bstring.substr(31-30, 6) + bstring.substr(31-11,4); //imm has all bits except first bit

    string imm_first_bit = bstring.substr(0, 1);
    int imm_b = strtoull(imm_first_bit.c_str(), NULL, 2)*(-1)*(1<<12) + strtoull(imm.c_str(), NULL, 2)*(1<<1);
    return imm_b;
}

int imm_finder_j(string bstring)
{
    string imm = "";
    imm += bstring.substr(31-19, 8);
    imm += bstring[31-20];
    imm += bstring.substr(31-30,10);

    string imm_first_bit = bstring.substr(0, 1);
    int imm_j = strtoull(imm_first_bit.c_str(), NULL, 2)*(-1)*(1<<20) + strtoull(imm.c_str(), NULL, 2)*(1<<1);
    return imm_j;
}



void parser(Instruction i)
{
    string final_inst = "";
    string operation = ""; //for add, sub etc.
    int flag = 0; //flag is set to 1 if instruction is illegal
    if(i.labelled != -1) final_inst += "L" + to_string(i.labelled) + ": "; //to label the instruction

    string rs2 = to_string(strtoull((i.bstring.substr(31-24, 5)).c_str(), NULL, 2)); //register number (string) for rs2
    string rs1 = to_string(strtoull((i.bstring.substr(31-19, 5)).c_str(), NULL, 2)); //register number (string) for rs1
    string rd = to_string(strtoull((i.bstring.substr(31-11, 5)).c_str(), NULL, 2)); //register number (string) for rd
    int funct3 = strtoull((i.bstring.substr(31-14, 3).c_str()), NULL, 2); //register number (string) for funct3
    int funct7 = strtoull((i.bstring.substr(31-31, 7).c_str()), NULL, 2); //register number (string) for funct7

    char thfirst_bit = i.bstring[0];
    string str_thfirst_bit = ""; str_thfirst_bit += thfirst_bit; //The 31st bit

    int imm_i = strtoull(str_thfirst_bit.c_str(), NULL, 2)*(-1)*(1<<11) + strtoull((i.bstring.substr(31-30,11)).c_str(), NULL, 2); //the immediate for I-Format
    int imm_i_shift = strtoull((i.bstring.substr(31-25,6)).c_str(), NULL, 2);

    if(i.instruction_type == 0)
    {
        if(funct7 == 0)
        {
            if(funct3 == 0) operation = "add";
            else if(funct3 == 4) operation = "xor";
            else if(funct3 == 6) operation = "or";
            else if(funct3 == 7) operation = "and";
            else if(funct3 == 1) operation = "sll";
            else if(funct3 == 5) operation = "srl";
            else
            {
                cout<<"Illegal instruction!"; return;
            }
        }
        else if(funct7 == 32)
        {
            if(funct3 == 0) operation = "sub";
            else if(funct3 == 5) operation = "sra";
            else
            {
                cout<<"Illegal instruction!"; return;
            }
        }
        else
        {
            cout<<"Illegal instruction!"; return;
        }
        final_inst += operation + " " + "x" + rd + ", " + "x" + rs1 + ", " + "x" + rs2;
    }

    else if(i.instruction_type == 1)
    {
        string str_funct6 = i.bstring.substr(31-31, 6);
        // reverse(str_funct6.begin(), str_funct6.end());
        int funct6 = strtoull(str_funct6.c_str(), NULL, 2);

        
        if(funct3 == 0) operation = "addi";
        else if(funct3 == 4) operation = "xori";
        else if(funct3 == 6) operation = "ori";
        else if(funct3 == 7) operation = "andi";
        else if(funct3 == 1 && funct6 == 0) operation = "slli";
        else if(funct3 == 5 && funct6 == 0) operation = "srli";
        else if(funct3 == 5 && funct6 == 16) operation = "srai"; //imm[11:6] == 010000
        else
        {
            cout<<"Illegal instruction!"; return;
        }
        if(funct3 != 1 && funct3 != 5) final_inst += operation + " " + "x" + rd + ", " + "x" + rs1 + ", " + to_string(imm_i);
        else final_inst += operation + " " + "x" + rd + ", " + "x" + rs1 + ", " + to_string(imm_i_shift);
    }


    else if(i.instruction_type == 2 || i.instruction_type == 3)
    {
        if(i.instruction_type == 2)
        {
            if(funct3 == 0) operation = "lb";
            else if(funct3 == 1) operation = "lh";
            else if(funct3 == 2) operation = "lw";
            else if(funct3 == 3) operation = "ld";
            else if(funct3 == 4) operation = "lbu";
            else if(funct3 == 5) operation = "lhu";
            else if(funct3 == 6) operation = "lwu";
            else
            {
                cout<<"Illegal instruction!"; return;
            }
        }
        else
        {
            if(funct3 == 0) operation = "jalr";
            else
            {
                cout<<"Illegal instruction!"; return;
            }
        }
        final_inst += operation + " " + "x" + rd + ", " + to_string(imm_i) + "(x" + rs1 + ")";
    }


    else if(i.instruction_type == 4)
    {
        int imm_s_top = strtoull(str_thfirst_bit.c_str(), NULL, 2)*(-1)*(1<<11) + strtoull((i.bstring.substr(31-30,6)).c_str(), NULL, 2)*(1<<5); //the immediate for I-Format
        int imm_s = imm_s_top + stoi(rd);

        if(funct3 == 0) operation = "sb";
        else if(funct3 == 1) operation = "sh";
        else if(funct3 == 2) operation = "sw";
        else if(funct3 == 3) operation = "sd";
        else
        {
            cout<<"Illegal instruction!"; return;
        }   
        final_inst += operation + " " + "x" + rs2 + ", " + to_string(imm_s) + "(x" + rs1 + ")";
    }


    else if(i.instruction_type == 5)
    {
        int imm_b = imm_finder_b(i.bstring);

        if(funct3 == 0) operation = "beq";
        else if(funct3 == 1) operation = "bne";
        else if(funct3 == 4) operation = "blt";
        else if(funct3 == 5) operation = "bge";
        else if(funct3 == 6) operation = "bltu";
        else if(funct3 == 7) operation = "bgeu";
        else
        {
            cout<<"Illegal instruction!"; return;
        }   
        final_inst += operation + " " + "x" + rs1 + ", " + "x" + rs2 + ", " + to_string(imm_b);
    }

    else if(i.instruction_type == 6)
    {
        int imm_j = imm_finder_j(i.bstring);
        operation = "jal";
        final_inst += operation + " " + "x" + rd + ", " + to_string(imm_j);
    }

    else if(i.instruction_type == 7)
    {
        string imm_first_bit_u = "";
        imm_first_bit_u += i.bstring[0];
        string imm_lower_u = i.bstring.substr(31-30, 19);
        operation = "lui";
        int imm_u = strtoull(imm_first_bit_u.c_str(), NULL, 2)*(-1)*(1<<19) + strtoull(imm_lower_u.c_str(), NULL, 2);
        final_inst += operation + " " + "x" + rd + ", " + to_string(imm_u);
    }



    cout<<final_inst<<endl;
}






int main()
{
    ifstream file_input("input.txt");
    string line;
    int no_of_lines = 0;
    

    while(getline(file_input, line))
    {
        
        int integer_line = strtol(line.c_str(), NULL, 16);
        bitset<32> b(integer_line);
        string bstring = b.to_string();
        Instruction inst = {no_of_lines, bstring, instruction_type_finder(bstring), -1, -1};
        v_inst.push_back(inst);
        

        no_of_lines++;
    }
    for(int i = 0; i < no_of_lines; i++) parser(v_inst[i]);
    file_input.close();

}
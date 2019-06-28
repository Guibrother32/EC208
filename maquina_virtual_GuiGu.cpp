/********************************************************************************
					MAQUINA VIRTUAL DE <EC-208>
Alunos:
    Guilherme Costa
    Gustavo Marins
    Felipe Jurioli
    
	Este codigo implementa uma maquina virtual (interpretador) capaz de buscar,
decodificar e executar um set de instrucao.Criado para demostrar o funcionamento
de uma maquina virtual durante as aulas de EC-208.
***********************************************************************************
						Informacoes do Interpretadr

	Tamanho das instruções: 32-bits
 * 
 *		ADD			0000 0000
 * 		LOAD		0000 0001
 * 		STORE		0000 0010
 * 		MUL			0000 0011
 * 
 * 		Instruções tipo 1:
 * 
 * 		Operação	Reg. 1		Reg. 2		Destino
 * 		8 bits		8 bits		8 bits		8 bits
 * 
 * 		Instruções tipo 2:
 * 
 * 		Operação	Reg			Memória
 * 		8 bits		8 bits		16 bits
			
			Memória Cache:
 * 			Cache L1 de 32 bytes;
 * 			2 palavras de 32 bits;
 * 			2 linhas;
 * 			Tag = 32 - (log_2 2 + log_2 2 + 0) = 30 bits;
 * 			Total = 32 * ((2 * 2) + 30 + 1) = 1120 bits = 140 bytes
********************************************************************************/

#include <iostream>
#include <cstring>
#include <cmath>

using namespace std;

/* Tipo de instrução */
uint32_t instr_type;
/* Registrador A */
uint32_t reg_a;
/* Registrador B */
uint32_t reg_b;
/* Registrador destino */
uint32_t reg_dest;
/* Registrador endereço de memória */
uint32_t reg_addr;
/* Registrador de propósito geral */
uint32_t reg[8];
/* Memória de programa */
uint32_t prog_mem[] = {
	0x01000000, // Carrega mem[0] em reg[0]
	0x01010001, // Carrega mem[1] em reg[1]
	0x00000102, // Soma reg[0] com reg[1] em reg[2]
	0x01030002, // Carrega mem[2] em reg[3]
	0x03020302, // Multiplica reg[2] com reg[3] em reg[2]
	0x02020003, // Salva reg[2] em mem[3]
};

uint32_t addr_mem[] = {
	2, 3, 10, 7, 23, 0, 9, 4,
};

#define WORDS 2
#define LINES 2

struct line
{
	bool valid;
	uint32_t tag;
	uint32_t words[WORDS];
};

line l1[LINES];

void decode(uint32_t instr)
{
	// Pegando o tipo de instrução
	instr_type = instr >> 24;
	switch (instr_type)
	{
		// ADD
		case 0x00:
		// MUL
		case 0x03:
			reg_a 	 = (instr & 0x00FF0000) >> 16;
			reg_b 	 = (instr & 0x0000FF00) >>  8;
			reg_dest = (instr & 0x000000FF) >>  0;
			break;
		// LOAD
		case 0x01:
			reg_dest = (instr & 0x00FF0000) >> 16;
			reg_addr = (instr & 0x0000FFFF) >>  0;
			break;
		// STORE
		case 0x02:
			reg_a 	 = (instr & 0x00FF0000) >> 16;
			reg_addr = (instr & 0x0000FFFF) >>  0;
			break;
		default:
			cout << instr_type << ": illegal instruction" << endl;
			break;
	}
}

void execute()
{
	switch (instr_type)
	{
		// ADD
		case 0x00:
			reg[reg_dest] = reg[reg_a] + reg[reg_b];
			cout << "Add: " << reg[reg_dest] << endl;
			break;
		// LOAD
		case 0x01:
			reg[reg_dest] = addr_mem[reg_addr];
			cout << "Load: " << reg[reg_dest] << endl;
			break;
		// STORE
		case 0x02:
			addr_mem[reg_addr] = reg[reg_a];
			cout << "Store: " << addr_mem[reg_addr] << endl;
			break;
		// MUL
		case 0x03:
			reg[reg_dest] = reg[reg_a] * reg[reg_b];
			cout << "Mul: " << reg[reg_dest] << endl;
			break;
	}
}

uint32_t load_cache(uint32_t pc)
{
	uint32_t w = pc & 0x00000001;
	uint32_t l = pc & 0x00000002;
	uint32_t tag = pc & 0xFFFFFFFC;
	
	if (!l1[l].valid || l1[l].tag != tag)
	{
		cout << "Miss\n";
		l1[l].valid = true;
		l1[l].tag = tag;
		for (int i = 0; i < WORDS; i++)
		{
			l1[l].words[i] = prog_mem[pc + i];
		}
	}
	
	return l1[l].words[w];
}

int main()
{
	memset(l1, 0, LINES * sizeof(line));
	
	for (uint32_t pc = 0; pc < 6; pc++)
	{
		uint32_t instr = load_cache(pc);
		decode(instr);
		execute();
	}
	
	return 0;
}

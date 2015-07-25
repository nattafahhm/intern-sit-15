#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

char *regs16[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
char *regs8[8]  = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
char *segreg[4] = {"es", "cs", "ss", "ds"};
enum segment_registers {ES=0, CS, SS, DS};

BYTE get_mod(BYTE);
BYTE get_regop(BYTE);
BYTE get_rm(BYTE);
char* read_file(char *name, long *num) ;
void disasm(unsigned char *buf, long num) ;
char *rm8_r8(char *buf, int *j, int *err) ;
char *rm(char *buf, int *j, char type, int *error) ;
char *rm16_r16(char *buf, int *j, int *err) ;
char *r8_rm8(char *buf, int *j, int *err) ;
char *r16_rm16(char *buf, int *j, int *err) ;
char *imm8(char *buf, int *j, int *err) ;
char *imm16(char *buf, int *j, int *err) ;
char *rel8(char *buufer, int *j, int *err) ;
char *rm8_imm8(char *buf, int *j, int *err) ;
char *rm16_imm16(char *buf, int *j, int *err) ;
char *rm16_imm8(char *buf, int *j, int *err) ;
char *sreg_rm16(char *buf, int *j, int *error) ;
char *rm16_sreg(char *buf, int *j, int *error) ;
char *m16(char *buf, int *j, int *err) ;
char *callf(char *buf, int *j, int *err) ;
char *moffs16(char *buf, int *j, int *err) ;
char *rm8(char *buf, int *j, int *err) ;
char *rm16(char *buf, int *j, int *err) ;
char *rel16(char *buf, int *j, int *err) ;
int parse(char *s, char*(*func)(char*, int*, int*), char *buf, int *j) ;
int parse_getop(char *s, char *buf, int *j) ;
int get_bytes(int k, int j) ;

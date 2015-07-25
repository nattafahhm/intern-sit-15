#pragma GCC diagnostic ignored "-Wformat-zero-length"
#include "disasm.h"
#include "minix.h"


int main(int argc, char **argv)
{
	char *buf;
	long num ;
	if (argc > 1)
	{
		if (strlen(argv[1]) < 255)
		{
			buf = read_file(argv[1], &num) ;
			disasm(buf, num) ;
		}
	}
	return 0 ;
}

signed char segment_override = -1;

int bytes = 0 ;
int num_bytes = 0 ;

int rm_segment_override = -1;

int parse_modrm(char *s, char*(*func)(char*, int*, int *), char *buf, int *j)
{
	bytes = 1;
	int temp_j = *j ;
	int error = 0 ;
	char *result = func(buf, j, &error) ;
	if (error)
	{
		char tmp_buf[20] ;
		memset(tmp_buf, '\0', 20) ;
		BYTE tmp_char = buf[*j] ;
		sprintf(tmp_buf, "db 0x%X\n", tmp_char) ;
		parse_getop(tmp_buf, buf, j) ;
		return 0 ;
	}
	int i=0;
	int k = 16 ;
	int t = 0 ;
	if (segment_override == -1 && rm_segment_override >= 0)
	{
		t = 1;
		k = k - 2 ;
		switch (rm_segment_override)
		{
			case ES: printf("%02X", 0x26) ;	 break ;
			case CS: printf("%02X", 0x2E) ;	 break ;
			case SS: printf("%02X", 0x36) ;	 break ;
			case DS: printf("%02X", 0x3E) ;	 break ;
		}
		rm_segment_override = -1 ;
		segment_override = -1 ;
	}
	char segment[20] ;

	if (t == 1)
	{
		memset(segment, '\0', 20) ;
		switch (segment_override)
		{
			case ES: sprintf(segment, "es") ; break ;
			case CS: sprintf(segment, "cs") ; break ;
			case SS: sprintf(segment, "ss") ; break ;
			case DS: sprintf(segment, "ds") ; break ;
		}
	}
	if (segment_override >= 0 )
	{
		k = k - 2 ;
		switch (segment_override)
		{
			case ES: printf("%02X", 0x26) ;	 break ;
			case CS: printf("%02X", 0x2E) ;	 break ;
			case SS: printf("%02X", 0x36) ;	 break ;
			case DS: printf("%02X", 0x3E) ;	 break ;
		}
		segment_override = -1 ;
		rm_segment_override = -1 ;
	}
	for (i=0; i < bytes; i++)
	{
		BYTE byte = buf[temp_j+i]  ;
		printf("%02X", byte) ;
	}
	k = (k - (bytes*2))  ;
	for (i=0; i < k; i++) printf(" ") ;
	if (t == 1)
	{
		char tmp_string[255] ;
		char tmp_string2[255] ;
		memset(tmp_string, '\0', 255) ;
		memset(tmp_string2, '\0', 255) ;
		sprintf(tmp_string, s, result) ;
		sprintf(tmp_string2, "%s %s", segment, tmp_string) ;
		printf("%s", tmp_string2) ;
	}
	else printf(s, result);
	return 0;
}

int get_bytes(int k, int j)
{
	if (k + j < num_bytes) return 0;
	else return 1;
}

int parse_getop(char *s, char *buf, int *j)
{
	int k = 16 ;
	if (segment_override >= 0)
	{
		switch (segment_override)
		{
			case ES: printf("%02X", 0x26) ;	 break ;
			case CS: printf("%02X", 0x2E) ;	 break ;
			case SS: printf("%02X", 0x36) ;	 break ;
			case DS: printf("%02X", 0x3E) ;	 break ;
		}
		k = k - 2 ;
	}
	BYTE tmp_char = buf[*j] ;
	printf("%02X", tmp_char) ;
	int i= 0 ;
	k = k - 1*2 ;
	for (i=0; i < k; i++) printf(" ") ;
	if (segment_override >= 0)
	{
		char segment[20] ;
		memset(segment, '\0', 20) ;
		switch (segment_override)
		{
			case ES: sprintf(segment, "es") ; break ;
			case CS: sprintf(segment, "cs") ; break ;
			case SS: sprintf(segment, "ss") ; break ;
			case DS: sprintf(segment, "ds") ; break ;
		}
		printf("%s %s", segment, s ) ;
		segment_override = -1 ;
		rm_segment_override = -1 ;
	}
	else
	{
		printf("%s", s) ;
	}
	return 0;
}

void disasm(BYTE *buf, long num)
{

	int j = 0;
	int z = 0 ;
	BYTE t = 0;
	BYTE opcode;
	while (j < num)
	{
		unsigned int addr = j;
		if (segment_override == -1)
		printf("%08X  ", addr);
		z = buf[j] ;
		switch (buf[j])
		{
			case 0x00: parse_modrm("add %s\n", rm8_r8, buf,&j); break;
			case 0x01: parse_modrm("add %s\n", rm16_r16,buf, &j); break;
			case 0x02: parse_modrm("add %s\n", r8_rm8, buf, &j); break;
			case 0x03: parse_modrm("add %s\n", r16_rm16, buf, &j); break;
			case 0x04: parse_modrm("add al,%s\n", imm8, buf, &j); break;
			case 0x05: parse_modrm("add ax,%s\n", imm16, buf, &j); break;
			case 0x06: parse_getop("push es\n", buf, &j); break;
			case 0x07: parse_getop("pop es\n", buf, &j); break;
			case 0x08: parse_modrm("or %s\n", rm8_r8, buf,&j); break;
			case 0x09: parse_modrm("or %s\n", rm16_r16, buf, &j); break ;
			case 0x0A: parse_modrm("or %s\n", r8_rm8, buf, &j); break ;
			case 0x0B: parse_modrm("or %s\n", r16_rm16, buf, &j); break ;
			case 0x0C: parse_modrm("or al,%s\n", imm8, buf, &j); break ;
			case 0x0D: parse_modrm("or ax,%s\n", imm16, buf, &j); break ;
			case 0x0E: parse_getop("push cs\n", buf, &j); break ;
			case 0x10: parse_modrm("adc %s\n", rm8_r8, buf,&j); break ;
			case 0x11: parse_modrm("adc %s\n", rm16_r16, buf, &j); break ;
			case 0x12: parse_modrm("adc %s\n", r8_rm8, buf, &j); break ;
			case 0x13: parse_modrm("adc %s\n", r16_rm16, buf, &j); break ;
			case 0x14: parse_modrm("adc al,%s\n", imm8, buf, &j); break ;
			case 0x15: parse_modrm("adc ax,%s\n", imm16, buf, &j); break ;
			case 0x16: parse_getop("push ss\n", buf, &j); break ;
			case 0x17: parse_getop("pop ss\n", buf, &j); break ;
			case 0x18: parse_modrm("sbb %s\n", rm8_r8, buf,&j); break ;
			case 0x19: parse_modrm("sbb %s\n", rm16_r16, buf, &j) ; break;
			case 0x1A: parse_modrm("sbb %s\n", r8_rm8, buf, &j); break;
			case 0x1B: parse_modrm("sbb %s\n", r16_rm16, buf, &j); break;
			case 0x1C: parse_modrm("sbb al,%s\n", imm8, buf, &j); break;
			case 0x1D: parse_modrm("sbb ax,%s\n", imm16, buf, &j); break;
			case 0x1E: parse_getop("push ds\n", buf, &j); break;
			case 0x1F: parse_getop("pop ds\n", buf, &j); break;
			case 0x20: parse_modrm("and %s\n", rm8_r8, buf, &j); break;
			case 0x21: parse_modrm("and %s\n", rm16_r16, buf, &j); break;
			case 0x22: parse_modrm("and %s\n", r8_rm8, buf, &j); break;
			case 0x23: parse_modrm("and %s\n", r16_rm16, buf, &j); break;
			case 0x24: parse_modrm("and al,%s\n", imm8, buf, &j); break;
			case 0x25: parse_modrm("and ax,%s\n", imm16, buf, &j); break;
			case 0x26:
			{
				segment_override = ES ;
				rm_segment_override = ES;
			}
			break ;
			case 0x27: parse_getop("daa\n", buf, &j) ; break;
			case 0x28: parse_modrm("sub %s\n", rm8_r8, buf, &j); break;
			case 0x29: parse_modrm("sub %s\n", rm16_r16, buf, &j) ; break;
			case 0x2A: parse_modrm("sub %s\n", r8_rm8, buf, &j); break;
			case 0x2B: parse_modrm("sub %s\n", r16_rm16, buf, &j); break;
			case 0x2C: parse_modrm("sub al,%s\n", imm8, buf, &j); break;
			case 0x2D: parse_modrm("sub ax,%s\n", imm16, buf, &j); break;
			case 0x2E:
			{
				segment_override = CS ;
				rm_segment_override = CS ;
			} break ;
			case 0x2F: parse_getop("das\n", buf, &j); break;
			case 0x30: parse_modrm("xor %s\n", rm8_r8, buf,&j); break;
			case 0x31: parse_modrm("xor %s\n", rm16_r16, buf, &j); break;
			case 0x32: parse_modrm("xor %s\n", r8_rm8, buf, &j); break;
			case 0x33: parse_modrm("xor %s\n", r16_rm16, buf, &j); break;
			case 0x34: parse_modrm("xor al,%s\n", imm8, buf, &j); break;
			case 0x35: parse_modrm("xor ax,%s\n", imm16, buf, &j); break;
			case 0x36:
			{
				segment_override = SS ;
				rm_segment_override = SS ;
			} break ;
			case 0x37: parse_getop("aaa\n", buf, &j); break;
			case 0x38: parse_modrm("cmp %s\n", rm8_r8, buf,&j); break;
			case 0x39: parse_modrm("cmp %s\n", rm16_r16, buf, &j) ; break;
			case 0x3A: parse_modrm("cmp %s\n", r8_rm8, buf, &j); break;
			case 0x3B: parse_modrm("cmp %s\n", r16_rm16, buf, &j); break;
			case 0x3C: parse_modrm("cmp al,%s\n", imm8, buf, &j); break;
			case 0x3D: parse_modrm("cmp ax,%s\n", imm16, buf, &j); break;
			case 0x3E:
			{
				segment_override = DS ;
				rm_segment_override = DS ;
			}
			break ;
			case 0x3F: parse_getop("ass\n", buf, &j); break;
			case 0x40: parse_getop("inc ax\n", buf, &j); break;
			case 0x41: parse_getop("inc cx\n", buf, &j); break;
			case 0x42: parse_getop("inc dx\n", buf, &j); break;
			case 0x43: parse_getop("inc bx\n", buf, &j); break;
			case 0x44: parse_getop("inc sp\n", buf, &j); break;
			case 0x45: parse_getop("inc bp\n", buf, &j); break;
			case 0x46: parse_getop("inc si\n", buf, &j); break;
			case 0x47: parse_getop("inc di\n", buf, &j); break;
			case 0x48: parse_getop("dec ax\n", buf, &j); break;
			case 0x49: parse_getop("dec cx\n", buf, &j); break;
			case 0x4A: parse_getop("dec dx\n", buf, &j); break;
			case 0x4B: parse_getop("dec bx\n", buf, &j); break;
			case 0x4C: parse_getop("dec sp\n", buf, &j); break;
			case 0x4D: parse_getop("dec bp\n", buf, &j); break;
			case 0x4E: parse_getop("dec si\n", buf, &j); break;
			case 0x4F: parse_getop("dec di\n", buf, &j); break;
			case 0x50: parse_getop("push ax\n", buf, &j); break;
			case 0x51: parse_getop("push cx\n", buf, &j); break;
			case 0x52: parse_getop("push dx\n", buf, &j); break;
			case 0x53: parse_getop("push bx\n", buf, &j); break;
			case 0x54: parse_getop("push sp\n", buf, &j); break;
			case 0x55: parse_getop("push bp\n", buf, &j); break;
			case 0x56: parse_getop("push si\n", buf, &j); break;
			case 0x57: parse_getop("push di\n", buf, &j); break;
			case 0x58: parse_getop("pop ax\n", buf, &j); break;
			case 0x59: parse_getop("pop cx\n", buf, &j); break;
			case 0x5A: parse_getop("pop dx\n", buf, &j); break;
			case 0x5B: parse_getop("pop bx\n", buf, &j); break;
			case 0x5C: parse_getop("pop sp\n", buf, &j); break;
			case 0x5D: parse_getop("pop bp\n", buf, &j); break;
			case 0x5E: parse_getop("pop si\n", buf, &j); break;
			case 0x5F: parse_getop("pop di\n", buf, &j); break;
			case 0x70: parse_modrm("jo %s\n", rel8, buf, &j); break;
			case 0x71: parse_modrm("jno %s\n", rel8, buf, &j); break;
			case 0x72: parse_modrm("jc %s\n", rel8, buf, &j); break;
			case 0x73: parse_modrm("jnc %s\n", rel8, buf, &j); break;
			case 0x74: parse_modrm("jz %s\n", rel8, buf, &j); break;
			case 0x75: parse_modrm("jnz %s\n", rel8, buf, &j); break;
			case 0x76: parse_modrm("jna %s\n", rel8, buf, &j); break;
			case 0x77: parse_modrm("ja %s\n", rel8, buf, &j); break;
			case 0x78: parse_modrm("js %s\n", rel8, buf, &j); break;
			case 0x79: parse_modrm("jns %s\n", rel8, buf, &j); break;
			case 0x7A: parse_modrm("jpe %s\n", rel8, buf, &j); break;
			case 0x7B: parse_modrm("jpo %s\n", rel8, buf, &j); break;
			case 0x7C: parse_modrm("jl %s\n", rel8, buf, &j); break;
			case 0x7D: parse_modrm("jnl %s\n", rel8, buf, &j); break;
			case 0x7E: parse_modrm("jng %s\n", rel8, buf, &j); break;
			case 0x7F: parse_modrm("jg %s\n", rel8, buf, &j); break;
			case 0x80:
			{
				opcode = get_regop(buf[++j]);
				j-- ;
				BYTE t = 0 ;
				switch (opcode)
				{
					case 0x00: parse_modrm("add %s\n", rm8_imm8, buf, &j); break;
					case 0x01: parse_modrm("or %s\n", rm8_imm8, buf, &j); break;
					case 0x02: parse_modrm("adc %s\n", rm8_imm8, buf, &j); break;
					case 0x03: parse_modrm("sbb %s\n", rm8_imm8, buf, &j); break;
					case 0x04: parse_modrm("and %s\n", rm8_imm8, buf, &j); break;
					case 0x05: parse_modrm("sub %s\n", rm8_imm8, buf, &j); break;
					case 0x06: parse_modrm("xor %s\n", rm8_imm8, buf, &j); break;
					case 0x07: parse_modrm("cmp %s\n", rm8_imm8, buf, &j); break;
					default: t = 1; break;
				}
				if (t) goto print_symbol;
			} break ;
			case 0x81:
			{
				opcode = get_regop(buf[++j]);
				j--;

				switch (opcode)
				{
					case 0x00: parse_modrm("add %s\n", rm16_imm16, buf, &j); break;
					case 0x01: parse_modrm("or %s\n", rm16_imm16, buf, &j); break;
					case 0x02: parse_modrm("adc %s\n", rm16_imm16, buf, &j); break;
					case 0x03: parse_modrm("sbb %s\n", rm16_imm16, buf, &j); break;
					case 0x04: parse_modrm("and %s\n", rm16_imm16, buf, &j); break;
					case 0x05: parse_modrm("sub %s\n", rm16_imm16, buf, &j); break;
					case 0x06: parse_modrm("xor %s\n", rm16_imm16, buf, &j); break;
					case 0x07: parse_modrm("cmp %s\n", rm16_imm16, buf, &j); break;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0x83:
			{
				opcode = get_regop(buf[++j]);
				j-- ;
				BYTE t = 0 ;
				switch (opcode)
				{
					case 0x00: parse_modrm("add %s\n", rm16_imm8, buf, &j); break;
					case 0x02: parse_modrm("adc %s\n", rm16_imm8, buf, &j); break;
					case 0x03: parse_modrm("sbb %s\n", rm16_imm8, buf, &j); break;
					case 0x05: parse_modrm("sub %s\n", rm16_imm8, buf, &j); break;
					case 0x07: parse_modrm("cmp %s\n", rm16_imm8, buf, &j); break;
					default: t = 1; break;
				}
				if (t) goto print_symbol;
			} break ;
			case 0x84: parse_modrm("test %s\n", rm8_r8, buf, &j); break;
			case 0x85: parse_modrm("test %s\n", rm16_r16, buf, &j); break;
			case 0x86: parse_modrm("xchg %s\n", rm8_r8, buf, &j); break;
			case 0x87: parse_modrm("xchg %s\n", rm16_r16, buf, &j); break;
			case 0x88: parse_modrm("mov %s\n", rm8_r8, buf, &j); break;
			case 0x89: parse_modrm("mov %s\n", rm16_r16, buf, &j); break;
			case 0x8A: parse_modrm("mov %s\n", r8_rm8, buf, &j); break ;
			case 0x8B: parse_modrm("mov %s\n", r16_rm16, buf, &j); break;
			case 0x8C:
			{
				parse_modrm("mov %s\n", rm16_sreg, buf, &j) ; break;
			}
			case 0x8D: parse_modrm("lea %s\n", r16_rm16, buf, &j) ; break;
			case 0x8E:
			{
				parse_modrm("mov %s\n", sreg_rm16, buf, &j) ; break;
			}
			case 0x8F:
			{
				opcode = get_regop(buf[++j]);
				j-- ;
				BYTE t = 0 ;
				switch (opcode)
				{
					case 0x00: parse_modrm("pop word %s\n", m16, buf, &j); break;
					default: t = 1; break;
				}
				if (t) goto print_symbol;
			} break;
			case 0x90: parse_getop("xchg ax,ax\n", buf, &j); break;
			case 0x91: parse_getop("xchg cx,ax\n", buf, &j); break;
			case 0x92: parse_getop("xchg dx,ax\n", buf, &j); break;
			case 0x93: parse_getop("xchg bx,ax\n", buf, &j); break;
			case 0x94: parse_getop("xchg sp,ax\n", buf, &j); break;
			case 0x95: parse_getop("xchg bp,ax\n", buf, &j); break;
			case 0x96: parse_getop("xchg si,ax\n", buf, &j); break;
			case 0x97: parse_getop("xchg di,ax\n", buf, &j); break;
			case 0x98: parse_getop("cbw\n", buf, &j); break;
			case 0x99: parse_getop("cwd\n", buf, &j); break;
			case 0x9A: parse_modrm("call %s\n", callf, buf, &j); break;
			case 0x9B: parse_getop("wait\n", buf, &j); break;
			case 0x9C: parse_getop("pushf\n", buf, &j); break;
			case 0x9D: parse_getop("popf\n", buf, &j); break;
			case 0x9E: parse_getop("sahf\n", buf, &j); break;
			case 0x9F: parse_getop("lahf\n", buf, &j); break;
			case 0xA0: parse_modrm("mov al,%s\n", moffs16, buf, &j); break;
			case 0xA1: parse_modrm("mov ax,%s\n", moffs16, buf, &j); break;
			case 0xA2: parse_modrm("mov %s,al\n", moffs16, buf, &j); break;
			case 0xA3: parse_modrm("mov %s,ax\n", moffs16, buf, &j); break;
			case 0xA4: parse_getop("movsb\n", buf, &j); break;
			case 0xA5: parse_getop("movsw\n", buf, &j); break;
			case 0xA6: parse_getop("cmpsb\n", buf, &j); break;
			case 0xA7: parse_getop("cmpsw\n", buf, &j); break;
			case 0xA8: parse_modrm("test al, %s\n", imm8, buf, &j); break;
			case 0xA9: parse_modrm("test ax, %s\n", imm16, buf, &j); break;
			case 0xAA: parse_getop("stosb\n", buf, &j); break;
			case 0xAB: parse_getop("stosw\n", buf, &j); break;
			case 0xAC: parse_getop("lodsb\n", buf, &j); break;
			case 0xAD: parse_getop("lodsw\n", buf, &j); break;
			case 0xAE: parse_getop("scasb\n", buf, &j); break;
			case 0xAF: parse_getop("scasw\n", buf, &j); break;
			case 0xB0: parse_modrm("mov al,%s\n",imm8, buf, &j); break;
			case 0xB1: parse_modrm("mov cl,%s\n",imm8, buf, &j); break;
			case 0xB2: parse_modrm("mov dl,%s\n",imm8, buf, &j); break;
			case 0xB3: parse_modrm("mov bl,%s\n",imm8, buf, &j); break;
			case 0xB4: parse_modrm("mov ah,%s\n",imm8, buf, &j); break;
			case 0xB5: parse_modrm("mov ch,%s\n",imm8, buf, &j); break;
			case 0xB6: parse_modrm("mov dh,%s\n",imm8, buf, &j); break;
			case 0xB7: parse_modrm("mov bh,%s\n",imm8, buf, &j); break;
			case 0xB8: parse_modrm("mov ax,%s\n",imm16, buf, &j); break;
			case 0xB9: parse_modrm("mov cx,%s\n",imm16, buf, &j); break;
			case 0xBA: parse_modrm("mov dx,%s\n",imm16, buf, &j); break;
			case 0xBB: parse_modrm("mov bx,%s\n",imm16, buf, &j); break;
			case 0xBC: parse_modrm("mov sp,%s\n",imm16, buf, &j); break;
			case 0xBD: parse_modrm("mov bp,%s\n",imm16, buf, &j); break;
			case 0xBE: parse_modrm("mov si,%s\n",imm16, buf, &j); break;
			case 0xBF: parse_modrm("mov di,%s\n",imm16, buf, &j); break;
			case 0xC2: parse_modrm("ret %s\n", imm16, buf, &j) ; break ;
			case 0xC3: parse_getop("ret\n", buf, &j) ; break ;
			case 0xC4: parse_modrm("les %s\n", r16_rm16, buf, &j); break;
			case 0xC5: parse_modrm("lds %s\n", r16_rm16, buf, &j); break;
			case 0xC6: parse_modrm("mov %s\n", rm16_imm8, buf, &j); break;
			case 0xC7: parse_modrm("mov %s\n", rm16_imm16, buf, &j); break;
			case 0xCA: parse_modrm("retf %s\n", imm16, buf, &j); break;
			case 0xCB: parse_getop("retf\n", buf, &j); break;
			case 0xCC: parse_getop("int3\n", buf, &j); break;
			case 0xCD: parse_modrm("int %s\n", imm8, buf, &j) ; break;
			case 0xCE: parse_getop("into\n", buf, &j); break;
			case 0xCF: parse_getop("iret\n", buf, &j); break;
			case 0xD0:
			{
				opcode = get_regop(buf[++j]);
				j-- ;
				BYTE t = 0;
				switch (opcode)
				{
					case 0x00: parse_modrm("rol %s,1\n", rm8, buf, &j) ; break;
					case 0x01: parse_modrm("ror %s,1\n", rm8, buf, &j) ; break;
					case 0x02: parse_modrm("rcl %s,1\n", rm8, buf, &j) ; break;
					case 0x03: parse_modrm("rcr %s,1\n", rm8, buf, &j) ; break;
					case 0x04: parse_modrm("shl %s,1\n", rm8, buf, &j) ; break;
					case 0x05: parse_modrm("shr %s,1\n", rm8, buf, &j) ; break;
					case 0x07: parse_modrm("sar %s,1\n", rm8, buf, &j) ; break;
					default: t = 1; break ;
				}
				if (t) goto print_symbol;
			} break ;
			case 0xD1:
			{
				opcode = get_regop(buf[++j]);
				j-- ;
				BYTE t = 0 ;
				switch (opcode)
				{
					case 0x00: parse_modrm("rol %s,1\n", rm16, buf, &j); break;
					case 0x01: parse_modrm("ror %s,1\n", rm16, buf, &j); break;
					case 0x02: parse_modrm("rcl %s,1\n", rm16, buf, &j); break;
					case 0x03: parse_modrm("rcr %s,1\n", rm16, buf, &j); break;
					case 0x04: parse_modrm("shl %s,1\n", rm16, buf, &j); break;
					case 0x05: parse_modrm("shr %s,1\n", rm16, buf, &j); break;
					case 0x07: parse_modrm("sar %s,1\n", rm16, buf, &j); break;
					default: t = 1; break ;
				}
				if (t) goto print_symbol;
			} break ;
			case 0xD2:
			{
				opcode = get_regop(buf[++j]);
				j--;

				switch (opcode)
				{
					case 0x00: parse_modrm("rol %s,cl\n", rm8, buf, &j); break;
					case 0x01: parse_modrm("ror %s,cl\n", rm8, buf, &j); break;
					case 0x02: parse_modrm("rcl %s,cl\n", rm8, buf, &j); break;
					case 0x03: parse_modrm("rcr %s,cl\n", rm8, buf, &j); break;
					case 0x04: parse_modrm("shl %s,cl\n", rm8, buf, &j); break;
					case 0x05: parse_modrm("shr %s,cl\n", rm8, buf, &j); break;
					case 0x07: parse_modrm("sar %s,cl\n", rm8, buf, &j); break;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xD3:
			{
				opcode = get_regop(buf[++j]);
				j-- ;
				BYTE t = 0 ;
				switch (opcode)
				{
					case 0x00: parse_modrm("rol %s,cl\n", rm16, buf, &j); break;
					case 0x01: parse_modrm("ror %s,cl\n", rm16, buf, &j); break;
					case 0x02: parse_modrm("rcl %s,cl\n", rm16, buf, &j); break;
					case 0x03: parse_modrm("rcr %s,cl\n", rm16, buf, &j); break;
					case 0x04: parse_modrm("shl %s,cl\n", rm16, buf, &j); break;
					case 0x05: parse_modrm("shr %s,cl\n", rm16, buf, &j); break;
					case 0x07: parse_modrm("sar %s,cl\n", rm16, buf, &j); break;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xD4: parse_getop("aam\n", buf, &j) ; break;
			case 0xD5: parse_getop("aad\n", buf, &j) ; break;
			case 0xD7: parse_getop("xlatb\n", buf, &j) ; break;
			/*D8-DF => ESC0-7*/
			case 0xE0: parse_modrm("loopne %s\n", rel8, buf, &j); break;
			case 0xE1: parse_modrm("loope %s\n", rel8, buf, &j); break;
			case 0xE2: parse_modrm("loop %s\n", rel8, buf, &j); break;
			case 0xE3: parse_modrm("jcxz %s\n", rel8, buf, &j); break;
			case 0xE4: parse_modrm("in al,%s\n", imm8, buf, &j); break;
			case 0xE5: parse_modrm("in ax,%s\n", imm8, buf, &j); break;
			case 0xE6: parse_modrm("out %s,al\n", imm8, buf, &j); break;
			case 0xE7: parse_modrm("out %s,ax\n", imm8, buf, &j); break;
			case 0xE8: parse_modrm("call %s\n", rel16, buf, &j); break;
			case 0xE9: parse_modrm("jmp %s\n", rel16, buf, &j); break;
			case 0xEA: parse_modrm("jmp %s\n", callf, buf, &j); break;
			case 0xEB: parse_modrm("jmp short %s\n", rel8, buf, &j) ; break;
			case 0xEC: parse_getop("in al,dx\n", buf, &j); break;
			case 0xED: parse_getop("in ax,dx\n", buf, &j); break;
			case 0xEE: parse_getop("out dx,al\n", buf, &j); break;
			case 0xEF: parse_getop("out dx,ax\n", buf, &j); break;
			case 0xF0: printf("lock ") ; break;
			case 0xF2: printf("repne ") ; break;
			case 0xF3: printf("rep ") ; break ;
			case 0xF4: parse_getop("hlt\n", buf, &j); break;
			case 0xF5: parse_getop("cmc\n", buf, &j); break;
			case 0xF6:
			{
				opcode = get_regop(buf[++j]);
				j-- ;
				BYTE t = 0 ;
				switch (opcode)
				{
					case 0x00: parse_modrm("test %s\n", rm8_imm8, buf, &j); break;
					case 0x02: parse_modrm("not %s\n", rm8, buf, &j); break;
					case 0x03: parse_modrm("neg %s\n", rm8, buf, &j); break;
					case 0x04: parse_modrm("mul %s\n", rm8, buf, &j); break;
					case 0x05: parse_modrm("imul %s\n", rm8, buf, &j); break;
					case 0x06: parse_modrm("div %s\n", rm8, buf, &j); break;
					case 0x07: parse_modrm("idiv %s\n", rm8, buf, &j); break;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xF7:
			{
				opcode = get_regop(buf[++j]);
				BYTE t = 0 ;
				j-- ;
				switch (opcode)
				{
					case 0x00: parse_modrm("test %s\n", rm16_imm16, buf, &j); break;
					case 0x02: parse_modrm("not %s\n", rm16, buf, &j) ; break;
					case 0x03: parse_modrm("neg %s\n", rm16, buf, &j) ; break;
					case 0x04: parse_modrm("mul %s\n", rm16, buf, &j) ; break;
					case 0x05: parse_modrm("imul %s\n", rm16, buf, &j) ; break;
					case 0x06: parse_modrm("div %s\n", rm16, buf, &j) ; break;
					case 0x07: parse_modrm("idiv %s\n", rm16, buf, &j) ; break;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xF8: parse_getop("clc\n", buf, &j); break;
			case 0xF9: parse_getop("stc\n", buf, &j); break;
			case 0xFA: parse_getop("cli\n", buf, &j); break;
			case 0xFB: parse_getop("sti\n", buf, &j); break;
			case 0xFC: parse_getop("cld\n", buf, &j); break;
			case 0xFD: parse_getop("std\n", buf, &j); break;
			case 0xFE:
			{
				opcode = get_regop(buf[++j]);
				j-- ;
				BYTE t = 0 ;
				switch (opcode)
				{
					case 0x00: parse_modrm("inc %s\n", rm8, buf,&j); break;
					case 0x01: parse_modrm("dec %s\n", rm8, buf,&j); break;
					default: t = 1; break ;
				}
				if (t) goto print_symbol;
			} break ;
			case 0xFF:
			{
				opcode = get_regop(buf[++j]);
				j-- ;

				switch (opcode)
				{
					case 0x00: parse_modrm("inc %s\n", rm16, buf,&j); break;
					case 0x01: parse_modrm("dec %s\n", rm16, buf,&j); break;
					case 0x02: parse_modrm("call near %s\n", rm16, buf,&j); break;
					case 0x03: parse_modrm("call far %s\n", rm16, buf,&j); break;
					case 0x04: parse_modrm("jmp near %s\n", rm16, buf,&j); break;
					case 0x05: parse_modrm("jmp far %s\n", rm16, buf,&j); break;
					case 0x06: parse_modrm("push %s\n", rm16, buf,&j); break;
					default: t = 1; break;
				}
				if (t) goto print_symbol;
			} break;
			print_symbol:
			default:
			{
				char tmp_buf[20];
				memset(tmp_buf, '\0', 20);
				sprintf(tmp_buf, "db 0x%X\n", buf[j]);
				parse_getop(tmp_buf, buf, &j);
				break ;
			}
		}
		j++ ;
	}
}

char str[255] ;

char *moffs16(char *buf, int *j, int *err)
{
	memset(str, '\0', 255) ;
	char segment[10] ;
	memset(segment, '\0', 10) ;
	if (segment_override >= 0)
	{
		switch (segment_override)
		{
			case ES: sprintf(segment, "es:") ; break ;
			case CS: sprintf(segment, "cs:") ; break ;
			case SS: sprintf(segment, "ss:") ; break ;
			case DS: sprintf(segment, "ds:") ; break ;
		}
		segment_override = -1;
	}
	if(get_bytes(2,*j)) *err =1; return str;
	(*j)++ ;
	bytes++ ;
	BYTE low = buf[*j];
	(*j)++ ;
	bytes++ ;
	BYTE high = buf[*j] ;
	WORD imm16 = ((high << 8) + low);
	sprintf(str, "[%s0x%x]", segment, imm16);
	return str;
}

char *rm8(char *buf, int *j, int *err)
{
	int error = 0 ;
	memset(str, '\0', 255);
	char *s =  rm(buf, j, 8, &error);
	if(error){ *err = 1; return str;}
	sprintf(str, "%s", s);
	return str ;
}

char *rm16(char *buf, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255) ;
	char *s =  rm(buf, j, 16, &error)  ;
	if(error){ *err = 1; return str;}
	sprintf(str, "%s", s) ;
	return str ;
}

char *callf(char *buf, int *j, int *err)
{
	memset(str, '\0', 255) ;
	if (get_bytes(4, *j))
	{
		*err = 1 ;
		return str ;
	}
	(*j)++ ;
	bytes++ ;
	BYTE offset_low = buf[*j] ;
	(*j)++ ;
	bytes++ ;
	BYTE offset_high = buf[*j] ;
	(*j)++ ;
	bytes++ ;
	BYTE seg_low = buf[*j] ;
	(*j)++ ;
	bytes++ ;
	BYTE seg_high = buf[*j] ;
	WORD offset = ((offset_high << 8) + offset_low) ;
	WORD seg = ((seg_high << 8) + seg_low) ;
	sprintf(str,"0x%x:0x%x", seg, offset) ;
	return str ;
}

char *m16(char *buf, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255) ;
	char *s =  rm(buf, j, 16, &error)  ;
	if(error){ *err = 1; return str;}
	sprintf(str,"%s", s) ;
	return str ;
}

char *sreg_rm16(char *buf, int *j, int *err)
{
	int error = 0 ;
	memset(str, '\0', 255) ;
	if (get_bytes(1, *j)){ *err = 1; return str; }
	BYTE reg = get_regop(buf[++(*j)]);
	(*j)-- ;
	char *s =  rm(buf, j, 16, &error) ;
	if (error) { *err = 1 ; return str ;}
	if (reg < 4){
		char *sreg = segreg[reg] ;
		sprintf(str,"%s,%s", sreg, s) ;
		*err = 0 ;
	}else *err = 1 ;
	return str ;
}

char *rm16_sreg(char *buf, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255) ;
	if (get_bytes(1, *j)){ *err = 1; return str; }
	BYTE reg = get_regop(buf[++(*j)]);
	(*j)-- ;
	char *s =  rm(buf, j, 16, &error) ;
	if (error) { *err = 1 ; return str; }
	if (reg < 4){
		char *sreg = segreg[reg] ;
		sprintf(str,"%s,%s", s, sreg);
		*err = 0 ;
	}
	else *err = 1;
	return str ;
}

char *rm16_imm8(char *buf, int *j, int *err)
{
	int error = 0 ;
	memset(str, '\0', 255) ;
	char *s = rm(buf, j, 16, &error);
	if (error){ *err = 1; return str; }
	(*j)++ ;
	if (get_bytes(1, *j)){ (*j)--; *err = 1; return str; }
	(*j)--;
	(*j)++;
	bytes++ ;
	signed char imm8 = buf[*j] ;
	char sign = '+';
	if (imm8 < 0){ sign = '-'; imm8 = -imm8; }
	sprintf(str, "%s,%c0x%x", s, sign, imm8);
	return str;
}

char *rm16_imm16(char *buf, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255) ;
	char *s = rm(buf, j, 16, &error) ;
	if(error){ *err = 1; return str;}
	(*j)++ ;
	if (get_bytes(2, *j)){ (*j)--; *err = 1; return str; }
	(*j)--;
	(*j)++;
	bytes++ ;
	BYTE low = buf[*j] ;
	(*j)++;
	bytes++;
	BYTE high = buf[*j] ;
	WORD imm16 = ((high << 8) + low);
	sprintf(str, "%s,0x%x", s, imm16) ;
	return str;
}

char *rm8_imm8(char *buf, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255) ;
	char *s = rm(buf, j, 8, &error) ;
	if(error){ *err = 1; return str;}
	(*j)++ ;
	if (get_bytes(1, *j)){ (*j)--; *err = 1; return str; }
	(*j)-- ;
	(*j)++;
	bytes++ ;
	BYTE imm8 = buf[*j];
	sprintf(str, "%s,0x%x", s, imm8);
	return str;
}

char *rel16(char *buf, int *j, int *err)
{
	memset(str, '\0', 255) ;
	if (get_bytes(2, *j)){*err = 1; return str; }
	(*j)++;
	bytes++;
	BYTE rel_low = buf[*j];
	(*j)++;
	bytes++;
	BYTE rel_high = buf[*j];
	signed short rel = ((rel_high << 8) + rel_low);
	WORD result = *j + rel + 1;
	sprintf(str, "0x%x", result);
	return str;
}

char *rel8(char *buf, int *j, int *err)
{
	memset(str, '\0', 255) ;
	if (get_bytes(1, *j)){ *err = 1; return str; }
	(*j)++ ;
	bytes++;
	signed char rel = buf[*j];
	WORD result = *j + rel + 1;
	sprintf(str, "0x%x", result);
	return str;
}

char *imm8(char *buf, int *j, int *err)
{
	memset(str, '\0', 255);
	if (get_bytes(1, *j)){ *err = 1; return str;}
	(*j)++;
	bytes++;
	BYTE imm8 = buf[*j];
	sprintf(str, "0x%x", imm8);
	return str;
}

char *imm16(char *buf, int *j, int *err)
{
	memset(str, '\0', 255) ;
	if (get_bytes(2, *j)){ *err = 1; return str; }
	(*j)++ ;
	bytes++ ;
	BYTE low = buf[*j];
	(*j)++;
	bytes++;
	BYTE high = buf[*j] ;
	WORD imm16 = ((high << 8) + low);
	sprintf(str, "0x%x", imm16);
	return str ;
}

char *r16_rm16(char *buf, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255);
	BYTE reg = get_regop(buf[++(*j)]);
	(*j)-- ;
	char *s = rm(buf, j, 16, &error);
	if (error){ *err = 1; return str; }
	char *reg16 = regs16[reg];
	sprintf(str, "%s,%s", reg16, s);
	return str;
}

char *rm8_r8(char *buf, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255);
	BYTE reg = get_regop(buf[++(*j)]);
	(*j)-- ;
	char *s = rm(buf, j, 8, &error);
	if (error){ *err = 1; return str; }
	char *reg8 = regs8[reg] ;
	sprintf(str, "%s,%s", s, reg8);
	return str;
}

char *r8_rm8(char *buf, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255);
	BYTE reg = get_regop(buf[++(*j)]);
	(*j)-- ;
	char *s = rm(buf, j, 8, &error);
	if(error){ *err = 1; return str;}
	char *reg8 = regs8[reg] ;
	sprintf(str, "%s,%s", reg8, s);
	return str;
}

char *rm16_r16(char *buf, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255);
	BYTE reg = get_regop(buf[++(*j)]);
	(*j)-- ;
	char *s = rm(buf, j, 16, &error);
	if(error){ *err = 1; return str;}
	char *reg16 = regs16[reg] ;
	sprintf(str, "%s,%s", s, reg16);
	return str ;
}

char rm_str[255] ;

char *rm(char *buf, int *j, char type, int *error)
{
	memset(rm_str, '\0', 255);
	bytes++ ;
	if (get_bytes(1, *j)){ bytes = 1; *error = 1; return rm_str;}
	BYTE rm_byte = buf[++(*j)];
	BYTE mod = get_mod(rm_byte);
	BYTE rm8 = get_rm(rm_byte);
	char disp_str[255] ;
	memset(rm_str, '\0', 255);
	memset(disp_str, '\0', 255);
	char segment[10] ;
	memset(segment, '\0', 10);
	if (segment_override >= 0){
		switch (segment_override){
			case ES: sprintf(segment, "es:"); break;
			case CS: sprintf(segment, "cs:"); break;
			case SS: sprintf(segment, "ss:"); break;
			case DS: sprintf(segment, "ds:"); break;
		}
	}
	switch (mod){
		case 0x0:{
			if (rm8 == 0x06)
			{
				if (get_bytes(2, *j)) { *error = 1 ; (*j)--; return rm_str; }
				(*j)++ ;
				bytes++ ;
				BYTE low = buf[*j];
				(*j)++ ;
				bytes++ ;
				BYTE high = buf[*j];
				WORD disp = ((high << 8) + low);
				char sign = '+' ;
				sprintf(disp_str, "%c0x%x", sign, disp);
			}
			else sprintf(disp_str, "");
		} break;
		case 0x01:
		{
			if (get_bytes(1, *j)){ *error = 1; (*j)--; return rm_str;}
			signed char disp_low = buf[++(*j)];
			signed short disp = disp_low ;
			bytes++ ;
			char sign = '+' ;
			if (disp < 0){ sign = '-'; disp = ~disp; disp++; }
			sprintf(disp_str, "%c0x%x", sign, disp);
		} break;
		case 0x02:
		{
			if (get_bytes(2, *j)){ *error = 1; (*j)-- ; return rm_str; }
			(*j)++ ;
			bytes++;
			BYTE low = buf[*j];
			(*j)++;
			bytes++;
			BYTE high = buf[*j] ;
			WORD disp = ((high << 8) + low);
			char sign = '+';
			sprintf(disp_str, "%c0x%x", sign, disp);
		} break ;
		case 0x03: {
			if (type == 8) return regs8[rm8];
			if (type == 16) return regs16[rm8];
		}break;
	}
	switch (rm8){
		case 0x00: sprintf(rm_str, "[%sBX+SI%s]", segment, disp_str); break;
		case 0x01: sprintf(rm_str, "[%sBX+DI%s]", segment, disp_str); break;
		case 0x02: sprintf(rm_str, "[%sBP+SI%s]", segment, disp_str); break;
		case 0x03: sprintf(rm_str, "[%sBP+DI%s]", segment, disp_str); break;
		case 0x04: sprintf(rm_str, "[%sSI%s]", segment, disp_str); break;
		case 0x05: sprintf(rm_str, "[%sDI%s]", segment, disp_str); break;
		case 0x06: sprintf(rm_str, "[%sBP%s]", segment, disp_str); break;
		case 0x07: sprintf(rm_str, "[%sBX%s]", segment, disp_str); break;
	}
	return rm_str;
}

/* Return Mod from given ModR/M byte, according to Figure 2-1 */
BYTE get_mod(BYTE buf) {
	return (buf & 0xC0) >> 6;
}

/* Return Reg/Opcode from given ModR/M byte, according to Figure 2-1 */
BYTE get_regop(BYTE buf) {
	return (buf & 0x38) >> 3;
}

/* Return R/M from given ModR/M byte, according to Figure 2-1 */
BYTE get_rm(BYTE b) {
	return (b & 0x07);
}

char* read_file(char *name, long *num)
{
	FILE *fp ;
	char *buf;
	fp = fopen(name, "rb") ;
	if (fp != NULL){
		fseek(fp, -a_hdrlen, SEEK_END);
		*num = ftell(fp) ;
		fseek(fp, a_hdrlen , SEEK_SET);
		buf = (char*) malloc(sizeof(char) * (*num));
		if (buf == NULL) { printf("Memory error!\n") ; exit(1); }
		long result = fread(buf, 1, *num, fp);
		if (result != *num){ printf("File read error!\n"); exit(1); }
		num_bytes  = *num;
		free(buf);
		return buf;
	}else{
		printf("Error opening file!\n");
		exit(1) ;
	}
}

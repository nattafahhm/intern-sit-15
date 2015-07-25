/* The <a.out> header file describes the format of executable files. */
#ifndef _AOUT_H
#define _AOUT_H

typedef	struct {			/* a.out header */
  unsigned char	a_magic[2];	/* magic number */
  unsigned char	a_flags;	/* flags, see below */
  unsigned char	a_cpu;		/* cpu id */
  unsigned char	a_hdrlen;	/* length of header */
  unsigned char	a_unused;	/* reserved for future use */
  unsigned short a_version;	/* version stamp (not used at present) */
  long		a_text;		/* size of text segement in bytes */
  long		a_data;		/* size of data segment in bytes */
  long		a_bss;		/* size of bss segment in bytes */
  long		a_entry;	/* entry point */
  long		a_total;	/* total memory allocated */
  long		a_syms;		/* size of symbol table */

  /* SHORT FORM ENDS HERE */
  long		a_trsize;	/* text relocation size */
  long		a_drsize;	/* data relocation size */
  long		a_tbase;	/* text relocation base */
  long		a_dbase;	/* data relocation base */

}EXECSTRUCT;

#define A_MAGIC0      (unsigned char) 0x01
#define A_MAGIC1      (unsigned char) 0x03
#define BADMAG(X)     ((X).a_magic[0] != A_MAGIC0 ||(X).a_magic[1] != A_MAGIC1)

#define a_hdrlen 0x20


#endif /* _AOUT_H */

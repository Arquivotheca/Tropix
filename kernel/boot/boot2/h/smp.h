/*
 ****************************************************************
 *								*
 *			smp.h					*
 *								*
 *	Definições para Multiprocessamento			*
 *								*
 *	Versão	3.2.3, de 11.04.00				*
 *		4.7.0, de 11.02.05				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	SMP_IDENT	(('_' << 24) | ('P' << 16) | ('M' << 8) | '_')
#define	MPH_IDENT	(('P' << 24) | ('M' << 16) | ('C' << 8) | 'P')

/*
 ******	Tipos de Entradas na tabela SMP *************************
 */
typedef	enum
{
	MPE_PROC,
	MPE_BUS,
	MPE_APIC,
	MPE_INTR,
	MPE_LINTR

}	MPTYPE;

/*
 ******	Entrada do tipo PROC na tabela SMP **********************
 */
typedef	struct
{
	uchar		pe_type;	/* Deve valer 0 */
	uchar		pe_apicid;
	uchar		pe_apicver;
	uchar		pe_flags;
	ulong		pe_signature;
	ulong		pe_fflags;
	ulong		pe_filler[2];

}	MPPROC;

/*
 ******	Entrada do tipo APIC na tabela SMP **********************
 */
typedef struct
{
	uchar		ae_type;	/* Deve valer 2 */
	uchar		ae_id;
	uchar		ae_version;
	uchar		ae_flags;
	ulong		ae_addr;

}	MPAPIC;

/*
 ******	Entrada do tipo INTR na tabela SMP **********************
 */
typedef struct
{
	uchar		ie_type;	/* Deve valer 3 ou 4 */
	uchar		ie_itype;
	uchar		ie_flag[2];
	uchar		ie_busid;
	uchar		ie_irq;
	uchar		ie_destid;
	uchar		ie_intin;

}	MPINTR;

/*
 ******	Cabeçalho da tabela SMP *********************************
 */
typedef struct
{
	long		mph_signature;
	ushort		mph_length;
	uchar		mph_spec;
	uchar		mph_checksum;
	char		mph_oemid[8];
	char		mph_prodid[12];
	void		*mph_oemp;
	ushort		mph_oemsz;
	ushort		mph_count;
	void		*mph_apic;
	ushort		mph_etlen;
	uchar		mph_etchecksum;
	uchar		mph_filler;

}	MPHEADER;

/*
 ******	Tabela MP Floating **************************************
 */
typedef	struct
{
	long		mpf_signature;
	MPHEADER	*mpf_addr;
	uchar		mpf_length;
	uchar		mpf_spec;
	uchar		mpf_checksum;
	uchar		mpf_feature[5];

}	MPF;

#define	NOMPF	(MPF *)NULL

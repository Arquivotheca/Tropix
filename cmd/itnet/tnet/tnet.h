/*
 ****************************************************************
 *								*
 *			<tnet.h>				*
 *								*
 *	Formato do segmento de comunicação da TNET		*
 *								*
 *	Versão	2.3.0, de 23.10.91				*
 *		3.0.5, de 05.08.97				*
 *								*
 *	Módulo: TCMPTO_C/TNET					*
 *		Utilitários de comunicações			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Algumas constantes universais				*
 ****************************************************************
 */
#define	IN_CALLs		/* Usa as chamadas especiais */

#define	MIN_COMPRESS_SZ	100

#define	SEGCRCSZ	(BLSZ/2)

/*
 ******	Verifica se as macro variáveis estão definidas **********
 */
#if	!defined (BIG_ENDIAN) && !defined (LITTLE_ENDIAN)
#error	"Uma das macrovariáveis \"BIG/LITTLE_ENDIAN\" deve estar definida"
#endif	BIG_ENDIAN/LITTLE_ENDIAN

/*
 ******	Definições relativas à ordem de bytes da CPU ************
 */
#ifdef	LITTLE_ENDIAN
#define	ENDIAN_LONG(x)	long_endian_cv (x)
#define	ENDIAN_SHORT(x)	short_endian_cv (x)
#else
#define	ENDIAN_LONG(x)	(x)
#define	ENDIAN_SHORT(x)	(x)
#endif	LITTLE_ENDIAN

/*
 ****************************************************************
 *	Códigos dos diversos tipos de SEGMENTOS			*
 ****************************************************************
 */
typedef enum
{
	FQUIT = -1,	/* Reservado pelo protocolo de segmentos */

	FFLAGS = 1,	/* Flags iniciais do programa para o parceiro */
	FEOF,		/* Final do programa */
	FMSG,		/* Mensagem para o parceiro */
	FNAME,		/* Nome de um arquivo */
	FSTAT,		/* Estado de um arquivo */
	FCMP,		/* Envie os CRCs */
	FCRC,		/* Segmento de CRCs */
	FBLOCK,		/* Bloco de um arquivo */
	FLINK		/* Informações de LINK */

}	SEG_TYPE;

/*
 ****** Segmento contendo uma mensagem **************************
 */
typedef struct
{
	int	m_type;			/* Tipo */
	int	m_code;			/* Código */
	int	m_len;			/* Tamanho */

}	MSG;

/*
 ****** Segmento do tipo "FFLAGS" *********************************
 */
typedef struct
{
	int	seg_type;		/* Tipo */
	int	seg_flags[16];		/* Flags */

}	SEGFLAGS;

/*
 ****** Estrutura das operações *********************************
 */
typedef enum
{
	CP = 0,			/* Cópia de sentido ainda indefinido */
	CPTO,			/* Cópia para o nó remoto */
	CPFROM,			/* Cópia para o nó local */

	L_LINK,			/* Operação de LINK local */
	R_LINK,			/* Operação de LINK remoto */

	L_RM,			/* Operação de remoção local */
	R_RM,			/* Operação de remoção remota */

	L_RM_LINK,		/* Operação de remoção e LINK locais */
	L_RM_CPFROM,		/* Operação de remoção e cópia locais */
	R_RM_LINK,		/* Operação de remoção e LINK remotos */
	R_RM_CPTO,		/* Operação de remoção e cópia remotos */

	F_PROCESS,		/* Arquivo aceito (-i) */
	F_PASS			/* Arquivo não aceito (-i) */

}	OP_NM;

#define OP_STR { "CP", "CPTO", "CPFROM", "L_LINK", "R_LINK", "L_RM", "R_RM" }

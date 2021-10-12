/*
 ****************************************************************
 *								*
 *			<sys/mntent.h>				*
 *								*
 *	Parâmetros de montagem de systemas de arquivos		*
 *								*
 *	Versão	4.8.0, de 27.09.05				*
 *		4.8.0, de 27.09.05				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2005 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	MNTENT_H		/* Para definir os protótipos */

/*
 ****** Estrutura com as opções de montagem *********************
 */
struct mntent
{
	int	me_flags;		/* Indicadores: veja abaixo */
	int	me_uid, me_gid;		/* Usuário e Grupo */

	/* Vários parâmetros para NFS */

	long	me_server_addr;		/* Endereço IP do servidor */
	int	me_rsize;		/* Tamanho de leitura */
	int	me_wsize;		/* Tamanho de escrita */
	int	me_timeout;		/* Tempo para retransmissão */
	int	me_retrans;		/* No. de retransmissões */
	int	me_port;		/* No. da porta */
	int	me_nfsvers;		/* Versão do protocolo NFS */

	int	me_reser[6];		/* Reserva para 64 bytes */
};

/*
 ****** Os valores de "me_flags" ********************************
 */
enum
{
	SB_RONLY	= (1 <<  0),	/* O FS é READONLY */
	SB_USER		= (1 <<  1),	/* Pode ser montado por usuários regulares */
	SB_ATIME	= (1 <<  2),	/* Atualiza os tempos de acesso */ 
	SB_DEV		= (1 <<  3),	/* Interpreta os dispositivos */ 
	SB_EXEC		= (1 <<  4),	/* Executa programas */ 
	SB_SUID		= (1 <<  5),	/* Interpreta os indicadores SUID/SGID */ 
	SB_EJECT	= (1 <<  6),	/* Ejeta após a desmontagem */ 
	SB_CASE		= (1 <<  7),	/* A "caixa alta/baixa" importa nos nomes */ 
	SB_ROCK		= (1 <<  8),	/* Usar a extensão "Rock Ridge", se presente */ 
	SB_JOLIET	= (1 <<  9),	/* Usar a extensão "Joliet", se presente */ 
	SB_LFN		= (1 << 10),	/* Lê/escreve com "long file names" */ 
	SB_HARD		= (1 << 11),	/* Montagem "soft/hard" para NFS */ 

	SB_AUTO		= (1 << 29),	/* Monta com "mount -a" */
	SB_FSCK		= (1 << 30)	/* Verifica o sistema de arquivos */

	/* Não usar o bit 31 para evitar números negativos */
};

/*
 ****** Os valores de "default" *********************************
 */
enum {	SB_DEFAULT_ON	= (SB_DEV|SB_EXEC|SB_SUID|SB_CASE|SB_ROCK|SB_JOLIET|SB_LFN|SB_AUTO) };

enum {	SB_NOT_KERNEL	= (SB_AUTO|SB_FSCK) };

enum {	SB_RSIZE = 1024, SB_WSIZE = 1024, SB_TIMEOUT = 1, SB_RETRANS = 8, SB_PORT = 2049, SB_NFSVERS = 2 };

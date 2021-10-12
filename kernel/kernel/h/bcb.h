/*
 ****************************************************************
 *								*
 *			<sys/bcb.h>				*
 *								*
 *	Conjunto de informações passado do BOOT para o KERNEL	*
 *								*
 *	Versão	3.0.0, de 31.08.94				*
 *		4.9.0, de 20.02.08				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2008 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	BCB_H			/* Para definir os protótipos */

#define	BCB_VERSION	5	/* Para conferir entre o "boot2" e o núcleo */

/*
 ******	Informação sobre os discos IDE **************************
 */
typedef struct
{
	int	h_unit;		/* No. do controlador */
	int	h_port;		/* Porta inicial do controlador */
	int	h_target;	/* No. do disco neste controlador */

	char	h_letter;	/* Identificação do "target" */
	char	h_is_present;	/* Está presente */
	char	h_bl_shift;	/* LOG (2) do tamanho do bloco */

	int	h_head;		/* No. de cabeças  (REAL) */
	int	h_sect;		/* No. de setores  (REAL) */
	int	h_cyl;		/* No. de cilindros (REAL) */

	int	h_multi;	/* No. máximo de setores / operação */
	int	h_atavalid;	/* Validação */
	int	h_piomode;	/* PIO modes */
	int	h_wdmamode;	/* DMA modes */
	int	h_udmamode;	/* UDMA modes */

	int	h_flags;	/* Indicadores (ver abaixo) */

}	HDINFO;

#define NOHDINFO (HDINFO *)0	/* Ponteiro NULO */

/*
 ****** Bloco de informações ************************************
 */
struct bcb
{
	int		y_version;	/* No. da versão desta estrutura */

	int		y_video_pos;	/* Posição do cursor */

	int		y_cputype;	/* Tipo da CPU */
	int		y_cpu_features;	/* Características da CPU */

	int		y_DELAY_value;	/* Para 1 micro-segundo */

	int		y_physmem;	/* Memória total (em KB) */

	int		y_ssize;	/* Tamanho da tabela de símbolos */

	const void	*y_disktb;	/* Endereço da tabela de discos */
	int		y_disktb_sz;	/* Tamanho (em bytes) da tabela de discos */

	int		y_rootdev;	/* Dispositivo RAIZ */

	char		*y_dmesg_area;	/* Mensagens do "boot2" */
	char		*y_dmesg_ptr;
	const char	*y_dmesg_end;

	const void	*y_videodata;	/* Os dados do VIDEO (estrutura VIDEODATA) */

	void		*y_int_10_addr;	/* Endereço (16 bits) da INT 10 */

	int		y_res[9];	/* Para 96 bytes */
};

/*
 ****************************************************************
 *								*
 *			<sys/mon.h>				*
 *								*
 *	Bloco de monitoração					*
 *								*
 *	Versão	3.0.0, de 01.12.94				*
 *		4.6.0, de 17.06.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	MON_H			/* Para definir os protótipos */

/*
 ******	A estrutura *********************************************
 */
struct	mon
{
	/*
	 *	Informações sobre o uso das CPUs
	 */
	int	y_ticks[NCPU];	/* No. de ticks do relógio */
	int	y_stop[NCPU];	/* No. de stops de idle */

	/*
	 *	Informações sobre o uso dos discos IDE + SCSI
	 */
	int	y_sasi;		/* No. de ticks do relógio */

	/*
	 *	Informações sobre ETHERNET
	 */
	int	y_ed;

	/*
	 *	Informações sobre o PPP
	 */
	int	y_ppp_in;
	int	y_ppp_out;

	/*
	 *	Informações sobre os blocos
	 */
	int	y_block_full_cnt;
	int	y_block_dirty_cnt;

	int	y_block_lru_total;	/* Estatísticas da LRU */
	int	y_block_lru_hit;
	int	y_block_lru_miss;

	/*
	 *	Informações sobre os INODEs
	 */
	int	y_inode_in_use_cnt;
	int	y_inode_dirty_cnt;

	/*
	 *	Informações sobre os KFILEs
	 */
	int	y_kfile_in_use_cnt;

	/*
	 *	Informações sobre o USB
	 */
	int	y_usb;		/* No. de ticks do relógio */

	/*
	 *	Reserva
	 */
	int	y_res[22];	/* Reservado para uso futuro */
};

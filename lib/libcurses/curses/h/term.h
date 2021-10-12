/*
 ****************************************************************
 *								*
 *			term.h					*
 *								*
 *	Estrutura TERM						*
 *								*
 *	Versão	2.0.0, de 10.05.88				*
 *		3.0.0, de 22.03.95				*
 *								*
 *	Modulo: term.h						*
 *		libcurses/curses				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 *	Os seguintes arquivos devem ser incluidos antes deste:
 *
 *		<sys/types.h>
 *		<termio.h>
 *		<stdio.h>
 *		<curses.h>
 *		<curses/defs.h>
 *		<curses/info.h>
 */

/*
 ******	Tipo KEY - tecla especial *******************************
 */
typedef struct
{
	bool	k_partial;
	chtype	k_key;
	char	*k_string;

}	KEY;

/*
 ******	Tipo TERM - estado de um terminal ***********************
 */
typedef	struct	_term	TERM;

struct	_term
{
	INFO	t_info;		/* Capacidades do Terminal */

	TERMIO	t_oldtermio,	/* Modo de Comunicação com o Terminal */
		t_termio;

	FILE	*t_infp,	/* File Pointers para o Terminal */
		*t_outfp;

	char	*t_inbuf,	/* Buffers para t_infp e t_outfp */
		*t_outbuf;

	ushort	t_lines,	/* No. total de Linhas */
		t_cols;			/*      Colunas */

	chtype	**t_screen;	/* Mapa do Vídeo */
	char	**t_map;	/* Janelas em cada posição do Vídeo */

	WINDOW	*t_stdwin;	/* Janela Padrão para este Terminal */

	ushort	t_curx,		/* Posição Atual do Cursor */
		t_cury;

	TERM	*t_list;	/* Lista de terminais */

	WINDOW	*t_first,	/* Fila de Janelas */
		*t_last;

	void	(*t_switch) (int, ...);	/* Função para troca de Janelas */

	int	t_oflags;	/* Flags do fcntl */

	KEY	*t_keys;	/* Tabela de Teclas Especiais */
	ushort	t_nkeys;	/* No. de '' */
	short	t_reserv;

	chtype	t_attr;		/* Atributos Correntes */
	ushort	t_flags;	/* Veja abaixo */

};

#define		TERMSZ		sizeof (TERM)
#define		NOTERM		(TERM *) 0

/*
 *	Valores para t_flag (*)
 */
#define	T_NOTHING	0x00	/* */
#define	T_CLEAR		0x01	/* Limpe o Vídeo no próximo refresh */
#define	T_ISOKEY	0x02	/* Teclado gera caracteres acentuados */
#define	T_FLUSH		0x04	/* Buffer de saída precisa ser descarregado */
#define T_OUT		0x08	/* Terminal fora do modo Curses */

/*
 ******	Lista de terminais **************************************
 */
extern	TERM	*_firstterm;		/* Terminal 0 */
extern	TERM	*_curterm;		/* Terminal Corrente */

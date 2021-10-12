/*
 ****************************************************************
 *								*
 *			<sys/kbd.h>				*
 *								*
 *	Definições do teclado e porta auxiliar do camundongo	*
 *								*
 *	Versão	4.4.0, de 27.11.02				*
 *		4.4.0, de 27.11.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	KBD_H			/* Para definir os protótipos */

/*
 ******	Portas de E/S *******************************************
 */
#define KBD_STATUS_PORT 	0x64	/* status port, read */
#define KBD_COMMAND_PORT	0x64	/* controller command port, write */
#define KBD_DATA_PORT		0x60	/* data port, read/write  */

/*
 ****** Comandos do controlador *********************************
 *
 *	Enviados à porta KBD_COMMAND_PORT
 */
#define KBDC_SET_COMMAND_BYTE 	0x60
#define KBDC_GET_COMMAND_BYTE 	0x20
#define KBDC_WRITE_TO_AUX    	0xD4
#define KBDC_DISABLE_AUX_PORT 	0xA7
#define KBDC_ENABLE_AUX_PORT 	0xA8
#define KBDC_TEST_AUX_PORT   	0xA9
#define KBDC_DIAGNOSE	     	0xAA
#define KBDC_TEST_KBD_PORT   	0xAB
#define KBDC_DISABLE_KBD_PORT 	0xAD
#define KBDC_ENABLE_KBD_PORT 	0xAE

/*
 ****** Argumento do comando KBDC_SET_COMMAND_BYTE **************
 */
#define KBD_TRANSLATION		0x40
#define KBD_RESERVED_BITS	0x04
#define KBD_OVERRIDE_KBD_LOCK	0x08
#define KBD_ENABLE_KBD_PORT    	0x00
#define KBD_DISABLE_KBD_PORT   	0x10
#define KBD_ENABLE_AUX_PORT	0x00
#define KBD_DISABLE_AUX_PORT	0x20
#define KBD_ENABLE_AUX_INT	0x02
#define KBD_DISABLE_AUX_INT	0x00
#define KBD_ENABLE_KBD_INT     	0x01
#define KBD_DISABLE_KBD_INT    	0x00
#define KBD_KBD_CONTROL_BITS	(KBD_DISABLE_KBD_PORT | KBD_ENABLE_KBD_INT)
#define KBD_AUX_CONTROL_BITS	(KBD_DISABLE_AUX_PORT | KBD_ENABLE_AUX_INT)

/*
 ******	Comandos do teclado *************************************
 *
 *	Enviados à porta KBD_DATA_PORT
 */
#define KBDC_RESET_KBD	     	0xFF
#define KBDC_ENABLE_KBD		0xF4
#define KBDC_DISABLE_KBD	0xF5
#define KBDC_SET_DEFAULTS	0xF6
#define KBDC_SEND_DEV_ID	0xF2
#define KBDC_SET_LEDS		0xED
#define KBDC_ECHO		0xEE
#define KBDC_SET_SCANCODE_SET	0xF0
#define KBDC_SET_TYPEMATIC	0xF3

/*
 ******	Comandos do Camundongo **********************************
 *
 *	Enviados à porta KBD_DATA_PORT
 */
#define PSMC_RESET_DEV	     	0xFF
#define PSMC_ENABLE_DEV      	0xF4
#define PSMC_DISABLE_DEV     	0xF5
#define PSMC_SET_DEFAULTS	0xF6
#define PSMC_SEND_DEV_ID     	0xF2
#define PSMC_SEND_DEV_STATUS 	0xE9
#define PSMC_SEND_DEV_DATA	0xEB
#define PSMC_SET_SCALING11	0xE6
#define PSMC_SET_SCALING21	0xE7
#define PSMC_SET_RESOLUTION	0xE8
#define PSMC_SET_STREAM_MODE	0xEA
#define PSMC_SET_REMOTE_MODE	0xF0
#define PSMC_SET_SAMPLING_RATE	0xF3

/*
 ******	Argumento do comando PSMC_SET_RESOLUTION ****************
 */
#define PSMD_RES_LOW		0	/* 25ppi */
#define PSMD_RES_MEDIUM_LOW	1	/* 50ppi */
#define PSMD_RES_MEDIUM_HIGH	2	/* 100ppi (default) */
#define PSMD_RES_HIGH		3	/* 200ppi */

#define PSMD_MAX_RESOLUTION	PSMD_RES_HIGH

/*
 ******	Freqüência de amostragem ********************************
 */
#define PSMD_MAX_RATE		255	/* FIXME: not sure if it's possible */

/*
 ******	Bits de estado ******************************************
 */
#define KBDS_BUFFER_FULL	0x21
#define KBDS_ANY_BUFFER_FULL	0x01
#define KBDS_KBD_BUFFER_FULL	0x01
#define KBDS_AUX_BUFFER_FULL	0x21
#define KBDS_ONLY_AUX_BUFFER	0x20
#define KBDS_INPUT_BUFFER_FULL	0x02
#define	KBDS_STAT_GTO		0x40	/* Tempo expirado de transmissão/recepção */
#define	KBDS_STAT_PERR		0x80	/* Erro de paridade */

/*
 ******	Códigos de retorno **************************************
 */
#define KBD_ACK 		0xFA
#define KBD_RESEND		0xFE
#define KBD_RESET_DONE		0xAA
#define KBD_RESET_FAIL		0xFC
#define KBD_DIAG_DONE		0x55
#define KBD_DIAG_FAIL		0xFD
#define KBD_ECHO		0xEE

#define PSM_ACK 		0xFA
#define PSM_RESEND		0xFE
#define PSM_RESET_DONE		0xAA
#define PSM_RESET_FAIL		0xFC

/*
 ******	Identificação do camundongo *****************************
 */
#define PSM_MOUSE_ID		0
#define PSM_BALLPOINT_ID	2
#define PSM_INTELLI_ID		3
#define PSM_EXPLORER_ID		4
#define PSM_4DMOUSE_ID		6
#define PSM_4DPLUS_ID		8

enum { MOUSE_MOUSE, MOUSE_TRACKBALL, MOUSE_UNKNOWN };

/*
 ******	Definições de tempos e número de tentativas *************
 */
#define KBD_MAXRETRY	8

#define KBD_RESETDELAY  200	 /* wait 200msec after kbd/mouse reset */

#define KBD_MAXWAIT	5 	/* wait 5 times at most after reset */

#define KBDC_DELAYTIME	20
#define KBDD_DELAYTIME	7

/*
 ******	Fila de caracteres **************************************
 */
#define KBDQ_BUFSIZE	32

typedef struct
{
	char	*head;
	char	*tail;
	int	count;
	char	q[KBDQ_BUFSIZE];

}	KBDQ;

extern KBDQ	kbd_kbd_q;

extern KBDQ	kbd_mouse_q;

/*
 ******	Vários modelos de camundongo ****************************
 */
enum
{
	MOUSE_MODEL_UNKNOWN	= -1,
	MOUSE_MODEL_GENERIC,
	MOUSE_MODEL_GLIDEPOINT,
	MOUSE_MODEL_NETSCROLL,
	MOUSE_MODEL_NET,
	MOUSE_MODEL_INTELLI,
	MOUSE_MODEL_THINK,
	MOUSE_MODEL_EASYSCROLL,
	MOUSE_MODEL_MOUSEMANPLUS,
	MOUSE_MODEL_KIDSPAD,
	MOUSE_MODEL_VERSAPAD,
	MOUSE_MODEL_EXPLORER,
	MOUSE_MODEL_4D,
	MOUSE_MODEL_4DPLUS
};

#define MOUSE_MSS_PACKETSIZE		3
#define MOUSE_INTELLI_PACKETSIZE	4
#define MOUSE_MSC_PACKETSIZE		5
#define MOUSE_MM_PACKETSIZE		3
#define MOUSE_PS2_PACKETSIZE		3
#define MOUSE_PS2INTELLI_PACKETSIZE	4
#define MOUSE_VERSA_PACKETSIZE		6
#define MOUSE_PS2VERSA_PACKETSIZE	6
#define MOUSE_4D_PACKETSIZE		3	
#define MOUSE_4DPLUS_PACKETSIZE		3	
#define MOUSE_SYS_PACKETSIZE		8

#define MOUSE_PS2PLUS_SYNCMASK		0x48
#define MOUSE_PS2PLUS_SYNC		0x48
#define MOUSE_PS2PLUS_ZNEG		0x08	/* sign bit */
#define MOUSE_PS2PLUS_BUTTON4DOWN	0x10	/* 4th button on MouseMan+ */
#define MOUSE_PS2PLUS_BUTTON5DOWN	0x20
#define MOUSE_PS2PLUS_CHECKBITS(b)	((((b[2] & 0x03) << 2) | 0x02) == (b[1] & 0x0F))
#define MOUSE_PS2PLUS_PACKET_TYPE(b)	(((b[0] & 0x30) >> 2) | ((b[1] & 0x30) >> 4))

/*
 ******	Protótipos de funções ***********************************
 */
extern void	kbd_restore_controller (int command_byte);
extern int	kbd_test_aux_port (void);
extern int	kbd_reset_aux_dev (void);
extern int	kbd_enable_aux_dev (void);
extern int	kbd_disable_aux_dev (void);
extern int	kbd_get_mouse_status (int *status, int flag, int len);
extern int 	kbd_mouse_ext_command (int command);
extern int	kbd_get_aux_id (void);
extern int	kbd_set_mouse_scaling (int scale);
extern int	kbd_set_mouse_resolution (int val);
extern int	kbd_get_mouse_buttons (void);
extern void	kbd_empty_both_buffers (int wait);
extern void	kbd_empty_aux_buffer (int wait);
extern int	kbd_get_controller_command_byte (void);
extern int	kbd_set_controller_command_byte (int mask, int command);
extern int	kbd_write_controller_command (int command);
extern int	kbd_wait_while_controller_busy (void);
extern int	kbd_read_controller_data (void);
extern int	kbd_write_controller_data (int data);
extern int	kbd_write_aux_command (int command);
extern int	kbd_read_aux_data (void);
extern int	kbd_read_aux_data_no_wait (void);
extern int	kbd_wait_for_aux_data (void);
extern int	kbd_wait_for_data (void);
extern int	kbd_send_aux_command (int command);
extern int	kbd_send_aux_command_and_data (int command, int data);
extern int	kbd_putq (KBDQ *q, int c);
extern int	kbd_getq (KBDQ *q);
extern void	kbd_emptyq (KBDQ *q);
extern void     key_send_cmd (int cmd);

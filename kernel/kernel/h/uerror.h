/*
 ****************************************************************
 *								*
 *			<sys/uerror.h>				*
 *								*
 *	Código dos erros do usuário				*
 *								*
 *	Versão	3.0.0, de 05.10.94				*
 *		4.8.0, de 23.10.05				*
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

#define	UERROR_H		/* Para definir os protótipos */

/*
 ****************************************************************
 *	Códigos de erros					*
 ****************************************************************
 */
typedef	enum
{
	/* 00: */ NOERR,		/* Erro ZERO */
	/* 01: */ EPERM,		/* Não é o dono ou superusuário */
	/* 02: */ ENOENT,		/* Nome de arquivo desconhecido */
	/* 03: */ ESRCH,		/* Número de processo desconhecido */
	/* 04: */ EINTR,		/* Chamada ao sistema interrompida */
	/* 05: */ EIO,			/* Erro de entrada/saída */
	/* 06: */ ENXIO,		/* Dispositivo desconhecido ou endereço fora dos limites */
	/* 07: */ E2BIG,		/* Lista de argumentos longa demais */
	/* 08: */ ENOEXEC,		/* Arquivo não-executável */
	/* 09: */ EBADF,		/* Descritor de arquivo inválido */
	/* 10: */ ECHILD,		/* O Processo não possui filhos */
	/* 11: */ EAGAIN,		/* Recurso não disponível, tente mais tarde */
	/* 12: */ ENOMEM,		/* Memória insuficiente */
	/* 13: */ EACCES,		/* Permissão negada */
	/* 14: */ EFAULT,		/* Endereço inválido de memória */
	/* 15: */ ENOTBLK,		/* Este dispositivo não é de blocos */
	/* 16: */ EBUSY,		/* Recurso ocupado */
	/* 17: */ EEXIST,		/* O arquivo já existe */
	/* 18: */ EXDEV,		/* Elo através de sistema de arquivos */
	/* 19: */ ENODEV,		/* Dispositivo inexistente */
	/* 20: */ ENOTDIR,		/* Não é um diretório */
	/* 21: */ EISDIR,		/* É um diretório */
	/* 22: */ EINVAL,		/* Argumento ou operação inválido */
	/* 23: */ ENFILE,		/* Tabela FILE cheia */
	/* 24: */ EMFILE,		/* Arquivos abertos demais */
	/* 25: */ ENOTTY,		/* O dispositivo não é de caracteres */
	/* 26: */ ETXTBSY,		/* Texto ocupado */
	/* 27: */ EFBIG,		/* Arquivo excessivamente grande */
	/* 28: */ ENOSPC,		/* Sistema de arquivos cheio */
	/* 29: */ ESPIPE,		/* \"Seek\" em PIPE ou FIFO */
	/* 30: */ EROFS, 		/* Proteção de escrita */
	/* 31: */ EMLINK,		/* Número excessivo de elos físicos */
	/* 32: */ EPIPE,		/* PIPE sem leitor */
	/* 33: */ EDOM,			/* Argumento fora dos limites */
	/* 34: */ ERANGE,		/* Resultado fora dos limites */
	/* 35: */ EDEADLK,		/* Foi evitado um \"deadlock\" */
	/* 36: */ ENOLCK,		/* Tabela de \"locks\" cheia */
	/* 37: */ EOLINE,		/* Dispositivo não pronto */
	/* 38: */ EPFULL,		/* FIFO Cheio */
	/* 39: */ ENOTFS,		/* Este dispositivo não contem um sistema de arquivos */
	/* 40: */ EOLDVER,		/* Referenciando biblioteca compartilhada antiga */
	/* 41: */ ENOTMNT,		/* Dispositivo não montado */
	/* 42: */ ETIMEOUT,		/* Tempo de espera esgotado */
	/* 43: */ ENOTREG,		/* Não é um arquivo regular */
	/* 44: */ EBADFMT,		/* Formato inválido */
	/* 45: */ ENOSHLIB,		/* Biblioteca compartilhada não carregada */
	/* 46: */ ELOOP,		/* Aninhamento excessivo de elos simbólicos */
	/* 63: */ ENAMETOOLONG = 63,	/* Nome excessivamente comprido */
	/* 66: */ ENOTEMPTY = 66,	/* Diretório não vazio */
	/* 69: */ EDQUOT = 69,		/* Quota de disco excedida */
	/* 70: */ ESTALE, 		/* Referência não mais válida a arquivo remoto */
	/* 71: */ EREMOTE, 		/* Nível excessivo de referências remotas em um caminho */

	/* 72: */ TBADNET,		/* Rede não inicializada */
	/* 73: */ TUNKNET,		/* Nome/endereço de rede desconhecido */
	/* 74: */ TBADADDR,		/* Erro no formato do endereço */
	/* 75: */ TADDRBUSY,		/* Endereço já sendo utilizado */
	/* 76: */ TNOADDR,		/* Não consegui alocar o endereço */
	/* 77: */ TBADOPT,		/* Informações das opções inválidas */
	/* 78: */ TBADDATA,		/* Informações do dado inválidas */
	/* 79: */ TNODATA,		/* Ainda não há dados disponíveis */
	/* 80: */ TBADSEQ,		/* Número de seqüência inválido */
	/* 81: */ TBADQLEN,		/* Tamanho de fila inválido */
	/* 82: */ TBADFLAG,		/* Indicadores inválidos */
	/* 83: */ TNODIS,		/* Não há pedido de desconexão abortiva */
	/* 84: */ TNOREL,		/* Não há pedido de desconexão ordenada */
	/* 85: */ TBUFOVFLW,		/* Mensagem maior do que a área disponível */
	/* 86: */ TOUTSTATE,		/* Estado inválido */
	/* 87: */ TLOOK,		/* Evento ocorrido exige atenção */
	/* 88: */ TFLOW,		/* Controle de fluxo impede transmissão */
	/* 89: */ TNOUDERR,		/* Não houve erro no protocolo sem conexão */
	/* 90: */ TNOTSUPPORT,		/* Função não implementada */
	/* 91: */ TNOROUTE,		/* Não há rota para o nó destino */
	/* 92: */ TDNSSERVER		/* Erro no servidor de DNS */

}	ERRNO;

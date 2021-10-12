.bp
.he 'USTAT (sys)'TROPIX: Manual de Referência'USTAT (sys)'
.fo 'Atualizado em 21.11.03'Versão 4.5.0'Pag. %'

.b NOME
.in 5
.wo "ustat -"
obtem o estado de um sistema de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/syscall.h>
#include <ustat.h>

int	ustat (dev_t dev, USTAT *up);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "ustat" fornece uma série de informações
sobre um sistema de arquivos montado.
O argumento "dev" dá a
identificação do dispositivo contendo o sistema de arquivos
(que pode ser obtido através de um "stat" (sys)).
O argumento "up" é um ponteiro para uma estrutura "USTAT", que
é preenchida pela chamada ao sistema.

.sp
A estrutura "USTAT" contém os seguintes membros:

.sp
.nf
	int	f_type;		/* Tipo/subtipo do sistema de arquivos */
	int	f_sub_type;
	int	f_bsize;	/* Tamanho do bloco */
	int	f_nmsize;	/* Tamanho máximo do identificador */
	daddr_t	f_fsize;	/* No. total de blocos */
	daddr_t	f_tfree;	/* No. total de blocos livres */
	ino_t	f_isize;	/* No. total de nós-índice */
	ino_t	f_tinode;	/* No. total de nós-índice livres */
	char	f_fname[16];	/* Nome do sistema de arquivos */
	char	f_fpack[16];	/* Nome do volume */
	short	f_m, f_n;	/* Fator de entrelaçamento */
	int	f_symlink_sz;	/* Tamanho a partir do qual está no disco */
.fi

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve zero.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
stat
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

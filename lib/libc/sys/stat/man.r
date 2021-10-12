.bp
.he 'STAT (sys)'TROPIX: Manual de Referência'STAT (sys)'
.fo 'Atualizado em 16.04.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
Obtem o estado de um arquivo:
.sp
.wo "stat  -"
dado o nome do arquivo (acompanha os elos simbólicos)
.br
.wo "lstat -"
dado o nome do arquivo (NÃO acompanha os elos)
.br
.wo "fstat -"
dado o descritor do arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/syscall.h>
#include <stat.h>

int	stat (const char *path, STAT *sp);
int	lstat (const char *path, STAT *sp);
int	fstat (int fd, STAT *sp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As chamadas ao sistema "stat", "lstat" e "fstat" obtém o estado
(isto é, uma série de informações) de um arquivo.

.sp
Nas chamadas "stat" e "lstat", é dado o nome do arquivo "path".
Não é necessário ter permissão para ler, escrever ou executar 
o arquivo, mas todos os diretórios contidos em seu caminho devem
ter acesso de busca.

.sp
A chamada "lstat" é igual à "stat", exceto quando o arquivo "path"
é um elo simbólico, caso no qual "lstat" retorna informações sobre
o elo, enquanto que "stat" retorna informações sobre o arquivo
apontado pelo elo.

.sp
Na chamada "fstat", o arquivo já está aberto, e é dado
o seu descritor "fd" obtido de uma chamada ao sistema "open", "creat", "dup",
"fcntl", ou "pipe".

.sp
O argumento "sp" é um ponteiro para uma estrutura "STAT", que
é preenchida com as informações sobre o arquivo.
A estrutura contém os seguintes membros:

.sp
.nf
	dev_t	st_dev;		/* Dispositivo de residencia */
	ino_t	st_ino;		/* No. do Inode */
	long	st_mode;	/* Modo do Arquivo (ver <stat.h>) */
	int	st_nlink;	/* No. de Pont. para este Arq. */
	ushort 	st_uid;		/* UID do Arquivo */
	ushort 	st_gid;		/* GID do Arquivo */
	dev_t	st_rdev;	/* Dispositivo (BLK e CHR) */
	off_t	st_size;	/* Tamanho do Arquivo */
	time_t	st_atime;	/* Tempo de Acesso */
	time_t	st_mtime;	/* Tempo de Modificação */
	time_t	st_ctime;	/* Tempo de Criação */
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
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
chmod, chown, creat, link, mknod, pipe, read, time,
.br
.wo "        unlink, utime, write, instat, upstat"
.br
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.bp
.he 'DIRENT (etc)'TROPIX: Manual de Referência'DIRENT (etc)'
.fo 'Atualizado em 06.08.01'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "dirent -"
operações em diretórios
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <dirent.h>

DIR	*opendir (const char *dirnm);
DIR	*inopendir (int dev, int ino);
DIRENT	*readdir (DIR *dp);
off_t	telldir (const DIR *dp);
int	seekdir (DIR *dp, off_t offset);
int	rewinddir (DIR *dp);
int	closedir (DIR *dp);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A série "dirent" de funções possibilita a leitura de diretórios em
um formato independente do sistema de arquivos (veja o arquivo <sys/sysdirent.h>).

.sp
A função "opendir" abre o diretório "dirnm" e retorna um ponteiro para
a estrutura DIR a ele associado, que será usado para identificar
o diretório nas futuras operações.
Um ponteiro NULO é retornado em caso do diretório não puder ser acessado,
ou não houver memória disponível para as estruturas de dados necessárias. 

.sp
A função "inopendir" é similar à "opendir", mas o diretório é identificado pelo
par (dispositivo, nó-índice) ao invés do nome.
Este par pode ser obtido através de uma chamada "stat" ou "fstat" (sys).

.sp
A função "readdir" retorna um ponteiro para a entrada seguinte do diretório.
O nome contido no membro "d_name" sempre é terminado com um NULO.
Entradas vazias do diretório são puladas, mas as entradas "." e ".."
são fornecidas.
Um ponteiro NULO é retornado no final do diretório, ou no caso de algum erro.

.sp
A função "telldir" retorna um valor representando a posição corrente do diretório.
Repare que os valores retornados por "telldir" são válidos apenas enquanto o
diretório não for fechado.

.sp
A função "seekdir" atribui a posição corrente do diretório para a próxima
leitura através de "readdir". O valor "offset" deverá ser um valor obtido anteriormente
através de "telldir"; se for dado um valor inválido, os resultados serão imprevisíveis.

.sp
A função "rewinddir" atribui a posição corrente do diretório para a primeira entrada.

.sp
A função "closedir" fecha o diretório e libera a memória associada
às estruturas de dados.

.in
.sp
.b
VALORES DE RETORNO
.r
.in 5
Em caso de erro, as funções retornam um ponteiro NULO ou o valor
inteiro -1, e indicam em "errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
stat, fstat, getdirent
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.bp
.he 'GETDIRPAR (sys)'TROPIX: Manual de Referência'GETDIRPAR (sys)'
.fo 'Atualizado em 02.11.05'Versão 4.8.0'Pag. %'

.b NOME
.in 5
.wo "getdirpar  -"
obtem informações sobre o pai de um diretório
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/types.h>
#include <sys/sysdirent.h>    (ou #include "dirent.h")

ino_t	getdirpar (dev_t dev, ino_t ino, DIRENT *dep);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "getdirpar" tenta obter informações
sobre o pai do diretório definido pelo par (dispositivo "dev", nó-índice "ino").

.sp
Se o diretório ("dev", "ino") for a própria raiz dos sistemas de arquivos ("/"),
"getdirpar" retorna um valor NULO, sem informações adicionais.

.sp
Em caso contrário, retorna o nó-índice do pai do diretório, e preenche
os membros da estrutura apontada por "dep" do seguinte modo:

.sp
.(l
	d_name:		Nome do diretório
	d_namlen:	Número de caracteres do nome
	d_ino:		Nó-índice do próprio diretório
	d_offset:	Dispositivo do próprio diretório
.)l

.sp
No caso particular do diretório dado ("dev", "ino")
for a raiz de um sistema de arquivos, os seguinte membros recebem:

.sp
.(l
	d_ino:		Nó-índice do diretório montado
	d_offset:	Dispositivo deste diretório
.)l

.sp
Não é necessário ter nenhuma permissão de leitura, escrita ou execução
do arquivo.

.sp
Esta chamada ao sistema é especialmente útil quando se caminha no
sistema de arquivos no sentido das folhas em direção à raiz
(inclusive ultrapassando os pontos de montagem), o que é útil para
a obtenção do caminho completo do diretório corrente.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve o número do nó-índice
do diretório pai do diretório dado.
No caso particular do diretório dado ser "/", retorna NULO.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Esta chamada ao sistema foi criada principalmente para facilitar
a função da biblioteca "getcwd" (libc) (e portanto o utilitário "pwd" (cmd)).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
pwd
.br
.wo "(libc): "
getcwd
.br
.wo "(sys):  "
stat, instat
.br

.in
.sp
.b ESTADO
.in 5
Experimental

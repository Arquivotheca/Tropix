.bp 1
.he 'GRENT (libc)'TROPIX: Manual de Referência'GRENT (libc)'
.fo 'Atualizado em 07.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Leitura do arquivo de grupos:
.sp
 getgrent - obtém a entrada seguinte do arquivo
 setgrent - prepara a releitura do arquivo
 endgrent - conclui a leitura do arquivo
 getgrgid - obtém a entrada do grupo de número dado
 getgrnam - obtém a entrada do grupo de nome dado
.br
.wo "grcache  -"
obtém rapidamente o nome do grupo 

.in
.sp
.b SINTAXE
.(l
#include <sys/types.h>
#include <grp.h>

GROUP	*getgrent (void);
void	setgrent (void);
void	endgrent (void);
GROUP	*getgrgid (int gid)
GROUP	*getgrnam (const char *name)
char 	*grcache (int gid)
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "getgrent" fornece, a cada chamada, uma entrada do arquivo de grupos
("/etc/group"), no formato da estrutura "GROUP". Com sucessivas
chamadas a esta função, pode-se ler sequencialmente todo o arquivo.

.sp
A função "setgrent" prepara o arquivo para que seja feita uma nova leitura.
Não é necessário chamá-la para o primeiro processamento.

.sp
A função "endgrent" encerra o processamento do arquivo.

.sp
A função "getgrgid" procura no arquivo a entrada com o número do grupo dado,
e "getgrnam" a entrada com o nome do grupo dado.
.sp

A função "grcache" fornece o nome do grupo, dado o seu número,
mantendo uma lista interna de todos os nomes já fornecidos.
Através desta lista, é capaz de fornecer rapidamente nomes
já pedidos anteriormente.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função "getgrent" retorna um ponteiro para a estrutura GROUP contendo
a entrada seguinte do arquivo. No caso de final do arquivo,
ela devolve um ponteiro NULO. 

.sp
As funções "getgrgid", "getgrnam" e "grcache"
devolvem ponteiros NULOs, caso a entrada desejada
não seja encontrada.

.in
.sp
.b OBSERVAÇÕES
.in 5
O processamento do membro "gr_mem" NÃO está implementado.
A função "getgrent" devolve sempre um ponteiro NULO.

.sp
A funções "getgrent", "getgrgid" e "getgrnam"
devolvem um ponteiro para uma área estática, e o
seu conteúdo deve ser copiado caso ele seja necessário após
as funções serem chamadas novamente.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
pwent
.br
.wo "(fmt): "
group
.br

.in
.sp
.b ARQUIVOS
.in 5
/etc/group

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

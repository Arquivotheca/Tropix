.bp 1
.he 'PWENT (libc)'TROPIX: Manual de Referência'PWENT (libc)'
.fo 'Atualizado em 07.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Leitura do arquivo de contas/senhas:
.sp
.nf
     getpwent  - obtém a entrada seguinte do arquivo
     setpwent  - prepara a releitura do arquivo
     endpwent  - conclui a leitura do arquivo
     getpwuid  - obtém a entrada do usuário de número dado
     getpwnam  - obtém a entrada do usuário de nome dado
     pwcache   - obtém rapidamente o nome do usuário 
     getpwpos  - obtém a posição do ponteiro do arquivo
     getpwslot - obtém o número da linha no arquivo
.fi
.in

.sp
.b SINTAXE
.(l
#include <sys/types.h>
#include <pwd.h>

PASSWD	*getpwent (void);
void	setpwent (void);
void	endpwent (void);
PASSWD	*getpwuid (int uid);
PASSWD	*getpwnam (const char *name);
char	*pwcache (int uid);
off_t	getpwpos (void);
int	getpwslot (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "getpwent" fornece, a cada chamada,
uma entrada do arquivo de contas/senhas
("/etc/passwd"), no formato da estrutura "PASSWD". Com sucessivas
chamadas a esta função, pode-se ler sequencialmente todo o arquivo.

.sp
A função "setpwent" prepara o arquivo para que seja feita uma nova leitura.
Não é necessário chamá-la para o primeiro processamento.

.sp
A função "endpwent" encerra o processamento do arquivo.

.sp
A função "getpwuid" procura no arquivo a entrada com
o número do usuário dado,
e "getpwnam" a entrada com o nome do usuário dado
(não é necessário chamar "setpwent").

.sp
A função "pwcache" fornece o nome do usuário, dado o seu número,
mantendo uma lista interna de todos os nomes já fornecidos.
Através desta lista, é capaz de fornecer rapidamente nomes
já pedidos anteriormente.

.sp
A função "getpwpos" fornece a posição corrente do ponteiro do arquivo
de contas/senhas.

.sp
A função "getpwslot" fornece o número da linha do arquivo de
contas/senhas (começando de 0) correspondente às informações
fornecidas por uma chamada a uma das
funções "getpwent",  "getpwuid" ou  "getpwnam".

.in
.sp
.(t
.b
VALOR DE RETORNO
.r
.in 5
A função "getpwent" retorna um ponteiro para a estrutura PASSWD contendo
a entrada seguinte do arquivo. No caso de final do arquivo,
ela devolve um ponteiro NULO. 
.)t

.sp
As funções "getpwuid", "getpwnam" e "pwcache"
devolvem ponteiros NULOs, caso a entrada desejada
não seja encontrada.

.sp
As funções "getpwpos" e "getpwslot" devolvem um número negativo,
caso o arquivo não esteja aberto.

.in
.sp
.b OBSERVAÇÃO
.in 5
As funções "getpwent", "getpwuid", e "getpwnam"
devolvem ponteiros para uma área estática, e o
seu conteúdo deve ser copiado caso ele seja necessário após
as funções serem chamadas novamente.

.in
.sp
.b
VEJA TAMBÉM
.r
.nf
     (libc): grent, getlogin
     (fmt):  passwd
.fi

.in
.sp
.b ARQUIVOS
.in 5
/etc/passwd

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

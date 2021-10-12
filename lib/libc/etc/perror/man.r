.bp 1
.he 'PERROR (libc)'TROPIX: Manual de Referência'PERROR (libc)'
.fo 'Atualizado em 23.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Obtenção das mensagens de erro das chamadas ao sistema:
.sp
.wo "perror      -"
imprime a mensagem de erro
.br
.wo "errno       -"
o número do erro
.br
.wo "strerror    -"
obtém uma cadeia com a mensagem de erro 
.br

.in
.sp
.b SINTAXE
.(l
#include <errno.h>

void	perror (const char *pgname);
extern int	errno;	

#include <string.h>

char	*strerror (int err);
.)l

.sp
.b DESCRIÇÃO
.in 5
A função "perror" escreve uma mensagem na saída de erro padrão
(descritor de arquivos 2), descrevendo
o último erro ocorrido durante uma chamada ao sistema.

.sp
Em primeiro lugar é impressa  a cadeia
"pgname" (que em geral contém o nome do programa), seguida de
":" (dois pontos), um <sp>, a mensagem de erro e um <nl>.
O número do erro é obtido da variável "errno".

.sp
A variável "errno" contém sempre o número do último erro ocorrido
durante uma chamada ao sistema. Note que esta variável NÃO é zerada
quando é feita uma chamada ao sistema com sucesso.

.sp
A função "strerror" devolve um ponteiro para uma cadeia contendo uma
descrição correspondente ao código numérico "err" de um erro.
A cadeia não contém o <nl> de final de linha.

.in
.sp
.b DIAGNÓSTICOS
.in 5
A função "strerror" devolve um ponteiro para as mensagens
"Erro Desconhecido" ou "Erro ZERO", caso o argumento "err" contenha,
respectivamente, um número de erro inválido ou NOERR (zero).
Em situações análogas, "perror" imprime estas mesmas mensagens.

.in
.sp
.b OBSERVAÇÃO
.in 5
Os textos das mensagens de erro NÃO estão mais incluídos em módulos
da "libc", mas são obtidos do núcleo do TROPIX através de uma chamada
ao sistema. Deste modo, as variáveis "sys_errlist" e "sys_nerr" NÃO
existem mais; sua funcionalidade foi assumida pela função "strerror".

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

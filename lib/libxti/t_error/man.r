.bp
.he 'T_ERROR (xti)'TROPIX: Manual de Referência'T_ERROR (xti)'
.fo 'Escrito em 06.08.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_error -"
imprime uma mensagem de erro
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>
#include <errno.h>

int t_error (const char *errmsg);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_error" escreve uma mensagem na saída de erro padrão
("stderr"), descrevendo o último erro ocorrido durante uma
chamada a uma função do sistema de transporte.

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | errmsg		 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

.sp
Em primeiro lugar é impressa  a cadeia
"errmsg" (que em geral contém o nome do programa), seguida de
":" (dois pontos), um <sp>, a mensagem de erro e um <nl>.
O número do erro é obtido da variável
externa "t_errno", que recebe o número do erro quando este ocorre.

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
Nesta implementação, os erros da interface de transporte
estão unificados com os erros do sistema operacional, de tal
modo que a variável "t_errno" é idêntica à variável "errno",
e a função "t_error" é idêntica à função "perror" (libc).
Isto já está definido no arquivo <xti.h>, e portanto é transparente
para o usuário.

.sp
Como conseqüências desta unificação, o erro [TSYSERR] nunca ocorre
e a função "strerror" (libc) pode ser usada para facilitar
a formatação das mensagens de erro.

.sp
Para facilitar a impressão de diagnósticos,
nesta implementação está disponível a função "t_strevent", que
fornece uma cadeia contendo a descrição de um evento.

.in
.sp
.b DIAGNÓSTICOS
.in 5
A função "t_error" imprime as mensagens
"Erro Desconhecido" ou "Erro ZERO", caso a variável "t_errno" contenha
um número de erro inválido ou NOERR (zero), respectivamente.

.in
.sp
.b OBSERVAÇÃO
.in 5
A variável "t_errno" não é zerada quando é feita uma chamada correta
ao sistema de transporte.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
perror, strerror
.br
.wo "(xti): "
t_intro, t_look, t_strevent
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
"X/OPEN Portability Guide: Networking Services",
X/OPEN Company Ltd.,
Prentice Hall, New Jersey, 1st Edition, 1988.

.in
.sp
.(t
.b ESTADO
.in 5
Efetivo.
.)t

.in

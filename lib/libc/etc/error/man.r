.bp
.he 'ERROR (libc)'TROPIX: Manual de Referência'ERROR (libc)'
.fo 'Atualizado em 14.03.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "error -"
Imprime uma mensagem de erro
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

void		error (const char *format, ...);

const char  	*error_default_text;
void		(*error_default_function) (int);
int		error_default_value;
int		error_standard_count;
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "error" imprime a mensagem de erro "format" na saída padrão
de mensagens de erro ("stderr"). A cadeia "format" pode conter especificações
de conversão ("\%..."), conforme as convenções de "printf" (libc).

.sp
Se o primeiro caractere de "format" for "$",
após a impressão da mensagem (sem o "$"),
será chamada a função "error_default_function"
com o argumento "error_default_value"
(cujos valores iniciais são a função "exit" (sys) e "1").
Isto significa que, normalmente, o caractere inicial "$" irá causar
o término do programa.

.bc	/*************************************/
O valor inicial de "error_default_function" é a função "exit" (sys),
e o valor inicial de "error_default_value" é "1".
Isto significa que, normalmente, 
(o que normalmente irá causar o término do programa, veja abaixo).
.ec	/*************************************/

.sp
Se o caractere seguinte for "*", é adicionada ao final do texto,
entre parêntesis, a
mensagem de erro (relativa às chamadas ao sistema) eventualmente presente
na variável global "errno" (veja "strerror" (libc)).

.sp
Se o caractere seguinte for "@", a variável "error_standard_count" é
incrementada de 1. Isto é útil para obter o número total de erros ocorridos
de certa classe (que são indicados através de um "@" na mensagem).

.sp
Finalmente, um caractere <nl> é automaticamente anexado ao final de "format".

.sp
Se o argumento "format" for um ponteiro NULO, será utilizada a cadeia
"error_default_text", cujo valor inicial é "$Memória esgotada",
ou seja, o programa será encerrado após a impressão do texto "Memória esgotada".

.sp
As 3 primeiras variáveis globais definidas acima podem ser alteradas,
para configurar o funcionamento de "error" conforme desejado.

.bc	/*************************************/
; os valores iniciais são:
.in +3
.ip "1." 3
O valor inicial de "error_default_text", é "$Memória esgotada".

.ip "2." 3
O valor inicial de "error_default_function" é a função "exit" (sys).

.ip "3." 3
O valor inicial de "error_default_value" é "1".

.ep
.in -3
.ec	/*************************************/

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
exit
.br
.wo "(libc):"
printf, strerror
.br

.(t
.in
.sp
.b EXEMPLO
.in 7
Se o valor de "file_nm" for "arquivo" e
a variável "errno" contiver o valor EACCES,
.sp
.nf
		error ("*Não consegui abrir \"\%s\"", file_nm);

.fi

irá imprimir

.sp
.nf
	 Não consegui abrir "arquivo" (Não tem Permissão de Acesso)

.fi
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

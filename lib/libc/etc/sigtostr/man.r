.bp 1
.he 'SIGTOSTR (libc)'TROPIX: Manual de Referência'SIGTOSTR (libc)'
.fo 'Atualizado em 26.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
sigtostr - obtém a descrição de um sinal

.in
.sp
.b SINTAXE
.(l
#include <signal.h>

const char	*sigtostr (int signum)
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "sigtostr" devolve um ponteiro
para uma cadeia com o nome e a descrição do sinal "signum".

.sp
Os primeiros 8 caracteres contém o nome do sinal,
enquanto que os caracteres  restantes descrevem o sinal.
A cadeia NÃO contém um <nl> no final.

.in
.sp
.b DIAGNÓSTICOS
.in 5
Se "signum" contiver um número de sinal inválido, "sigtostr" devolverá
"Sinal  desconhecido".

.sp
Se "signum" contiver 0, "sigtostr" devolverá
"SIGNULL: Sinal NULO".

.in
.sp
.b
VEJA TAMBÉM
.r
.nf
     (cmd): kill
     (sys): signal
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

.bp 1
.he 'ABORT (libc)'TROPIX: Manual de Referência'ABORT (libc)'
.fo 'Atualizado em 06.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "abort  -"
termina o processo anormalmente
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

void	abort (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "abort" termina o processo anormalmente
(enviando o sinal "SIGABRT")
a não ser que este sinal seja interceptado,
e a função interceptadora não retorne.

.sp
As funções registradas são chamadas (veja "atexit" (libc))
e os arquivos abertos têm os seus conteúdos atualizados antes do término
do processo.

.sp
Se o sinal não tiver sido programado para ser ignorado ou interceptado,
uma imagem do processo é gravada em disco ("core dump").

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
A função "abort" não pode retornar.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.br
.wo "(sys):  "
exit, kill, signal
.br
.wo "(libc): "
atexit

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

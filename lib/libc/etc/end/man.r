.bp 1
.he 'END (libc)'TROPIX: Manual de Referência'END (libc)'
.fo 'Atualizado em 20.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Finais do programa:
.sp
.wo "end   -"
final do programa
.br
.wo "etext -"
final do texto
.br
.wo "edata -"
final da área de dados
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

extern char	end;
extern char	etext;
extern char	edata;
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas variáveis possibilitam a um programa saber o endereço
final (e portanto o tamanho) de cada uma de suas seções.

.sp
O endereço de "etext" (e NÃO
o seu conteúdo) dá o final do texto, o endereço de "edata"
o final da área de dados inicializados,
e o endereço de "end" o final do programa (que é o final 
da área de dados não inicializados - "bss").

.in
.sp
.b OBSERVAÇÕES
.in 5
Repare que em um programa reentrante (compilado ou "link-editado"
com a opção "-n"), o final do texto não coincide com o
começo da área de dados.

.sp
Diversas rotinas de alocação de memória (tais como "brk", "sbrk" (sys),
"malloc", "smalloc" (libc), etc...) modificam o final do programa,
desatualizando o valor do endereço de "end".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cc, ld
.br
.wo "(sys): "
brk
.br
.wo "(libc):"
malloc
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

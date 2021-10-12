.bp
.he 'CVTB (libc)'TROPIX: Manual de Referência'CVTB (libc)'
.fo 'Atualizado em 11.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Tabelas de conversões de caracteres:
.sp
.wo "isotoatb   -"
tabela para conversão de ISO para ASCII 
.br
.wo "isotoabitb -"
tabela para conversão de ISO para ABICOMP
.br
.wo "isotoisotb -"
tabela para conversão de ISO para ISO


.in
.sp
.b SINTAXE
.in 5
.(l
#include <string.h>

extern const char	isotoatb[];
extern const char	isotoabitb[];
extern const char	isotoisotb[];
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas tabelas são usadas para a conversão de
caracteres de um código para outro.
O código padrão do TROPIX é o ISO (ver "iso" (fmt)),
mas em certas aplicações especiais pode tornar-se
necessário o processamento de informações em outros códigos.

.sp
A tabela "isotoatb" converte o código ISO para ASCII.
Como o código ISO contém 256 caracteres e o ASCII apenas 128,
esta conversão não é biunívoca.
As letras acentuadas são mapeadas nas letras correspondentes sem acento;
alguns caracteres que não têm equivalente exato em ASCII são
mapeados em caracteres similares do ASCII quando existe existe
esta possibilidade; em caso contrário são convertidos em '.'.

.sp
A tabela "isotoabitb" converte o código ISO para ABICOMP.
alguns caracteres que não têm equivalente exato em ABICOMP são
mapeados em caracteres similares do ABICOMP quando existe existe
esta possibilidade; em caso contrário são convertidos em '.'.

.sp
A tabela "isotoisotb" converte o código ISO para ISO, isto é,
não realiza conversão alguma. Ela é útil quando não se deseja
relizar nenhuma conversão, mas no entanto, o caracter deve
passar por uma tabela de conversão. Isto pode ocorrer durante
a utilização das funções da série "str" e "mem" (libc) ou
em algum utilitário especial.

.in
.sp
.b OBSERVAÇÃO
.in 5
Todas as tabelas assumem um código de representação de
caracteres contendo 8 bits. Desta forma, todas elas tem
um tamanho de 256 bytes.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
strtcmp, strntcmp, strttcmp, strnttcmp, memtcmp,
.br
.wo "        memttcmp"
.br
.wo "(fmt): "
iso
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

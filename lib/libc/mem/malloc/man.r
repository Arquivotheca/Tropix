.bp 1
.he 'MALLOC (libc)'TROPIX: Manual de Referência'MALLOC (libc)'
.fo 'Atualizado em 06.07.95'Versão 3.0'Pag. %'

.b NOME
.in 5
Alocação dinâmica de memória:
.sp
.wo "malloc   -"
alocação de memória
.br
.wo "free     -"
liberação de memória
.br
.wo "realloc  -"
realocação de memória
.br
.wo "calloc   -"
alocação de área zerada
.br
.wo "chkalloc -"
teste de consistência
.br
.wo "smalloc  -"
alocação rápida
.br
.wo "sfree    -"
liberação da alocação rápida
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdlib.h>

void	*malloc (size_t size);
void	free (void *ptr);
void	*realloc (void *ptr, size_t newsize);
void	*calloc (size_t n_elem, size_t elem_size);
int	chkalloc (void);

#include <malloc.h>

void	*smalloc (size_t size);
void	sfree (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
As funções "malloc" e "free" provêem um método simples de alocação
dinâmica de memória.
Quando memória adicional é necessária, ela é obtida através da
chamada ao sistema "brk" (sys), sendo incorporada à seção
"data" do processo.
Quando esta memória (ou parte dela) é liberada, ela se torna disponível
para futuras alocações através de "malloc" ("realloc", "calloc" ou "smalloc"),
mas não é devolvida ao sistema operacional
(isto é, o processo não diminui de tamanho).

.sp
A função "malloc" retorna um ponteiro para uma área de pelo menos
"size" bytes, já adequadamente alinhada para qualquer uso.

.sp
A função "free" libera a área de endereço "ptr", que foi
previamente alocada por "malloc" ("realloc" ou "calloc").

.sp
A função "realloc" troca o tamanho da área previamente alocada por
"malloc" ("realloc" ou "calloc") apontada por "ptr" para "newsize" bytes
e retorna um ponteiro para a área (possivelmente movida).
O conteúdo da área original permanece inalterado
(até o menor dos tamanhos novo e velho).

.sp
A função "calloc" aloca uma área para um vetor de "n_elem" elementos,
cada um de tamanho "elem_size" bytes.
A área alocada é inicializada com ZEROs.

.sp
A função "chkalloc" analisa a consistência da estrutura de dados
manipulada pelas funções descritas acima.
Ela emite diagnósticos na saída padrão de erros ("stderr")
caso hajam inconsistências.

.sp
A macro "smalloc" (juntamente com a função "sfree") forma um sistema
de alocação de memória sintonizado para a alocação rápida de muitas
áreas pequenas (até dezenas de bytes); ela aloca inicialmente uma região
de tamanho médio através de "malloc", e em seguida fornece rapidamente
pequenas áreas contíguas (dispensando os ponteiros entre as áreas alocadas,
que são utilizados por "malloc").

.sp
A macro "smalloc" retorna um ponteiro para uma área de pelo
menos "size" bytes, alinhada para qualquer uso.
A função "sfree" libera TODAS as áreas alocadas por "smalloc".

.in
.sp
.(t
.b
VALORES DE RETORNO
.r
.in 5
As funções "malloc", "realloc", "calloc" e a macro "smalloc"
retornam um ponteiro NULO quando não há memória disponível suficiente.
.)t

.sp
A função "chkalloc" retorna "0" se não encontrar inconsistências
e "-1" em caso contrário.

.in
.sp
.b OBSERVAÇÕES
.in 5
Alterar as áreas fornecidas além de seus tamanhos fornecidos, ou chamar
as funções "free" ou "realloc" com ponteiros incorretos irá causar
êrros imprevisíveis.

.sp
A chamada
.sp
.nf
			realloc (NOVOID, newsize)
.fi
.sp
é equivalente a
.sp
.nf
			malloc (newsize).
.fi

.sp
Quando "realloc" retorna NULO, a área apontada por "ptr" está liberada,
e pode estar alterada.

.sp
Repare que Áreas alocadas por "smalloc" não podem ser argumentos
para "free" e "realloc".
No entanto, Áreas liberadas por "free" ou por "sfree" tornam-se
disponíveis tanto para "malloc" e "realloc" como para "smalloc".

.sp
Lembre-se de que "smalloc" não é uma função, mas sim uma macro,
portanto evite construções com efeitos colaterais,
tais como "smalloc (size[i++])".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
brk
.br
.wo "(libc):"
alloca
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

.bp
.he 'T_FREE (xti)'TROPIX: Manual de Referência'T_FREE (xti)'
.fo 'Escrito em 17.07.92'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "t_free -"
libera uma estrutura de dados da biblioteca
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <xti.h>

int t_free (const void *ptr, T_ALLOC_ENUM struct_type);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "t_free" libera memória alocada previamente por "t_alloc".
Esta função libera a memória alocada para a estrutura
especificada assim como para as áreas referenciadas por ela.

.sp
.(l
	    +--------------------+------------+------------+
	    |   Argumentos	 | Na chamada | No retorno |
	    +--------------------+------------+------------+
	    | ptr		 |     x      |     /      |
	    | struct_type	 |     x      |     /      |
	    +--------------------+------------+------------+
.)l

.sp
O argumento "ptr" deve apontar para a estrutura alocada por
"t_alloc" e o argumento "struct_type" identifica o tipo da estrutura,
que deve ter um dos seguintes valores:

.(l

		T_BIND_STR		Estrutura "T_BIND"
		T_CALL_STR		Estrutura "T_CALL"
		T_OPTMGMT_STR		Estrutura "T_OPTMGMT"
		T_DIS_STR		Estrutura "T_DISCON"
		T_UNITDATA_STR		Estrutura "T_UNITDATA"
		T_UDERROR_STR		Estrutura "T_UDERR"
		T_INFO_STR		Estrutura "T_INFO"
.)l

.sp
A função "t_free" analisa os membros "addr", "opt" e "udata"
da estrutura dada (conforme apropriado) e libera as áreas apontadas
pelo membro "buf" das estruturas "NETBUF".
Se "buf" contiver um ponteiro NULO, "t_free" não tenta liberar memória
da área.
Após a liberação de todas as áreas, "t_free" irá liberar a memória
associada à estrutura apontada por "ptr". 

.sp
Resultados imprevisíveis irão ocorrer se "ptr" ou algum dos ponteiros
"buf" apontarem para uma região de memória que não foi alocada
por "t_alloc".

.in
.sp
.b 'DETALHES DE IMPLEMENTAÇÃO'
.in 5
A função NÃO ESTÁ IMPLEMENTADA na presente versão.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso a função devolve ZERO.
Em caso contrário, devolve -1 e indica em "t_errno" a causa do erro.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(xti): "
t_alloc
.br

.in
.sp
.(t
.b
REFERÊNCIAS
.r
.in 5
"X/OPEN Portability Guide: Networking Services",
X/OPEN Company Ltd.,
Prentice Hall, New Jersey, 1st Edition, 1988.
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

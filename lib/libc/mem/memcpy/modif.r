.bp
.he 'MEM (libc)'TROPIX: Manual de Modificações'MEM (libc)'
.fo 'Atualizado em 06.07.91'Versão 2.3.0'Pag. %'

.b NOME
.in 5
Operações com áreas de memória: 
.sp
.wo "memcpy   -"
copia uma área de memória
.br
.wo "memmove  -"
cópia otimizada de uma área de memória
.br
.wo "memocpy  -"
cópia otimizada de uma área de memória
.br
.wo "memccpy  -"
copia uma área para outra até encontrar caractere
.br
.wo "memeq    -"
testa a igualdade de duas áreas de memória
.br
.wo "memcmp   -"
compara duas áreas de memória
.br
.wo "memtcmp  -"
compara duas áreas de memória  usando uma tabela
.br
.wo "memttcmp -"
compara duas áreas de memória usando duas tabelas
.br
.wo "memchr   -"
acha a 1a. ocorrência de um caractere
.br
.wo "memset   -"
preenche uma área de memória com um caractere
.br
.wo "memsetw  -"
preenche um vetor de palavras com um valor
.br
.wo "memsetl  -"
preenche um vetor de longos com um valor
.br

.in
.sp 2
.ip "17.10.86  1.0.0" 20
VERSÃO CORRENTE
.sp

.ip "13.04.91  2.3.1" 20
Nas funções "memset", "memcpy", "memcmp", "memeq", "memtcmp", "memttcmp",
"memsetw", "memsetl" e "memmove" foi estendido o "dbra" para funcionar com
32 bites.

.bp 1
.he 'TTYSLOT (libc)'TROPIX: Manual de Referência'TTYSLOT (libc)'
.fo 'Atualizado em 27.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
ttyslot - Procura um nome de terminal no arquivo "/etc/initab"

.in
.sp
.b SINTAXE
.(l
#include <stdlib.h>

int	ttyslot (const char *name)
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "ttyslot" retorna o número da linha do arquivo "/etc/initab",
onde se encontra o terminal "name". 

.sp
A numeração comeca de 0,
e "name" deve ser dado sem o prefixo "/dev/".

.in
.sp
.b DIAGNÓSTICOS
.in 5
Se "/etc/initab" não puder ser lido, "ttyslot" retorna -1, 
e se "name" não for encontrado na tabela "/etc/initab",
retorna -2.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
login, init, who
.br

.in
.sp
.b ARQUIVOS
.in 5
 /etc/initab
 /etc/utmp

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

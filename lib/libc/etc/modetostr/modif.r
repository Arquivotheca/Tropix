.bp
.he 'MODETOSTR (LIBC)'TROPIX: Manual de Modificações'MODETOSTR (LIBC)'
.fo 'Atualizado em 22.6.87'Versão 1.0'Pag. %'

.b NOME
.in 5
.wo "modetostr -"
obtém a forma simbólica de modo um arquivo.
.br

.in
.sp 2
.ip "01.04.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "22.06.87  1.0.1" 20
Foi adaptado para mostrar os 8 novos bites do modo.
Agora são também mostrados os bites "m" (mantém o texto na memória),
e os provisóriamente chamados de 1, 2, 3, 4, 5, 6 e 7, mas ainda
com funcionalidade indefinida. A cadeia de caracteres cujo ponteiro
é retornado agora tem 23 caracteres. Eles são o tipo de arquivo(1),
os atributos(11) e o modo de proteção(9), separados por espaços
em branco.

.in
.sp 2
.ip "01.01.88  1.1" 20
VERSÃO CORRENTE
.sp

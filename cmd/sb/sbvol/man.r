.bp
.he 'SBVOL (cmd)'TROPIX: Manual de Referência'SBVOL (cmd)'
.fo 'Atualizado em 15.4.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "sbvol -"
controla o volume da placa de som SB-16
.br

.in
.sp
.b SINTAXE
.in 5
.(l
sbvol [-v] [-s <vol>]
sbvol+
sbvol-
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "sbvol" é um utilitário simples para controlar o volume mestre
da placa de som "Sound Blaster 16" ou sucessoras
("AWE-32", "AWE-64", ...).

.sp
Nesta versão, são controlados apenas os volumes mestres dos canais esquerdo
e direto simultaneamente.

.sp
Sem opções, "sbvol" informa o volume atual (0 a 100 \%).

.sp
Na forma "sbvol+" aumenta o volume de aproximadamente 5 \%.

.sp
Na forma "sbvol-" diminui o volume de aproximadamente 5 \%.

.sp
As opções do comando são:

.in +3
.ip "-v" 9
Verboso.

.ip "-s <vol>" 9
O volume passa a ser <vol> \%.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sbwave
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

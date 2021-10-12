.bp
.he 'UNTEXT (cmd)'TROPIX: Manual de Referência'UNTEXT (cmd)'
.fo 'Atualizado em 14.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "untext -"
libera os recursos de programas reentrantes
.br

.in
.sp
.b SINTAXE
.in 5
.(l
untext <texto> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Um programa reentrante ("texto") ocupa uma série de recursos do sistema
operacional, e em particular,
regiões da memória interna e da área de "swap" em disco.

.sp
Se um programa reentrante tiver o indicador "m" ligado (ver "chmod" (cmd)),
a memória interna continua ocupada, mesmo quando o programa
não tiver mais nenhum usuário.

.sp
É aconselhavel liberar os recursos ocupados por programas reentrantes,
quando se sabe que eles não serão utilizados durante algum tempo.

.sp
Para liberar os recursos de um programa <texto>, "untext" realiza
uma busca nos vários diretórios da variável do ambiente "PATH"
(ver "environ" (fmt)).
A busca termina quando é encontrado um arquivo com o nome dado, ou
a lista de diretórios termina.

.in
.sp
.b OBSERVAÇÕES
.in 5
Se o nome do arquivo dado contiver um "/",
este nome é considerado o nome definitivo, e a busca não é realizada.

.sp
Se o ambinte não contiver a variável "PATH", será utilizado 
o valor ".:/bin:/usr/bin".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cc, ld, chmod (opções "-m", "-t"), textmap
.br
.wo "(sys): "
untext
.br
.wo "(fmt): "
environ
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

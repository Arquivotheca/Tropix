.bp
.he 'LINKOPTIM (cmd)'TROPIX: Manual de Referência'LINKOPTIM (cmd)'
.fo 'Atualizado em 19.06.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "linkoptim -"
substitui arquivos duplicados por "link"s
.br

.in
.sp
.b SINTAXE
.in 5
.(l
linkoptim [-fvN] <árvore> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "linkoptim" caminha as <árvore>s dadas,
à procura de pares de arquivos regulares
iguais com o objetivo de substituir um dos arquivos por um "link" ao outro.
Isto pode economizar um espaço apreciável no sistema de arquivos.

.sp
As opções do comando são:

.in +3
.ip -f
Força: Normalmente, o usuário é consultado para confirmar que
realmente deseja substituir o arquivo por um "link" ao outro.
Com esta opção, a substituição é realizada sem consulta.

.ip -v
Verboso.

.ip -N
Lê os nomes das <árvores>s da entrada padrão.
Esta opção é implícita se não forem dados argumentos

.ep
.in -3

.in
.sp
.b OBSERVAÇÕES
.in 5
A substituição de um arquivo por um "link" a outro
(mesmo de conteúdo igual) poderá alterar as propriedades
do arquivo (tais como o dono, grupo, proteção, datas, ...),
o que pode ter conseqüências indesejáveis
(como por exemplo, atrapalhar o funcionamento do comando "make").

.sp
O comando NÃO considera arquivos vazios (de tamanho zero).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
make, cp, ln
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

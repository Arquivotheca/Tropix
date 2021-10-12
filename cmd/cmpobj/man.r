.bp
.he 'CMPOBJ (cmd)'TROPIX: Manual de Referência'CMPOBJ (cmd)'
.fo 'Atualizado em 23.08.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "cmpobj -"
compara pares de módulos objeto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
cmpobj [-q] <módulo1> <módulo2>
cmpobj [-q] <módulo1> [<módulo2> ...] <diretório>
cmpobj [-q] [-N] <diretório>
.)l


.lp
.in
.b DESCRIÇÃO
.in 5
O programa "cmpobj" é uma complementação de "cmp" (cmd), para módulos objeto.
Ele compara independentemente as diversas seções dos módulos objeto, 
indicando as diferenças (se houver).

.sp
Em sua primeira forma, o comando "cmpobj" compara <módulo1> com
<módulo2>.

.sp
Na segunda forma, compara um ou mais <módulo>s com
os respectivos módulos homônimos do <diretório> especificado.

.sp
A terceira forma é análoga à segunda, com exceção dos nomes
dos <módulo>s, que são lidos da entrada padrão, um por linha.

.sp
O programa "cmpobj" obedece às convenções da função "modecolor",
consultando a variável MODECOLOR do ambiente para obter
as cores dos nomes dos arquivos e das mensagens
de erro (veja "modecolor" (libc)).

.sp
As opções do comando são:

.in +3
.ip -q
Comparação rápida: Se os tamanhos totais dos <módulo>s e
as datas contidas nos cabeçalhos forem iguais, os <módulo>s
são considerados iguais.
Esta comparação é muito mais rápida, e a probabilidade de indicar
uma igualdade falsa é extremamente baixa.

.ip -N
Lê os nomes dos <módulo>s da entrada padrão 
(usado para a terceira forma).
Esta opção é implícita se for dado apenas o nome do <diretório>.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
modecolor
.br
.wo "(cmd): "
cmp
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in

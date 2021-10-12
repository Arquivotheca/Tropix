.bp
.he 'ECHO (cmd)'TROPIX: Manual de Referência'ECHO (cmd)'
.fo 'Atualizado em 20.09.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
.wo "echo -"
ecoa os argumentos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
echo [-nl1234567] [<argumento> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "echo" escreve os seus <argumento>s na saída padrão.

.sp
Sem opções, é escrita apenas uma linha, com os <argumento>s
separados por um <sp> e a linha terminada por um <nl>.

.sp
Se for dada a opção "-n", não é escrito o <nl> no final da linha.

.sp
Se for dada a opção "-l", os <argumento>s são escritos na forma de um por linha.

.sp
Se for dada uma das opções "-1", "-2", "-3", "-4", "-5", "-6" ou "-7"
escreve a linha com a cor de número dado (veja a tabela de cores em "modecolor" (libc)).

.sp
O comando "echo" é útil para produzir diagnósticos em arquivos de comandos
para o "sh" (cmd), e também para enviar dados conhecidos para um "pipe".

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd):  "
sh
.br
.wo "(libc): "
modecolor
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

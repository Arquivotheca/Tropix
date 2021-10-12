.bp 1
.he 'TTY (cmd)'TROPIX: Manual de Referência'TTY (cmd)'
.fo 'Atualizado em 13.07.97'Versão 1.0'Pag. %'

.b NOME
.in 5
tty - imprime o nome do terminal/vídeo

.in
.sp
.b SINTAXE
.in 5
tty [-s]

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "tty" escreve o caminho ("pathname") do terminal/vídeo
no qual o usuário se encontra.

.sp
Com a opção "-s", o comando "tty" apenas
fornece o código de retorno 1 ou 0,
se a entrada padrão respectivamente é ou não um terminal/vídeo.

.in
.sp
.b OBSERVAÇÃO
.in 5
Se a entrada padrão não for um terminal/vídeo e a opção "-s" não
for dada, "tty" imprime "Terminal/vídeo desconhecido".

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in

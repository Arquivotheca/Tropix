.bp
.he 'SLEEP (cmd)'TROPIX: Manual de Referência'SLEEP (cmd)'
.fo 'Atualizado em 05.07.97'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "sleep -"
suspende a execução por um intervalo de tempo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
sleep <n>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "sleep" suspende a execução por <n> segundos.

.sp
É utilizado normalmente para executar um comando após um determinado tempo.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
alarm
.br
.wo "(libc):"
sleep
.br

.in
.sp
.b EXEMPLO
.in 5
A linha de comandos
.sp
.nf
		(sleep 90; <comando> )  &
.fi
.sp
Irá executar o <comando> após 90 segundos, mas permitindo executar
outros comandos antes deste intervalo de tempo.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

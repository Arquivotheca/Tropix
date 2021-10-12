.bp
.he 'CDPLAY (cmd)'TROPIX: Manual de Referência'CDPLAY (cmd)'
.fo 'Atualizado em 19.3.00'Versão 3.2.3'Pag. %'

.b NOME
.in 5
.wo "cdplay -"
reproduz faixas de CDs
.br

.in
.sp
.b SINTAXE
.in 5
.(l
cdplay [-prt] [-s <inicial>] [-f <final>] [<dev>]
cdplay+
cdplay-
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "cdplay" permite reproduzir faixas de áudio em CDs.

.sp
As opções do comando são:

.in +3
.ip -p
Suspende a reprodução. Se esta opção for dada duas vezes ("cdplay -pp"),
a reprodução é definitivamente interrompida, sem possibilidade de continuá-la.

.ip -r
Reinicia a reprodução, do ponto onde foi suspensa.

.ip -t
Mostra o conteúdo do CD (Table of Contents).

.ip -s
Começa a reprodução na faixa <inicial>.

.ip -f
Encerra a reprodução na faixa <final>.

.ep
.in -3

.sp
Se invocado sem argumentos, o programa "cdplay" funciona da seguinte
maneira: havendo reprodução em andamento, todo o CD é reproduzido;
caso contrário, o programa informa qual faixa está sendo reproduzida.
.sp
"cdplay+" avança a reprodução, se possível, para a faixa seguinte.
Caso não haja reprodução em andamento, é equivalente a "cdplay".
.sp
"cdplay-" reinicia a reprodução, se possível, na faixa anterior.
Caso não haja reprodução em andamento, é equivalente a "cdplay".
.sp
Se <dev> for omitido, é assumido "/dev/cdrom".

.in
.sp
.b OBSERVAÇÃO
.in 5

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
dosmp
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

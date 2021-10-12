.bp
.he 'SHOW (cmd)'TROPIX: Manual de Modificações'SHOW (cmd)'
.fo 'Atualizado em 28.12.04'Versão 4.7.0'Pag. %'

.b NOME
.in 5
.wo "show -"
mostra um arquivo na tela de um terminal
.br

.in
.sp 2
.ip "01.04.87  1.0" 20
VERSÃO CORRENTE
.sp

.ip "15.04.87  1.0.1" 20
O problema com os comandos <^U> e '-', que quando usados
na última pagina do texto (quando esta não ocupava a tela inteira)
geravam a perda da sequência das linhas, foi corrigido.

.in
.sp 2
.ip "01.01.88  1.1" 20
VERSÃO CORRENTE
.sp

.ip "16.05.88  1.1.1" 20
Os comandos <^F> e <^B> foram modificados visando
aceitar o avanço ou retrocesso de 'n' páginas no texto.
Isto é obtido através da especificação do número de páginas antes do comando.
Para os outros comandos, estes números serâo ignorados.
O comando '1' (volta para a primeira página do texto), já existente,
foi transformado no comando 'g', só que precedido pelo caracter '1'.

.sp
.ip "08.06.88  1.1.2" 20
Os problemas existentes com o uso de linhas grandes e
com o comando <^D> foram solucionados.

.ip "05.09.89  2.3.0" 20
O comando foi alterado para utilizar a "terminfo".

.ip "24.01.91  2.3.1" 20
O programa sofreu (ou deliciou-se com) uma revisão global.

.ip "22.06.91  2.3.2" 20
Forma acrescentados os comandos de página.

.ip "12.09.92  3.0.0" 20
Agora pode-se escolher o arquivo desejado de vários nome dados.

.ip "20.08.96  3.0.1" 20
Cancelando o programa através de "kill" ou "ps -k" o modo do video
agora é restaurado.

.sp
Usando "show" sem pipe nem argumentos, não prende mais o video.

.ip "18.05.97  3.0.2" 20
Generalizado para aceitar arquivos estilo DOS (<cr><nl>).

.ip "20.01.98  3.0.3" 20
Colocada uma mensagem para o caso de comandos "nN" antes de "/".
Acrescentados os comandos de setas ...

.ip "14.08.98  3.1.0" 20
Acrescentado o comando interno "?".

.ip "04.12.02  4.4.0" 20
Unificados os programas "show", "man" e "modif".

.ip "28.12.04  4.7.0" 20
Alterado o tamanho da página para 75 linhas.

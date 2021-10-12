.bp
.he 'XTERM (xwin)'TROPIX: Manual de Modificações'XTERM (xwin)'
.fo 'Atualizado em 17.07.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "xterm -"
Emulador de Terminal ANSI para o X-Window
.br

.in
.sp 2
.ip "05.02.99  3.1.6" 20
VERSÃO CORRENTE
.sp

.in
.ip "01.04.99  3.1.7" 20
Revistas/eliminadas algumas opções na linha de comando.

.ip "27.05.99  3.2.0" 20
Introduzida a alteração do tamanho da janela, através da
seqüência ESC]46;<ncol>x<nlin>BEL, enviada pelo "xtset".

.ip "14.10.01  4.2.0" 20
Revisto para a versão 4.1.0 do X-Window.

.ip "06.05.02  4.2.0" 20
Agora é possível alterar os nomes das cores básicas.

.ip "11.11.02  4.4.0" 20
Removido o tratamento especial para a seta ascendente.
Alterado o módulo "src/command.c".

.ip "19.03.03  4.4.0" 20
O programa foi revisto para a versão 4.3.0 do X-Window.

.ip "18.06.03  4.5.0" 20
Introduzido o tratamento das teclas numéricas no teclado auxiliar.
Alterado o módulo "src/command.c".

.ip "18.08.03  4.5.0" 20
Introduzida a opção "-h". Modificados: "h/main.h", "src/xterm.c" e
"src/xdefaults.c".

.ip "11.12.03  4.5.0" 20
Introduzido o rolamento através da "rodinha" do camundongo.
Modificado: "src/command.c".

.ip "17.07.04  4.6.0" 20
Agora usando "getpty" e "initent".

.ip "18.03.05  4.8.0" 20
Pequena revisão.

.bp
.he 'GETCH (curses)'TROPIX: Manual de Modificações'GETCH (curses)'
.fo 'Atualizado em x.2.87'Versão 1.0'Pag. %'

.b NOME
.in 5
Entrada de um caractere do teclado
.sp
.wo "wgetch   -"
na posição corrente na janela dada
.br
.wo "getch    -"
na posição corrente na janela padrão
.br
.wo "mvwgetch -"
dadas a janela e a posiçao
.br
.wo "mvgetch  -"
dada a posição na janela padrão

.in
.sp 2
.ip "01.04.89  2.1" 20
VERSÃO CORRENTE
.sp

.ip "29.06.89  2.2.1" 20
Corrigido o não funcionamento da opção "R_INPUT".

.ip "08.05.90  2.3.0" 20
Corrigido erro no funcionamento da opção "G_TOISO".
No caso do último caractere teclado ser um acento,
a sua remoção não cancelava a formação de um caractere acentuado.

.ip "04.06.90  2.3.1" 20
Funcionamento completo do reconhecimento de teclas especiais.

.ip "18.04.91  2.3.2" 20
Os caracteres XON e XOFF são ignorados
no caso dos terminais que usam o protocolo XON/XOFF.

.ip "24.06.91  2.3.3" 20
Consertado erro no salvamento de contexto
ativado pelo caractere "switch"
através da inclusão de um parâmetro adicional para a rotina "doswitch"
indicando se a fila de caracteres lidos está em uso.

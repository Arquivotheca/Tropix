.bp
.he 'EX (cmd)'TROPIX: Manual de Modificações'EX (cmd)'
.fo 'Atualizado em 05.02.98'Versão 3.0.4'Pag. %'

.b NOME
.in 5
ex - editor visual de textos
.br

.in
.sp 2
.ip "17.08.88  2.0" 20
VERSÃO CORRENTE

.ip "23.11.88  2.1.1" 20
Foram acrescentadas entre parênteses as frases "só leitura" e "não atualizado"
quando o arquivo em edição só tem permissão para leitura
e não está atualizado em disco.

.ip "24.11.88  2.1.2" 20
Foi acrescentada a capacidade de indentar várias linhas em um só comando.

.ip "28.11.88  2.1.3" 20
Consertados os erros
na indentação de linhas contendo apenas o caractere <ht>
e na repetição do comando "o" (open).

.ip "29.11.88  2.1.4" 20
Foi alterada a posição corrente seguinte ao rolamento da janela.

.ip "01.12.88  2.1.5" 20
Consertado o erro que impedia a utilização de marcação
na última linha do texto.

.ip "26.12.88  2.1.6" 20
Alterada a posição corrente seguinte à indentação de um grupo de linhas
e ao comando "X".

.ip "23.01.89  2.1.7" 20
Religação para utilizar as novas potencialidades
da rotina "wrefresh" da "P-Curses"
(uso de inserção e remoção de linhas realizadas pelo terminal).
.sp
O caractere de "switch" passa a deslocar a página de 40 colunas,
possibilitando ao usuário visualizar até 120 colunas.
.sp
Caracteres de XON e XOFF não mais aparecem no vídeo.
.sp
O comando "s" do modo de comandos globais só lista as linhas substituídas
quando seguido do sufixo "p".
.sp
A listagem de linhas na janela pequena do modo de comandos globais
pode ser interrompida com o caractere <esc>.

.ip "17.05.89  2.1" 20
VERSÃO CORRENTE

.ip "01.09.89  2.2.1" 20
O editor passou a dispor de uma janela de 120 colunas.
Toda vez em que um caractere
ocupando uma posição fora das 80 colunas mostradas
for apontado pelo cursor
a janela é automaticamente deslocada
para a esquerda ou para a direita de 40 colunas
de modo a tornar este caractere visível.
Também é possível deslocar a janela
teclando-se o caractere de "switch",
se o caractere apontado é visível
em ambas as posições da janela.

.sp
O modo comandos tornou-se mais rápido
com a utilização do modo de escrita "A_REAL"
da "P-Curses".

.sp
Consertado o erro na substituição de caractere
(comando "r" do modo visual)
quando aplicado em uma linha vazia.

.ip "02.10.89  2.2.2" 20
A busca de uma expressão regular retrocedendo no texto
(por exemplo: a partir do comando "?")
não funcionava quando iniciava em uma linha vazia.
Este erro foi corrigido.

.ip "28.12.89  2.3.1" 20
Consertados erros nos comandos para substituição de texto
no modo Visual.

.ip "04.01.90  2.3.2" 20
Foi instalada a capacidade de verificação a cada comando
da memória alocada via "malloc"
através da opção 'm'.

.ip "22.04.91  2.3.3" 20
Consertado um problema de iniciação,
que fazia com que na primeira vez em que um movimento
deslocasse a posição corrente para uma coluna
fora da tela, a janela e, conseqüentemente, o cursor
permanecessem (erronamente) imóveis.

.sp
Eliminado o erro que impedia a cópia do conteúdo de um buffer
(contendo apenas parte de uma linha)
para uma linha vazia.

.ip "28.10.91  2.3.4" 20
Informa os nomes dos arquivos, se forem dados mais de um.

.ip "20.11.91  2.3.5" 20
Se for dado mais de um arquivo, podemos agora escolher o desejado.

.sp
Foi aumentado para 4 Kb o "buffer" da STDIO na entrada.

.ip "24.03.95  3.0.0" 20
O programa foi compilado no PC.

.ip "30.04.97  3.0.1" 20
O programa sofreu (ou deliciou-se com) uma revisão global.

.ip "18.05.97  3.0.2" 20
Estendido para tratar arquivos no estilo DOS (<cr><nl>).

.ip "24.08.97  3.0.3" 20
Estendido para usar o tamanho real da janela.

.ip "05.02.98  3.0.4" 20
Estendido para usar as teclas das setas, page up, page down ...

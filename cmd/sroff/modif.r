.bp
.he 'SROFF (cmd)'TROPIX: Manual de Modificações'SROFF (cmd)'
.fo 'Atualizado em 28.12.04'Versão 4.7.0'Pag. %'

.b NOME
.in 5
.wo "sroff -"
formatador de textos
.br

.in
.sp 2

.ip "11.05.86  1.0.0" 20
VERSÃO CORRENTE
.sp

.ip "02.02.87  1.1.0" 20
???

.ip "17.08.88  2.0.0" 20
VERSÃO CORRENTE
.sp

.ip "08.11.88  2.0.1" 20
Foram acrescentados os comandos ".bc" e ".ec" que
delimitam um bloco de comentários.
Assim, linhas entre um ".bc" e um ".ec" serão ignoradas.

.ip "14.05.89  2.2.0" 20
Foi feita pequena alteração nos comandos ".bc" e ".ec"
para não examinar os restos das linhas após os comandos.
Alterado "/src/procarq".

.ip "04.08.89  2.2.1" 20
Foi alterado para aceitar "headers" pares e ímpares
e "footers" pares e ímpares.

.ip "29.08.89  2.2.1" 20
Foi alterado para que, se houver mudança de página
no meio de uma tabela que seja "allbox",
sejam usadas duas linhas divisórias,
uma após a última linha da tabela na página que está no fim,
e outra antes da primeira linha da tabela na página que continua.
Além disso, o número de linhas por página passou
a ser no mínimo 10.

.ip "04.09.89  2.2.1" 20
Foi acrescentado o comando "title" às opções das tabelas.
O argumento deste comando indica a linha de dados
que deve ser usada como título no início de cada
página em que a tabela continua.

.ip "05.10.89  2.2.1" 20
Foi colocada uma mensagem de erro para o caso
em que o comando "title" é usado em uma tabela
que se encontra dentro de um bloco.
Neste caso o comando "title" não funciona.

.ip "25.10.89  2.2.1" 20
Foi acrescentada a opção "-l" que possibilita
a impressão de textos formatados pelo sroff
na impressora laser.

.ip "08.11.89  2.3.1" 20
Foi alterado para imprimir caracteres sublinhados
na impressora laser.

.ip "21.11.89  2.3.1" 20
Foi alterado para não trocar de página quando
o header ou o footer forem trocados.
A troca de página só ocorre no momento da criação
do primeiro header ou do primeiro footer. 

.ip "23.11.89  2.3.1" 20
Foi alterado para imprimir 6 ou 8 linhas por polegadas
na impressora laser. Para isso foi criado o comando ".ln".

.ip "01.12.89  2.3.1" 20
Foi definido que os parâmetros para
a laser serão enviados antes da primeira margem.
Sendo assim, após o inicio da entrada do texto,
não é conveniente a alteração do tamanho da página (comando ".pl")
e do número de linhas por página (comando ".ln").

.ip "31.01.90  2.3.2" 20
Foi criado o comando ".it" que é semelhante ao ".ip",
só que não pula linha.

.ip "08.02.90  2.3.2" 20
Foram feitas novas alterações para a utilização
da impressora laser.

.ip "05.03.90  2.3.2" 20
Foi consertado o problema que havia na utilização
do comando ".sp" dentro de um bloco.

.ip "11.04.90  2.3.2" 20
O comando ".ts" foi alterado, passando a gerar uma quebra da linha corrente. 
Além disso, o formato "s" na primeira coluna de uma linha
de formatos e a não finalização de um comando de bloco
passaram a ser detectados.

.ip "16.04.90  2.3.2" 20
Foi consertado um erro que havia na colocação de tabelas
dentro de blocos.

.ip "07.07.97  3.0.0" 20
Adaptado para o PC

.ip "28.12.04  4.6.7" 20
Alterado o tamanho da pagina para 75 linhas.

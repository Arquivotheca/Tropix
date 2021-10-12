.bp
.he 'MODIF (cmd)'TROPIX: Manual de Referência'MODIF (cmd)'
.fo 'Atualizado em 04.12.02'Versão 4.4.0'Pag. %'

.b NOME
.in 5
.wo "modif -"
mostra um manual de modificações na janela do vídeo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
modif [-cslwnxD] [<manual de modificações>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "modif" permite ler/reler de forma conveniente
um <manual de modificações> na janela do vídeo.

.sp
O <manual de modificações> pedido pode ser o de referência de um comando,
de uma chamada ao sistema, de uma função da biblioteca padrão
ou de uma função da biblioteca "curses".
Através da opção "-D" (ver abaixo) podemos obter um índice dos
manuais de modificações disponíveis em cada seção.

.sp
A busca do <manual de modificações> é realizada na ordem indicada acima;
como em alguns casos seções distintas possuem manuais homônimos,
podemos indicar qual a seção desejada através de uma opção. 

.sp
O comando permite que a página visível do <manual de modificações> na janela
avance ou retroceda, e podemos ler páginas posteriores ou reler páginas
anteriores do <manual de modificações>.

.sp
Os comandos internos que permitem avançar/retroceder as páginas do
<arquivo> são dados abaixo. Eles são semelhantes aos comandos internos
do "vi" (cmd), e o o usuário habituado com o "vi" não terá dificuldades
em utilizar o comando "modif".

.sp 2
As opções do comando são:

.in +3
.ip -c
Seção "cmd": o <manual de modificações> desejado é o de referência
de um comando.

.ip -s
Seção "sys": o <manual de modificações> desejado é o de uma chamada ao sistema.

.ip -l
Seção "libc": o <manual de modificações> desejado é o de uma função
da biblioteca padrão da linguagem "C".

.ip -w
Seção "curses": o <manual de modificações> desejado é o de uma função
da biblioteca "curses".

.ip -n
Seção "network": o <manual de modificações> desejado é o de uma função
da biblioteca de redes (padrão XTI).

.ip -x
Seção "X-Window": o <manual de modificações> desejado é o de uma função
ou de um comando do sistema "X-Window".

.ip -D
Ao invés de um manual de modificações, mostra na janela o índice dos manuais
de modificações disponíveis  em uma das seções (que deve ser escolhida
através de uma das opções acima ("-c" por omissão)).

.ep
.in -3

.sp 2
.(t
Os comandos internos para avançar/retroceder a página visível do
<manual de modificações> na janela são os seguintes:

.in +3

.ip '[<n>] +' 13
.it '[<n>] <nl>' 13
.it '[<n>] <cr>' 13
Avança <n> linhas. Se <n> for omitido, avança uma linha.
.)t

.ip '[<n>] -' 13
Retrocede <n> linhas. Se <n> for omitido, retrocede uma linha.

.ip <^D> 13
.it '<Page Down>' 13
Avança meia janela (11 linhas).

.ip <^U> 13
.it '<Page Up>' 13
Retrocede meia janela.

.ip <Home> 13
Retrocede para o início do diretório/carta.

.ip <End> 13
Avança para o final do diretório/carta.

.ip "[<n>] <^F>" 13
Avança <n> janelas (de 21 linhas cada). Se <n> for omitido, avança uma janela.

.ip "[<n>] <^B>" 13
Retrocede <n> janelas. Se <n> for omitido, retrocede uma janela.

.ip "[<n>] <^V>" 13
Avança <n> páginas (de 75 linhas cada). Se <n> for omitido, avança uma página.

.ip "[<n>] <^Z>" 13
Retrocede <n> páginas. Se <n> for omitido, retrocede uma página.

.ip <^R> 13
Reescreve a janela.

.ip 'f' 13
.it 'F' 13
.it <^G> 13
Escreve o nome do <manual de modificações> na última linha da janela.

.ip '<sp>' 13
Tira a mensagem da última linha da janela.

.ip "[<n>] g" 13
.it "[<n>] G" 13
Avança/retrocede para a janela que começa na linha <n>.
Se <n> for omitido, avança para a última janela do <manual de modificações>.

.ip "[<n>] p" 13
.it "[<n>] P" 13
Avança/retrocede para a página <n>.
Se <n> for omitido, avança para a última página do <manual de modificações>.

.ip '/ <cadeia>' 13
Busca de cadeia: avança/retrocede para a primeira janela do <manual de
modificações> cuja primeira linha contém a <cadeia> dada.

.ip 'n' 13
Repete a busca de cadeia avançando: mostra a janela seguinte do
<manual de modificações> cuja primeira linha contém a <cadeia> dada.

.ip 'N' 13
Repete a busca de cadeia retrocedendo: mostra a janela anterior do
<manual de modificações> cuja primeira linha contém a <cadeia> dada.

.ip 'w <nome>' 13
.it 's <nome>' 13
Cria o arquivo <nome> com o conteúdo do <manual> sendo lido,
desde que o arquivo <nome> ainda não exista.

.ip 'W <nome>' 13
.it 'S <nome>' 13
Escreve/reescreve o conteúdo do <manual> sendo examinado no arquivo <nome>,
mesmo que o arquivo <nome> já exista.

.ip '! <comando>' 13
Executa o <comando> através do "sh" (cmd).

.ip <^C> 13
.it 'q' 13
.it 'Q' 13
Encerra a execução de "man".

.ep
.in -3

.in
.sp 2
.b OBSERVAÇÃO
.in 5
Repare que os comandos internos "/", "n" e "N" procuram apenas
a cadeia como originalmente dada, não realizando uma busca de expressões
regulares, como o comando "vi" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
vi, ex, sh, show
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

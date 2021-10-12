.bp
.he 'SHOW (cmd)'TROPIX: Manual de Referência'SHOW (cmd)'
.fo 'Atualizado em 04.12.02'Versão 4.4.0'Pag. %'

.b NOME
.in 5
.wo "show -"
mostra um arquivo em uma janela do video
.br

.in
.sp
.b SINTAXE
.in 5
.(l
show [-d] [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "show" permite examinar de forma conveniente
um <arquivo> em uma janela do video. 
Se for dado mais de um nome de <arquivo>, "show" pede ao usuário
para escolher o nome desejado.
Se o nome do <arquivo> for omitido, será lida a entrada padrão ("pipe").

.sp
O comando permite que o trecho visível do <arquivo> na janela avance ou
retroceda, podendo-se examinar páginas posteriores ou reexaminar páginas
anteriores do <arquivo>.
Isto é especialmente útil para arquivos lidos da entrada padrão.

.sp
Os comandos internos que permitem avançar/retroceder as páginas do
<arquivo> são dados abaixo. Eles são semelhantes aos comandos internos
do "vi" (cmd), e o o usuário habituado com o "vi" não terá dificuldades
em utilizar o comando "show".

.sp
Normalmente, se o <arquivo> tiver linhas longas (maiores do que a janela)
elas serão truncadas. No entanto, se o tamanho da janela for aumentada,
as linhas serão reescritas completas (se o novo tamanho permitir).

.sp 2
As opções do comando "show" são:

.in +3
.ip -d
Dobra:  as linhas longas são dobradas ao invés de truncadas.
Isto é uma operação definitiva; mesmo que o tamanho da janela seja aumentada, 
as linhas continuarão dobradas. 

.ep
.in -3

.sp 2
Os comandos internos para avançar/retroceder a página visível do
<arquivo> na janela são os seguintes:

.in +3

.ip '[<n>] +' 13
.it '[<n>] <nl>' 13
.it '[<n>] <cr>' 13
Avança <n> linhas. Se <n> for omitido, avança uma linha.

.ip '[<n>] -' 13
Retrocede <n> linhas. Se <n> for omitido, retrocede uma linha.

.ip <^D> 13
.it '<Page Down>' 13
Avança meia janela.

.ip <^U> 13
.it '<Page Up>' 13
Retrocede meia janela.

.ip <Home> 13
Retrocede para o início do diretório/carta.

.ip <End> 13
Avança para o final do diretório/carta.

.ip "[<n>] <^F>" 13
Avança <n> janelas. Se <n> for omitido, avança uma janela.

.(t
.ip "[<n>] <^B>" 13
Retrocede <n> janelas. Se <n> for omitido, retrocede uma janela.
.)t

.ip "[<n>] <^V>" 13
Avança <n> páginas (de 75 linhas cada). Se <n> for omitido, avança uma página.

.ip "[<n>] <^Z>" 13
Retrocede <n> páginas. Se <n> for omitido, retrocede uma página.

.ip <^R> 13
Reescreve a janela.

.ip 'f' 13
.it 'F' 13
.it <^G> 13
Escreve o nome do <arquivo> na última linha da janela.

.ip '<sp>' 13
Tira a mensagem da última linha da janela.

.ip "[<n>] g" 13
.it "[<n>] G" 13
Avança/retrocede para a janela que começa na linha <n>.
Se <n> for omitido, avança para a última janela do <arquivo>.

.ip "[<n>] p" 13
.it "[<n>] P" 13
Avança/retrocede para a página <n>.
Se <n> for omitido, avança para a última página do <arquivo>.

.ip '/ <cadeia>' 13
Busca de cadeia: avança/retrocede para a primeira janela do <arquivo>
cuja primeira linha contém a <cadeia> dada.

.ip 'n' 13
Repete a busca de cadeia avançando: mostra a janela seguinte do <arquivo>
cuja primeira linha contém a <cadeia> dada.

.ip 'N' 13
Repete a busca de cadeia retrocedendo: mostra a janela anterior do <arquivo>
cuja primeira linha contém a <cadeia> dada.

.ip 'w <nome>' 13
.it 's <nome>' 13
Cria o arquivo <nome> com o conteúdo do <arquivo> sendo examinado,
desde que o arquivo <nome> ainda não exista.
Isto é especialmente útil para arquivos lidos da entrada padrão ("pipe").

.ip 'W <nome>' 13
.it 'S <nome>' 13
Escreve/reescreve o conteúdo do <arquivo> sendo examinado no arquivo <nome>,
mesmo que o arquivo <nome> já exista.

.ip '! <comando>' 13
Executa o <comando> através do "sh" (cmd).

.ip '?' 13
Mostra o manual do comando "show", para poder consultar esta lista
de comandos internos.

.ip <^C> 13
.it 'q' 13
.it 'Q' 13
Encerra a execução de "show".

.ep
.in -3

.in
.sp 2
.(t
.b OBSERVAÇÕES
.in 5
É feita uma análise simples dos caracteres do <arquivo>, e os caracteres
de controle (exceto <bs>, <cr> e <ht>) são convertidos nas
seqüências padronizadas "^X".
.)t

.bc	/*************************************/
.sp
Se o <arquivo> tiver linhas longas, e elas forem dobradas (isto é, não
for dada a opção "-t"), o comando "show" perde a referencia aos números
originais das linhas no <arquivo> dado.
Neste cado, os comandos "g" e "G" funcionam apenas com <n> igual a 1
ou com <n> omitido.
.ec	/*************************************/

.sp
Se o <arquivo> tiver linhas dobradas e/ou caracteres de controle,
o arquivo <nome> criado/reescrito
pelos comandos "w", "W", "s" e "S" não será idêntico ao <arquivo> original.

.sp
Repare que os comandos internos "/", "n" e "N" procuram apenas
a cadeia como originalmente dada, não realizando uma busca de expressões
regulares, como o comando "vi" (cmd).

.sp
O comando "show" identifica se o arquivo em edição é do estilo UNIX
(linhas terminadas por <nl>) ou estilo DOS (linhas terminadas por <cr><nl>),
e processa corretamento os dois estilos.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
vi, ex, sh
.br

.in
.sp
.b EXEMPLO
.in 5
.nf
		grep INC *.c | show
.fi
.sp
mostra o arquivo de saída do comando "grep".

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

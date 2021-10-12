.bp
.he 'NM (cmd)'TROPIX: Manual de Referência'NM (cmd)'
.fo 'Atualizado em 08.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "nm -"
imprime a tabela de símbolos de módulos objeto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
nm [-gutdbcarfonepN] [<módulo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "nm" imprime a tabela de símbolos de cada <módulo> objeto dado.
Se não for dado nenhum <módulo>, nem a opção "-N" (ver abaixo), é impressa
a tabela de símbolos do módulo "a.out".

.sp
Cada <módulo> objeto pode ser relocável ou absoluto, e é normalmente obtido
através do comando "as" ou "ld" (cmd).

.bc	/*************************************/
.sp
Cada símbolo vem precedido do seu valor (ou de brancos se for
indefinido), e de uma letra indicando
seu tipo, que pode ser: "U" (indefinido),
"A" (absoluto), "R" (registro do processador),
"C" ("common"), "T" ("text"),
"D" ("data", dados inicializados),
"B" ("bss", dados não inicializados) ou "F" (nome do módulo).
.ec	/*************************************/

.sp
Cada símbolo vem precedido do seu valor em hexadecimal (ou de brancos se for
indefinido), e de uma letra indicando um de seus 8 tipos possíveis:

.in +8
.ip 'U'
indefinido;

.ip 'T'
seção TEXT;

.ip 'D'
seção DATA: dados inicializados;

.ip 'B'
seção BSS: dados não inicializados;

.ip 'C'
seção COMMON;

.ip 'A'
absoluto;

.ip 'R'
registro do processador;

.ip 'F'
nome de módulo.

.ep
.in -8

.sp
Se a símbolo for local, o tipo será dado pela respectiva letra minúscula.
Para cada um destes tipos de símbolos, existe uma opção
correspondente (ver abaixo). Assim, temos a opção "-t" para a seção
TEXT (tipos 't' ou 'T'), opção "-d" para a seção DATA, e assim por diante.

.sp
Normalmente, "nm" imprime todos os símbolos, e a
saída é ordenada em ordem alfabética crescente pelo nome do símbolo.

.sp
Se não for dada nenhuma das 8 opções referentes aos tipos ("-utdbcarf"),
são impressos todos os símbolos. Se for dada uma (ou mais) destas opções
são impressos apenas os símbolos dos tipos correspondentes.

.sp
O formato da tabela de símbolo pode ser controlado pelas seguintes opções:

.in +3
.ip -g 6
Imprime somente os símbolos globais.

.ip -u 6
Imprime os símbolos indefinidos.

.ip -t 6
Imprime os símbolos da seção TEXT.

.ip -d 6
Imprime os símbolos da seção DATA.

.ip -b 6
Imprime os símbolos da seção BSS.

.ip -c 6 
Imprime os símbolos da seção COMMON.

.ip -a 6 
Imprime os símbolos absolutos.

.ip -r 6 
Imprime os símbolos de registros.

.ip -f 6 
Imprime os nomes de módulos.

.ip -o 6
Inclue o nome do <módulo> em cada linha da saída.

.ip -n 6
Ordena numericamente pelo valor do símbolo
(e alfabeticamente pelo nome em caso de valores iguais).

.ip -e 6
Ordena na ordem inversa (decrescente).

.ip -p 6
Não ordena; imprime a tabela na ordem original.

.ip -N 6
Os nomes dos <módulo>s são lidos da entrada padrão.

.ep
.in -3

.sp
Se for pedida a ordenação numérica ascendente de apenas uma seção
("text", "data" ou "bss"), será incluída uma coluna adicional, contendo
o tamanho (em decimal) de cada elemento da tabela de símbolos.

.sp
O tamanho do último símbolo é obtido através de cálculos
utilizando os tamanhos das seções (o que nem sempre fornece o
valor correto). 

.sp
Além disto, como a tabela de símbolos pode estar incompleta (por exemplo,
sem os símbolos locais), estes tamanhos podem não ser os reais.

.in
.sp
.b OBSERVAÇÃO
.in 5
As tabelas de símbolos de módulos objeto de bibliotecas
podem ser obtidas dos seguintes modos:
.sp
.nf
		nm /lib/libc/stdio/*.o
	ou
		walk -p "*.o" /lib/libc/stdio | nm -N
.fi

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mklib, ld, as
.br
.wo "(fmt): "
a.out
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

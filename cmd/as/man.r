.bp
.he 'AS (cmd)'TROPIX: Manual de Referência'AS (cmd)'
.fo 'Atualizado em 24.08.01'Versão 4.1.0'Pag. %'

.b NOME
.in 5
.wo "as -"
montador para INTEL-80386/486/PENTIUM
.br

.in
.sp
.b SINTAXE
.in 5
.(l
as [-avwe] [-o <obj>] [-l <lst>] [-D <def> ...] <fonte> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "as" é o montador do sistema TROPIX para programas em
linguagem simbólica dos microprocessadores INTEL-80386/486/PENTIUM.

.sp
Sua finalidade é traduzir programas em linguagem simbólica
contidos nos arquivos <fonte> ... para o formato de módulo
objeto padrão do TROPIX, que serve de entrada para o
"link-editor" ld (cmd). 

.sp
Nesta versão do sistema, não há mais limite no tamanho dos identificadores.

.sp
As opções do comando são:

.in +3

.bc	/*************************************/
		"\t-w: Monta para um segmento de 16 bits (ao invés de 32)\n"
		"\t-e: A listagem deve ser no estilo \"little-endian\"\n"
		"\t-o: O nome do módulo objeto gerado deve ser <obj> "
			"(ao invés de \"a.out\")\n"
		"\t-l: Gera um arquivo de listagem com o nome <lst>\n"
		"\t-D: Predefine variáveis "
			"(<def> é da forma \"var\" ou \"var=valor\")\n"
.ec	/*************************************/
.ip -a
Inclui também os símbolos (variáveis e rótulos) locais na tabela
de símbolos no módulo objeto gerado.
Normalmente, são incluídos apenas os símbolos globais.

.ip -v
Com esta opção, o comando "as" emite algumas estatísticas sobre os
SDIs ("span dependent instructions"). 

.ip -w
Inicia a montagem para um segmento de 16 bits -
normalmente, o montador inicia a montagem para um segmento de 32 bits
(isto pode também ser alterado através das pseudo-instruções
".seg16" e ".seg32").

.ip -e
Caso seja pedida uma listagem (opção "-l"), os endereços são
impressos no formato "little-endian" ao invés de "big-endian".

.ip -o
Gera o módulo objeto com o nome <obj> (ao invés de "a.out").
Por homogeneidade, recomendamos que este nome termine com a extensão ".o".

.ip -l
Gera um arquivo de listagem com o nome <lst>.
A listagem contém os códigos e endereços de instruções utilizadas
no programa e a tabela de símbolos produzida.

.ip -D
Antes de iniciar a montagem, define a variável "var" com o "valor"
dado, ou com "1", se o "valor" não for dado.

.ep
.in -3

.in
.sp
.b DIAGNÓSTICOS
.in 5
As incorreções são classificadas em "erros" e "advertências".
Para cada incorreção é impressa a linha, com um ponteiro
indicando o local provável da incorreção, seguida de uma
mensagem de diagnóstico (por hipótese auto-explicativa).
Erros detetados no passo 1 suprimem a execução do passo 2.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ld
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

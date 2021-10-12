.bp
.he 'DIFF (cmd)'TROPIX: Manual de Referência'DIFF (cmd)'
.fo 'Atualizado em 24.04.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "diff -"
comparador diferencial de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
diff [-efc] [-i <nm>] [-C <n>] [-b] <arquivo1> <arquivo2>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "diff" compara os arquivos de texto <arquivo1> e <arquivo2>,
indicando todas as linhas diferentes entre eles.

.sp
Se um dos <arquivo>s (mas não ambos) for "-", ele é lido da entrada padrão.

.sp
Se um dos <arquivo>s (mas não ambos) for um diretório, será utilizado
o arquivo homônimo ao outro neste diretório.
Neste caso não pode ser dado "-".

.sp 2
As diferenças entre os <arquivo>s podem ser indicadas de vários modos,
dependendo da chave dada:

.in +3
.ip -- 9
Se não for dada nenhuma das chaves abaixo, é emitida a saída "normal"
do comando, que consiste em linhas nas formas:
.sp
.(l
			n1     a  n3,n4
			n1,n2  d  n3
			n1,n2  c  n3,n4
.)l
.sp
Estas linhas são semelhantes a comandos do editor "ed" (cmd) para converter
o <arquivo1> no <arquivo2>. 
Os números após as letras são referentes ao <arquivo2>.
Como no "ed", se n1 = n2 ou n3 = n4, o par de números é
abreviado em um número único.

.sp
Além disto, se as letras "a" e "d" dos comandos forem intercambiadas,
e as linhas forem lidas de trás para frente, obtemos comandos para
converter o <arquivo2> no <arquivo1>. 

.sp
Seguindo cada uma destas linhas, vem todas as linhas afetadas do
<arquivo1> indicadas por "<", e todas as linhas afetadas do
<arquivo2> indicadas por ">".

.ip -e 9
Gera uma seqüência de comandos "a", "c" e "d" para o "ed", que converte
o <arquivo1> no <arquivo2>.

.ip -f 9
Gera uma seqüência de comandos semelhante à chave "-e",
na ordem inversa, mas que não pode ser utilizada pelo "ed".

.ip -c 9
Para cada diferença entre os <arquivo>s, mostra a região afetada de cada
<arquivo> incluindo <n> linhas antes e <n> linhas após a região (o contexto).
O valor de <n> é normalmente 2, mas pode ser alterado através da opção "-C"
(veja abaixo).

.sp
Se houve substituição de linhas, isto é indicado através 
do prefixo "!"; a remoção de linhas do <arquivo1> é indicada pelo
prefixo "-"; a adição de linhas no <arquivo2> é indicada
através do prefixo "+".

.ip '-i <nm>' 9
Gera um arquivo "universal" contendo tanto o <arquivo1> como o 
<arquivo2> onde as distinções entre os <arquivo>s  
estão codificadas na forma das diretivas "#ifdef <nm>", "#ifndef <nm>",
"#else" e "#endif" do préprocessador da linguagem "C"
(veja "cpp" (cmd)).

.sp
A partir deste arquivo "universal" podemos recuperar tanto o <arquivo1>
como o <arquivo2> através do comando "ifdef" (cmd)
utilizando a opção "-U <nm>" ou "-D <nm>" respectivamente.

.sp
Se os <arquivo>s não possuírem macros/diretivas do "cpp", ou ainda,
se se tratarem de programas fontes em "C" que desejamos
compilar, podemos aplicar o "cpp" (ou "cc" (cmd)) diretamente
ao arquivo "universal".
 
.ep
.in -3

.sp 2
Além das chaves, temos ainda as opções:

.in +3
.ip -b 8
Ignora brancos nos finais das linhas e considera outras seqüências
de brancos iguais entre si.

.ip '-C <n>' 8
O número de linhas de contexto deve ser <n> ao invés de 2 (valor por omissão).

.ep
.in -3

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Retorna 0 para arquivos iguais, 1 para arquivos diferentes e 2 para
arquivos inacessíveis ou erros. 

.in
.sp
.b OBSERVAÇÕES
.in 5
O comando "diff" obtém o conjunto mínimo de diferenças, exceto em
(raros) casos patológicos.  

.sp
Uma seqüência de comandos gerada pelas chaves "-e" e "-f"
pode não funcionar corretamente se alguma das linhas modificadas
consistir de apenas um ponto.

.sp
Um arquivo "universal" gerado pela chave "-i"
pode não funcionar corretamente se alguma das linhas modificadas
consistir justamente de uma diretiva para o "cpp".

.(t
.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cmp, comm, ed, cpp, cc, ifdef, show
.br
.)t

.in
.sp
.b ARQUIVO
.in 5
/tmp/diff??????

.in
.sp
.b EXEMPLO
.in 5
Uma das maneiras mais cômodas para a utilização do comando
utiliza a saída das diferenças com contexto associado ao comando "show":

.sp
.ce
	diff -c fonte.c ../old | show 


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

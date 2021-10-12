.bp
.he 'IFDEF (cmd)'TROPIX: Manual de Referência'IFDEF (cmd)'
.fo 'Atualizado em 08.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "ifdef -"
realiza préprocessamentos parciais
.br

.in
.sp
.b SINTAXE
.in 5
.(l
ifdef [-vr] [-D <var>] [-U <var>] <arquivo1> [<arquivo2]
ifdef [-vr] [-D <var>] [-U <var>] <arquivo> ... <diretório>
ifdef [-vr] [-D <var>] [-U <var>] -N <diretório>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "ifdef" é um préprocessador simplificado,
realizando um subconjunto das operações do "cpp" (cmd).
A função de "ifdef" é a de realizar préprocessamentos parciais, em que
apenas algumas das variáveis de tempo de préprocessamento
são resolvidas.

.sp
O objetivo primordial de "ifdef" é o de, dados programas
com muitas opções complexas de préprocessamento,
retirar os trechos de programas não desejados, para simplificar e
tornar os programas mais compreensíveis.

.sp
Em sua primeira forma, "ifdef" processa o <arquivo1>, colocando
o resultado em <arquivo2>. Se <arquivo2> não for dado, escreve
o resultado na saída padrão.

.sp
Na segunda forma, um ou mais <arquivo>s são processados, e os
resultados colocados no <diretório> especificado, com os mesmos
nomes dos respectivos <arquivo>s.

.sp
A terceira forma é análoga à segunda, com exceção dos nomes dos
<arquivo>s, que são lidos da entrada padrão, um por linha.

.bc	/*************************************/
.sp
Normalmente, a saída de "ifdef" é utilizado adiante como entrada
para o préprocessador "cpp" numa etapa de compilação,
quando então são resolvidas as variáveis restantes.
.ec	/*************************************/

.sp 2
Para a utilização de "ifdef", dá-se uma lista de variáveis
que devem ser consideradas "definidas" e outra lista de variáveis
que devem ser consideradas "indefinidas".

.sp
São processadas:

.in +3
.ip 1. 3
As pseudo-intruções "#ifdef", "#ifndef", "#else" e "#endif", que referenciam
alguma das variáveis dadas.

.ip 2. 3
A pseudo-intrução "#if", apenas no caso particular
.wo '"#if (0)".'

.ip 3. 3
As pseudo-intruções "#define" e "#undef"
referentes às variáveis dadas, que
são eliminadas.

.ep
.in -3
.sp
Todos as demais pseudo-intruções do préprocessador,
assim como os pseudo-intruções  
referentes a variáveis não dadas
são incluídas sem alteração nos <arquivo>s de saída.

.sp
As opções do comando são:

.in +3 
.ip "-D <var>" 10
Considera a variável <var> definida. 
Esta definição é mais forte do que os "#undef"s encontrados
nos <arquivo>s de entrada.
Todas as referencias a esta variável são resolvidas,
incluindo ou não os trechos adequados dos <arquivo>s de entrada.
Os <arquivo>s de saída não contém mais referências a esta variável.

.ip "-U <var>" 10
Considera a variável <var> indefinida. 
Esta indefinição é mais forte do que os "#define"s encontrados
nos <arquivo>s de entrada.
Todas as referências a esta variável são resolvidas,
incluindo ou não os trechos adequados dos <arquivo>s de entrada.
Os <arquivo>s de saída não contém mais referências a esta variável.

.ip "-N" 10
Le os nomes dos <arquivo>s de entrada da entrada padrão, um por linha.

.ip "-v" 10
Lista os nomes dos <arquivo>s de entrada durante o processamento.

.ip "-r" 10
Lista as variáveis residuais, isto é, as que são referenciadas
em um "#define", "#undef", "#ifdef" ou "#ifndef" e que não
foram dadas em nenhumas das listas de variáveis do comando.
Estas são as variáveis que ainda são referenciadas nos <arquivo>s
de saída.

.in -3 
.ep

.in
.sp
.b OBSERVAÇÃO
.in 5
Deve-se lembrar que o programa processa a pseudo-instrução "#if"
apenas no caso particular "#if (0)".

.sp
Isto significa que ele não reconhece que
.sp
.(l
		#if defined (ZETA)

é equivalente a

		#ifdef	ZETA
.)l
.sp
o que pode gerar programas de saída errados.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cpp
.br

.in
.sp
.b EXEMPLO
.r
.in 5
.wo "				ifdef -D ALPHA -U BETA a.c b.c"
.br
.sp
préprocessa o arquivo "a.c", eliminando todas as referências às
variáveis de préprocessamento "ALPHA" e "BETA" (incluindo ou não
os trechos de "a.c" apropriados), pondo o resultado em "b.c".
As pseudo-instruções referentes às demais variáveis são copiadas
sem mudanças no arquivo "b.c". 

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

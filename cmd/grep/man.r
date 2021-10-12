.bp 1
.he 'GREP (cmd)'TROPIX: Manual de Referência'GREP (cmd)'
.fo 'Atualizado em 08.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
grep - procura padrões em arquivos
.sp

.in
.sp
.b SINTAXE
.in 5
grep [-clnsmrN] <padrão>[|<padrão> ...] [<arquivo> ...]

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "grep" lê um ou mais <arquivo>s, procurando por linhas que contenham
pelo menos um dos <padrões> dados.
Normalmente, cada linha selecionada é copiada para a saída padrão ("stdout").

.sp
Os nomes dos arquivos podem ser dados como argumentos,
podem ser lidos de um arquivo de nomes (ver a opção "-N"),
ou por omissão, o arquivo pode ser a entrada padrão ("stdin").


.sp
Os padrões utilizados pelo comando "grep" são expressões regulares com
a mesma sintaxe das funções "regcmp" e "regex" (libc).

Podem ser dados vários padrões, separados pelo caractere "|".
Para que um padrão possa conter este caractere sem que seja confundido
com o separador de padrões, basta precedê-lo por "\".

.sp
As opções de funcionamento do "grep" são:

.in 8
.ip "-c" 5
Lista apenas o número total de linhas selecionadas
(não copia as linhas selecionadas para a saída padrão).

.ip "-l" 5
Lista apenas os nomes dos arquivos (uma única vez)
que contêm linhas selecionadas
(não copia as linhas selecionadas para a saída padrão).

.ip "-n" 5
Na listagem das linhas selecionadas, inclui além do nome do arquivo,
também o número da linha no respectivo arquivo.

.ip "-s" 5
Não lista nada; retorne apenas o estado (ver o valor de retôrno).

.ip "-m" 5
Procura os padrões sem fazer distinção entre letras maíusculas e minúsculas.

.ip "-r" 5
Inverte o sentido da busca, isto é, selecione linhas que NÃO
contenham nenhum dos padrões.

.ip "-N" 5
Lê os nomes dos arquivos da entrada padrão, ao invés de obtê-los
como argumentos.
Esta opção é útil para utilização com o comando "walk" (cmd).
Se não for dada esta opção,
nem arquivos como argumentos,
o conteúdo do arquivo será lido da entrada padrão.

.ep
.in 5
.sp 2
Deve-se tomar cuidado com a utilização dos caracteres
"$", "*", "[", "]", "|", e "\" nos padrões,
já que eles também têm significado especial
para o interpretador de comandos "sh" (cmd).
O mais seguro é colocar o argumento com os padrões entre aspas.

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Retorna 0 se foi selecionada pelo menos uma linha, retorna 1 se
não foi selecionada nenhuma linha, e retorna 2 se houveram erros
de sintaxe ou arquivos inacessíveis.

.in
.sp
.b OBSERVAÇÃO
.in 5
O tamanho das linhas é limitado a 1024 caracteres. Linhas maiores
são divididas em duas (ou mais).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh, walk
.br
.wo "(libc):"
regcmp, regex
.br


.in
.sp
.b EXEMPLOS
.in 11
.sp
grep comando /etc/passwd
.sp
.br
.in 5
lista as linhas do arquivo "/etc/passwd" que contêm
a cadeia "comando" da seguinte forma:
.sp
.(b
.nf
	/etc/passwd:  data::comando data:10:0:0:0::/:/bin/data
	/etc/passwd:  who::comando who:10:0:0:0::/:/bin/who
.fi
.)b

.sp 3
.in 11
grep -v "^a" *
.sp
.in 5
percorre todos os arquivos do diretório,
listando as linhas que não começam com o caractere "a".

.sp 3
.in 11
walk -p "*.h" /usr/include | grep -N short
.sp
.br
.in 5
lista as linhas que contém "short" de todos os arquivos
de inclusão da árvore cuja raiz é "/usr/include".
.sp

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

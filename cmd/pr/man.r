.bp
.he 'PR (cmd)'TROPIX: Manual de Referência'PR (cmd)'
.fo 'Atualizado em 30.12.04'Versão 4.7.0'Pag. %'


.b NOME
.in 5
.wo "pr -"
imprime arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
pr [-o <o>] [-p <p>] [-e <e>] [-h "<h>"] [-l <l>]
					       [-N] [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "pr" edita um ou mais arquivos para um formato próprio de
impressão, pondo o resultado na saída padrão.

.sp
A saída de "pr" é separada em páginas incluindo um cabeçalho que contém
a data/hora, nome do arquivo (ou um título dado pelo usuário,
veja a opção "-h") e o número da página.

.sp
Se não for dado o nome de nenhum <arquivo>, nem a opção "-N", será editado
o arquivo lido da entrada padrão.
Podemos também especificar a entrada padrão através de "-".


.sp
As opções do comando são:

.in +3
.ip -o
Imprime <o> brancos antes do início de cada linha.
Isto é útil quando não podemos mover lateralmente o papel da impressora.
Lembre-se no entanto, de que se <o> não for múltiplo de 8, as tabulações
podem se modificar.

.ip -p
Começa a imprimir cada <arquivo> a partir da página <p>
(ao invés do seu início).

.ip -e
Imprime somente até a página <e>. Utilizando as opções "-p" e "-e"
podemos especificar quais são as páginas de cada <arquivo> que desejamos
imprimir. Repare que estas opções se aplicam a todos os <arquivo>s.

.ip -h
Utiliza a cadeia de caracteres <h> como cabeçalho das páginas
(ao invés dos nomes dos respectivos <arquivo>s).

.ip -l
Usa a página com <l> linhas (ao invés de 75).
Repare que 10 linhas são sempre reservadas para o cabeçalho, espaços, etc...,
de tal modo que normalmente são impressas apenas 56 linhas do arquivo por
página.

.ip -N
Os nomes dos <arquivo>s são lidos da entrada padrão.

.ep
.in -3

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
cat
.bc	/*************************************/
cat, lpr
.ec	/*************************************/
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

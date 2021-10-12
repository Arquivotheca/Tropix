.bp
.he 'GZIP (cmd)'TROPIX: Manual de Referência'GZIP (cmd)'
.fo 'Escrito em 22.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
Comprime ou expande arquivos usando o algoritmo de Lempel-Ziv:
.sp
.wo "gzip   -"
comprime
.br
.wo "gunzip -"
expande
.br

.in
.sp
.b SINTAXE
.in 5
.(l
gzip [-cdfrRt1..9vN] [<arquivo> ...]
gunzip [-cfrRtvN] [<arquivo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "gzip" tenta reduzir o tamanho dos <arquivo>s dados
usando o método de Lempel-Ziv (LZ77, veja abaixo).
Quando possível, cada "<arquivo>" é substituído por "<arquivo>.gz"
comprimido, conservando o dono e os tempos de acesso/modificação.
O nome original do <arquivo> é armazenado no <arquivo> comprimido.
Se não for dado nenhum <arquivo> nem a opção "-N" (veja abaixo),
a entrada padrão é comprimida e posta na saída padrão.
O comando "gzip" comprime apenas arquivos regulares.

.sp
O comando "gunzip" restaura os <arquivo>s de volta à sua forma original.

.sp
As opções do comando são:

.in +3
.ip -c
Escreve na saída padrão. Não modifica os <arquivo>s originais.
Se forem dados vários <arquivo>s de entrada, a saída consiste de uma
seqüência de membros comprimidos independentemente. Para obter
uma taxa de compressão melhor, concatene todos os <arquivo>s de
entrada antes da compressão. 

.ip -d
Descomprime. Esta opção é desnecessária se o nome do comando
usado for "gunzip".

.ip -f
Executa a compressão/descompressão mesmo que o <arquivo> tenha vários
nomes (links), ou o <arquivo> correspondente já existe. Se esta opção
não for dada e o comando não está sendo executado em "background",
"gzip" consulta o usuário para decidir se um <arquivo> existente
deve ser reescrito.

.ip -r
Recursivo.
Para cada <arquivo> dado que for um diretório, comprime/descomprime
todos os arquivos da árvore cuja raiz é o diretório.

.ip -R
Como "-r".

.ip -t
Teste. Verifica a integridade do <arquivo> comprimido.

.ip -#
Nível de compressão. Com as opções "-1", "-2", ..., "-9" podemos
regular a eficiência do algoritmo de compressão.
Com a opção "-1" temos uma compressão mais rápida (porém com taxa
de compressão mínima) e com "-9" temos uma compressão mais lenta
(porém com taxa de compressão máxima).
Se nenhuma destas opções for dada, é usado o nível "-5".

.sp
O nivel de compressão também pode ser dado no ambiente através da
variável "GZIP". Para usar o nivel 8 (por exemplo) ela deve ter o
valor "8" ou "-8". Se for dado um nível de compressão através da
variável "GZIP" e outro em uma opção do comando, prevalecerá o da
opção do comando.

.ip -v
Verboso.

.ip -N
Le os nomes dos <arquivo>s da entrada padrão.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
Este é um programa originalmente desenvolvido pelo projeto GNU.
Copyright (C) 1989, 1991 Free Software Foundation, Inc.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
gar
.br
.wo "(fmt): "
ambiente
.br

.in
.sp
.b
REFERÊNCIAS
.r
.in 5
[LZ77]: Ziv J., Lempel A., "A Universal Algorithm for Sequential Data
Compression", IEEE Transactions on Information Theory", Vol. 23, No. 3,
pp. 337-343.

.in
.sp
.b ESTADO
.in 5
Programa contribuído.

.in

.bp
.he 'SORT (cmd)'TROPIX: Manual de Referência'SORT (cmd)'
.fo 'Atualizado em 05.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "sort -"
ordena e/ou intercala arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
sort [-vcraidf]   [-o <saída>] [-t <d>] [-k "x:y.zot ... "]
						[<entrada>]

sort -m [-vraidf] [-o <saída>] [-t <d>] [-k "x:y.zot ... "]
						<entrada> ...
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "sort" ordena/intercala as linhas
dos arquivos de <entrada> dados. O resultado da ordenação/intercalação
é escrito na saída padrão ou no arquivo <saída>, se este for dado
(através da opção "-o").

.sp
Em sua primeira forma (sem a opção "-m"), "sort" ordena as linhas
do arquivo <entrada> (ou da entrada padrão se este não for dado).

.sp
Em sua segunda forma (com a opção "-m"), "sort" intercala ("merge") as linhas
dos vários arquivos <entrada> ..., supondo que as linhas destes arquivos
já estejam ordenadas.

.sp
As comparações são realizadas baseadas em uma ou mais chaves
contidas nas linhas de entrada (veja as opções abaixo).
Normalmente (isto é, sem opções), é considerada apenas uma chave
(a linha inteira), e a ordenação é lexicográfica pelos caracteres
na ordem normal da máquina.

.sp
As opções do comando são:

.in +3
.ip -m
Intercalação: ao invés de uma ordenação, "sort" realiza a intercalação
("merge") das linhas dos vários arquivos <entrada> ..., supondo que
as linhas destes arquivos já estejam ordenadas.

.ip -v
Verboso: "sort" imprime algumas informações acerca da ordenação/intercalação.

.ip -c
Verifica se o arquivo já está ordenado: apenas escreve uma mensagem informando
se o arquivo de entrada está ou não ordenado (NÃO escreve a saída ordenada).
Além disto, o código de retorno de "sort" é ZERO se o arquivo de entrada
já está ordenado e um valor NÃO-NULO em caso contrário.
Esta opção é util (por exemplo) para certificar-se de que as diversas entradas
de uma futura intercalação estão ordenadas.

.ip -r
Ordem reversa: inverte o sentido das comparações, isto é, ordena
descendentemente ao invés de ascendentemente.

.ip -a
Tipo de ordenação LEXICOGRÁFICA (tipo por omissão).

.ip -i
Tipo de ordenação ISO: Usa o codigo padrão ISO (BRASCII ou CBII)
para as comparações. Com este tipo de ordenação, as letras acentuadas
têm o mesmo valor das letras sem acento, e é o tipo 
recomendado quando as chaves contem letras acentuadas
(veja "iso" (fmt)).

.ip -d
Tipo de ordenação DICIONÁRIO: para as comparações são levados em
conta apenas as letras e dígitos das chaves, e além disto, as letras
maiúsculas/minúsculas/com acento/sem acento têm o mesmo valor.

.ip -f
Tipo de ordenação FOLD: as letras maiúsculas e minúsculas das chaves têm
o mesmo valor (embora as minúsculas precedam as maiúsculas numa ordenação
ascendente).

.ip -o
Escreve o arquivo ordenado em <saída> ao invés de "stdout".

.ip -t
Utiliza o caracter <d> como delimitador de campos:
Normalmente, os campos são delimitados por um ou mais <sp> ou <ht> (não
existem campos vazios). Com a opção "-t", o delimitador usado será
o caractere <d>. Neste caso, cada ocorrência de <d> é significativa:
a cadeia "...<d><d>..." contém um campo vazio. A delimitação dos campos
de uma linha é importante para a especificação das chaves. 

.ip -k
Define as chaves pelas quais a ordenação deve ser feita:
após a delimitação dos vários campos de uma linha (veja acima),
os seguintes parâmetros podem ser dados para definir quais os trechos
de quais campos serão utilizados como chaves:

.in +5
.ip "x -"
Número do campo: se for zero, a chave considerada é a linha
inteira. Neste caso, o número do campo pode ser omitido mas o
caractere ":" deve ser especificado caso outros parâmetros sejam
desejados para a ordenação.

.ip "y -"
Número do caractere dentro do campo onde começa a chave:
se não for especificado, a chave começa no início do campo.

.ip "z -"
Largura da chave em caracteres: se não for especificada,
a chave termina no final do campo.

.ip "o -"
Letra que define o sentido da ordenação:
"a" para ascendente e "d" para descendente. 
Se for omitido, a ordenação será ascendente.

.ip "t - "
Letra que define o tipo da ordenação:
"a" para LEXICOGRÁFICO, "i" para ISO, "d" para DICIONÁRIO e "f" para FOLD
(veja as respectivas opções acima).
Se for omitido, a ordenação será LEXICOGRÁFICA.

.ep
.in -5

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
O programa "sort" (em sua versão atual) é capaz de ordenar
apenas arquivos que caibam integralmente na memória interna disponível do
sistema. Se isto não for o caso, é responsabilidade do usuário
dividir o arquivo em pedaços menores, ordená-los
separadamente e depois usar uma intercalação (opção "-m").

.sp
O programa "sort" é orientado por linhas (de tamanho variável),
isto é, ele assume que os registros sejam delimitados por <nl>.
Caso este não seja o caso, também é responsabilidade do usuário
inserir um caractere <nl> ao final de cada registro.

.sp
Na versão atual da operação de intercalação, assume-se que as linhas
não tenha mais de 1024 caracteres.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
comm, uniq
.br
.wo "(libc):"
qsort, qtsort, cmptb
.br
.wo "(fmt): "
iso
.br

.in
.sp
.b EXEMPLOS
.in 5
.nf
		sort alpha
.fi

.sp
ordena o arquivo "alpha" em ordem ascendente e põe a saída em "stdout".

.sp 2
.nf
		sort -t : -k "3 4 1" beta
.fi

.sp
ordena o arquivo "beta", que tem campos delimitados pelo
caractere ":", em ordem ascendente considerando primeiro o
campo 3, depois o 4 e finalmente o 1.

.sp 2
.nf
	sort -t : -m -k "3 4 1" arq1 arq2 arq3 arq4 arq5
.fi

.sp
faz a intercalação dos cinco arquivos mencionados 
seguindo as chaves conforme o exemplo anterior.

.sp 2
.nf
		sort -k :ai
.fi

.sp
ordena o arquivo lido do "stdin" em ordem ascendente utilizando tipo de
ordenação ISO usando a linha inteira como chave.

.sp 2
.nf
		sort -t = -k "3:2.3af 4:3.2di" estátuas
.fi

.sp
ordena o arquivo "estátuas", que tem campos delimitados pelo
caractere "=", em primeiro lugar pelo campo 3 a partir da
posição 2 com uma chave de largura 3 em ordem ascendente e
considerando letras maiúsculas e minúsculas indistintamente e,
a seguir, pelo campo 4 a partir da posição 3 com uma chave de
largura 2 em ordem descendente utilizando tipo de ordenação ISO.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

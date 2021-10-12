.bp 1
.he 'GAR (cmd)'TROPIX: Manual de Referência'GAR (cmd)'
.fo 'Atualizado em 20.07.06'Versão 4.9.0'Pag. %'

.b NOME
.in 5
gar - cria/atualiza coleções de arquivos

.in
.sp
.b SINTAXE
.in 5
.(l
gar -i [-xtce] [-F <col>] [-rnvugfmd#] [-B <n>] <PAD>

gar -o [-Tngv#] [-F <col>] [-B <n>] <COMP> < <nomes>
gar -o [-Tnvd.#] [-F <col>] [-B <n>] <PAD> <COMP> <nome> ...

gar -a [-ngv#] [-F <col>] [-B <n>] <COMP> < <nomes>
gar -a [-nvd.#] [-F <col>] [-B <n>] <PAD> <COMP> <nome> ...

Valores de <PAD>: [-s] [-p "<p>"] [-P "<P>"]

Valores de <COMP>: [-D] [-b <b>] [-l <l>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "gar" é um utilitário genérico para guardar e recuperar arquivos
e/ou sistemas de arquivos, em/de fitas magnéticas, disquetes,
arquivos em disco, ou outro meio físico qualquer.
O conjunto de arquivos no referido meio chamaremos de "coleção".

.sp
O formato GAR contém códigos de CRC para cada cabeçalho e cada bloco
(de 512 bytes) dos arquivos, e por este motivo é mais confiável
do que coleções no formato TAR e CPIO.
Além disto, o comando processa todos os tipos de arquivos (regulares,
diretórios, especiais de bloco/caractere, elos simbólicos e "fifo"s),
conservando os elos físicos entre arquivos (quando possível).

.sp
Normalmente, "gar" comprime os arquivos regulares antes de
escrevê-los na coleção.
Com isto é possível um aumento substancial do aproveitamento do meio
físico (como um disquete, por exemplo).
Se a compressão for utilizada, TODAS as operações descritas abaixo
se realizam do mesmo modo
(inclusive
.bc	/*************************************/
a comparação rápida de objetos (opção "-q") e
.ec	/*************************************/
a continuação da coleção em mais um volume).
Veja a seção "COMPRESSÃO DE DADOS".

.sp
Nas operações de saída, o utilitário gera coleções
no seu formato nativo GAR ou no formato TAR, segundo as opções
e os argumentos descritos abaixo.
Nas operações de entrada, o comando lê (reconhecendo automaticamente)
coleções nos formatos GAR, TAR, CPIO e CPIO reverso.
O formato CPIO reverso é um formato obtido quando geramos
uma coleção com o utilitário "cpio" em um computador que
inverte os bytes de uma palavra.

.sp
O comando usualmente não grava nas coleções
os "arquivos ocultos" (arquivos cujos <nomes>
contém algum componente que começa com um ponto - veja a opção "-.").

.sp
A ação do comando é controlada por uma chave e (possivelmente)
um modificador, além de outras opções e argumentos.

.sp
.(t
As chaves são:

.in +3
.ip -i
Entrada: é lida uma coleção da entrada padrão (veja as opções "-#" e "-F")
reconhecendo automaticamente
os formatos GAR, TAR, CPIO e CPIO reverso.
.)t

.sp 1
A operação realizada depende do modificador utilizado,
que deve ser um dos seguintes:
.in +5

.ip -x
Extração de arquivos: os arquivos da coleção são criados
no sistema de arquivos,
com o mesmo nome contido na coleção (veja a opção "-d").
Se forem dados <padrões> (veja as opções "-p", "-P" e "-s"),
só serão criados os arquivos aceitos.
Se o arquivo já existe no sistema de arquivos, o usuário é
consultado para decidir
se recria o arquivo a partir da coleção,
ou se conserva a versão antiga (veja a opção "-f").

.ip -t
Tabela de conteúdo: é gerada (na saída padrão)
uma tabela contendo os nomes dos arquivos
da coleção. Se forem dados <padrões>, a tabela só conterá
nomes de arquivos aceitos. Se for dada a opção "-v", a tabela 
conterá mais informações sobre cada arquivo.
Se ela for dada duas vezes ("-vv"), a tabela conterá ainda mais informações, 
e os nomes simbólicos dos donos dos arquivos são substituídos
pelas identificações numéricas dos donos e dos grupos. 
Nenhum arquivo é criado.

.ip -c
Comparação de arquivos:
esta operação é semelhante à extração ("-x"), mas ao invés de
criar os arquivos, é realizada uma comparação entre os
arquivos da coleção e os arquivos do disco.
As mensagens são dadas na ordem: coleção, arquivo no disco.

.bc	/*************************************/
Caso os arquivos sejam módulos objeto (veja "a.out" (fmt)),
os campos não significativos do cabeçalho são mascarados
de tal modo que apenas os componentes realmente significativos
são levados em conta na comparação.
.ec	/*************************************/

.sp
Durante uma comparação, podemos gerar o arquivo "garfield",
que irá facilitar uma subseqüênte extração dos arquivos diferentes 
(veja a opção "-g").
Também durante uma comparação, os arquivos diferentes podem
logo ser atualizados (veja a opção "-u").

.ip -e
Pipe:
esta operação é semelhante à extração mas ao invés
dos arquivos serem criados no disco, eles são copiados
para a saída padrão. Com isto podemos passar os arquivos para
um processo subsequente (como por ex. o "grep", ou "pr"), ou ainda
podemos simplesmente ver os arquivos na tela.
.in -5

.sp
.ip -o
Saída: é criada uma coleção GAR (ou TAR, veja a opção "-T") na saída padrão
(veja as opções "-#" e "-F") contendo arquivos cujos <nomes> são dados na
entrada padrão (veja a opção "-g"), um por linha, ou dados como argumentos.
Se <nome> for
.bc	/*************************************/
Nesta segunda forma, cada argumento dado, se for
.ec	/*************************************/
o nome de um diretório, será interpretado como sendo
a raiz de uma árvore, e substituído pelos nomes de todos os arquivos da
árvore (veja as opções "-d" e "-.").

.sp
Se forem dados <padrões> (veja as opções "-p", "-P" e "-s")
os nomes obtidos são comparados com os <padrões> e somente
os nomes aceitos são utilizados para serem incluídos na coleção.

.ip -a
Acréscimo:
Esta chave é análoga à "saída" ("-o"), exceto que os arquivos são
adicionados ao final de uma coleção GAR criada anteriormente.

.ep
.in -3
.sp
.(t
.lp
Além das chaves e dos modificadores, temos as seguintes opções: 

.in +3
.ip -T
Gera a coleção no formato TAR ao invés de GAR: Isto é útil para
o transporte de "software" para sistemas SOFIX que aceitam o
formato TAR, mas não GAR.
.)t

.ip -r
Verifica sempre os CRCs (apenas para o formato GAR):
Normalmente, durante uma operação de leitura, somente são verificados
os CRCs dos arquivos processados
(isto é, os arquivos que foram selecionados através dos <padrões>,
confirmados em operações interativas, etc...).
Com esta opção, os CRCs de todos os arquivos são sempre verificados.

.ip -n
Interativo:
para cada ação a realizar (inclusão de um arquivo na
coleção ou criação/comparação de um arquivo lido da coleção)
o usuário é consultado para indicar se o arquivo
em questão deve ser processado ou não.

.sp
No caso da geração interativa de uma coleção, 
a indicação de que o usuário não deseja processar um diretório,
significa que toda a respectiva subárvore será pulada.

.ip -u
Atualização:
Durante uma comparação (modificador "-c"), para cada arquivo diferente,
o usuário é consultado se deseja logo atualizar o arquivo.
Isto só é possível se o conteúdo do arquivo em questão (na coleção)
puder ser lido novamente, o que de acordo com o dispositivo da
coleção, pode não ser possível.

.ip -v
"Verboso": esta opção faz com que informações sejam
dadas sobre o andamento das operações.
No caso da impressão da tabela, faz com que sejam dadas mais
informações sobre cada arquivo listado.

.ip -B
Blocagem: o tamanho do bloco da coleção é determinada automaticamente,
embora possa ser dada explicitamente, na forma de <n> KB
(veja "VOLUMES DE CONTINUACÃO").

.bc	/*************************************/
.sp
Para a saída, caso não seja dada, a blocagem utilizada é de 5120 bytes
para dispositivos não estruturados ("raw") e 512 bytes
para dispositivos estruturados. 

.ip -C
Durante a criação de uma coleção, comprime os arquivos regulares
com o método de "Lempel-Ziv-Welch" (LZW).
Veja a seção "COMPRESSÃO DE DADOS".

.ip -b
Tamanho máximo do código (em bits) usado na compressão LZW.
Esta opção deve ser utilizada apenas na criação da coleção
(a coleção já contém o tamanho máximo a ser utilizado na expansão).
Este valor deve ser dado na faixa de 9 a 16 bits
(se for omitido, será usado um tamanho máximo de 14 bits, que é um bom
compromisso entre o fator de compressão e a memória necessária para arquivos
de 30 Kb ou menores).
.ec	/*************************************/

.ip -D
Normalmente, durante a criação de uma coleção,
os arquivos regulares são comprimidos. Com esta opção, estes arquivos
serão gravados sem compressão.
Veja a seção "COMPRESSÃO DE DADOS".

.ip -l
Nível a ser utilizado na "deflação".
Esta opção deve ser utilizada apenas na criação da coleção.
O nível <l> deve ser dado no intervalo de 
1 (menor compressão, maior velocidade) até 9 (maior compressão,
menor velocidade). Se não for dado, o nível usado é 5. 

.ip -F
A coleção será lida/escrita do/no dispositivo/arquivo <col>.

.sp
Se não for dada nenhuma das opções "-F" nem "-#" (ver abaixo),
a coleção será lida/escrita do/no entrada/saída padrão.
Isto NÃO pode ser usado para acréscimos.

.ip -#
Esta opção é uma abreviatura para nomes de dispositivos
freqüentemente utilizados: # = 0 ou 1 equivale a "-F /dev/fd#";
# = 2 ou 3 equivale a "-F /dev/rzip0" ou "-F /dev/rzip1",
respectivamente.

.ip -p
Padrões de inclusão:
tanto na entrada como na saída, podemos dar <padrões>
(isto é, expressões regulares, como os
do "sh", formados com "*", "?", "!", "[...]") 
para a seleção dos nomes dos arquivos desejados.
Repare que os <padrões> são aplicados ao nome completo do arquivo,
de tal modo que podemos utilizar (por exemplo) um padrão da forma "*/etc/*.c".
No momento podemos dar até 20 <padrões>, entre aspas, separados
por brancos ou tabulações.

.ip -P
Padrões de exclusão:
Esta opção é análoga à opção "-p", exceto que os padrões são usados
para a exclusão de arquivos. Todos os arquivos cujos nomes são
aceitos por estes padrões não são processados, mesmo que tenham sido
aceitos pelos padrões da opção "-p" ou "-s".

.ip -s
Fontes em "C":
esta opção é útil para o processamento de programas fontes
na linguagem "C", e é uma abreviatura para "-p '*[Mm]akefile *.[cshryv]'".
Padrões adicionais podem ser dados com a opção "-p".

.ip -d
Diretório corrente:
durante a extração ou comparação de arquivos de uma coleção,
cria/compara os arquivos no diretório corrente, ao invés de utilizar
o diretório indicado pelo nome completo. 
Com esta opção, elos físicos não são processados.

.sp
Durante a geração de coleções, esta opção pode ser usada
para não incluir os diretórios na coleção.

.(t
.ip -f
Força: durante a criação de uma coleção ou extração de arquivos de uma coleção,
se o arquivo destino já existir, normalmente o usuário é
consultado para decidir
se atualiza o arquivo,
ou se conserva a versão antiga.
A opção "-f" suprime esta consulta, atualizando o arquivo.
.)t

.sp
Analogamente, durante uma comparação com atualização (opções "-icu"),
se for dada a opção "-f", o arquivo é atualizado sem consulta.

.sp
Um outro método de evitar as consultas ao usuário é a de responder com letras
maíusculas: com a resposta "S" ou "Y" todas as perguntas seguintes
serão automaticamente respondidas afirmativamente, e (analogamente)
com a resposta "N" serão respondidadas negativamente
(veja "askyesno" (libc)).

.ip -m
Restaura o estado: durante a extração de arquivos de uma coleção,
restaura os donos, grupos, proteção e tempos originais do arquivo.
Esta opção, só será inteiramente eficaz se o usuário for o superusuário.

.ip -g
"garfield":
durante uma comparação, cria o arquivo "garfield" no diretório
corrente, contendo os <nomes> dos arquivos que acusaram diferenças. 
Durante uma subseqüente extração (também com a opção "-g"),
estes nomes serão usados para extrair apenas os arquivos diferentes.
Na extração podemos ainda utilizar o modo interativo ("-n")
e dar <padrões> ("-s", "-p" ou "-P"), para termos ainda mais controle
sobre a escolha dos arquivos a extrair.

.sp
Durante uma operação de saída ou acréscimo, a opção "-g" pode ser
usada para ler os <nomes> dos arquivos de "garfield" ao
invés da entrada padrão.
 
.bc	/*************************************/
.ip -q
Comparação rápida de módulos objeto:
se durante uma operação de comparação de arquivos, o arquivo da
coleção e o arquivo correspondente do sistema de arquivos forem
módulos objeto, considera os arquivos iguais,
se os tamanhos totais dos arquivos e
as datas contidas nos cabeçalhos forem iguais.
Esta comparação é muito mais rápida, e a probabilidade de indicar
uma igualdade falsa é extremamente baixa.
.ec	/*************************************/

.ip -.
"Arquivos ocultos":
Durante a geração de uma coleção em que os <nomes> dos arquivos
são dados como argumentos do comando, arquivos cujos nomes
contém algum componente que comece com um ponto (exceto "..")
não são gravados (como por exemplo "/usr/src/sys/.old/rot.c").
A opção "-." faz com que estes arquivos sejam também
incluídos na coleção.
.bc	/*************************************/
Além disto, a opção faz com que os diretórios sejam gravados,
mesmo que seus nomes não sejam aceitos pelos <padrões>.
Esta opção pode ser importante para "dump/restore" de sistemas
de arquivos.
.ec	/*************************************/

.ep
.in -3

.sp 2
.in
.b "COMPRESSÃO DE DADOS"
.in 5
Para a compressão de dados, o comando "gar" utiliza
o método da "deflação" (utilizada pelos programas "gzip" (GNU) e PKZIP).

.sp
Com a compressão da dados é possível um aumento substancial do
aproveitamento do meio físico (como um disquete, por exemplo).
Com programas fontes, a capacidade de um meio físico tipicamente
mais do que dobra, chegando a triplicar ou até a quadruplicar
quando os arquivos contém muita redundância.

.bc	/*************************************/
.sp
Quando gravado com compressão, os arquivos regulares contém uma
dupla verificação de CRC: além dos CRCs gravados normalmente em cada
bloco da coleção para verificar a integridade da coleção
no meio físico, cada arquivo recebe CRCs adicionais, para
verificar que o processo de expansão está de fato recuperando o arquivo
original.

.sp
Se a compressão for utilizada, TODAS as operações descritas
se realizam do mesmo modo (inclusive a comparação rápida de objetos
(opção "-q")) e continuação da coleção em mais um volume.
.ec	/*************************************/

.sp
Durante a leitura de uma coleção,
"gar" identifica quais são os arquivos comprimidos,
e torna a descompressão automática e transparente.

.sp
Se um arquivo ficar maior na forma comprimida (o que é possível para
arquivos muitos pequenos), "gar" identifica isto e grava o arquivo
na sua forma original. Podemos ter coleções com arquivos comprimidos
e originais misturados (o que pode facilmente ocorrer com a opção de
acréscimo (chave "-a").

.bc	/*************************************/
.sp
Ao contrário do que se possa imaginar, nem sempre a criação/recuperação
de coleções comprimidas é mais lenta - com disquetes, por exemplo, a
compressão/descompressão é realizada em paralelo com a leitura/escrita,
e como lemos/escrevemos menos blocos do/no disquete, na realidade a operação é
normalmente até mais rápida!

.sp
Normalmente, o método da "deflação" obtém compressões maiores, embora
seja um pouco mais lento. Veja as opções "-C", "-D", "-b" e "-l".

.sp
Para simplificar o uso, os parâmetros de compressão podem ser pré-definidos no
ambiente (veja "environ" (fmt)) através da variável "GAR",
cujo valor deve ter a forma:

.sp
.(l
		<método>[,<n>]
.)l
.sp
onde <método> pode ser "reduce", "compress" ou "lzw" para o método LZW
e "deflate" ou "zip" para a deflação. Se dado o valor numérico <n>, ele será
interpretado como número de bits ou nível de compressão de acordo com o
<método> dado.
.ec	/*************************************/

.sp 2
.in
.b "VOLUMES DE CONTINUAÇÃO"
.in 5
O utilitário processa volumes de continuação, isto é,
ele prevê o caso de uma coleção não caber em um volume
(fita magnética ou disquete).
Isto é de particular interesse para disquetes, que podem conter
apenas pequena quantidade de informação.

.sp
Durante a geração da coleção, ao verificar que um volume
chegou ao final, é emitida uma mensagem no terminal, pedindo
o nome do dispositivo aonde deve continuar a gravação da coleção.

.sp
Durante a leitura de uma coleção, ao final de um volume,
o utilitário pede o nome do dispositivo de continuação,
e verifica se o volume de continuação realmente é o
correto.

.sp
Tanto na leitura como na escrita,
se o dispositivo de continuação for o mesmo do volume inicial, e este não
for a entrada/saída padrão, basta dar um <nl>.

.bc	/*************************************/
.sp
Para o funcionamento correto de volumes de continuação,
é importante utilizar na leitura a mesma blocagem 
utilizada na geração da coleção.
.ec	/*************************************/

.sp
Repare que uma coleção pode ficar armazenada em diversos
meios, tal como
o começo em disquete, a continuação em uma fita
magnética e o final em um arquivo em disco.

.sp
.in
.b OBSERVAÇÕES
.in 5
.bc	/*************************************/
O utilitário é próprio para realizar "dump/restore" de sistemas
de arquivos. Neste caso, durante a criação da coleção,
não esquecer os arquivos ocultos (opção "-.")
e durante a recuperação, não esquecer o estados originais dos
arquivos (opção "-m").

.sp
No momento, a operação de "acréscimo" ("-a") pode ser realizada
apenas em disquetes ou fita magnética não blocada. 
.ec	/*************************************/

Deve-se tomar cuidado na utilização do arquivo "garfield"
("-g") com coleções em que foram acrescentados arquivos
(operação "-a"), pois a coleção pode conter arquivos repetidos.

.sp
O comando não realiza uma consistência completa nas opções.
Isto significa que em certos casos, opções supérfluas são
ignoradas.

.sp
.in

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
reduce, gzip
.br
.wo "(libc):"
askyesno
.br
.wo "(fmt): "
gar
.br

.in
.sp
.b ARQUIVOS
.in 5
 /dev/fd?
 /dev/rzip?
 ./garfield

.in
.sp
.(t
.b EXEMPLOS
.in 5
Cria a coleção "col" no diretório corrente, contendo o arquivo "abc.c"
e todos os arquivos da árvore cuja raiz é "dir":
.sp
.nf
		gar -ov abc.c dir  >col
.fi
.)t

.sp 2
.(t
O arquivo "names" contém os nomes de diversos arquivos, um por linha.
Cria uma coleção na unidade de disquete ("/dev/fd0"):
.sp
.nf
		gar -ov0 <names
.fi
.)t

.sp 2
Le uma coleção de um disquete, e extrai para o disco todos os arquivos
contendo programas fontes em "C":
.sp
.nf
		gar -ixv0 -p "*.c"
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in

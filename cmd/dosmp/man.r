.bp
.he 'DOSMP (cmd)'TROPIX: Manual de Referência'DOSMP (cmd)'
.fo 'Atualizado em 19.10.05'Versão 4.8.0'Pag. %'

.b NOME
.in 5
.wo "dosmp -"
processa um sistema de arquivos no formato MS-DOS
.br

.in
.sp
.b SINTAXE
.in 5
.(l
dosmp [-crv] [-0123] [<dispositivo>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "dosmp" é uma interface para o processamento de sistemas
de arquivos no formato MS-DOS. Ele permite uma série de operações
tais como listagens, cópias e comparações de arquivos de/para o
sistema TROPIX.

.sp
O nome do <dispositivo> contendo o sistema de arquivos DOS pode ser dado
juntamente com o nome do programa, ou pode ser dado posteriormente
através do comando "mount" (ver abaixo). Caso o <dispositivo> ainda
não contenha um sistema de arquivos DOS, pode ser utilizada a opção
"-c" (ver abaixo) para criar um sistema DOS, ou pode ser utilizado
posteriormente o comando "mkfs".

.sp
Nesta versão, "dosmp" só está
aceitando <dispositivo>s especiais estruturados (isto é, de blocos),
e arquivos regulares (isto é, uma imagem de um sistema de arquivos
DOS contida em um arquivo regular).
São aceitas FATs de 12, 16 e 32 bits.

.sp
Se for dado um diretório, "dosmp" supõe que seja um ponto de montagem
e procura o  <dispositivo> correspondente na tabela "fstab" (veja "fstab" (fmt)).

.sp
Após a análise do <dispositivo> (possivelmente) dado, "dosmp" passa
o controle para o interpretador de comandos, aguardando instruções
do usuário. Para indicar que está aguardando comandos, "dosmp" imprime
uma linha contendo os caracteres
.sp
.nf
		"dosmp [n]> "
.fi
.sp
onde "n" é o número
do comando no histórico (ver os comandos "h", "eh" e "xh" abaixo). 
Caso o sistema de arquivos contenha o nome de um volume no diretório
raiz (por exemplo, "cartas"), a linha impressa será
.sp
.nf
		"dosmp (cartas) [n]>".
.fi
.sp

.sp
As opções do comando são:

.in +3
.ip --
(Caso normal, sem as opções "-c" e "-r"): Se for dado um <dispositivo>,
"dosmp" assume que contém um sistema de arquivos DOS, e monta-o somente
para leituras (o que pode ser alterado posteriormente através do comando "rw").
Se NÃO for dado um <dispositivo>, "dosmp" aguarda comandos do usuário.

.ip -c
Se for dado um <dispositivo>, "dosmp" nele cria um sistema de arquivos
DOS. Em caso contrário, aguarda comandos do usuário.

.ip -r
Modo garantido contra modificações dos sistemas de arquivos:
como o caso normal acima, só que não é aceito o comando "rw", nem
qualquer outro que modifique o sistema de arquivos DOS.

.ip -v
Verboso.

.ip -0
O <dispositivo> usado é "/dev/fd0".

.ip -1
O <dispositivo> usado é "/dev/fd1".

.ip -2
O <dispositivo> usado é "/dev/zip0".

.ip -3
O <dispositivo> usado é "/dev/zip1".

.ep
.in -3

.(t
.in
.sp
.b 'MODO DE REPRESENTAÇÃO DE DADOS'
.in 5
O formato de armazenamento de arquivos de texto em sistemas DOS é
ligeiramente diferente do que no TROPIX. As separações de linhas,
por exemplo, são feitas através de dois caracteres, <cr><nl> (ao
invés apenas de <nl>), e o final do arquivo pode conter o caratere <^Z>.
.)t

.sp
Para controlar a conversão (ou não) entre os formatos, temos
o modo de representação de dados corrente, que pode ser ISO (ASCII) ou
BINÁRIO. O modo ISO supõe que estejamos processando textos, e portanto
todos os conteúdos dos arquivos regulares lidos/escritos de/para
o sistema DOS são convertidos conforme a descrição acima.
O modo BINÁRIO supõe que estejamos processando dados binários, e portanto
os conteúdos não são convertidos.
Os conteúdos de diretórios nunca são convertidos.

.sp
O comando "status" informa o modo corrente,
o comando "iso" altera o modo para ISO e
o comando "bin" altera o modo para BINÁRIO.
Durante a descrição de cada comando (abaixo), é mencionado se o modo de
representação de dados corrente é relevante para o seu funcionamento.

.sp
O programa "dosmp" inicia no modo ISO.

.in
.sp
.b 'NOMES DE ARQUIVOS'
.in 5
O comando "lfn" do programa "dosmp" permite escolher o modo de conversão
dos nomes do sistema TROPIX para o DOS. Com o modo "off", os nomes
são convertidos para o formato antigo (tradicional) do DOS, que permite
apenas 11 caracteres maíusculos, 8 na base e 3 na extensão.

.sp
Com o modo "on", os nomes são convertidos para o formato novo ("Long
File Names" (LFN), do Windows 95/98), que permite até 255 caracteres.

.sp
Independente do modo, "dosmp" sempre le integralmente os LFNs encontrados.
O comando "ls" (veja abaixo) imprime os nomes completos, enquanto que
o comando "lc" (veja abaixo) os trunca a 35 caracteres.

.sp
O programa "dosmp" inicia no modo "on".
O comando "status" informa o modo corrente (veja o comando "lfn", abaixo).

.bc	/*************************************/
.in
.sp
.b 'NOMES DE ARQUIVOS'
.in 5
A sintaxe dos nomes de arquivos também são diferentes entre os sistemas
DOS e TROPIX. Enquanto que no TROPIX, os nomes podem ter até 14 caracteres
maiúsculos ou minúsculos, no DOS os nomes podem ter apenas 11 caracteres
maíusculos, 8 na base e 3 na extensão.

.sp
Na criação ou cópia de arquivos, o usuário é alertado quando o nome
dado (ou obtido do TROPIX) não é adequado para o DOS. Neste caso,
o próprio "dosmp" sugere um truncamento adequado, e consulta o usuário
se ele aceita a sugestão.
No caso da sugestão ser aceita, "dosmp" tenta conservar os caracteres
truncados em uma área não utilizada dos diretórios DOS
(apenas para facilitar o usuário do próprio "dosmp"). 
Em caso contrário, "dosmp" pede um nome substituto para o usuário,
mas neste caso, os (possíveis) caracteres truncados não são conservados.

.sp
As sugestões de truncamento são feitas sempre no final da base/extensão,
como mostram os seguintes exemplos:
.nf

		abcdefghijkl		=>  ABCDEFGH
		abc.uvwxyz		=>  ABC		UVW
		abcdefghijkl.uvwxyz	=>  ABCDEFGH	UVW
		.abc.uvw		=>  ABC		UVW
		abc.def.uvw		=>  ABC		DEF
.fi

.sp
Utilizando o comando "lc" (ver abaixo) podemos ver os nomes dos
arquivos com os caracteres truncados (possivelmente) restaurados,
e usando "lc -d", podemos ver os nomes dos arquivos como serão
"vistos" pelos sistemas DOS. 

.sp
O que infelizmente não é conservado, é a informação de quais os caracteres
são originalmente maiúsculos no TROPIX. Todos caracteres são armazenados
em maíusculo no sistema DOS, e convertidos para minúsculos quando
"dosmp" se referencia a eles.
Portanto (por exemplo), o nome "Makefile" será armazenado como
"MAKEFILE" no sistema de arquivos DOS, e convertido de volta para "makefile"
quando "dosmp" se referir a ele.
Por este motivo, nestes casos, é preferível usar os comandos de comparação
que obtenham os nomes do sistema TROPIX, tais como "cmpput" e "cmpputtree"
(ao invés de "cmp" e "cmpgettree").
.ec	/*************************************/

.in
.sp
.b 'EXPANSÃO DOS META-CARACTERES DOS ARGUMENTOS'
.in 5
De acordo com o comando, os meta-caracteres "?*[]!" dos seus argumentos
não são expandidos,
são expandidos no sistema de arquivos DOS ou
são expandidos no sistema de arquivos TROPIX.

.sp
Normalmente, quando os argumentos não se referirem a arquivos ou
se referirem a arquivos que ainda vão ser criados, eles
não são expandidos (tal como nos comandos "mount" e "mkdir");
quando os argumentos se referirem a arquivos DOS, eles são expandidos
no sistema DOS (tal como nos comandos "mv" e "get"); e
quando os argumentos se referirem a arquivos TROPIX, eles são expandidos
no sistema TROPIX (tal como nos comandos "put" e "puttree").

.sp
Repare que na expansão no sistema DOS, os arquivos com o atributo "h"
(ver "chmod", abaixo) não são considerados.
Durante a descrição de cada comando, o usuário é alertado se por acaso
a expansão não segue o bom senso descrito acima.

.in
.sp
.b 'DIRETÓRIO CORRENTE'
.in 5
Ao utilizar o comando "dosmp", temos em cada momento, dois diretórios
correntes: o do DOS e o do TROPIX. Eles podem ser alterados respectivamente
pelos comandos "cd" e "!cd".

.sp
O modo mais cômodo de usar o "dosmp" é o de trabalhar entre diretórios
correntes. Isto é uma decorrência do fato de que alguns dos comandos mais
importantes ("get" e "put", entre outros) sempre se referirem a arquivos no
diretório corrente do sistema de arquivos destino.

Por exemplo, no comando "get alpha/beta/gamma.c" é copiado o arquivo
"alpha/beta/gamma.c" do sistema DOS no arquivo "gamma.c" do diretório
corrente do sistema TROPIX.

.sp
Se durante a descrição de um comando, nada for mencionado,
são utilizados os nomes completos dos arquivos
(isto é, todos os componentes do caminho).
Em caso contrário, é indicado em qual sistema é usado o diretório
corrente.

.in
.sp
.(t
.b COMANDOS
.in 5
O programa "dosmp" reconhece os seguintes comandos:

.bc	/*************************************/
.in +3
.ec	/*************************************/
.ip " 1." 4
?
.sp
Como "help", abaixo.

.ip " 2." 4
!<comando> [<argumento> ...]
.sp
Executa o <comando> com os respectivos <argumento>s no sistema TROPIX.
Isto vale inclusive para o comando "cd".
.)t

.ip " 3." 4
arena [-v]
.sp
Imprime e verifica a consistência da arena de "malloc" (libc).
Este comando é indicado para a depuração e/ou manutenção do programa,
e não para o uso normal.

.(t
.ip " 4." 4
ascii
.sp
Altera o modo de representação de dados para ISO (ASCII).
.)t

.ip " 5." 4
bin
.sp
Altera o modo de representação de dados para BINÁRIO.

.ip " 6." 4
bye
.sp
Encerra o programa. Um EOF (normalmente <^D>) tem o mesmo efeito.

.ip " 7." 4
cat <arquivo>
.sp
Imprime um arquivo DOS na saída padrão.
O conteúdo do arquivo é convertido,
conforme o modo de representação de dados.

.ip " 8." 4
cd <novo_diretório>
.sp
Altera o diretório corrente DOS para <novo_diretório>.

.ip " 9." 4
chd <novo_diretório>
.sp
Como "cd", acima.

.ip "10." 4
chm [-v] <modo> <arquivo> ...
.sp
Como "chmod", abaixo.

.ip "11." 4
chmod [-v] <modo> <arquivo> ...
.sp
Altera atributos de arquivos DOS.
Podem ser alterados os atributos "m" (arquivo modificado),
"s" (arquivo do sistema DOS), "h" (arquivos ocultos),
"r" (arquivo com proteção de escrita), além dos atributos
"1" e "2" (reservados).
O <modo> pode ser dado como uma constante inteira decimal,
octal ou hexadecimal (conforme a sintaxe da linguagem "C")
ou ainda simbolicamente com a sintaxe
.sp
.nf
			[u] <op> <atributo>
.fi
.sp
onde <op> pode ser "=", "+" ou "-" e <atributo> deve ser uma das
letras descritas acima. Vários modos podem ser dados, separados
por virgulas, como no exemplo
.sp
.nf
			chmod u-h,+r alpha beta
.fi
.sp
Se for dada a opção "-v", são listados os modos antigo e novo de
cada <arquivo>. A sintaxe do <modo> deste comando é baseada em
"chmod" (cmd), onde podem ser vistos mais detalhes.

.ip "12." 4
chsize [-s <n>]
.sp
O objetivo do comando "chsize" é o de liberar <n> MB ainda não
alocados do final do sistema de arquivos DOS, para a
criação de uma nova partição
(por exemplo, para um sistema de arquivos de um outro sistema operacional).
Antes de usar este comando, é recomendável desfragmentar
o sistema de arquivos através do comando DEFRAG ou SPEED DISK
(dos NORTON UTILITIES).
Talvez seja útil também usar o comando "compac" (veja abaixo).

.sp
Sem opções, o comando informa o tamanho atual da partição,
o tamanho atual do sistema de arquivos DOS
e o espaço livre disponível no final do sistema de
arquivos (em MB).

.sp
Com a opção "-s <n>", além dos dados acima, são propostos novos tamanhos
para a partição DOS, o sistema de arquivos DOS e a nova partição
a ser criada. Este último valor pode não ser exatamente igual ao
valor dado <n>, por arredondamentos decorrentes do tamanho do cilindro
do disco.

.sp
Se a proposta for aceita, o superbloco original do sistema de arquivos
é gravado no arquivo "dos_sb" (no diretório corrente do TROPIX),
e o sistema de arquivos tem o seu tamanho
reduzido para o valor da proposta.
Anote o novo tamanho da partição DOS e o tamanho da nova partição
a ser criada (em MB e blocos).

.sp
Se tudo correr de acordo com as expectativas (isto é, o DOS reconhecer
corretamente o novo tamanho do sistema de arquivos DOS), utilize o
"fdisk" (disponível no "boot2") para alterar o tamanho 
da partição DOS e criar a nova partição (de acordo com os tamanhos anotados).

.sp
Se houver problemas com o novo sistema de arquivos DOS
(de tamanho reduzido), o tamanho original pode ser restaurado através do
comando

.sp
.nf
		dd if=dos_sb of=/dev/dos_c
.fi
.sp
onde "/dev/dos_c" deve ser substituído pelo dispositivo correspondente
à partição DOS.

.ip "13." 4
clri <arquivo> ...
.sp
Remove entradas de diretório DOS (modo de pânico).
Em casos de emergência, remove <arquivos>, que por alguma razão,
o comando "rm" (ver abaixo) não consegue remover. Após o uso, deve
ser usado o comando "fsck" (ver abaixo) para restaurar a consistência
do sistema de arquivos DOS.
Este comando não é indicado para o uso normal.

.(t
.ip "14." 4
cmp <arquivo> ...
.sp
Compara pares de arquivos correspondentes entre os sistemas DOS e TROPIX.
O comando "cmp" nada comenta sobre arquivos iguais.
Para arquivos diferentes, indica o número do primeiro caractere
diferente, ou se um dos arquivos é prefixo do outro.
Se NÃO forem iguais, informa qual das versões é a mais recente.
.)t
.sp
Se algum nome de <arquivo> for um caminho com vários componentes,
o nome completo será usado apenas para referenciar o arquivo DOS;
no TROPIX será utilizado o diretório corrente.
Os conteúdos dos arquivos DOS são convertidos,
conforme o modo de representação de dados.
Os meta-caracteres "?*[]!" são expandidos no sistema DOS.

.ip "15." 4
cmpget <arquivo> ...
.sp
Como "cmp", acima.

.ip "16." 4
cmpgettree [-ui.s] [-p "<p>"] [-P "<P>"] <árvore>
.sp
O comando "cmpgettree" caminha a <árvore> DOS comparando cada
arquivo com o arquivo correspondente da <árvore> TROPIX.
Se for encontrada alguma diferença entre as árvores, ela é indicada,
e de acordo com a opção "-u" a árvore TROPIX pode
ser atualizada.
.sp
Para maiores detalhes sobre as opções "-ui.spP" consulte "cmptree" (cmd).
Normalmente não são selecionados arquivos com o atributo "h"
(ver "chmod" acima). A opção "-." seleciona também estes arquivos.
.sp
O comando "cmpgettree" nada comenta sobre pares de arquivos iguais.
Para arquivos diferentes, indica o número do primeiro caractere
diferente, ou se um dos arquivos é prefixo do outro.
Se NÃO forem iguais, informa qual das versões é a mais recente.
.sp
O nome completo da <árvore> é usado tanto para referenciar o
sistema TROPIX como o DOS.
Os conteúdos dos arquivos DOS são convertidos,
conforme o modo de representação de dados.
Os meta-caracteres "?*[]!" são expandidos no sistema DOS.

.ip "17." 4
cmpput <arquivo> ...
.sp
Como "cmp", acima, só que
se algum nome de <arquivo> for um caminho com vários componentes,
o nome completo será usado apenas para referenciar o arquivo TROPIX;
no DOS será utilizado o diretório corrente. Além disto,
os meta-caracteres "?*[]!" são expandidos no sistema TROPIX.

.(t
.ip "18." 4
cmpputtree [-ui.s] [-p "<p>"] [-P "<P>"] <árvore>
.sp
O comando "cmpputtree" caminha a <árvore> TROPIX comparando cada
arquivo com o arquivo correspondente da <árvore> DOS.
Se for encontrada alguma diferença entre as árvores, ela é indicada,
e de acordo com a opção "-u" a árvore DOS pode
ser atualizada.
.)t
.sp
Para maiores detalhes sobre as opções "-ui.spP" consulte "cmptree" (cmd).
.sp
O comando "cmpputtree" nada comenta sobre pares de arquivos iguais.
Para arquivos diferentes, indica o número do primeiro caractere
diferente, ou se um dos arquivos é prefixo do outro.
Se NÃO forem iguais, informa qual das versões é a mais recente.
.sp
O nome completo da <árvore> é usado tanto para referenciar o
sistema TROPIX como o DOS.
Os conteúdos dos arquivos DOS são convertidos,
conforme o modo de representação de dados.
Os meta-caracteres "?*[]!" são expandidos no sistema TROPIX.

.ip "19." 4
compac [-v]
.sp
Compacta o sistema de arquivos. Este comando é um desfragmentador
especial para retirar do final do sistema de arquivos aqueles
arquivos que contém "cluster"s os quais os desfragmentadores
tais como o DEFRAG ou SPEED DISK (dos NORTON UTILITIES)
consideram que "não devem ser movidos".
Isto é útil para um posterior uso do comando "chsize" (veja acima).

.sp
Lembre-se no entanto, ao usar o comando "compac", de que ele não tem
consciência das possíveis conseqüências ocasionadas por
estas mudanças no funcionamento do DOS/Windows.

.bc	/*************************************/
.ps
O comando "compac"
deve (naturalmente) ser usado somente após o sistema de
arquivos tiver sido desfragmentado por um desfragmentador
do Windows, e ele ter decidido não mover alguns "cluster"s
no final do sistema de arquivos.
Mesmo assim, deve ser usado apenas quando 
.ec	/*************************************/

.sp
Este comando é interativo; a cada ciclo ele imprime o grau de
compacticidade e o espaço livre ao final do sistema de arquivos
(100 \% de compacticidade significa que todos os "cluster"s ocupados
estão contíguos no início do sistema de arquivos).

.sp
Em seguida é impresso o nome do arquivo que contém o último
"cluster" ocupado do sistema de arquivos, e o usuário é consultado
se deseja mover o arquivo para o início do sistema.

.sp
Se a resposta for afirmativa, o arquivo é movido,
e o ciclo se repete. Deste modo podemos mover arquivos até que haja
um certo espaço livre desejado ao final do sistema para um
"chsize" posterior.

.ip "20." 4
del [-ifv] <arquivo> ...
.sp
Como "rm", abaixo.

.(t
.ip "21." 4
df
.sp
Imprime o espaço total e livre do sistema de arquivos em KB.
Informa também o nome do dispositivo, se está protegido contra
escritas ("RO") e a identificação contida no super-bloco do sistemas
de arquivos.
.)t

.(t
.ip "22." 4
diff <arquivo>
.sp
Compara diferencialmente arquivos entre os sistemas TROPIX e DOS.
Este comando copia o arquivo DOS para uma arquivo temporário,
compara as versões através de "diff" (cmd) e mostra as diferenças
com "show" (cmd).
A ordem das diferenças indicadas é TROPIX, DOS.
.)t
.sp
Se o nome do <arquivo> for um caminho com vários componentes,
o nome completo será usado apenas para referenciar o arquivo TROPIX;
no DOS será utilizado o diretório corrente.
O conteúdo do arquivo DOS é convertido,
conforme o modo de representação de dados.
Os meta-caracteres "?*[]!" são expandidos no sistema DOS.

.ip "23." 4
du [-sa] [<árvore> ...]
.sp
Informa sobre o espaço usado (em KB) por todos os arquivos e diretórios 
contidos em cada <árvore> dada do sistema de arquivos.
.sp
Se for dada a opção "-s", imprime somente o total de cada <árvore>.
Se for dada a opção "-a", imprime uma linha para cada arquivo.
Os meta-caracteres "?*[]!" são expandidos no sistema DOS.

.(t
.ip "24." 4
dir [-d] [<diretório> ...]
.sp
Como "lc", abaixo.
.)t

.ip "25." 4
dump <arquivo>
.sp
Como "xd", abaixo.

.ip "26." 4
echo [-d] <argumento> ...
.sp
Ecoa os <argumento>s (expandidos no sistema DOS).
Isto é útil para saber como os argumentos irão ser expandidos,
antes de utilizar um comando cujos argumentos são expandidos no DOS.
.sp
Se for dada a opção "-d", os argumentos serão impressos em um formato
vetorial (ao invés de linear).

.ip "27." 4
echoget [-d] <argumento> ...
.sp
Como "echo", acima.

.ip "28." 4
echoput [-d] <argumento> ...
.sp
Como "echo", acima, só que
os meta-caracteres "?*[]!" são expandidos no sistema TROPIX.

.ip "29." 4
eh [<n>]
.sp
Edita (e opcionalmente executa) o comando de número <n> do histórico.
Se <n> for omitido, edita o último comando presente no histórico.
A edição é realizada pela função "edit" (libc).
Após a edição, o usuário é consultado se deseja realmente executar
o comando editado.

.(t
.ip "30." 4
era [-ifv] <arquivo> ...
.sp
Como "rm", abaixo.
.)t

.ip "31." 4
exit
.sp
Encerra o programa. Um EOF (normalmente <^D>) tem o mesmo efeito.

.ip "32." 4
fsck [-f]
.sp
Verifica a consistência do sistema de arquivos. Se o sistema de arquivos
estiver montado com permissão de escrita, o usuário é consultado
se deseja consertar alguns erros mais simples.
Se for dada a opção "-f", "fsck" conserta sem consultar o usuário.

.sp
De modo normal, no entanto, recomendamos efetuar o conserto
do sistema de arquivos MS-DOS através do próprio MS-DOS.

.(t
.ip "33." 4
get [-ifv] <arquivo> ...
.sp
Copia arquivos DOS para o diretório corrente do sistema TROPIX.
Para detalhes sobre as opções "-ifv", use "get -H".
.)t
.sp
Se algum nome de <arquivo> for um caminho com vários componentes,
o nome completo será usado apenas para referenciar o arquivo DOS.
Os conteúdos dos arquivos DOS são convertidos,
conforme o modo de representação de dados.

.ip "34." 4
gettree [-ifv.s] [-p "<p>"] [-P "<P>"] <árvore>
.sp
Copia árvores DOS para o sistema TROPIX.
Para maiores detalhes sobre as opções "-ifv.spP" consulte "cptree" (cmd).
Normalmente não são selecionados arquivos com o atributo "h"
(ver "chmod" acima). A opção "-." seleciona também estes arquivos.
.sp
O nome completo da <árvore> é usado tanto para referenciar o
sistema TROPIX como o DOS.
Os conteúdos dos arquivos DOS são convertidos,
conforme o modo de representação de dados.

.ip "35." 4
h
.sp
Imprime os comandos do histórico. Nesta versão, o histórico armazena
os últimos 20 comandos executados.

.ip "36." 4
help
.sp
Imprime uma lista de todos os comandos disponíveis. Em certos
casos, recomenda o uso de alguns comandos.
Para obter um resumo de um <comando>, use "<comando> -H".

.ip "37." 4
image
.sp
Altera o modo de representação de dados para BINÁRIO.

.ip "38." 4
iso
.sp
Altera o modo de representação de dados para ISO (ASCII).

.(t
.ip "39." 4
lc [<diretório> ...]
.sp
Imprime o conteúdo dos <diretório>s dados.
Se não for dado nenhum <diretório> será considerado o diretório corrente.
.)t

.sp
Se o ambiente contiver a variável MODECOLOR, os diversos tipos
de arquivos serão impressos com cores distintas (veja "modecolor" (libc)).

.(t
.ip "40." 4
lfn [off|8.3] [on|255|w95|W95]
.sp
Indica o modo de criação dos nomes DOS. Com o argumento "off" ou "8.3",
os nomes serão criados no estilo "antigo" ou "ortodoxo", isto é, da época
pré-windows-95, com nome de até 8 caracteres e uma extensão de até 3
caracteres.
.)t
.sp
Com o argumento "on", "255", "w95" ou "W95",
os nomes serão criados no estilo "novo", isto é, da época
windows-95/98, com nomes de até 255 caracteres.

.(t
.ip "41." 4
ls [-al] <arquivo> ...
.sp
Imprime características dos <arquivo>s dados.
Se um <aquivo> for um diretório, imprime características de todos
arquivos do diretório.
Se não for dado nenhum <arquivo> será considerado o diretório corrente.
.)t
.sp
Com a opção "-a" são listados também as entradas "." e ".." dos diretórios.
Sem a opção "-l" são listados apenas os nomes dos arquivos;
com esta opção são listados também o tipo do arquivo, os seus atributos,
o seu tamanho e sua data. Se o arquivo tiver o atributo "h"
(ver "chmod" acima), o seu nome virá entre parenteses.

.sp
Se o ambiente contiver a variável MODECOLOR, os diversos tipos
de arquivos serão impressos com cores distintas (veja "modecolor" (libc)).

.ip "42." 4
md <diretório> ...
.sp
Como "mkdir", abaixo.

.ip "43." 4
mkd <diretório> ...
.sp
Como "mkdir", abaixo.

.ip "44." 4
mkdir <diretório> ...
.sp
Cria diretórios DOS.
O diretório pai de cada <diretório> a ser criado já deve existir.

.ip "45." 4
mkfile <arquivo> <MB sz>
.sp
Cria o <arquivo> DOS de tamanho <MB sz>.
O espaço em disco necessário é alocado em uma área contígua.
Se o sistema de arquivos não possuir <MB sz> contíguos,
é emitida uma mensagem de erro, e o arquivo não é criado.
O conteúdo do arquivo NÃO é inicializado (ou em outras palavras,
permanecem os valores já existentes nos blocos do disco).

.sp
O objetivo usual deste comando é criação dos arquivos necessários
para a instalação do TROPIX em arquivos de uma partição DOS.

.ip "46." 4
mkfs [-v] [-sfc ...] [-01] [-c <b>] [<dispositivo>]
.sp
Cria um sistema de arquivos DOS.
Para detalhes sobre as opções do comando, use "mkfs -H".
.sp
Se não for dado o nome do <dispositivo>, será usado o corrente.

.bc	/*************************************/
.sp
Repare que com a criação de sistemas de arquivos com parâmetros não
convencionais há o risco de desentendimentos (fatais)
com o sistema operacional MS-DOS já que ele (aparentemente)
não examina o superbloco dos sistemas de arquivos.
.sp
Embora que o programa "dosmp" possa ser usado para criar sistemas de
arquivos nos discos rígidos, isto deve ser feito com muito cuidado,
pela mesma razão do parágrafo anterior.
.ec	/*************************************/

.ip "47." 4
mkv <nome_do_volume>
.sp
Cria um nome de volume de sistema de arquivos DOS. Repare que o
<nome_do_volume> deve vir normalmente na raiz do sistema de arquivos.

.ip "48." 4
mount [-01] [<dispositivo>]
.sp
Monta o <dispositivo> com um sistema de arquivos DOS.
Para detalhes sobre as opções do comando, use "mount -H".
.sp
Se o nome do <dispositivo> não for dado, será usado o dispositivo corrente.
Neste caso, o meio (por exemplo, um disquete) pode ser trocado,
para ser montado um novo sistema de arquivos.

.ip "49." 4
mv [-ifv] <arquivo1> <arquivo2>
.br
mv [-ifv] <arquivo1> [<arquivo2> ...] <diretório>
.sp
Move (troca o nome) de arquivos (regulares ou volumes) DOS.
Para detalhes sobre as opções do comando, use "mv -H".
.sp
Em sua primeira forma, o comando "mv" move
(troca o nome de) <arquivo1> para <arquivo2>.
Na segunda forma, um ou mais <arquivo>s são movidos para o
<diretório> especificado, com seus nomes originais.

.ip "50." 4
mvtree <diretório1> <diretório2>
.sp
Move (troca o nome) de diretórios DOS.
Se o <diretório2> ainda não existe, o novo nome de <diretório1>
será <diretório2>. Em caso contrário, será <diretório2>/<nome1>, onde
<nome1> é o último componente do caminho de <diretório1>.

.ip "51." 4
param
.sp
Imprime os parâmetros do sistema de arquivos DOS montado.

.ip "52." 4
path
.sp
Como "pwd", abaixo.

.(t
.ip "53." 4
put [-ifv] <arquivo> ...
.sp
Copia arquivos TROPIX para o diretório corrente do sistema DOS.
Para detalhes sobre as opções "-ifv", use "get -H".
.)t
.sp
Se algum nome de <arquivo> for um caminho com vários componentes,
o nome completo será usado apenas para referenciar o arquivo TROPIX.
Os conteúdos dos arquivos DOS são convertidos,
conforme o modo de representação de dados.

.ip "54." 4
puttree [-ifv.s] [-p "<p>"] [-P "<P>"] <árvore>
.sp
Copia árvores TROPIX para o sistema DOS.
Para maiores detalhes sobre as opções "-ifv.spP" consulte "cptree" (cmd).
.sp
O nome completo da <árvore> é usado tanto para referenciar o
sistema TROPIX como o DOS.
Os conteúdos dos arquivos DOS são convertidos,
conforme o modo de representação de dados.

.ip "55." 4
pwd
.sp
Imprime o diretório corrente DOS.

.ip "56." 4
quit (ou simplesmente "q")
.sp
Encerra o programa. Um EOF (normalmente <^D>) tem o mesmo efeito.

.ip "57." 4
rd <diretório> ...
.sp
Como "rmdir", abaixo.

.ip "58." 4
ren [-ifv] <arquivo1> <arquivo2>
.br
ren [-ifv] <arquivo1> [<arquivo2> ...] <diretório>
.sp
Como "mv", acima.

.ip "59." 4
rm [-ifv] <arquivo> ...
.sp
Remove arquivos DOS (regulares ou volumes).
Para detalhes sobre as opções "-ifv", use "rm -H".

.(t
.ip "60." 4
rmd <diretório> ...
.sp
Como "rmdir", abaixo.
.)t

.ip "61." 4
rmdir <diretório> ...
.sp
Remove diretórios DOS.

.(t
.ip "62." 4
rmtree [-fv] <árvore>
.sp
Remove uma árvore DOS.
Para detalhes sobre as opções "-fv", use "rmtree -H".
.)t

.ip "63." 4
rmv [-ifv] <arquivo> ...
.sp
Como "rm", acima.

.ip "64." 4
ro
.sp
Permite apenas leituras no sistema de arquivos DOS.

.ip "65." 4
rv [-ifv] <arquivo> ...
.sp
Como "rm", acima.

.(t
.ip "66." 4
rw
.sp
Permite leituras e escritas no sistema de arquivos DOS.
Este comando não é aceito se foi dada a opção "-r" para "dosmp".
.)t

.(t
.ip "67." 4
set
.sp
Como "status", abaixo.
.)t

.ip "68." 4
seta
.sp
Altera o modo de representação de dados para ISO (ASCII).

.ip "69." 4
setb
.sp
Altera o modo de representação de dados para BINÁRIO.

.(t
.ip "70." 4
show <arquivo>
.sp
Mostra um arquivo na tela do terminal.
Este comando copia o arquivo DOS para uma arquivo temporário,
e mostra-o através de "show" (cmd).
.)t
.sp
O conteúdo do arquivo DOS é convertido,
conforme o modo de representação de dados.

.(t
.ip "71." 4
stat
.sp
Como "status", abaixo.
.)t

.ip "72." 4
status
.sp
Informa o estado do sistema de arquivos e o modo de representação
de dados corrente.

.ip "73." 4
sys [-v] [-d <dir>]
.sp
Grava os arquivos "IO.SYS", "DRVSPACE.BIN", "MSDOS.SYS" e "COMMAND.COM"
na raiz do sistema de arquivos DOS para torná-lo carregável.

.sp
Estes arquivos são procurados no diretório <dir> (normalmente ".").

.(t
.ip "74." 4
tdat <arquivo> ...
.sp
Como "touch", abaixo.
.)t

.ip "75." 4
touch <arquivo> ...
.sp
Atribui a data atual a cada <arquivo> DOS dado.

.ip "76." 4
tree [-l] [<árvore> ...]
.sp
Como "walk", abaixo.

.ip "77." 4
type <arquivo>
.sp
Como "cat", acima.

.(t
.ip "78." 4
umount
.sp
O sistema de arquivos é desmontado, e "dosmp" aguarda que outro
seja montado (através de "mount", acima) ou que seja criado um (através
de "mkfs", acima).
.)t

.(t
.ip "79." 4
walk [-l] [<árvore> ...]
.sp
Percorre árvores DOS listando os nomes dos arquivos.
Se não for dada nenhuma <árvore>, é percorrido o diretório corrente.
.)t
.sp
Normalmente, é dado o tipo do arquivo, seu tamanho e nome.
Se for dada a opção "-l" são dados também o número do primeiro
"cluster" do arquivo e outras informações.

.sp
Se o ambiente contiver a variável MODECOLOR, os diversos tipos
de arquivos serão impressos com cores distintas (veja "modecolor" (libc)).

.ip "80." 4
waste
.sp
Analisa o sistema de arquivos e imprime o espaço total (em KB)
desperdiçados nos finais dos "cluster"s alocados aos arquivos.

.ip "81." 4
xd <arquivo>
.sp
"Dump" em hexadecimal e ISO do <arquivo>,
que pode ser regular ou um diretório.
.sp
Se o <arquivo> for regular, o seu conteúdo é convertido,
conforme o modo de representação de dados.

.ip "82." 4
xh [<n>]
.sp
Excuta o comando de número <n> do histórico.
Se <n> for omitido, executa o último comando presente no histórico.

.ep
.bc	/*************************************/
.in -3
.ec	/*************************************/

.in
.sp
.b OBSERVAÇÃO
.in 5
Como o sistema de arquivos DOS não tem o conceito de elo físico nem simbólico,
os comandos de cópia não testam a existência de elos no sistema TROPIX.
Isto significa (por exemplo) que se durante uma operaçâo de cópia
para o DOS, dois nomes de arquivos se referirem a um mesmo arquivo TROPIX,
ele será copiado duas vezes, uma vez com cada nome.

.in
.sp
.b AGRACECIMENTO
.in 5
O autor agradece a Norival Ribeiro Figueira cujo programa "pldos" (cmd)
foi meticulosamente estudado, e serviu de inspiração para o "dosmp".

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc): "
edit, modecolor
.br
.wo "(cmd):  "
pldos, chmod, cptree, cmptree, diff, show
.br
.wo "(fmt):  "
ambiente, fstab
.br
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

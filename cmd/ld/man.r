.bp 1
.he 'LD (cmd)'TROPIX: Manual de Referência'LD (cmd)'
.fo 'Atualizado em 12.07.02'Versão 4.3.0'Pag. %'

.b NOME
.in 5
ld - "link-editor" de módulos objeto

.in
.sp
.b SINTAXE
.in 5
ld [-rbsmnvu.] [-o <obj>] [-e <ent>] [-T <tb>] [-D <db>]
.in 45
<mod> ... [-l<x> ...]

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "ld" reúne diversos módulos objeto em apenas um,
efetuando relocações e resolvendo símbolos externos.

.sp
Dependendo das opções, "ld" efetua uma das 4 seguintes funções:

.in +3
.ip "1. " 3
Gera um módulo executável regular:
Neste caso, são dados vários módulos objeto <mod> ..., que
são reunidos, gerando um módulo objeto executável.
Isto é obtido se NÃO for dada nenhuma das opões "-n", "-r" nem "-b".

.ip "2. " 3
Gera um módulo executável reentrante:
Como acima, mas gerando módulo objeto executável reentrante,
isto é, durante a execução, a secão TEXT e a seções DATA+BSS ficarão em
regiões diferentes e o texto será protegido contra escritas. 
Isto é obtido com a opção "-n".

.ip "3. " 3
Gera um módulo objeto NÃO executável, que pode ser usado como futuras
entradas para "ld".
Isto é obtido com a opção "-r".

.ip "4. " 3
Gera uma biblioteca compartilhada.
Isto é obtido com a opção "-b".

.ep
.in -3

.bc	/*************************************/
  ld [-rbsmnvu.] [-o <obj>] [-e <ent>] [-T <tb>] [-D <db>] <m> ... [-l<x> ...]

Opções:	-r: Mantem a relocação, não define COMMON
	-b: Gera uma biblioteca compartilhada
	-s: Gera o módulo objeto sem tabela de símbolos
	-m: Gera um mapa de link-edição
	-n: Gera o módulo objeto reentrante
	-v: Verboso
	-u: Lista os símbolos não referenciados
	    Se for dado duas vezes, lista também os das bibliotecas
	-.: Usa os arquivos temporários no diretório corrente
	-o: O nome do módulo gerado será <obj> ao invés de "a.out"
	-e: O nome do símbolo de entrada será <ent> ao invés de "start"
	-T: O endereço do início da seção "text" será <tb>
	-D: O endereço do início da seção "data" será <db>

Obs.:	<m> ... são módulos objeto
	<x> ... são abreviaturas para nomes de bibliotecas
.ec	/*************************************/

.bc	/*************************************/
.sp
No caso mais simples, são dados vários módulos objeto <mod> ..., que
são reunidos, produzindo um módulo objeto que pode ser executado, ou
se foi dada a opção "-r", ser usado como a entrada de uma execução
subseqüente de "ld".
.ec	/*************************************/

.sp
Normalmente, a saída de "ld" é posta no arquivo "a.out", que é um
módulo objeto executável, se NÃO ocorreu nenhum erro durante a execução
de "ld" e NÃO foram dadas as opões "-r" nem "-b".
(veja as opções "-o", "-r", "-b").

.sp
Cada <mod> dado pode ser o nome de um módulo objeto
(veja o formato em  "a.out" (fmt)), o nome de uma biblioteca convencional
de módulos objeto (veja "mklib" (cmd) e "lib" (fmt)) ou ainda o nome de
uma biblioteca compartilhada.
Das bibliotecas são somente carregados os módulos necessários para
definir referências externas ainda não resolvidas.

.sp
Os nomes das bibliotecas podem ser dados na sua forma completa
(por ex. "/lib/lib<x>"), ou na sua forma abreviada -l<x>.
O nome -l<x> representa uma das bibliotecas "/lib/lib<x>", "/usr/lib/lib<x>"
ou "/usr/xwin/lib/lib<x>", pesquisadas nesta ordem.
Como exemplo, a biblioteca padrão de funções da linguagem "C" ("libc")
encontra-se no diretório "/lib/libc" e pode ser representada abreviadamente
por "-lc", na chamada de "ld".

.sp
As bibliotecas compartilhadas seguem as mesmas convenções de nomes, por
exemplo "-lt.o" representa "/lib/libt.o".

.sp
Normalmente, o ponto de entrada do módulo objeto final
(ou seja, o endereço inicial de execução do programa)
é o símbolo "start" (que deve ser global e pertencer à seção TEXT)
ou se este não existir, o endereço de início da seção TEXT
(veja a opção "-e").

.sp
As opções do comando são:

.in +3
.bc	/*************************************/
.ip -a
Normalmente, a tabela de símbolos do módulo objeto final contém apenas
os símbolos globais. Com esta opção, são conservados todos os símbolos.
.ec	/*************************************/

.ip -r
Mantém a relocação no módulo objeto final e não define a área de COMMON.
O comando não emite diagnósticos sobre símbolos ainda não definidos,
e o módulo objeto de saída não é executável. 

.sp
O objetivo da opção é o de realizar uma "link-edição" parcial de alguns módulos,
gerando um módulo objeto intermediário que pode ser usado como
a entrada de uma execução subseqüente de "ld".

.ip -b
Gera uma biblioteca compartilhada.
Esta função é semelhante à opção "-r", mas define a área de COMMON.

.ip -s
Gera o módulo objeto final sem a tabela de símbolos.
Somente permitido para módulos objeto executáveis.
.bc	/*************************************/
Gera o módulo objeto final sem a tabela de símbolos nem as
informações de depuração.
.ec	/*************************************/

.ip -m
Gera um mapa de "link-edição", contendo os endereços e tamanhos das
três seções (TEXT, DATA e BSS) de cada um dos módulos carregados.

.ip -n
Gera um módulo objeto executável reentrante.

.ip -v
Com esta opção, são dadas informações sobre os módulos carregados.

.ip -u
Lista os símbolos não referenciados.
Se for dado apenas uma vez, serão listados os símbolos não
referenciados apenas dos módulos dados explicitamente.
Se for dado duas vezes (na forma "-uu") serão listados
todos os símbolos não referenciados, incluindo os das bibliotecas.

.ip -.
Aloca os arquivos temporários no diretório corrente.
Isto pode ser útil se o tamanho do dispositivo de "/tmp" for pequeno.

.ip -o
Normalmente, o nome do módulo objeto final é "a.out".
Com esta opção, o nome do módulo passa a ser <obj>.

.ip -e
O ponto de entrada do módulo objeto final será o símbolo <ent>,
ao invés do símbolo "start", ou o endereço de início da seção TEXT.
O símbolo <ent> deve ser global e pertencer à seção TEXT.

.ip -T
O endereço inicial de carga da seção TEXT do módulo-objeto final será <tb>.
Se esta opção for omitida, este endereço será 4 MB.
Se o endereço dado começar por "0x" ou "0X" ele será considerado dado em
hexadecimal; se começar por "0", em octal;
em caso contrário em decimal.

.ip -D
O endereço inicial de carga da seção DATA do módulo-objeto final será <db>.
Se esta opção for omitida, este endereço será contíguo ao fim da seção TEXT,
no caso de programas normais (0407) ou 512 MB
no caso de programas reentrantes (0410).
Se o endereço dado começar por "0x" ou "0X" ele será considerado dado em
hexadecimal; se começar por "0", em octal;
em caso contrário em decimal.

.in +3
.ep

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mklib, mkshlib
.bc	/*************************************/
mklib, wsdb
.ec	/*************************************/
.br
.wo "(fmt): "
a.out, lib
.br

.in
.sp
.(t
.b ARQUIVOS
.in 5
 /usr/include/a.out.h
.bc	/*************************************/
 /usr/include/lib.h
.ec	/*************************************/
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

.bp
.he 'SIZE (cmd)'TROPIX: Manual de Referência'SIZE (cmd)'
.fo 'Atualizado em 11.08.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
.wo "size -"
imprime tamanhos e características de módulos objeto
.br

.in
.sp
.b SINTAXE
.in 5
.(l
size [-dcxaN] [<módulo> ...]
.)l

.in
.sp
.b DESCRICAO
.in 5
O comando "size" imprime uma linha para cada <módulo> objeto dado,
contendo os tamanhos das suas várias seções,
além de algumas outras informações.

.sp 
As informações dadas são as seguintes:

.in +3
.ip MACH 8
Modelo do computador de origem do <módulo> objeto
(Por exemplo, "486", indicando um módulo objeto nativo para
INTEL 386/486/PENTIUM).

.ip TIPO 8
Tipo do <módulo> objeto: "." para normal, "n" para reentrante e
"s" para biblioteca compartilhada.
Para módulos reentrantes, são indicados ainda, com "t" ou "m"
os indicadores S_ISVTX e S_IMETX do seu estado (ver "chmod" (sys)).

.ip TS 8
Presença ("*") ou ausência (".") da tabela de símbolos.

.ip RL 8
Presença ("*") ou ausência (".") da informação de relocação.

.ip VER 8
Versão das bibliotecas compartilhadas referenciadas por este
<módulo> objeto.

.ip FLAG 8
Indicadores (atualmente sem uso).

.ip TEXTO 8
Tamanho ou endereço inicial do texto (veja a opção "-s").

.ip DATA 8
Tamanho ou endereço inicial da área de dados inicializados (veja a opção "-s").

.ip BSS 8
Tamanho da área de dados não inicializados.

.ip COMMON 8
Soma dos tamanhos das variáveis "comuns" a outros módulos objeto.
Estas variáveis serão apenas alocadas em tempo de "link-edição"
(ver "ld" (cmd)), e ficarão superpostas
a variáveis homômimas de outros módulos objeto.
Este valor só é impresso com a opção "-c" (ver adiante).

.ip REFTB 8
Tamanho da tabela de referencias externas. Esta tabela é utilizada
na execução de módulos objetos que referenciam bibliotecas compartilhadas.
Este valor só é impresso com a opção "-c" (ver adiante).

.ip TOTAL 8
Soma das seções TEXTO + DATA + BSS. Repare que NÃO inclui COMMON.

.ip -DATA- 8
Data de geração do <módulo>. Esta é a data contida no cabeçalho
do módulo; não confundir com alguma das datas do arquivo.
Ela só é impressa com a opção "-d" (ver adiante).

.ip NOME 8
Nome do módulo.

.ep
.in -3
.sp
.(t
As opções do comando são:

.in +3
.ip -s
Imprime o endereço inicial (em hexadecimal) das seções TEXT e DATA ao invés dos tamanhos.
.)t

.ip -d
Imprime também a data de geração do <módulo> objeto.
.)t

.ip -c
Imprime também os tamanhos das seções COMMON e REFTB.

.ip -x
Os tamanhos são dados em hexadecimal ao invés de decimal.

.ip -a
Emite mensagens de erro para arquivos que não são módulos objeto
(ao invés de simplesmente ignorá-los).

.ip -N
Lê os nomes dos <módulos> de "stdin"

.ep
.in -3
.sp
Se não for dado nenhum <módulo>, nem a opção "-N", serão
dadas informações sobre "a.out".

.in
.sp
.b OBSERVAÇÕES
.in 5
.bc	/*************************************/
Como o tamanho real da área de dados não inicializados (BSS) só é conhecido
após a "link-edição", o comando "size" pode dar tamanhos errôneos para módulos
não executáveis. Isto ocorre em virtude das variáveis em "common".
.sp
.ec	/*************************************/

Para obter informações sobre módulos contidos em bibliotecas, podemos
usar a expansão proporcionada pelo "sh" (cmd), como por exemplo:
.sp
.nf
		size /lib/libc/id/*.o
.fi
.sp
ou então usar o comando "walk" (cmd) para obter o nome
de todos os módulos de uma biblioteca, como por exemplo:
.sp
.nf
		walk -p "*.o" /lib/libc | size -N
.fi

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
chmod
.br
.wo "(cmd): "
sh, walk, ld
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

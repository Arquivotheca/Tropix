.bp
.sp 5
.nf
     *********************************************************
     *                                                       *
     *                      CAPÍTULO 1                       *
     *                                                       *
     *                CONCEITOS FUNDAMENTAIS                 *
     *                                                       *
     *********************************************************
.fi
.sp 4

.pp
.b
Editores de texto
.r
são programas interativos para criação 
e modificação de
.b
textos.
.r
Textos são geralmente documentos como este ou código fonte de programas
e são armazenados em memória secundária no Sistema de Arquivos.
Tipicamente, a edição de um texto consiste em copiá-lo
para uma área intermediária de memória,
modificá-lo por meio de uma série de comandos
dados pelo usuário em um terminal
e, finalmente, copiá-lo de volta para o Sistema de Arquivos,
a partir do que o texto modificado torna-se efetivo.

.pp
O utilitário "vi" é um editor 
.b
visual
.r
de textos,
isto significa que o "vi" transforma o vídeo do terminal
em uma 
.b
janela
.r
sobre o texto em edição
e o cursor do terminal em um apontador no texto.
Existem comandos que movem a janela
e comandos que movem o cursor em unidades de linhas, palavras ou caracteres.
Assim, qualquer parte do texto pode ser alcançada e observada
e qualquer caractere do texto pode ser apontado.
De modo geral, os comandos de edição são relativos à posição corrente
(linha e caractere apontados pelo cursor)
e as alterações no texto são acompanhadas instantaneamente pelo usuário
através da janela sobre o texto.

.pp
O editor "vi" tem dois modos principais de funcionamento:
o modo 
.b Visual
.r
e o modo 
.b
Comandos.
.r
O modo Visual é o modo principal e inicial do editor "vi".
No modo Visual podemos passear pelo texto
e realizar alterações localizadas pelo cursor.
O modo Comandos é usado para  escrita e leitura no Sistema de Arquivos,
para interação com o Interpretador de Comandos ("sh" (cmd))
e para a realização de operações de edição orientadas a linhas.

.pp
Alguns caracteres especiais são importantes
para a utilização do editor "vi",
por isso é importante localizá-los no teclado do terminal.
O caractere ISO <esc> deve estar próximo ao canto superior esquerdo,
provavelmente denominado ESC ou ALT.
Os caracteres <cr> e <nl> devem estar localizados
próximos e no lado direito; eles são normalmente indicados por
CR ou ENTER e LF ou NL respectivamente.
Os caracteres <bs> e <del>
podem ser encontrados sob os nomes BS e DEL.
Os caracteres ISO de controle são gerados
teclando-se uma letra enquanto a tecla CTRL é pressionada,
e serão representados neste manual na forma <^X>,
onde "X" é a letra teclada.

.pp
Os caracteres de controle da comunicação com terminais
estabelecidos pelo comando "stty"
são reconhecidos pelo editor "vi"
em determinados contextos,
normalmente conservando as suas funções originais.
Os caracteres de controle referenciados neste manual são os seguintes:
<erase>, <aerase>, <word>, <kill>, <retype> e <eol>.

.pp
Nos próximos capítulos serão descritos todos os comandos do editor "vi".
Quando oportuno, será indicado junto aos comandos
e entre parênteses os verbos ou nomes em inglês
que motivaram a escolha de determinados caracteres para representá-los.

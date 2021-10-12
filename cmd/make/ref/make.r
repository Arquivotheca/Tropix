.bp
.tp
.sp 10
.nf
      *******   *******  *******   *******    *    *     *
         *      *     *  *     *   *     *    *     *   *
         *      *     *  *     *   *     *    *      * *
         *      *******  *     *   *******    *       *
         *      *  *     *     *   *          *      * *
         *      *   *    *     *   *          *     *   *
         *      *    *   *******   *          *    *     *
.fi
.sp 10
.ce
MAKE - Gerenciador de Manutenção de Programas
.sp 2
.ce
Versão 3.2.3
.sp 4
.ce
Guia
.sp 11
.ce
Dezembro de 1999
.bp
.sp 10
.ce
CONTEÚDO
.sp 5
.nf
	Cap. 1 - Introdução ...........................	 2

	Cap. 2 - Definições Básicas ...................  3

	Cap. 3 - Formato do Arquivo de Descrição ......  5

	Cap. 4 - Macros ...............................	 7

	Cap. 5 - Uso do Comando ....................... 10
.fi
.bp
.he 'Sistema TROPIX'Guia'MAKE'
.fo 'Escrito em 13.12.99'Versão 3.2.3'Pag. %'
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 1			     *
     *							     *
     *	     		INTRODUÇÃO			     *
     *							     *
     *********************************************************
.fi
.sp 4
.pp
Normalmente, um programa grande e complexo é dividido em módulos
de tamanhos menores, mais fáceis de manipular.
Estes módulos em geral recebem tratamentos diversos:
alguns são programas fonte em "C"
e necessitam ser processados pelo compilador "C",
outros são programas fonte em "assembly"
e necessitam ser processados pelo montador, etc...
Os módulos objetos gerados por este tradutores
são em seguida geralmente "link-editados" com
certas bibliotecas, e o módulo final instalado em algum diretório.

.pp
Infelizmente, é muito fácil para um programador esquecer
que arquivos dependem de outros, que arquivos
foram modificados recentemente e a seqüência exata de operações
necessárias para construir uma nova versão do programa.
Após a edição de vários arquivos, é natural que se perca a visão
geral dos arquivos modificados e dos módulos
objetos ainda válidos, já que a alteração de um arquivo
de declarações ("*.h") pode tornar obsoletos dezenas de módulos
objetos.

.pp
Esquecer de recompilar um módulo que foi modificado ou que
inclui um arquivo de declarações modificado resultará em um programa
que não executa corretamente.
Por outro lado, recompilar sistematicamente todos os módulos a cada
alteração desperdiça tempo.

.pp
O programa "make", objeto deste manual, automatiza muitas das atividades
de desenvolvimento e manutenção de programas. Se as informações
de interdependência dos arquivos e a seqüência de operações
são armazenadas em um arquivo (chamado de "arquivo de 
descrição"), o simples comando
.sp
.nf
		make
.fi
.sp
é suficiente para atualizar os arquivos necessários,
independentemente do número de arquivos editados desde o último
"make". Na maioria dos casos, o arquivo de descrição é simples,
e não necessita de mudanças freqüentes.

.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 2			     *
     *							     *
     *	     	    DEFINIÇÕES BÁSICAS			     *
     *							     *
     *********************************************************
.fi
.sp 4
.pp
A operação básica do "make" é a atualização de um módulo, que
consiste em:
.in 2
.ip 1. 3
providenciar para que todos os arquivos dos quais
o módulo dependa existam e estejam atualizados;
.ip 2. 3
criar o módulo (se ele não existe), ou atualizá-lo (caso não esteja atualizado)
em relação às suas dependências, executando uma seqüência de operações
especificada.
.in 0
.lp
Vamos dar um exemplo: 
um programa "pgm" é criado a partir da 
compilação e "link-edição" de três módulos escritos em "C": 
"md1.c", "md2.c" e "md3.c". Os módulos "md1.c" e "md2.c" usam declarações 
definidas em "md0.h" (isto é, contêm a linha "#include md0.h").
.pp
As interdependências e operações necessárias
para a criação do programa podem ser resumidas no seguinte texto:
.sp
.nf
	pgm:	md1.o md2.o md3.o
		cc -n -o pgm md1.o md2.o md3.o

	md1.o:	md0.h md1.c
		cc -O -c md1.c

	md2.o:	md0.h md2.c
		cc -O -c md2.c

	md3.o:	md3.c
		cc -O -c md3.c
.fi
.pp
Se este texto estiver em um arquivo com o nome de "Makefile",
o comando
.sp
.nf
	make
.fi
.sp
irá executar as operações estritamente necessárias para criar/recriar
o programa "pgm" após qualquer mudança realizada em alguns dos
arquivos "md1.c", "md2.c", "md3.c" ou "md0.h". 
.pp
Suponhamos que este programa ("pgm") seja novo, isto é, os seus componentes
nunca foram compilados. Neste caso, o "make" fará executar 
os seguintes comandos:
.sp
.nf
		=> cc -c md1.c
		=> cc -c md2.c
		=> cc -c md3.c
		=> cc -n -o pgm md1.o md2.o md3.o
.fi
.sp
O "make" lista cada comando antes de executá-lo, precedendo-o do sinal "=>".
.pp
Suponhamos agora que, ao executarmos o programa "pgm", constatamos a
necessidade de uma alteração no módulo "md2.c".
Se, após a edição de "md2.c", invocarmos novamente o "make", serão
executados apenas os seguinte comandos:
.sp
.nf
		=> cc -c md2.c
		=> cc -n -o pgm md1.o md2.o md3.o
.fi
.sp
que, como podemos ver, são exatamente os necessários.

.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 3			     *
     *							     *
     *	       FORMATO DO ARQUIVO DE DESCRIÇÃO		     *
     *							     *
     *********************************************************
.fi
.sp 4

.pp
Normalmente, o comando "make" procura o arquivo de descrição no diretório
em que é invocado com os nomes "Makefile" ou "makefile", nesta ordem.
Caso seja chamado com a opção "-f" (ver "make" (cmd)),
outros nomes podem ser utilizados.

.pp
O arquivo de descrição é constituído de quatro tipos de entradas:
.in 2
.ip 1. 3
entradas para especificação de dependências;
.ip 2. 3
entradas para declaração de alvos virtuais;
.ip 3. 3
entradas para declaração de macros;
.ip 4. 3
entradas para inclusão de macros como variáveis do ambiente.
.in 0

.pp
As entradas para especificação de dependências podem estender-se por diversas
linhas consecutivas do arquivo de descrição, possuindo o seguinte
formato geral:

.sp
.nf
	<alvos>:	<pré-requisitos>
			<operação 1>
			<operação 2>
			   .   .
			   .   .
			<operação n>
.fi

.sp
onde <alvos> são os nomes do módulos a serem criados/atualizados e
<pré-requisitos> são nomes de módulos dos quais os alvos dependem.
Os nomes podem ser separados por brancos (<sp>) ou tabuladores (<ht>).

.pp
As linhas <operação 1> a <operação n> contêm os comandos necessários à
criação/atualização dos alvos e devem iniciar com um tabulador (<ht>).

.pp
O conjunto de operações é executado, visando a criação/atualização dos
alvos, quando:

.in 2
.ip 1. 3
são especificados pré-requisitos e o tempo de última modificação de um
deles for mais recente que o tempo de última modificação de um dos alvos;
.ip 2. 3
não são especificados pré-requisitos;
.ip 3. 3
é especificada a palavra reservada ".exist" no lugar de pré-requisitos
e um dos alvos é inexistente.
.in 0

.pp
As operações são listadas na tela imediatamente antes de
serem executadas; havendo erros, o processamento do "make" é interrompido.
Podemos utilizar as variantes <@operação>, para que ela não seja listada,
ou <-operação>, para que o "make" continue o processamento
mesmo em caso de erro nesta operação.

.pp
Normalmente, o conjunto de operações especificadas para atualizar um
alvo deve produzir alteração do tempo de última modificação do alvo,
tornando-o mais recente que todos os seus pré-requisitos, como 
é o caso de compilações e link-edições.
Do contrário, a cada chamada do "make", este conjunto será
re-executado.

.pp
Quando uma linha termina pelo caractere "\", a linha seguinte é considerada
uma continuação. Isto permite estender as listas de alvos e pré-requisitos
ou mesmo uma linha de operação.

.pp
O arquivo de descrição pode conter comentários,
que são iniciados pelo caractere "#" e se estendem até o fim da linha
em que iniciam.
Linhas vazias são ignoradas.

.pp
Normalmente, alvos correspondem a arquivos, que devem existir fisicamente.
Alvos virtuais são declarados com entradas do tipo
.sp
.ce
".virtual <alvo> ..."
.pp
Tais alvos não correspondem a arquivos, funcionando apenas como índices
para seqüências de comandos. Um exemplo comum de aplicação é declarar
um alvo cujo efeito é copiar o programa-objeto final para o diretório
apropriado. 
.pp
Utilizando o exemplo anterior, vamos introduzir o alvo virtual "cp",
que possui como pre-requisito "pgm".
A operação utilizada para atualizar o alvo "cp" consiste em copiar "pgm"
para o diretório "/usr/bin".
.sp
.nf
	.virtual cp

	pgm:	md1.o md2.o md3.o
		cc -n -o pgm md1.o md2.o md3.o

	md1.o:	md0.h md1.c
		cc -O -c md1.c

	md2.o:	md0.h md2.c
		cc -O -c md2.c

	md3.o:	md3.c
		cc -O -c md3.c

	cp:	pgm
		cp -f pgm /usr/bin
.fi
.pp
O comando
.sp
.nf
	make cp
.fi
.sp
compila e linkedita, se necessário, o programa "pgm", copiando-o para
o diretório "/usr/bin".

.pp
As entradas para declaração de macros e inclusão no ambiente serão analisadas
no próximo capítulo.

.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 4			     *
     *							     *
     *	       		  MACROS			     *
     *							     *
     *********************************************************
.fi
.sp 4
.pp
O "make" provê um mecanismo simples de substituição de macros no
arquivo de descrição.

.pp
Uma entrada no arquivo de descrição para declaração de macro tem a forma:
.sp
.nf
	<macro> = <texto>
.fi
.sp
onde <macro> é um identificador (seqüência de letras, dígitos e "_" iniciada
por letra) e <texto> é uma seqüência qualquer de caracteres.
O sinal de "=" pode ser precedido e sucedido de separadores (<sp> ou <ht>),
que são ignorados. O <texto> se encerra com o fim da linha de declaração,
ou prossegue em linhas de continuação com o uso do caractere "\".

.pp
São exemplos de declarações de macros:
.sp
.nf
	PGM	= pgm
	OBJ	= md1.o	\
		  md2.o \
		  md3.o
	FLAGS	= -n
.fi

.pp
A referência a uma macro consiste em preceder seu nome do caractere "$",
como "$PGM", "$OBJ" e "$FLAGS". Este mecanismo provoca a inserção do
texto da macro no lugar onde foi referenciada.

.pp
Às vezes, é conveniente, na referência a uma macro, delimitar seu nome por
parênteses: $(PGM), $(OBJ) e $(FLAGS).

.pp
Com as definições acima, o arquivo de descrição do capítulo 2
pode ser reescrito da seguinte maneira:
.sp
.nf
	PGM	= pgm
	OBJ	= md1.o md2.o md3.o
	INC	= md0.h
	CC	= cc
	CFLAGS	= -O
	LDFLAGS	= -n

	$PGM:	$OBJ
		$CC $LDFLAGS -o $PGM $OBJ

	md1.o:	md1.c $INC
		$CC $CFLAGS -c md1.c

	md2.o:	md2.c $INC
	 	$CC $CFLAGS -c md2.c

	md3.o:	md3.c
		$CC $CFLAGS -c md3.c
.fi

.pp
Com a utilização de macros, evitamos a repetição dos nomes dos
módulos, o que pode abreviar substancialmente o tamanho do arquivo
de descrição, bem como facilitar-lhe a manutenção.

.pp
Existem algumas macros especiais, para as quais o próprio "make"
atribui valores intrinsecamente.
Seus nomes são: "$", "@", "*", "<" e "?", e correspondem aos seguintes
textos:

.in 1
.ip $$ 4
o caractere "$";
.ip $@ 4
nome do alvo a criar/atualizar;
.ip $* 4
nome do alvo a criar/atualizar sem a extensão;
.ip $< 4
nomes dos pré-requisitos dos quais dependem o alvo;
.ip $? 4
nomes dos pré-requisitos mais recentes que o alvo e, portanto, causadores
de sua atualização/criação.
.in 0
.lp
Os valores das macros @, *, < e ? são atualizados de acordo com a lista de
dependências que estiver sendo processada durante a execução. Estas
macros só têm utilidade em linhas de comandos.

.sp
 Exemplo:

.sp
.nf
	pgm:	md1.o md2.o md3.o
		cc -n -o pgm $<

	md1.o:	md0.h md1.c
		cc -c $*.c

	md2.o:	md0.h md2.c
		cc -c md2.c

	md3.o:	md3.c
		cc -c md3.c

	print:	md0.h md1.c md2.c md3.c
		pr $?
		touch print
.fi

.pp
Repare o módulo "print": a idéia é imprimir os programas fontes que tenham
sido alterados desde a última impressão. 

.pp
Além das macros definidas no arquivo de descrição, todas as cadeias do
ambiente (ver "environ" (fmt)) são automaticamente disponíveis como macros.

.pp
O terceiro tipo de entrada possível em um arquivo de descrição destina-se
a inserir como variáveis do ambiente macros declaradas no arquivo de
descrição ou transmitidas ao "make" através da opção "-D" (ver "make" (cmd)).
Com linhas do tipo

.sp
.ce
".putenv <macro> ..."
.sp

colocamos macros selecionadas como variáveis no ambiente,
para que os programas executados
possam obter os seus valores (por exemplo, o "cpp" (cmd)).

.bp
.sp 5
.nf
     *********************************************************
     *							     *
     *			CAPÍTULO 5			     *
     *							     *
     * 		      USO DO COMANDO			     *
     *							     *
     *********************************************************
.fi
.sp 4
.pp
A sintaxe do comando "make" é
.sp
.nf
	make [-intvab] [-f <desc>] [-D <macro>=<valor>] ...
	<módulo> ...
.fi

.pp
A opção "-i" funciona como se todas as linhas de comandos no arquivo
de descrição estivesses precedidas de "-" (ignorar erros).

.pp
A opção "-n" permite
listar as operações necessárias à atualização sem executá-las.

.pp
Cuidado com a opção "-t", pois ela atualiza as datas de modificação
dos arquivos sem na realidade atualizá-los.
Com isto, a memória dos módulos não atualizados é perdida.

.pp
A opção "-v" explica o motivo das atualizações.

.pp
A opção "-a" apenas lista os alvos especificados no arquivo de
descrição, exceto os da forma "*.o". Se for duplicada ("-aa"),
todos os alvos, sem exceção, são listados.

.pp
A opção "-b" provoca a atualização de todos os módulos,
independentemente das datas.

.pp
A opção "-f" permite especificar o nome do arquivo de descrição.

.pp
A opção "-D" define a macro <macro> com o texto <valor>.
Esta definição prevalece sobre definições homônimas
no arquivo de descrição e/ou no ambiente.

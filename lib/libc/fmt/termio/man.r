.bp
.he 'TERMIO (fmt)'TROPIX: Manual de Referência'TERMIO (fmt)'
.fo 'Atualizado em 29.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "termio -"
interface geral de linhas de comunição
.br

.in
.sp
.b DESCRIÇÃO
.in 5
Todas as linhas de comunicação assíncronas
(usadas usualmente para terminais ou impressoras;
neste texto, terminal será usado como sinônimo de linha de comunicação)
usam a mesma interface,
independente dos dispositivos utilizados. Este texto descreve as
características comuns desta interface.

.sp
Um terminal pode ser aberto diretamente por um processo de usuário,
mas em geral o terminal é aberto pelo utilitário especial "init",
e ele se torna a entrada e saída padrão e a saída de erro do processo. 
O primeiro terminal aberto por um processo ainda não associado
a um terminal se torna o "terminal de controle" do grupo de processos
formado pelo processo e seus descendentes
(ver "grupo de terminais" em "intro" (sys)).
Um terminal de controle desempenha um papel importante no tratamento
dos sinais "SIGINT" e "SIGQUIT", como descrito abaixo.

.sp
Um terminal associado a um destes arquivos opera normalmente
no modo "full-duplex". Caracteres podem ser teclados a qualquer
tempo, mesmo quando está havendo saída no terminal, e somente
são perdidos se a área de entrada do sistema encher totalmente,
o que é raro, ou quando foi alcançado o
número máximo de caracteres de entrada
ainda não lidos por nenhum processo.
Atualmente este limite é de 1124 caracteres. Quando este
limite é atingido,
a área é liberada, todos os caracteres são perdidos.

.sp
Normalmente, a entrada de terminais é processada em unidades de
linhas. Uma linha é delimitada por um <nl> ou um caractere
"eof" (normalmente <^D>, ver caracteres de controle).
Isto significa que um processo tentando ler será suspenso
até que uma linha completa seja teclada. Portanto, independente
do número de caracteres pedidos na leitura, no máximo uma
linha será devolvida. No entanto, não é necessário ler
a linha inteira de uma vez; qualquer número de caracteres
pode ser pedido em uma leitura, mesmo apenas um, sem perda
de informação.

.sp
Durante a entrada, normalmente se realiza o processamento
dos caracteres "erase", "aerase" e "kill".
Os caracteres "erase" e "aerase", cujos valores normais
são <del> e <bs> apagam o último caractere teclado,
desde que haja pelo menos um caractere na linha.
O caractere "kill", cujo valor normal é <^U> apaga
a linha inteira.

.sp
Alguns caracteres têm função especial na entrada. As funções
e seus valores normais são dados a seguir:

.in +3
.ip "intr" 8
Este caractere gera um sinal de interrupção ("SIGINT"), 
que é enviado a todos os processos associados ao terminal
de controle. Normalmente, estes processos terminam
ao receber este sinal, mas eles podem utilizar a chamada
ao sistema "signal" (sys) para ignorá-lo ou interceptá-lo.
O valor inicial deste caractere é <^C>. 

.ip "quit" 8
Este caractere gera um sinal de "quit" ("SIGQUIT"), 
que é enviado a todos os processos associados ao terminal
de controle. Normalmente, estes processos terminam com a gravação
da imagem do processo no disco ("core-dump")
ao receber este sinal, mas eles podem utilizar a chamada
ao sistema "signal" (sys) para ignorá-lo ou interceptá-lo.
O valor inicial deste caractere é <^X>. 

.ip "switch" 8
O sistema operacional não trata este caractere;
ele está reservado para as rotinas de gerenciamento de janelas
de terminais.
O valor inicial deste caractere é <nul>.

.ip "erase" 8
Este caractere apaga o caractere anterior; se estivermos
no começo de uma linha, nada faz.
O valor inicial deste caractere é <del> ("^*"). 

.ip "aerase" 8
Este caractere é análogo ao "erase":
apaga o caractere anterior; se estivermos
no começo de uma linha, nada faz.
O valor inicial deste caractere é <bs>.

.ip "kill" 8
Este caractere apaga a linha inteira.
O valor inicial deste caractere é <^U>.

.ip "eof" 8
Este caractere pode ser usado para gerar uma condição de 
fim de arquivo ("EOF") do terminal. Quando recebido, todos os
caracteres da fila de entrada são passados imediatamente para
o programa, sem esperar um <nl>, e o caractere é ignorado.
Se não havia nenhum caractere na fila, zero caracteres são
passados para o programa, o que é a indicação padrão
de EOF. 
O valor inicial deste caractere é <^D>.

.ip "min" 8
Este valor só é utilizado nos modos "raw" e "comm",
e representa o número mínimo de caracteres
necessários na fila de entrada para que eles sejam passados
para o processo. Em outras palavras, o processo espera
até que a fila de entrada contenha "n" caracteres.  
O valor inicial deste parâmetro é 1.

.ip "eol" 8
Este caractere é um delimitador adicional (como <nl>).
Não é utilizado normalmente, e no momento não está implementado.
O valor inicial deste parâmetro é <nul>.

.ip "time" 8
Este valor só é utilizado nos modos "raw" e "comm",
e representa um tempo utilizado para medir o tempo decorrido
para receber "min" caracteres.
No momento não está implementado.
O valor inicial deste parâmetro é 0.

.ip "res" 8
Este caractere no momento não têm função; está reservado
para uso futuro.
O valor inicial deste parâmetro é <nul>.

.ip "stop" 8
A função deste caractere é de suspender temporariamente
a saída no terminal,
para evitar que a saída
desapareça da tela antes que possa ser lida.
O valor deste caractere é <^S>, e não pode ser mudado.

.ip "start" 8
É utilizado para reiniciar a saída que foi suspensa por um
caractere "stop" ou foi alcançado o limite de linhas
de "page".
O valor deste caractere é <^Q>, e não pode ser mudado.

.ip "page" 8
Este valor representa o número de linhas após o qual a saída
no terminal pára, aguardando um "start" (ou um caractere qualquer,
veja "ixany").
Isto é útil em terminais de vídeo para evitar que a saída
desapareça da tela antes que possa ser lida.
Fazendo "n" igual a zero, o controle não é realizado.
O valor inicial deste caractere de controle é 22.

.ip "retype" 8
Este caractere reimprime o a linha no terminal.
É útil quando a linha sendo teclada é misturada com
mensagens de processos executando em "back-ground".
O valor inicial deste caractere é <^R>.

.ip "word" 8
Este caractere apaga a última palavra teclada.
Uma palavra é definida como sendo um conjunto de caracteres
que não contenham <sp> nem <ht>.
O valor inicial deste caractere é <^W>.

.ip "color" 8
Este caractere define as cores de frente e fundo dos terminais
coloridos (para terminais monocromáticos não tem função).
O valor inicial (ou seja, as cores iniciais) variam de
acordo com o terminal.

.ip "nline" 8
Este caractere define o número de linhas do terminal.
O valor inicial é 24.

.ip "ncol" 8
Este caractere define o número de colunas do terminal.
O valor inicial é 80.

.in -3
.ep

.sp
.in 5
Normalmente, os caracteres de entrada são ecoados colocando-os
na fila de saída assim que são recebidos.
Se o um processo produz caracteres mais rapidamente do que
podem ser escritos no terminal, o processo é suspenso quando
a fila de saída excede certo limite. Quando a fila esvaziou
abaixo de certo valor, o processo retoma a sua execução. 

.sp
Diversos comandos da chamada ao sistema "ioctl" (sys) se aplicam
a terminais. Normalmente estes comandos utilizam a estrutura
dada abaixo, definida em <termio.h>:

.(t
.sp
.nf
	#define	NCC	15	/* No. de Carac. de Controle */

	typedef struct termio	TERMIO;

	struct	termio
	{
		ushort	c_iflag;	/* Modos de Entrada */
		ushort	c_oflag;	/* Modos de Saida */
		ushort	c_cflag;	/* Modos de Controle */
		ushort	c_lflag;	/* Modos locais */
		uchar	c_line;		/* Disciplina da Linha */
		uchar	c_cc[NCC];	/* Caracteres de Controle */
		char	c_name[16]; 	/* Nome da Linha */
		uchar	c_wc[4];	/* Controle da janela */
		long	c_res[4];	/* Reser. p/ uso Futuro */
	};
.fi
.)t

.sp 2
Os caracteres de controle estão definidos nos vetores "c_cc" e "c_wc".
A posição relativa e valor inicial de cada caractere são os seguintes:

.sp
.nf
		0	VINTR		<^C>
		1	VQUIT		<^X>
		2	VERASE		<del>
		3	VKILL		<^U>
		4	VEOF		<^D>
		5	VMIN		1
		6	VEOL		<nul>
		7	VTIME		0
		8	VRES		0
		9	SWTCH		0
		10	VPAGE		22
		11	VAERASE		<bs>
		12	VRETYPE		<^R>
		13	VWORD		<^W>
		14	VCOLOR		(variável)

		0	VNLINE		24
		1	VNCOL		80
.fi
.sp 2
O campo "c_iflag" descreve os modos de entrada do terminal:

.sp
.nf
	IGNBRK	0x0001	Ignore a Condicao de BREAK
	BRKINT	0x0002	Sinalize uma interrupcao no caso de BREAK
	IGNPAR	0x0004	Ignorar Caracteres com erro de Paridade
	PARMRK	0x0008	Marcar Erros de Paridade
	INPCK	0x0010	Habilitar Paridade na Entrada
	ISTRIP	0x0020	Mascara o Oitavo Bit
	INLCR	0x0040	Mapeie <nl> em <cr> na entrada
	IGNCR	0x0080	Ignore <cr>s
	ICRNL	0x0100	Mapeie <cr> em <nl>
	IUCLC	0x0200	Mapeie Maiusculas em minusculas
	IXON	0x0400	Habilite XON/XOFF na Saida
	IXANY	0x0800	Qualquer caracter recomeca a saida
	IXOFF	0x1000	Habilite XON/XOFF na Entrada
.fi

.sp
Se IGNBRK está ligado, a condição de "break" (um erro de formato
de caractere com todos os bites em zero) é ignorado; em caso
contrário, se também BRKINT está ligado, a condição de "break"
gera um sinal "SIGINT", e esvazia as filas de entrada
e de saída. Se IGNPAR está ligado, caracteres com outros erros
de formato e de paridade são ignorados.

.sp
Se PARMRK está ligado, um caractere com erro de formato ou
de paridade que não é ignorado, é lido como a seqüencia de
3 caracteres: <del>, "0", "x", onde "x" é o valor do caractere
recebido com erro. Para evitar ambiguidade neste caso,
um caractere válido <del> é lido como <del>, <del>.
Se PARMRK está desligado, um erro de paridade ou de formato
que não é ignorado, é lido como o caractere <nul>.

.sp
Se INPCK está ligado, a verificação de paridade da entrada 
é habilitada; em caso contrário a verificação é desabilitada.
Isto permite a geração de paridade na saída sem verificação
na entrada.

.sp
Se ISTRIP está ligado, os caracteres de entradas têm o seu
oitavo bit (de mais alta ordem) apagado; em caso contrário
os oito bites são processados. 

.sp
Se INCLR está ligado, um caractere <nl> é convertido em
<cr>; se IGNCR está ligado, um <nl> é ignorado; em caso
contrário, se ICRNL está ligado, um <cr> é convertido
em <nl>.

.sp
Se IUCLC está ligado, uma letra maiúscula é convertida na
letra minúscula correspondente.

.sp
Se IXON está ligado, o controle de "start/stop" da saída
é habilitado. Um caractere de entrada "stop" (<^S>) suspende a sáida
e um caractere "start" (<^Q>) a reinicia. Todos os caracteres
de "start/stop" são ignorados para efeito de leitura.  
Se IXANY estiver ligado, qualquer caractere de entrada irá
reiniciar a saída.

.sp
Se IXOFF está ligado, o sistema transmite caracteres de "start/stop"
quando a fila de entrada está quase vazia/cheia.


.sp 2
O campo "c_oflag" descreve os modos de saída do terminal:
.sp
.nf
	OPOST	0x0001	Processe a Saida (saída "cooked")
	OLCUC	0x0002	Mapeie Minusculo em Maiusculo
	ONLCR	0x0004	Mapeie <nl> em <cr><nl>
	OCRNL	0x0008	Mapeie <cr> em <nl>
	ONOCR	0x0010	Evite <cr> na Coluna 0
	ONLRET	0x0020	NL executa a funcao de <cr>
	OFILL	0x0040	Use caracteres de enchimento para atrasos
	OFDEL	0x0080	O enchimento é <del>, em caso cont. <nul>
	NLDLY	0x0100	Atrasos do <nl>:
	NL0	0x0000	Tipo 0
	NL1	0x0100	Tipo 1
	CRDLY	0x0600	Atrasos do <cr>:
	CR0	0x0000	Tipo 0
	CR1	0x0200	Tipo 1
	CR2	0x0400	Tipo 2
	CR3	0x0600	Tipo 3
	TABDLY	0x1800	Atrasos do <ht>:
	TAB0	0x0000	Tipo 0
	TAB1	0x0800	Tipo 1
	TAB2	0x1000	Tipo 2
	TAB3	0x1800	Tipo 3: Converta em brancos
	BSDLY	0x2000	Atrasos do <bs>: 
	BS0	0x0000	Tipo 0
	BS1	0x2000	Tipo 1
	VTDLY	0x4000	Atraso do <vt>:
	VT0	0x0000	Tipo 0
	VT1	0x4000	Tipo 1
	FFDLY	0x8000	Atraso do <ff>:
	FF0	0x0000	Tipo 0
	FF1	0x8000	Tipo 1
.fi

.sp
Se OPOST está ligado, os caracteres de saída são processados,
de acordo com os indicadores abaixo; em caso contrário,
são transmitidos sem modificação.

.sp
Se OLCUC está ligado, uma letra minúscula é convertida na
letra maiúscula correspondente. Esta função em geral é
utilizada conjuntamente com IUCLC.

.sp
Se ONLCR está ligado, o caractere <nl> é transmitido como o
par <cr><nl>. Se OCRNL está ligado, <cr> é transmitido como
<nl>. Se ONOCR está ligado, o <cr> não é transmitido
no começo da linha. Se ONLRET está ligado, assume-se que o caractere <nl>
realiza a função de <cr>; o ponteiro da coluna é zerado,
e serão usados os atrasos especificados para o <cr>;
em caso contrário, o ponteiro de coluna permanecerá no lugar.
Este ponteiro também volta para a coluna zero se o <cr>
é realmente transmitido.

.sp
Os bites de atrasos especificam o tempo de interrupção da
transmissão para permitir movimentos mecânicos quando certos
caracteres são transmitidos para o terminal. Em todos os casos,
um valor de 0 indica "sem atraso". Se OFILL está ligado,
caracteres de enchimento são transmitidos para realizar a função
do atraso; isto é útil para terminais de grande velocidade
em que apenas são necessários atrasos mínimos. Se OFDEL
está ligado, o caractere de enchimento é <del>, em caso
contrário <nul>. As funções de atrasos não estão
implementadas; a única função disponível é a TAB3,
com a qual <ht> é convertido no número apropriado de brancos.

.sp 2
O campo "c_cflag" descreve os modos de controle do terminal:
.sp
.nf
	CBAUD	0x000F	Velocidade da Linha:
	B0	0x0000	Desconecte
	B50	0x0001	50 Baud
	B75	0x0002	75 Baud
	B110	0x0003	110 Baud
	B134	0x0004	134.5 Baud
	B150	0x0005	150 Baud
	B200	0x0006	200 Baud
	B300	0x0007	300 Baud
	B600	0x0008	600 Baud
	B1200	0x0009	1200 Baud
	B1800	0x000A	1800 Baud
	B2400	0x000B	2400 Baud
	B4800	0x000C	4800 Baud
	B9600	0x000D	9600 Baud
	B19200	0x000E	19200 Baud
	B38400	0x000F	38400 Baud
	CSIZE	0x0030	Tamanho do Caracter:
	CS5	0x0000	5 Bits
	CS6	0x0010	6 Bits
	CS7	0x0020	7 Bits
	CS8	0x0030	8 Bits
	CSTOPB	0x0040	Envie 2 Bits de Stop, em caso contrario 1
	CREAD	0x0080	Habilite a Leitura
	PARENB	0x0100	Habilite a Paridade
	PARODD	0x0200	Paridade Impar, em caso contrario par
	HUPCL	0x0400	Desconecte no ultimo close
	CLOCAL	0x0800	Linha local, em caso contrario discada
	LOBLK	0x1000	Block layer Output
.fi

.sp
Os bites CBAUD especificam a velocidade do terminal. A velocidade
nula, B0 é utilizada para desfazer a conecção.
Nem todos os dispositivos aceitam todas as velocidades;
mudanças para velocidades inválidas são ignoradas.
.sp

Os bites CSIZE especificam o tamanho do caractere (em bites) para
transmissão e recepção. Este tamanho não inclui o (possível)
bit de
paridade. Se CSTOB está ligado, 2 bites de "stop" são utilizados;
em caso contrário é utilizado apenas um.

.sp
Se PARENB está ligado, geração e deteção de paridade é habilitada,
e um bite de paridade é acrescentado a cada caractere. Se ela
está habilitada, o bite PARODD especifica paridade ímpar; em caso
contrário paridade par.

.sp
Se CREAD está ligado, o receptor está habilitado; em caso contrário
não há recepção de caracteres.

.sp
Se HUPCL está ligado, a linha é desconectada quando o último
processo fecha a linha ou termina.

.sp
Se CLOCAL está ligado, a linha é considerada local, isto é,
sem modem; em caso contrário é assumido que contém um modem.

.sp
Se LOBLK está ligado, a saída de um nível de controle
de "job"s será bloqueado quando este não é o nível corrente;
em caso contrário, a saída gerada por aquele nível será
multiplexado no nível corrente. 


.sp 2
O campo "c_lflag" descreve os modos locais do terminal:
.sp
.nf
	ISIG	0x0001	Habilite Sinais
	ICANON	0x0002	Processe a Entrada (entrada "cooked")
	XCASE	0x0004	Processamento de Maiu/Min com '\'
	ECHO	0x0008	Habilite o eco
	ECHOE	0x0010	Ecoe "erase" como <bs><sp><bs>
	ECHOK	0x0020	Ecoe <nl> apos "kill"
	ECHONL	0x0040	Ecoe <nl>
	NOFLSH	0x0080	Deshabilite "flush" apos inter. ou quit

	ISOKEY	0x0200  Teclado ISO
	CODE	0x0C00	Codigo de Caracteres:
	ISO	0x0000	 ISO
	ASCII	0x0400	 ASCII
	USER1	0x0800	 USER1
	USER2	0x0C00	 USER2 
	CNTRLX	0x1000	Caracteres de controle na forma '^X'
	VIDEO	0x2000	O terminal e' de Video
	ICOMM	0x8000	Modo de comunicações na entrada
.fi

.sp
Se ISIG está ligado, cada caractere de entrada é comparado com os
caracteres de controle "intr" e "quit". Se o caractere confere,
a função associada a êle é executada; caso contrário não é
(ver ICOMM).

.sp
se ICANON está ligado, o processamento dos caracteres de entrada
é realizado. Isto habilita as funções de "erase" e "kill",
e a montagem de caracteres de entrada em linhas delimitadas
por <nl> e "eof". Se ICANON não está ligado, as leituras são
realizadas diretamente da fila de entrada
(ver ICOMM).

.sp
Se XCASE está ligado, e ICANON está ligado, uma letra maíuscula
é aceita na entrada precedendo-a pelo caractere "\", e é colocada
na saída na mesma forma. Neste modo, as seguintes sequências
são geradas na saída e aceitas na entrada:

.sp
.(t
.nf
	  ao invés de:	     utilize:

		`		\`
		|		\!
		~		\^
		{		\(
		}		\)
		\		\\
.fi
.)t

.sp
Se ECHO estiver ligado, os caracteres são ecoados assim que
recebidos.

.sp
Quando ICANON está ligado, as seguintes funções de eco são possíveis:
Se ECHOE está ligado, 
ecoa "erase" como <bs><sp><bs>.
Se ECHOK está ligado, 
ecoa <nl> apos "kill".
Se ECHONL está ligado, 
ecoa <nl> mesmo com ECHO desligado.

.sp
Se NOFLSH está ligado, a descarga normal das áreas de entrada e
saída associadas ao "intr" e "quit" não são realizadas.

.sp
Se CNTRLX está ligado, 
os caracteres de controle na saída serão convertidos na forma "^X";
em caso contrário serão escritos sem modificação.

.sp
Se ISOKEY está desligado, supõe-se que o teclado do terminal
não processa acentos; a interface converte pares de caracteres
da entrada
nos caracteres acentuados correspondentes. Os pares definidos
são "," seguido de "c" ou "C" (representando "c"-cedilha),
":" seguido de vogal (representando a vogal com trema) e os
acentos "'", "`", "^" e "~" seguidos de vogais, representando
as vogais com acentos. Para evitar a união dos pares de caracteres,
basta colocar um "\" entre eles (como por ex. ",\c" para gerar ",c").
Se ISOKEY está ligado, supõe-se que o teclado já gere os
caracteres com acentos, e o procedimento acima descrito não é
realizado. 

.sp
A cada linha está associada uma tabela de conversão de caracteres.
O valor de CODE normal é ISO, indicando que não será feita
conversão de código. Com o valor ASCII, será feita a conversão
para o código "ascii", com o mapeamento de vogais acentuadas
em vogais sem acentos, e outras modificações.
Com os valores USER1 e USER2 serão utilizadas as tabelas
"user1" e "user2", cujos valores normais são ABICOMP e ISO,
mas que podem ser modificadas através de "kcntl" (sys).

.sp
Se VIDEO está ligado, o terminal é considerado de vídeo:
para o processamento de "kill", apaga a linha inteira,
e para o processamento de "word", apaga a última palavra;
se estiver desligado, para o processamento de "kill"
imprime o caractere de controle
e pula de linha, e não processa o "word".

.sp
Se ICOMM está ligado, a entrada de caracteres será realizada
no modo de comunicações, que é um modo especialmente adequado
para comunicação máquina/máquina, pois é o mais eficiente.
Ligar ICOMM é equivalente a desligar ISIG, ICANON e ECHO, mas
a interface utiliza outro trecho de código, que é mais rápido. 

.sp 2
Os comandos principais disponíveis da chamada ao sistema "ioctl" (sys)
referentes às linhas de comunicação
têm a forma:

.sp
.nf
		#include <sys/types.h>
		#include <termio.h>

		int ioctl (int fd, int cmd, TERMIO *arg);
.fi

.sp

Os comandos utilizando esta forma são:
.in +3

.ip TCGETS 11
Obtenha os parâmetros associados com o terminal e guarde-os
na estrutura apontada por "arg".

.ip TCSETS 11
Atribua os parâmetros do terminal com os valores da estrutura
apontada por "arg". A mudança é imediata.

.ip TCSETAW 11
Espere até que a file de saída esteja vazia antes de atribuir
os novos parâmetros. Esta forma deve ser usada quando mudar
parâmetros que irão afetar a saída.

.ip TCSETAF 11
Espere até que a file de saída esteja vazia,
em seguida descarregue
a fila de entrada e só então atribua os novos parâmetros.

.ip TCSETNR 11
Este modo de atribuir os parâmetros pode ser usado quando
se deseja modificar parâmetros apenas da interface de terminais
(por exemplo, passar do modo "raw" para "cooked" ou vice-versa),
e NÃO modificar parâmetros de "hardware" (tal como a velocidade
de transmissão).
Quando este for o caso, êste é o modo mais indicado para a
mudança, pois NÃO é dado "reset" nos dispositivos de "hardware",
nem são modificadas as filas de entrada/saída. 
É de responsabilidade do usuário certificar-se de que
não estão sendo modicados parâmetros de "hardware".

.ep
.in -3

.in 5
.sp 2
Comandos adicionais da chamada ao sistema "ioctl" (sys)
têm a forma:

.sp
.nf
		int ioctl (int fd, int cmd, int arg);
.fi

.sp
Os comandos utilizando esta forma são:

.in +3
.ip TCSBRK 11
Espere a fila de saída esvaziar.

.ip TCXONC 11
Controle de "start/stop". Se "arg" é 0, suspenda a saída; se é
1, reinicie a saída.

.ip TCFLSH 11
Se "arg" é 0, descarregue a fila de entrada; se é 1, descarregue
a fila de saída; se é 2, descarregue ambas. 

.ip TCNREAD 11
Devolva o número de caracteres disponíveis na fila de entrada.

.ip TCISATTY 11
Devolva 1 se o descritor de arquivos se refere a um terminal, e   
0 se não.

.ip TCGETCTS 11
Devolve um valor não nulo se o sinal CTS ("clear to send") está
sendo recebido no dispositivo serial.

.ip TCSETRTS 11
Envie o sinal RTS ("request to send") através do dispositivo serial.

.ip TCRESETRTS 11
Não envie o sinal RTS ("request to send") através do dispositivo serial.

.ip TCINTERNET 11
Devolve um valor não nulo se o dispositivo pode ser usado como meio de
tranmissão/recepção de datagramas da INTERNET.

.ip TCFIFOSZ 11
Ajusta o tamanho a ser usado do FIFO de saída. Isto só pode ser
utilizado em alguns dispositivos, como por exemplo NS-16550.

.ep
.in -3

.in

.sp
.b OBSERVAÇÕES
.in 5
Os seguintes bites dos vários modos (e respectivas funções)
não estão implementados: IGNBRK, BRKINT, IGNPAR, PARMRK,
INPCK, IXOFF, CSIZE, CSTOPB, CREAD, PARENB, HUPCL, CLOCAL, LOBLK,

.sp
Também não estão implementados os atrasos dos diversos caracteres.

.in

.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
intro
.br
.wo "(cmd): "
stty
.br
.wo "(sys): "
ioctl, kcntl, signal
.br

.in
.sp
.b ARQUIVOS
.in 5
 /dev/console
 /dev/tty*
 /dev/ptys*
 /dev/ptyc*
 /dev/slip*

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

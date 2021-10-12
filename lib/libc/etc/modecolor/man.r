.bp 1
.he 'MODECOLOR (libc)'TROPIX: Manual de Referência'MODECOLOR (libc)'
.fo 'Atualizado em 27.08.04'Versão 4.6.0'Pag. %'

.b NOME
.in 5
modecolor - obtém um vetor de cores para os tipos de arquivos

.in
.sp
.b SINTAXE
.(l
#include <stdlib.h>

int modecolor (int active,
		char **value_vector, const char ***string_vector);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "modecolor" prepara dois vetores ("value_vector" e
"string_vector"), contendo informações
sôbre cores que serão associadas a cada tipo de arquivo e também
as cores que devem ser usadas para mensagens de "advertência" e "erro".

Estas informações são obtidas a partir da variável MODECOLOR
do ambiente (ver "environ" (fmt)).

.sp
Ambos vetores contém 19 elementos, que devem ser indexados do seguinte modo:

.in +3
.ip 1. 3
Os 16 primeiros elementos a partir do tipo do arquivo, que
é parte do modo de um arquivo,  tal como obtido de uma chamada
ao sistema "stat" ou "fstat" (ver "stat" (sys), e tabela abaixo).

.ip 2. 3
O elemento de índice MC_WARN define a cor que deve ser usada nas
mensagens de "advertência".

.ip 3. 3
O elemento de índice MC_ERR define a cor que deve ser usada nas
mensagens de "erro".

.ip 4. 3
O elemento de índice MC_DEF (apenas do vetor "string_vector") contém a
cadeia de caracteres para restaurar a cor original do vídeo.

.ep
.in -3

.sp
O primeiro vetor fornece valores de cor (de 0 a 15), segundo
o código das cores VGA (ver tabela, abaixo).
O endereço do vetor é armazenado no conteúdo do ponteiro "value_vector".
Se este vetor não for de interêsse, o ponteiro "value_vector" pode ser dado NULO.

.sp
O segundo vetor fornece cadeias para alterar a cor do vídeo/monitor,
segundo os padrões VT100.
.bc	/*************************************/
Este vetor contém um elemento extra, de
índice MC_DEF, contendo a cadeia para restaurar a cor original.
.ec	/*************************************/
O endereço do vetor é armazenado no conteúdo do ponteiro "string_vector".
Se este vetor não for de interêsse, o ponteiro "string_vector" pode ser dado NULO.

.sp
Se o argumento "active" for diferente de zero, "modecolor" analisa
a variável MODECOLOR do ambiente (ver "environ (fmt)), obtendo
as cores associadas aos tipos de arquivo presentes no valor da variável,
e prepara os vetores.

.sp
Para cada tipo de arquivo para o qual não foi obtida uma informação de cor,
o elemento correspondente do primeiro vetor conterá o valor 255 (valor inválido) e
o elemento do segundo vetor apontará para uma cadeia vazia.

.sp
Os vetores devem ser indexados pelo tipo do arquivo/mensagem
(veja o arquivo de inclusão <stat.h>):

.sp
.nf
	      Valor	Tipo de Arquivo		Nome simbólico

		 1	Fifo			    fifo
		 2	Especial de caracteres	    chr
		 4	Diretório		    dir
		 6	Especial de blocos	    blk
		 8	Regular			    reg
		10	Elo simbólico		    lnk

			Tipo da mensagem

	     MC_WARN	Advertência		    warn
	     MC_ERR	Erro			    err
.fi

.sp 2
As cores disponíveis são:

.sp
.nf
			Nomes			Valor

		preto		black		 0
		azul		blue		 1
		verde		green		 2
		ciano		cyan		 3
		vermelho	red		 4
		magenta		magenta		 5
		marrom		brown		 6
		branco		white		 7
		azulclaro	lightblue	 9
		verdeclaro	lightgreen	10
		cianoclaro	lightcyan	11
		vermelhoclaro	lightred	12
		magentaclaro	lightmagenta	13
		marromclaro	lightbrown	14
		brancoclaro	lightwhite	15
.fi

.sp
A cadeia MODECOLOR deve ter o seguinte formato:
.nf

		"MODECOLOR=<tipo>=<cor>,..."
.fi

.sp
onde <tipo> é um nome simbólico de um tipo de arquivo ou mensagem, e <cor>
é o nome ou valor de uma cor (veja as duas tabelas, acima).

.sp
Exemplo:

.sp
.nf
	MODECOLOR=chr=verde,dir=red,blk=6,reg=cyan,err=lightmagenta
.fi

.in
.sp
.(t
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a função devolve 0. Em caso contrário devolve -1;
isto irá ocorrer caso haja algum erro de sintaxe na cadeia MODECOLOR,
ou a função não consiga alocar memória para uma ou mais cadeias.

.sp
De qualquer modo, os vetores preparados podem ser sempre usados
(possivelmente, em caso de erro, não contendo as informações completas). 
.)t

.in
.sp
.b OBSERVAÇÃO
.in 5
Ao inserir a variável MODECOLOR no ambiente, não esqueça de usar
o "export" (veja "sh", (cmd)).

.sp
Se o argumento "active" for dado igual a zero, ou não há
a variável MODECOLOR no ambiente,
o vetor "value_vector"
retornará todas as cores com o valor 255 (valor inválido),
e o vetor "string_vector" apontará em todos os seus elementos
para uma cadeia vazia.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh
.br
.wo "(sys): "
stat
.br
.wo "(fmt): "
environ
.br

.in
.sp
.b EXEMPLO
.r
.in 5
Abaixo damos um exemplo de uso do vetor de cadeias. Estamos supondo
que a variável "st_mode" contém o modo de um arquivo
(como retornado de uma chamada ao sistema "stat" ou "fstat").

.sp
O primeiro "printf" atribui a cor de frente associada ao arquivo
de modo "st_mode", e o último restaura a cor original.
Repare que as cores somente são alteradas se "stdout" for um terminal/vídeo.

.sp
.nf
	#include <stdlib.h>

	...........

	const char	**string_vector;

	...........

	if (modecolor (isatty (fileno (stdout)),
				(void *)0, &string_vector) < 0)
		........

	...........

	printf ("\%s", string_vector[(st_mode & S_IFMT) >> 12]);

	printf (....);

	printf ("\%s", string_vector[MC_DEF]);
.fi

.sp
Para a impressão de uma mensagem de erro, podemos usar:

.sp
.nf
	printf
	(	"\%sTexto da mensagem de erro\%s\n",
		string_vector[MC_ERR], string_vector[MC_DEF]
	);
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

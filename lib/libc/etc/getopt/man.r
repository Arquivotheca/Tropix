.bp 1
.he 'GETOPT (libc)'TROPIX: Manual de Referência'GETOPT (libc)'
.fo 'Atualizado em 22.06.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "getopt -"
obtém as opções dadas a um programa
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>
#include <stdlib.h>

int getopt (int argc, const char *argv[], const char *optstr);
	
extern const char	*optarg;
extern int		optind;
extern int		opterr;
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "getopt" é uma ferramenta para auxiliar a decodificação
das diversas opções dadas para um programa. Ela tem a dupla finalidade
de verificar se a sintaxe está correta e a de fornecer cada opção
juntamente com o seu (possível) argumento.

.sp
A cada chamada, "getopt" devolve a opção seguinte dada ao programa,
relativa ao conjunto de argumentos e opções passados em
"argv", "argc" e "optstr".

.sp
A cadeia "optstr" deve conter todas as opções válidas para o programa.
Se nesta cadeia uma opção é seguida de dois pontos (':'), espera-se
que a opção tenha um argumento, e neste caso o ponteiro para este argumento
é devolvido em "optarg".

.sp
Quando a lista de opções for exaurida, "getopt" devolve o valor
EOF (definido em <stdio.h>).
Neste momento,
a variável "optind" contem
o índice de "argv" do próximo argumento (do comando) a ser processado.
Um modo simples de acessar estes argumentos é
incrementar a variável "argv" do valor de "optind", de tal
modo que "*argv" aponte para o primeiro argumento.

.in
.sp
.b DIAGNÓSTICOS
.in 5
Encontrando uma opção inválida ou um erro de sintaxe,
"getopt" imprime uma mensagem
de erro em "stderr",
posiciona "optarg" para apontar para a opção envolvida com o erro
e devolve '?'.
A mensagem de erro pode ser suprimida zerando-se a variável "opterr".

.in
.sp
.b OBSERVAÇÕES
.in 5
A opção especial "--" pode ser usada como delimitador
para indicar o final
das opções. Ao encontrar este delimitador, "getopt"
devolve EOF, e "--" é  pulado. 

.sp
Ao final das opções, "getopt" é reinicializada, de modo
a permitir a sua reutilização em outro conjunto de argumentos
e opções.

.(t
.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(fmt): "
cmd
.br
.)t

.in
.sp
.b EXEMPLO
.nf
	#include <stdio.h>
	#include <stdlib.h>

	main (int argc, const char *argv[])
	{
		int		opc,
		int		hflag;
		char		*ifile, *ofile;

		   . . .

		while ((opc = getopt (argc, argv, "hi:o:H")) != EOF)
		{
			switch (opc)
			{
			    case 'h' :
				hflag++;
				break;

			    case 'i' :
				ifile = optarg;
				break;

			    case 'o' :
				ofile = optarg;
				break;

			    case 'H' :
				help ();

			    default:
				help ();

			}	/*  end switch (opc)  */

		}	/* end while (opc != EOF)  */

		argc -= optind;
		argv += optind;

		     . . .

	}	/* end main */
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

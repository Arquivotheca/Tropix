.bp
.he 'FEXCEP (libc)'TROPIX: Manual de Referência'FEXCEP (libc)'
.fo 'Escrito em 11.03.96'Versão 3.0'Pag. %'

.b NOME
.in 5
Manuseio de exceções em operações de ponto flutuante:
.sp
.wo "fexcep -"
habilita a ocorrência de exceções
.br
.wo "_fgsr  -"
obtém o valor do "Status Register"
.br
.wo "_fssr  -"
armazena um valor no "Status Register"
.br
.wo "_fgcr  -"
obtém o valor do "Control Register"
.br
.wo "_fscr  -"
armazena um valor no "Control Register"
.br
.wo "_fgiar -"
obtém o valor do "Instruction Address Register"
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <math.h>

long	fexcep (long cond);
long	_fgsr (void);
long	_fssr (long x);
long	_fgcr (void);
void	_fscr (long x);
long	_fgiar (void);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Estas funções operam com o "Floating-Point Status Register" (fsr), 
o "Floating-Point Control Register" (fcr) e o "Floating-Point
Instruction Address Register" (fiar).
Estes registros controlam e informam a correção das operações
em números de ponto flutuante.

.sp
A função "fexcep" indica ao processador numérico
quais as condições que devem causar exceção, isto é, quais as condições
em que o processador numérico deve gerar uma chamada ao sistema operacional.
Tipicamente essas condições são "overflow", "underflow", divisão
por zero, etc... O módulo "math.h" contém as definições dessas condições.
.sp
Normalmente as exceções estão mascaradas e, embora o resultado numérico
possa estar incorreto, o processador numérico não gera exceções.
Mesmo não gerando exceções, as possíveis causas dos resultados incorretos
são sempre indicadas cumulativamente por bits do "Status Register".

.sp
A função "_fgsr" acessa o "Status Register" que contém informações
cumulativas sobre condições passíveis de gerar exceções. Os bits deste
registro são sempre ligados quando estas exceções ocorrem e não são
desligados automaticamente. Cumpre ao usuário desligá-los, usando
a função "_fssr". 

.sp
A função "_fgcr" acessa o "Control Register" que contém informações sobre
a ação a tomar no caso da ocorrência de algum tipo de exceção.
Um bit ligado indica que um sinal SIGFPE deve ser mandado
para o programa quando uma determinada exceção ocorrer.
Normalmente nenhum sinal é mandado. Cumpre ao usuário, utilizando a
função "fexcep" (ou mesmo, "_fscr"), indicar para quais exceções o sinal
"SIGFPE" deve ser mandado para o processo em execução. O sinal mandado
normalmente cancela o processo, a não ser que seja interceptado.

.sp
A função "_fgiar" obtém o conteúdo do registro "Instruction Address
Register" que indica o endereço da instrução que causou a exceção.

.in
.sp
.b OBSERVAÇÕES
.in 5
Todas as funções que carregam um valor em algum registro retornam o valor
anterior ali encontrado.

.sp
Alguns processadores numéricos (como os da Intel) não são capazes de
desligar seletivamente os bits do "Status Register". Neste caso, a função
"_fssr" desliga todos os bits que estavam indicando as condições de exceção
independentemente do valor do argumento.

.sp
No caso dos modernos processadores numéricos que se utilizam de
"pipeline", o "Instruction Address Register" pode não indicar exatamente
o endereço da instrução que causou a exceção mas um endereço próximo a ele.

.sp
Para se trabalhar diretamente com os registros do processador numérico,
recomenda-se consultar o manual do processador (veja "Referências" abaixo).

.in
.sp
.b REFERÊNCIAS
.in 5
Intel Corporation, "Intel486 Microprocessor Family Programmer's
Reference Manual", 1993.

.(t
.in
.sp
.b EXEMPLO
.in 7
No programa abaixo, as exceções que ocorrem geram um sinal que
é interceptado para melhor análise da causa dos erros.
.sp 2
.nf
	#include <math.h>
	#include <signal.h>
	
	main ()
	{
		int	fexcepid ();
	
		fexcep (FP_OVERFLOW | FP_UNDERFLOW | FP_ZERODIV);

		signal (SIGFPE, fexcepid);	/* Arma o signal */
	
		/*
		 *	Aqui colocar o programa a ser monitorado
		 */
	}
	
	int
	fexcepid (int sig)
	{
		if ((_fgsr () & FP_OVERFLOW) == FP_OVERFLOW)
			printf ("Exceção de overflow\n");
	
		if ((_fgsr () & FP_UNDERFLOW) == FP_UNDERFLOW)
			printf ("Exceção de underflow\n");
	
		if ((_fgsr () & FP_ZERODIV) == FP_ZERODIV)
			printf ("Exceção de divisão por zero\n");
	
		if ((_fgsr () & FP_INVALOPER) == FP_INVALOPER)
			printf ("Exceção de operação inválida\n");
	
		_fssr ();	/* Desliga a indicação das exceções */

		signal (SIGFPE, fexcepid);	/* Rearma o signal */
	}
.fi
.)t

.sp
.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

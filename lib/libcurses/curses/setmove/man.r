.bp
.he 'SETMOVE (curses)'TROPIX: Manual de Referência'SETMOVE (curses)'
.fo 'Atualizado em 31.08.89'Versão 2.3'Pag. %'

.b NOME
.in 5
.sp
.wo "setmove -"
estabelece a rotina de troca de contexto

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void setmove (void  (*func) ());
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "setmove" permite ao programador especificar
uma rotina a ser chamada quando a posição que o cursor ocuparia
não está no vídeo.

.in
.sp
.b OBSERVAÇÃO
.in 5
Se o endereço fornecido da rotina de troca de contexto é nulo,
a entrada do caractere "switch" é simplesmente ignorada.
Este é o modo inicial de funcionamento.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd)   :"
stty
.br
.wo "(sys)   :"
ioctl
.br
.wo "(curses):"
refset
.br

.in
.sp
.b EXEMPLOS
.in 10
.(l
void	getswitch ();

main (argc, argv)
int	argc;
char	*argv[];
{
	...

	setmove (getswitch);

	...

	incurses (argv[0]);

	...
}

void
getswitch (signal)
int	signal;
{
	...
}
.)l

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

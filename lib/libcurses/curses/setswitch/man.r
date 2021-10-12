.bp
.he 'SETSWITCH (curses)'TROPIX: Manual de Referência'SETSWITCH (curses)'
.fo 'Atualizado em 18.11.88'Versão 2.3'Pag. %'

.b NOME
.in 5
.sp
.wo "setswitch -"
estabelece a rotina de troca de contexto

.in
.sp
.b SINTAXE
.in 5
.(l
#include <curses.h>

void setswitch (void (*func) ());
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "setswitch" permite ao programador especificar
uma rotina a ser chamada quando o caractere de "switch" for teclado.
Após a execução da rotina especificada o programa segue normalmente
a partir do ponto onde se encontrava quando o caractere foi teclado.

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
getset
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

	setswitch (getswitch);

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

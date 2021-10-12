.bp
.he 'PHYS (sys)'TROPIX: Manual de Referência'PHYS (sys)'
.fo 'Atualizado em 17.08.95'Versão 3.0'Pag. %'

.b NOME
.in 5
.wo "phys -"
acessa diretamente a memoria física
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>
#include <fcntl.h>

void	*phys (const void *area, int count, int oflag);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "phys" prepara a unidade de gerência de memória
da UCP na qual o processo está sendo executado para modificar
o mapeamento de endereços virtuais em endereços físicos. 
Com isto, abre-se a possibilidade de um processo acessar regiões
de memória fora das regiões próprias do processo, tais como
as regiões de outros processos ou do núcleo do sistema operacional
TROPIX. 

.sp
O argumento "oflag" é construído a partir dos indicadores da lista
abaixo (somente um dos dois primeiros e um dos dois
últimos pode ser usado):

.in 8
.ip O_RDONLY 10
O mapeamento é realizado apenas para leituras.

.ip O_RDWR 10
O mapeamento é realizado para leituras e escritas.

.ip O_PHYS 10
O endereço dado "area" é de um endereço físico da memória
que desejamos acessar. 

.ip O_KERNEL 10
O endereço dado "area" é de um endereço virtual do espaço do
núcleo do TROPIX
que desejamos acessar. 

.ep
.sp
.in 5
O argumento "count" fornece o tamanho (em bytes) da região mapeada.
Se for NULO, desfaz um mapeamento anterior, e neste caso, em
"area" temos que dar o endereço virtual retornado na chamada
que realizou o mapeamento. 

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Obtendo sucesso: no caso de realização de um mapeamento,
a chamada ao sistema devolve o endereço virtual que deve ser
usado para acessar a área; no caso de desfazer o mapeamento,
ela devolve zero. 
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÕES
.in 5
Esta chamada ao sistema é relativamente dependente da
máquina, e somente o superusuário pode utilizá-la.
Normalmente ela é reservada para utilitários especiais.

.sp
As unidades de gerência de memória tem normalmente um número máximo
de mapeamentos que podem realizar simultaneamente.
Quando este limite for atingido, somente podemos
realizar novos mapeamentos desfazendo mapeamentos anteriores.

.sp
O tamanho máximo da área mapeada também é dependente
das unidades de gerência de memória. 

.in
.sp
.b EXEMPLO
.nf
	#include <sys/syscall.h>
	#include <fcntl.h>

	   ..........

	register int	*ip, n;

	/*
	 *	Preparando o acesso ao inteiro cujo endereço na
	 *	memória física é 0x50CDE, apenas para leitura
	 */ 
	ip = phys ((char *)0x50CDE, sizeof (int), O_PHYS|O_RDONLY);

	if ((int)ip == -1)
	{
		/* Erro ... */
	}

	/*
	 *	Acessando o inteiro
	 */
	n = *ip;

	/*
	 *	Desfazendo o mapeamento
	 */
	phys (ip, 0, 0);
.fi

.in
.sp
.b ESTADO
.in 5
Efetivo.

.bp
.he 'SHMEM (sys)'TROPIX: Manual de Referência'SHMEM (sys)'
.fo 'Atualizado em 04.01.99'Versão 3.1'Pag. %'

.b NOME
.in 5
.wo "shmem -"
cria uma região compartilhada
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <sys/syscall.h>

void	*shmem (int fd, int size);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A chamada ao sistema "shmem" permite a obtenção de uma região de memória
que pode ser compartilhada entre vários processos.

.sp
A identificação de uma região de memória compartilhada no TROPIX
é feita através de um arquivo regular cujo nome é conhecido pelos
diversos processos. O arquivo em si não é usado; apenas o seu
nome é utilizado como uma referência comum.

.sp
Inicialmente deve-se criar ("creat" (sys)) e/ou abrir ("open" (sys))
o arquivo, obtendo-se o descritor "fd", que será usado em seguida na
chamada a "shmem".
O acesso à região terá os mesmos privilégios
(de leitura/escrita) do arquivo aberto.

.sp
Se a região compartilhada ainda não existia, ela é criada com o tamanho
"size" bytes. Se ela já existia (isto é, um outro processo já chamou
"shmem" com o mesmo arquivo), o tamanho dado deve ser o mesmo.

.sp
A região compartilhada é liberada quando o último dos processos
que a usam der o "close" (sys) referente ao arquivo.


.bc	/*************************************/
.sp
Se por acaso a região ("fd", "n") dada já tinha memória alocada
(por exemplo, por um outro processo),
"shmem" utiliza a área de memória já alocada
(certificando-se de que os tamanhos são iguais).
Deste modo, os dois (ou mais) processos podem compartilhar esta mesma
área de memória.
.ec	/*************************************/

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a chamada ao sistema devolve o endereço virtual que deve ser
usado para acessar a região.
Em caso contrário, devolve -1 e indica em "errno" a causa do erro.

.in
.sp
.b OBSERVAÇÃO
.in 5
Os tamanhos das áreas de memória efetivamente alocados às regiões
são arredondados de acordo com as características da gerência de
memória de cada computador.

.sp
Na presente versão, o arredondamento é de 4 KB,
e um processo pode ter no máximo 32 regiões,
com um total de 4 MB alocados.

.bc	/*************************************/
.sp
No TROPIX as regiões de memória compartilhadas existem enquanto o
seu arquivo associado estiver aberto.
Quando o arquivo associado a um conjunto de regiões é fechado, o processo
perde o acesso a estas regiões. Quando o último processo fecha este arquivo
as regiões são destruídas.
Desta forma, a liberação de regiões é feita,
simplesmente, fechando-se seus arquivos associados.
.ec	/*************************************/

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
creat, open, close
.br

.in
.sp
.(t
.b EXEMPLO
.in 7
A aplicação usual da chamada ao sistema para se obter uma região
de memória que pode ser compartilhada entre processos é feita
da seguinte forma:
.sp
.nf
	#include <sys/syscall.h>

	   ..........

	int		fd, size, *ip, n;

	/*
	 *	Abrindo o arquivo associado
	 */
	fd = open ("nome", ...);

	/*
	 *	Alocando/referindo a região compartilhada
	 */
	ip = shmem (fd, size);

	if ((int)ip == -1)
	{
		/* Erro ... */
	}

	/*
	 *	Acessando a região
	 */
	n = *ip;	/* Acessa a região */

	/*
	 *	Libera a região alocada
	 */
	close (fd);
.fi
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

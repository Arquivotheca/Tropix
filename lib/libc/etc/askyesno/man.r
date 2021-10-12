.bp
.he 'ASKYESNO (libc)'TROPIX: Manual de Referência'ASKYESNO (libc)'
.fo 'Atualizado em 18.10.98'Versão 3.1'Pag. %'

.b NOME
.in 5
.wo "askyesno -"
obtém uma resposta afirmativa ou negativa
.br

.in
.sp
.b SINTAXE
.in 5
.(l
#include <stdio.h>

int		askyesno (void);
void		askyesno_set (int mode);

extern FILE	*askyesno_stdin, *askyesno_stderr;
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A função "askyesno" consulta o usuário de um utilitário interativo
para saber se ele deseja (ou não) realizar uma certa operação.

.sp
A função tem 2 modos de operação: o modo "regular" e o modo "de força".
Inicialmente descreveremos o modo "regular" (no qual a função inicia).

.sp 2
Em seu modo "regular", a função lê uma linha (normalmente de "stdin",
veja abaixo) e devolve o valor:

.in +3
.ip " 1" 5
Resposta afirmativa:
caso o primeiro caractere da linha seja "s", "S", "y" ou "Y".

.ip " 0" 5
Resposta indefinida:
caso a linha esteja vazia
(isto é, o usuário teclou apenas um <nl>).

.ip "-1" 5
Resposta negativa:
caso o primeiro caractere da linha seja "n" ou "N".

.ep
.in -3

.sp 2
Em seu modo "de força", a função "askyesno" faz distinção entre
as letras minúsculas e maiúsculas.
Enquanto forem dadas letras apenas minúsculas, ela se porta como
em seu modo "regular" (veja acima).

.sp
No momento em que for dada uma linha começando por uma letra maiúscula,
isto é, começando por "S", "Y" ou "N", tôdas as perguntas seguintes
serão respondidas automaticamente. As respostas serão afirmativas
caso tenha sido dado um "S" ou "Y" e negativas no caso de um "N".


.sp 2
A função "askyesno_set" altera o modo de funcionamento de "askyesno",
de acordo com o seu argumento "mode":

.in +3
.ip "0" 5
A função "askyesno" entra em seu modo "regular" de funcionamento. As letras
maísculas e minúsculas têm o mesmo significado.

.ip "1" 5
A função "askyesno" entra em seu modo "de força" de funcionamento.
As letras maísculas têm significado especial, como descrito acima.
Se a função "askyesno" já estava no modo "de força",
a resposta automática é cancelada (e só será restabelecida caso
seja dada novamente uma letra maiúscula).

.ep
.in -3

.sp
Normalmente, a função "askyesno" lê de "stdin" e escreve em "stderr".
Para alterar os dispositivos de entrada/saída, basta alterar os
valores das variáveis externas "askyesno_stdin" e/ou "askyesno_stderr".

.in
.sp
.b OBSERVAÇÕES
.in 5
Antes de ler a linha, a função "askyesno" descarrega a área de saída de
"askyesno_stderr", para garantir a impressão da pergunta
(já que ela em geral NÃO termina por "\n").
Portanto é importante atualizar o valor de "askyesno_stderr",  caso
a pergunta não seja escrita através de "stderr".

.sp
Se for teclada uma linha começando por caracteres distintos dos
discutidos acima, "askyesno" irá pedir outra linha.

.sp
Se a função "askyesno" não conseguir ler nenhuma linha
(erro de leitura em "askyesno_stdin"), ela retorna o valor 0.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(libc):"
fflush
.br

.(t
.in
.sp
.b EXEMPLO
.in 7
No exemplo abaixo, uma resposta indefinida (linha em branco) será
interpretada como indicação para não remover o arquivo. 

.sp
.nf
		fprintf (stderr, "Deseja remover o arquivo? (n): ");

		/* O "fflush (stderr)" é realizado por "askyesno" */

		if (askyesno () > 0)
		{
			/*
			 *	Remove o arquivo
			 */
		}
.fi
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

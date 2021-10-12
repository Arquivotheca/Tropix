.bp 1
.he 'EDIT (libc)'TROPIX: Manual de Referência'EDIT (libc)'
.fo 'Atualizado em 13.09.99'Versão 3.2.2'Pag. %'

.b NOME
.in 5
edit - edição interativa de uma cadeia de caracteres

.in
.sp
.b SINTAXE
.in 5
.(l
int edit (const char *prompt, const char *old, char *new)
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "edit" permite a edição visual 
de uma cadeia de caracteres
através de um subconjunto dos comandos
do editor visual de textos "vi".

.sp
A cadeia dada pelo argumento "prompt"
é utilizada como uma mensagem que precede
a cadeia em edição no terminal e pode ser usada
para indicar que está ocorrendo uma edição da cadeia a seguir.

.sp
O argumento "old" aponta para a cadeia dada, que irá ser editada.
Esta edição é realizada em uma cópia da cadeia, de tal modo
que a cadeia dada não é alterada.

.sp
A cadeia resultante da edição
é armazenada na área apontada pelo argumento "new"
(que se supõe conter o tamanho necessário).

.in
.sp
.b
INTERAÇÃO COM O USUÁRIO
.r
.in 5
A rotina "edit" inicia a operação
mostrando a cadeia em edição no terminal após
a cadeia de "prompt". 
O cursor é colocado sobre
o primeiro caractere da cadeia, que passa a ser
o caractere corrente.

.sp
Os comandos são formados por um ou dois caracteres.
Estes caracteres não são ecoados,
mas o efeito do comando é percebido instantaneamente
pelo usuário através do movimento do cursor
e/ou da alteração da cadeia no terminal.

.sp 2
.b
Movimento do caractere corrente:
.r

.sp
Os comandos de movimento fazem o cursor 
se mover sobre a cadeia,
alterando o caractere corrente.
Estão disponíveis os seguintes comandos de movimento:

.in 10

.ip 0 11
Coloca o cursor sobre o
primeiro caractere da cadeia.

.ip $ 11
Coloca o cursor sobre o
último caractere da cadeia.

.ip '<sp>, l ou <seta para a direita>' 11
.sp
Coloca o cursor sobre o
caractere seguinte da cadeia.

.ip '<bs> ou h ou <seta para a esquerda>' 11
.sp
Coloca o cursor sobre o
caractere anterior da cadeia.

.ip 'w' 11
Coloca o cursor sobre o
primeiro caractere
da próxima palavra da cadeia.

.ip "e" 11
Coloca o cursor sobre o
último caractere
de uma palavra da cadeia.

.ip 'b' 11
Coloca o cursor sobre o
primeiro caractere de uma palavra anterior
da cadeia.

.ep

.in 5
.sp 2
.b
Inserção de caracteres:
.r
.sp
Os comandos de inserção permitem acrescentar
novos caracteres à cadeia.
Há dois comandos básicos para inserção de caracteres:
"i" (insert) que insere antes da posição do cursor
e "a" (append) que insere após a posição do cursor.
Durante a inserção a cadeia é mantida atualizada no terminal.
Os caracteres <bs> e <del> removem o último caractere inserido.
O caractere <^U> remove todo o texto inserido.
O caractere <esc> encerra a inserção.

.sp 2
.b
Operadores:
.r
.sp
Utilizamos operadores para manipular grupos de caracteres contíguos.
Os operadores são usados em conjunção com os comandos para movimento,
que especificam o escopo da operação
(da posição do cursor até o caractere indicado pelo comando de movimento).
Estão disponíveis na versão atual dois operadores:
"d" (delete) para remoção e
"c" (change) para substituição (remoção e inserção).

.sp 2
.b
Outros Comandos:
.r
.sp
Existem alguns outros comandos:

.in 10
.ip "q" 8
O comando "q" (quit) encerra a edição da cadeia.

.ip 'r' 8
O comando "r" (replace) substitui o caractere corrente pelo
caractere teclado em seguida.

.ip "U" 8
O comando "U" (undo) retorna a cadeia ao seu estado original.

.ip <^R> 8
O comando <^R> (retype) mostra novamente a cadeia em edição.

.ep

.sp 2
.(t
.in 5
.b
Sinônimos:
.r
.sp
Existem alguns sinônimos usando apenas um caractere
para os comandos utilizados com maior frequência e que
requerem dois caracteres para serem executados.
A tabela abaixo relaciona todos estes sinônimos
e seus respectivos significados:

.sp
.nf
	+----------+-----------+------------------------------+
	| Comando  | Sinônimo  |        Significado           |
	+----------+-----------+------------------------------+
	|   $a     |    A      | adiciona no final da linha   |
	+----------+-----------+------------------------------+
	|   c$     |    C      | muda até o final da linha    |
	+----------+-----------+------------------------------+
	|   d$     |    D      | apaga até o final da linha   |
	+----------+-----------+------------------------------+
	|   0i     |    I      | insere no início da linha    |
	+----------+-----------+------------------------------+
	|   dl     |    x      | apaga o carac. do cursor     |
	+----------+-----------+------------------------------+
	|   dh     |    X      | apaga o carac. antes do cur. |
	+----------+-----------+------------------------------+
	|   cl     |    s      | substitui a partir do cursor |
	+----------+-----------+------------------------------+
	|    q     |   <cr>    | termina a edição             |
	+----------+-----------+------------------------------+

.fi
.)t

.in
.sp
.b
VALOR DE RETORNO
.r
.in 5
Em caso de sucesso, a rotina devolve 0 (zero).
Em caso de erro, a rotina devolve -1 (menos um).

.in
.sp
.b DIAGNÓSTICOS
.in 5
As seguintes situações causam erro:
incapacidade para adquirir memória via "malloc" (libc) e
entrada padrão e/ou saida padrão não são terminais.

.in
.sp
.b OBSERVAÇÕES
.in 5
A rotina "edit" espera que o terminal
tenha capacidade de executar as funções
normalmente associadas aos caracteres <bs>, <ht>, <cr> e <nl>
quando estes forem enviados.
A rotina "edit" também supõe que o terminal
apaga o caractere sobre o cursor antes de escrever
um novo caractere enviado.

.sp
Nesta versão da função, os caracteres de contrôle descritos acima,
<bs>, <del>, <^U> e <^R> são fixos, isto é,
não podem ser mudados através de um "ioctl" (ver "termio" (fmt)),
ou através do comando "stty" (cmd).

.sp
A cadeia em edição pode ter até 255 caracteres.

.sp
A rotina "edit" trabalha com a linha do terminal em modo "raw" e sem "echo".

.in
.sp
.(t
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
vi
.br
.wo "(sys): "
ioctl
.)t

.in
.sp
.(t
.b EXEMPLO
.in 7
char	line[256], newline[256];
.sp
.in 14
.wo '. . .'
.in 7
.sp
if (edit ("Editando: ", line, newline) == 0)
.in 14
.br
{	/* Ok */
}
.in 7
.br
else
.in 14
.br
{	/* Erro */	}
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

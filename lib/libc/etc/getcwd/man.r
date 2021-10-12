.bp 1
.he 'GETCWD (libc)'TROPIX: Manual de Referência'GETCWD (libc)'
.fo 'Atualizado em 02.11.05'Versão 4.8.0'Pag. %'

.b NOME
.in 5
getcwd - obtém o nome do diretório corrente

.in
.sp
.b SINTAXE
.in 5
.(l
char	*getcwd (char *area, int size);
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
A rotina "getcwd" devolve um ponteiro para uma área contendo
o nome completo (caminho) do diretório corrente.

.sp
Se "area" for um ponteiro não NULO, "getcwd" supõe que ele aponte
para uma área de "size" bytes, e a preenche com o nome do diretório corrente.

.sp
Se "area" for um ponteiro NULO, "getcwd" aloca uma área de tamanho
"size", utilizando "malloc" (libc), e recai no caso anterior.

.sp
Em ambos casos, o valor de "size" deve ser suficiente para conter o nome do
diretório corrente acrescido de um caractere NULO no final.

.in
.sp
.b DIAGNÓSTICOS
.in 5
A rotina devolve um ponteiro NULO e um código de erro em "errno"
caso não consiga alocar memória suficiente ou haja
um erro na leitura dos diretórios. 

.in
.sp
.b OBSERVAÇÃO
.in 5
A função "getcwd" preenche a área de trás para frente,
e portanto o ponteiro retornado não é o do início da área.
Isto significa que se "getcwd" alocou a área via "malloc",
este ponteiro NÃO pode ser usado para "free" (libc).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys): "
chdir
.br
.wo "(libc):"
malloc, free
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

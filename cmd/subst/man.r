.bp 1
.he 'SUBST (cmd)'TROPIX: Manual de Referência'SUBST (cmd)'
.fo 'Atualizado em 08.05.02'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "subst -"
Busca/substitui cadeias em vários arquivos
.br

.in
.sp
.b SINTAXE
.in 5
subst [-fsvN] [-d <c>] -p <ab>[:<cd>] ... [<arquivo> ...]

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "subst" realiza uma série de buscas e/ou substituições
de cadeias de caracteres em um ou mais <arquivo>s.

.sp
Cada busca/substituição é definida por uma opção "-p"
(ver abaixo). Podem ser dadas várias destas opções.
É importante notar que as cadeias de busca/substituição NÃO são
expressões regulares tais como utilizadas no comando "grep" (cmd),
mas apenas seqüências de caracteres que são procuradas/substituídas
(literalmente como dadas) nos <arquivo>s.

.sp
O comando lista todas as linhas onde encontrou uma (ou mais) cadeia(s)
de busca, precedidas do nome do <arquivo> e do número da linha
(veja a opção "-s").
Se tiver sido dada uma cadeia de substituição,
a linha será impressa após feita a substituição.
Uma linha pode ser impressa mais de uma vez, se contiver mais de uma
cadeia de busca.

.sp
As opções do comando são:
.in 8
.ip "-f" 7
Atualização sem consulta.
Normalmente, o usuário é consultado para confirmar a atualização dos <arquivo>s
modificados; com esta opção, os <arquivo>s são atualizados sem consulta.

.ip "-s" 7
Modo silencioso.
Imprime apenas o nome de cada arquivo, ao invés de
listar as linhas onde foram encontradas as cadeias de busca.

.ip "-v" 7
Verboso.

.ip "-N" 7
Lê os nomes dos <arquivo>s da entrada padrão.
Esta opção é implícita se não forem nomes de <arquivo>s.

.ip "-d <c>" 7
Delimitador.
Normalmente o caractere de delimitação entre as cadeias
da opção "-p" é o ":" (dois pontos).
Se for usada esta opção, o delimitador usado passa a ser o caractere <c>.

.ip "-p <ab>[:<cd>]" 7
.sp
Cadeia de busca/substituição.
Cada opção "-p" define uma operação a ser feita nos <arquivo>s.

.sp
Operação de busca: 
Se a opção for da forma "-p <ab>", são listadas todas as linhas
que contém a cadeia <ab>.

.sp
Operação de substituição:
Se a opção for da forma "-p <ab>:<cd>", são modificadas todas as
linhas que contém a cadeia <ab>, substituindo-a por <cd>.

.sp
No caso particular de <cd> ser vazia (mas conservando-se o ":"),
todas as ocorrências da cadeia <ab> são eliminadas.

.ep
.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
grep, walk
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in

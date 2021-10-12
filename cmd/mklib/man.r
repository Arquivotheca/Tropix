.bp
.he 'MKLIB (cmd)'TROPIX: Manual de Referência'MKLIB (cmd)'
.fo 'Atualizado em 21.08.01'Versão 4.0.0'Pag. %'

.b NOME
.in 5
mklib - cria/atualiza bibliotecas de módulos objeto

.in
.sp
.b SINTAXE
.in 5
.(l
mklib [-rudtkfv] [-cm] [<biblioteca>] [<módulo> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "mklib" é um utilitário de criação/manutenção de bibliotecas de
módulos objeto, como por exemplo a "libc" - biblioteca de funções básicas
usadas por programas escritos na linguagem "C".

.sp
No sistema operacional TROPIX, uma biblioteca consiste de uma árvore
de módulos objeto, em que a raiz contém o "arquivo de sinopse" da
biblioteca. Este arquivo, que é de texto, contém os nomes dos módulos
da biblioteca, com os respectivos símbolos externos definidos.
Isto é necessário para o bom funcionamento do  "link-editor" "ld" (cmd)
- mais detalhes são dados nas OBSERVAÇÕES abaixo e em "lib" (fmt").

.sp
Sem opções, o comando "mklib" espera como argumento somente o nome
da <biblioteca> (que é identificada pelo diretório raiz da árvore),
e constrói o arquivo de sinopse a partir dos módulos presentes na
árvore. Uma possível sinopse antiga é removida.

.sp
As opções do comando são:

.in +3
.ip -r
Substitui ou inclui os <módulo>s da/na <biblioteca>, atualizando o arquivo
de sinopse. Os nomes dos <módulo>s devem ser dados relativos à raiz
da <biblioteca>, mas os <módulo>s propriamente ditos
devem estar no diretório corrente.

.sp
Por exemplo, para substituir/incluir o módulo "/lib/libc/etc/ftw.o"
da biblioteca "/lib/libc" deve ser dado o comando

.sp
.nf
			mklib -r /lib/libc etc/ftw.o
.fi

.sp
mas o módulo "ftw.o" deve estar no diretório corrente.

.ip -u
Esta opção é semelhante à "-r", mas ao invés de dar uma lista de
módulos, as substituições são feitas a partir de todos os módulos do
diretório corrente cujos nomes terminam por ".o".

.sp
A substituição é realizada apenas se um módulo com o mesmo nome
já se encontra na <biblioteca> (isto é, não são feitas inclusões)
e a data de modificação do
módulo do diretório corrente for mais recente. 

.sp
Repare que se a <biblioteca> tiver dois (ou mais) módulos homônimos
(em subdiretórios distintos) será considerado o primeiro encontrado
no arquivo de sinopse.

.ip -d
Remove da <biblioteca> os <módulo>s dados.

.ip -t
Imprime na saída padrão uma tabela dos módulos da <biblioteca>,
juntamente com seus símbolos.
Se forem dados nomes de <módulo>s, só são impressas informações sobre eles.
Em conjunto com a opção "-v",
produz uma listagem mais detalhada de cada módulo.

.ip -k
Realiza uma verificação de consistência do arquivo de sinopse da <biblioteca>.
Se hover alguma inconsistência, é necessário recriar a sinopse
(usando "mklib" sem opções).

.ip -f
Normalmente o usuário é consultado para confirmar cada
atualização ou remoção a ser feita. Com a opção "-f" estas operações
são feitas sem consulta.

.ip -v
Descreve detalhes das operações realizadas.
No caso da opção "-t", são impressas mais informações sobre os módulos.

.ip -c
Esta opção é uma abreviação para a <biblioteca> "/lib/libc".

.ip -m
Esta opção é uma abreviação para a <biblioteca> "/lib/libm".

.ep
.in -3

.in
.sp
.b OBSERVAÇÕES
.in 5
O arquivo de sinopse tem o nome de ".LIBSYM". Assim, por exemplo,
a sinopse da biblioteca "/lib/libc" é "/lib/libc/.LIBSYM".

.sp
Embora o comando "mklib" não exija que os nomes dos módulos
terminem em ".o" (exceto com a opção "-u"), é recomendável que
isto seja o caso, por questões de interação com outros comandos.

.sp
Os módulos objeto podem ficar distribuídos livremente pela árvore
da biblioteca. Nesta versão do sistema, não há mais limites no número
de subdiretórios (altura da árvore) nem nos tamanhos dos módulos/símbolos.

.bc	/*************************************/
Há no entanto, uma limitação da altura da árvore,
imposta pelo tamanho do caminho do módulo (relativo à biblioteca),
que é (nesta versão) de 29 caracteres. Assim, por exemplo, o módulo
da "libc" "/lib/libc/etc/ftw.o" é permitido, pois "etc/ftw.o" tem 9
caracteres, mas "/lib/libc/alpha/beta/gamma/delta/etc/ftw.o" não é,
pois "alpha/beta/gamma/delta/etc/ftw.o" tem 32 caracteres.
.ec	/*************************************/

.sp
Módulos cujo caminho contém um componente que começa por "." não
são considerados por "mklib" (tal como "/lib/libc/.old/ftw.o" ou
"/lib/libc/old/.ftw.o"). Esta característica pode ser usada
para "tornar invisíveis" módulos provisórios ou antigos.

.sp
O comando "mklib" mantém ordenado o arquivo de sinopse. Isto é
importante para que o comando "ld" crie os módulos executáveis
com os módulos sempre na mesma ordem, independentemente se
módulos foram inseridos, removidos ou substituídos na biblioteca.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ld
.br
.wo "(fmt): "
lib
.br

.in
.sp
.b ARQUIVOS
.in 5
<biblioteca>/.LIBSYM

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

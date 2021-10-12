.bp
.he 'TIC (cmd)'TROPIX: Manual de Referência'TIC (cmd)'
.fo 'Atualizado em 12.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
Compila e instala a descrição de um terminal/vídeo

.in
.sp
.b SINTAXE
.in 5
.(l
tic [-f] [-n <nome>] <fonte>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "tic" compila a descrição fonte de um terminal/vídeo,
dada em "<fonte>",
e instala a descrição objeto correspondente no arquivo apropriado,
de acordo com a variável "TERMINFO" do ambiente,
e as opções dadas para o programa.

.sp
A variável "TERMINFO" do ambiente define o diretório no qual o
arquivo será instalado; se ela não for definida será utilizado
o diretório padrão "/usr/lib/terminfo". 

.sp
As opções definem o nome do arquivo dentro do diretório:

.ip "--" 10
Sem nenhuma opção, "tic" instala a descrição objeto com o nome "test".
Este modo é destinado a testes; pode-se testar a descrição
ora compilada (sem alterar a descrição do terminal/vídeo em uso)
atribuindo-se à variavel do ambiente "TERM"
o valor "test". 
 
.ip "-n <nome>" 10
Com a opção "-n", a instalação é realizada com o nome dado
"<nome>".

.ip "-f" 10
A opção "-f" realiza a instalação definitiva, utilizando-se
os nomes contidos no próprio arquivo de descrição "<fonte>"
(exceto o último).

.ep

.in
.sp
.b OBSERVAÇÃO
.in 5
O programa verifica se uma descrição já existe antes de instalá-la
para evitar a perda acidental de uma descrição antiga. 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
tac
.br

.in
.sp
.b ARQUIVOS
.in 5
/usr/lib/terminfo

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

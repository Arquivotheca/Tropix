.bp
.he 'TAC (cmd)'TROPIX: Manual de Referência'TAC (cmd)'
.fo 'Atualizado em 12.07.95'Versão 3.0.1'Pag. %'

.b NOME
.in 5
.wo "tac -"
descompila a descrição instalada de um terminal/vídeo

.in
.sp
.b SINTAXE
.in 5
.(l
tac [<term> [<fonte>]]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "tac" descompila a descrição objeto instalada
de um terminal/vídeo de nome <term>
e escreve um fonte correspondente no arquivo <fonte>.

.sp
Se <term> não for dado, ou for dado como "-" será considerado
o terminal/vídeo "$TERM" do ambiente.

.sp
Se <fonte> não for dado, escreve na saída padrão.

.sp
O nome do terminal/vídeo deve ser dado sem o prefixo
(normalmente "/usr/lib/terminfo").
O arquivo contendo a descrição objeto instalada
é procurado no diretório dado pela variável "$TERMINFO" do ambiente;
Se ela não estiver definida
é utilizado o diretório padrão "/usr/lib/terminfo". 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
tic
.br

.in
.sp
.b ARQUIVOS
.in 5
/usr/lib/terminfo/*

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

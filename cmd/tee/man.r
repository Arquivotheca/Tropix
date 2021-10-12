.bp
.he 'TEE (cmd)'TROPIX: Manual de Referência'TEE (cmd)'
.fo 'Atualizado em 12.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "tee -"
copia a entrada padrão para
arquivos e a saida padrão
.br

.in
.sp
.b SINTAXE
.in 5
.(l
tee [-ai] [<arquivo> ...]
.)l

.in
.sp
.b DESCRICAO
.in 5
O comando "tee"  copia a entrada padrão para os diversos <arquivo>s
dados e a saida padrão.

.sp
As opções do comando são:

.in +3
.ip -a
Adiciona o conteúdo da entrada padrão ao final dos <arquivo>s
ao invés de reescreve-los.

.ip -i
Ignora interrupções.

.in -3
.ep
.sp

.in
.sp
.b EXEMPLOS
.in 7
O comando "tee" é normalmente conectado através de "pipes",
como no exemplo
.sp
.nf
		grep include *.c | tee inc_file 
.fi
.sp
Para examinar-se um "pipe" interno, podemos usar o arquivo "/dev/tty":
.sp
.nf
	walk -p "*.c" . | tee /dev/tty | grep -N include >inc_file
.fi

.in
.sp
.b OBSERVAÇÕES
.in 5
Deve-se lembrar que os "pipes" usam áreas de entrada/saída ("buffers")
normais (isto é, NÃO áreas de linha), e portanto,
a saída na tela é feita apenas quando a área
é completado (ao invés de escrever linha a linha).

.sp
Se não for dado nenhum <arquivo>, o comando simplesmente
copia a entrada padrão na saída padrão.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
sh
.br
.wo "(sys): "
pipe
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.
.in

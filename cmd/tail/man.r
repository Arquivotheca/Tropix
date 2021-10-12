.bp
.he 'TAIL (cmd)'TROPIX: Manual de Referência'TAIL (cmd)'
.fo 'Atualizado em 12.07.97'Versão 3.0.0'Pag. %'

.b NOME
.in 5
.wo "tail -"
obtém a parte final de um arquivo
.br

.in
.sp
.b SINTAXE
.in 5
.(l
tail [-b <n>[lbc]] [-f] [<arquivo>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "tail" copia a parte final do <arquivo> dado para a saída
padrão, a partir do local designado.
Se o nome do <arquivo> não for dado, será lida a entrada padrão.

.sp
A cópia começa normalmente na décima linha antes do final do arquivo
(isto é, são copiadas as suas últimas 10 linhas); veja a opção "-b".

.sp
As opções do comando são:

.in +3
.ip -b
Inicia a cópia a <n> linhas ("l"), blocos de 512 bytes ("b") ou
caracteres ("c") antes do final do <arquivo>.

.ip -f
Após copiar a parte final do <arquivo>, entra em uma malha sem fim
tentando, a cada segundo, copiar linhas adicionais do <arquivo>.
Esta opção só pode ser usada se o nome do <arquivo> for dado.

.sp
Esta opção é útil para monitorar o crescimento de um arquivo no qual
um outro processo está escrevendo.

.ep
.in -3

.in
.sp
.b OBSERVAÇÃO
.in 5
O comando "tail" trabalha com uma área interna de tamanho limitado,
e são copiados um máximo de 4096 bytes, independente do valor de <n>.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
dd
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

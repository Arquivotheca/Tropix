.bp
.he 'XEDIT (xwin)'TROPIX: Manual de Referência'XEDIT (xwin)'
.fo 'Atualizado em 14.10.01'Versão 4.2.0'Pag. %'

.b NOME
.in 5
.wo "xedit -"
editor de textos simples para o sistema X-Window
.br

.in
.sp
.b SINTAXE
.in 5
.(l
xedit [<arquivo>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O utilitário gráfico "xedit" edita o <arquivo> de nome dado.
Se o nome do <arquivo> NÃO for dado, ele pode ser dado na seção
de comandos (veja abaixo).

.sp
As janelas do utilitário gráfico "xedit" contém as seguintes informações:

.in +3
.ip 1. 3
Seção de comandos:
um conjunto de comandos com os quais podemos sair de "xedit", salvar o arquivo,
ou ler um novo arquivo para a janela de edição. O nome do arquivo deve ser
dado/é mostrado após os comandos.

.ip 2. 3
Janela de mensagens:
contém as diversas mensagens de "xedit".
.bc	/*************************************/
Adicionalmente, esta janela pode ser usada como scratch pad.
.ec	/*************************************/

.ip 3. 3
Nome do arquivo:
contém o nome do arquivo atualmente sendo editado. Este arquivo pode ser
editado com a permissão para leitura e escrita ou somente leitura.

.ip 4. 3
Janela de edição:
Contém o texto que está sendo editado ou criado.

.ep
.in -3

.sp
A edição do texto é realizada usando as 4 setas ou o "mouse" para movimentar o cursor,
as teclas <del> ou <bs> para apagar o caractere antes do cursor, e as demais
teclas para redigir o texto.

.bc	/*************************************/
.in
.sp
.b COMANDOS
.in 7
Quit
.in 5
Sai da seção atual do xedit. Se alguma alteração não foi salva, xedit mostra
uma mensagem de alerta, permitindo ao usuário salvar o arquivo.
.sp
.in 7
Save
.in 5
Salva o arquivo com o nome que está na caixa de texto ao lado do botão Load.
.sp
.in 7
Load
.in 5
Lê o arquivo com o nome que está na caixa de texto ao lado direito do botão
Load, e o mostra na janela dde edição. Se o arquivo atual foi modificado e
não salvo, uma mensagem de alerta vai dizer ao usuário para salvar as
alterações, ou apertar o botão Load novamente.
.br
.ec	/*************************************/

.in
.sp
.b OBSERVAÇÃO
.in 5
Na presente versão, "xedit" ainda não está possibilitando a utilização
de caracteres acentuados. Para tanto, é necessário usar "ed" ou "vi" (cmd).

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
ed, vi
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

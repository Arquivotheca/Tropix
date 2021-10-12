.bp
.he 'CPFS (cmd)'TROPIX: Manual de Referência'CPFS (cmd)'
.fo 'Atualizado em 15.01.99'Versão 3.1.0'Pag. %'

.b NOME
.in 5
.wo "cpfs -"
copia sistemas de arquivos
.br

.in
.sp
.b SINTAXE
.in 5
.(l
cpfs <fonte> <destino>
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "cpfs" copia sistemas de arquivos.
A cópia é feita "imagem", isto é, não é feita nenhuma
alteração no conteúdo copiado.

.sp
Tanto o sistema de arquivos <fonte> como o <destino> devem
ser dispositivos não estruturados (isto é, de caracteres).
Se os sistemas de arquivos não tiverem o mesmo tamanho,
o número de blocos copiados é o do menor sistema de arquivos.

.sp
O sistema de arquivo destino não pode estar montado.
É preferível que o sistema de arquivo fonte também esteja não montado,
no entanto a cópia é permitida, após uma advertência.

.in
.sp
.b OBSERVAÇÃO
.in 5
Nesta versão do comando, o conteúdo é copiado em unidades de 64 KB. 

.sp
Como a tabela de partições não é atualizada automaticamente,
pode ocorrer (após uma cópia) que o tipo do sistema de arquivos
não corresponda mais ao tipo indicado na tabela de partições.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
mkfs, fsck
.br

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

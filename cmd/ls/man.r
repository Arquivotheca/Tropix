.bp
.he 'LS (cmd)'TROPIX: Manual de Referência'LS (cmd)'
.fo 'Atualizado em 18.09.05'Versão 4.8.0'Pag. %'


.b NOME
.in 5
.wo "ls -"
lista características de arquivos e diretórios
.br

.in
.sp
.b SINTAXE
.in 5
.(l
ls [-lL3ditaRN] [<nome> ...]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O comando "ls" lista características de arquivos e conteúdos de diretórios.
Para cada <nome> dado, se for um arquivo normal (não diretório),
"ls" fornece uma série de características, e se for um diretório,
fornece as mesmas características para cada membro deste diretório.

.sp
Se não for dado nenhum <nome> (nem a opção "-N"), é considerado o
diretório corrente.

.sp 2
O formato da listagem é controlado pelas opções:

.in +3
.ip --
Simples: se não for dada nenhuma das 3 opções abaixo, a listagem contém
apenas os nomes dos arquivos/diretórios.

.ip -l
Longo: a listagem contém o tipo do arquivo, proteção,
número de elos físicos, o dono (UID), o grupo (GID), tamanho (bytes),
data da última modificação e o seu nome.
Para arquivos especiais, é dado (ao invés do tamanho),
a identificação numérica ("major" e "minor") do dispositivo.
Normalmente são impressos os nomes simbólicos do dono/grupo,
a não ser que eles não sejam encontrados nos arquivos correspondentes.

.ip -L
Muito longo: como o longo, mas com o modo completo (os 24 bites).

.ip -3
Tempos: a listagem contém o tipo do arquivo, data da criação,
data da última modificação, data do último acesso e o seu nome.

.ep
.in -3

.sp 2
Temos ainda, as seguintes outras opções:

.in +3
.ip -d
Se <nome> for um diretório, informa sobre o próprio diretório
(e não sobre o seu conteúdo); é normalmente usado
em conjunto com "-l" para obter o estado de um diretório.

.ip -i
Dá também o número do nó-índice do arquivo.

.ip -t
Ordena os arquivos pela data da última modificação, ao invés de
alfabeticamente pelo nome do arquivo, como é usual.

.ip -a
Dá informações sobre todas as entradas; normalmente as informações
sobre as entradas "." e ".." são suprimidas.

.ip -R
Lista os conteúdos dos subdiretórios encontrados recursivamente.
Deste modo, podemos obter características de todos os arquivos de uma árvore.

.ip -N
Lê os <nome>s dos arquivos da entrada padrão.

.ep
.in -3

.sp 2
O tipo/modo/proteção do arquivo dado pela opção "-L" tem a forma:
.nf

		"R 7654321mtsg rwxrwxrwx"
.fi

.sp
onde o primeiro caractere indica o tipo de arquivo:

.sp
.in +3
.wo 'R  '
Arquivo regular
.br
.wo 'B  '
Dispositivo especial de blocos (estruturado)
.br
.wo 'C  '
Dispositivo especial de caracteres (não estruturado)
.br
.wo 'L  '
Elo simbólico
.br
.wo 'D  '
Diretório
.br
.wo 'F  '
"Fifo"
.br
.wo '?  '
Arquivo de tipo inválido
.br
.in -3

.sp
Após o tipo do arquivo, temos então 11  caracteres relativos aos atributos
do arquivo. Se algum indicador estiver desligado, o caracter
assume o valor '-' ao invés do caracter indicado acima.
Os indicadores de nomes provisórios "7" a "1" ainda não são usados
e estão reservados para uso futuro. os 4 restantes são:

.in +3
.ip 'm  '
(S_IMETX)
Uma imagem do programa é guardada na memória interna
após a execução, mesmo que não haja mais usuários do programa.

.ip 't  '
(S_ISVTX)
Uma imagem do programa é guardada na área de SWAP (em disco)
após a execução, mesmo que não haja mais usuários do programa.

.ip 's  '
(S_ISUID)
O UID efetivo do processo passa a ser igual ao
dono do arquivo quando o arquivo é executado como um programa
("set user ID"). 

.ip 'g  '
(S_ISGID)
O GID efetivo do processo passa a ser igual ao
grupo do arquivo quando o arquivo é executado como um programa
("set group ID"). 

.ep
.in -3

.sp
Para mais informações sobre os diversos atributos, veja "chmod" (sys).

.sp
Os últimos 9 caracteres referem-se ao modo de proteção do arquivo,
dividido em 3 grupos de 3 caracteres.
O primeiro grupo de 3 caracteres dá a proteção para o dono. Esta proteção
contém a permissão para leitura (1. caractere), escrita (2. caractere)
e execução (arquivo regular)/pesquisa (diretório) (3. caractere) do arquivo,
de acordo com o seguinte código:

.sp
.in +9
.wo 'r  '
permissão para leitura
.br
.wo 'w  '
permissão para escrita
.br
.wo 'x  '
permissão para execução/pesquisa
.br
.wo '-  '
sem a permissão correspondente
.in -9

.sp
Analogamente, os três caracteres seguintes indicam a proteção
relativa a usuários do mesmo grupo do arquivo, e os últimos três
a proteção relativa os demais usuários.

.sp
O tipo/proteção do arquivo dado pela opção "-l" tem a forma:
.nf

		"R rwxrwxrwx"

.fi
que é uma simplificação da forma acima, onde o grupo central é suprimido.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
chmod
.br
.wo "(sys): "
intro, chmod
.br
.wo "(libc):"
modetostr
.br

.in
.sp
.b ARQUIVOS
.in 5
 /etc/passwd
 /etc/group

.in
.sp
.b EXEMPLO
.r
.in 5
Uma execução do comando "ls -l ls" poderá imprimir:
.br
.nf

	R rwxr-x--x 1 <root> <sys>  5288  21:26:05 2005 18 set  ls

.fi
o que significa que "ls" é um arquivo regular,
com permissão para leitura, escrita e execução para o dono,
leitura e execução para os usuários do mesmo grupo
e de execução para os outros usuários.
O dono do arquivo é "root" e o seu grupo é "sys".
Vemos ainda, que o arquivo possui 5288 bytes, e a última modificação
no arquivo foi feita às 21 horas, 26 minutos e 5 segundos de 18 de setembro de 2005.

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

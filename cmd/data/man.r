
.bp
.he 'DATA (cmd)'TROPIX: Manual de Referência'DATA (cmd)'
.fo 'Atualizado em 24.04.97'Versão 3.0.1'Pag. %'


.b NOME
.in 5
Mostra/atualiza a data/hora corrente do sistema:
.sp
.wo "data    -"
data e hora LOCAL em português
.br
.wo "date    -"
data e hora LOCAL em inglês
.br
.wo "datagmt -"
data e hora UT (GMT) em português
.br

.in
.sp
.b SINTAXE
.in 5
.(l
data    [[[[[<ano>]<mês>]<dia>]<hora>]<minuto>][:<segundo>]

date    [[[[[<ano>]<mês>]<dia>]<hora>]<minuto>][:<segundo>]

datagmt [[[[[<ano>]<mês>]<dia>]<hora>]<minuto>][:<segundo>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
Este comandos manipulam a data/hora corrente do sistema.
Para detalhes sobre o tempo padrão interno do sistema
e a diferença entre os tempos LOCAL e UT (Tempo Universal,
antes chamado de GMT) veja "ctime" (libc).

.sp
Sem argumentos, os comandos imprimem a data e hora corrente do sistema.
Os comandos "data" e "datagmt" apresentam a data/hora em português,
enquanto que "date" a apresenta em inglês.
O comando "data" e "date" dão a hora
LOCAL (levando em conta o fuso horário, veja "gettzmin" (sys)).

.sp
Se um argumento for dado, ele deve representar uma data/hora no formato
dado acima.
Esta data/hora é utilizada para atualizar o tempo padrão interno do sistema,
o que só pode ser feito pelo superusuário.

.sp
Cada componente do argumento (<ano>, <mês>, ...) deve ser dado
no formato de 2 dígitos decimais.
O caractere "." pode ser usado no lugar de ":".
Cada componente não dado será obtido a partir da data/hora corrente,
isto é, não será modificado.
Uma exceção é o componente <segundo>, que se não dado, será zerado.

.sp
Se a modificação da data/hora foi completada corretamente,
a nova data/hora será impressa, como descrito acima.

.in
.sp
.b OBSERVAÇÕES
.in 5
Os valores de <ano> podem variar de 70 a 99 (para indicar 1970 a 1999)
ou 00 a 37 (para indicar 2000 a 2037).

.sp
Não é recomendado alterar a data/hora em modo multiusuário.

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(sys):"
gettzmin
.br
.wo "(libc):"
ctime
.br
.wo "(fmt): "
ambiente
.br

.in
.sp
.(t
.b EXEMPLOS
.in 7
.nf
		date :27
.fi
.in 5
.sp
mantém a data, hora e minutos correntes, atualizando somente
os segundos.
.)t

.sp 2
.nf
		data 2246
.fi
.sp
atualiza a hora para 22 horas e 46 minutos do dia corrente zerando os segundos.

.sp 2
.nf
		data 8610141745:38
.fi
.sp
atualiza a data para 14 de outubro de 1986 e a hora para 17
horas e 45 minutos e 38 segundos.

.sp 2
.nf
		datagmt 1800
.fi
.sp
atualiza a data para 18 horas UT.


.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

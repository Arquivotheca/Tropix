
.bp
.sp 4
.nf
     *********************************************************
     *                                                       *
     *                      CAPÍTULO  4                      *
     *                                                       *
     *        ENTRADA E INSERÇÃO DE TEXTO VIA TECLADO        *
     *                                                       *
     *********************************************************
.fi
.sp 4
.pp
Este capítulo descreve a inclusão
de texto teclado no terminal no texto em edição.
A inclusão via teclado é chamada de
.b
inserção
.r
quando realizada a partir do modo Visual
e de
.b
entrada
.r
quando realizada a partir do modo Comandos.
Em ambos os casos, o texto digitado
é editado linha a linha,
permitindo ao usuário
corrigir erros de digitação em uma linha ainda não terminada.

.pp
A edição da entrada/inserção
é comandada pelos caracteres de edição (descritos abaixo);
os outros caracteres são simplesmente inseridos ou escritos
na posição apontada pelo cursor.
Os caracteres de edição não são ecoados,
mas seus efeitos são percebidos instantaneamente pelo usuário
através do movimento do cursor
e/ou da alteração da linha no vídeo do terminal.

.pp
Neste capítulo estão descritos todos os caracteres
que têm significado especial para a entrada e inserção de texto.
O nome que acompanha a explicação de cada caractere
é o nome através do qual ele pode ser alterado pelo comando "stty" (cmd).

.pp
Os caracteres de edição são apresentados a seguir:

.in +3
.ip <erase>: 11
Este caractere remove o caractere imediatamente à esquerda do cursor.
Valor normal: <del>.

.ip <aerase>: 11
Este caractere é uma alternativa ao caractere "erase".
Valor normal: <bs>.

.ip <word>: 11
Este caractere remove a palavra imediatamente à esquerda do cursor.
Uma palavra é definida como uma seqüência de caracteres
que não contenha <sp> nem <ht>.
Valor normal: <^W>.

.ip <kill>: 11
Este caractere remove toda a linha teclada até o momento.
Valor normal: <^X>.

.ip <retype>: 11
Este caractere reescreve o vídeo do terminal.
Ele é útil quando processos executando em "background"
enviam mensagens ao vídeo que interferem
na apresentação do texto em edição.
Valor normal: <^R>.

.ip <eol>: 11
No modo Inserção este caractere é usado para terminar a inserção.
Sem efeito no modo Entrada.
Valor normal: <esc>.

.ip <eof>: 11
Caractere alternativo a "eol".
Valor normal: <^D>.

.ip <nl>: 11
Este caractere indica o término da linha corrente
e o início de uma nova linha.

Quando um <nl> é teclado no modo Inserção
uma nova linha é inserida e o cursor é posicionado no seu início;
se existiam caracteres à direita do cursor
estes caracteres são transferidos para a nova linha inserida.

.ip <cr>: 11
Caractere alternativo a <nl>.

.ip ". (ponto):"  11
O modo Entrada termina quando uma linha contendo
apenas o caractere "." (ponto) é teclada.

.ip <^V>: 11
Este caractere anula o significado especial do caractere que o segue,
exceto <nl> e "." (ponto).

.ep
.in -3

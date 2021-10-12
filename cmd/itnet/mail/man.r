.bp
.he 'MAIL (cmd)'TROPIX: Manual de Referência'MAIL (cmd)'
.fo 'Atualizado em 12.08.01'Versão 4.0.0'Pag. %'

.b NOME
.in 5
.wo "mail -"
le ou envia cartas do correio eletrônico
.br

.in
.sp
.b SINTAXE
.in 5
.(l
mail [-ve] [-f <arquivo>] <destinatário> ...

mail [-rsu] [-d <caixa postal>]
.)l

.in
.sp
.b DESCRIÇÃO
.in 5
O programa "mail" le cartas recebidas através do sistema de correio eletrônico,
ou envia cartas para um ou mais <destinatário>s.

.sp
Se forem dados <destinatário>s, "mail" irá enviar uma carta;
em caso contrário irá abrir o índice de cartas recebidas.

.sp
As opções do programa são:

.in +3
.ip -v
Durante o processo de envio de cartas (ver abaixo), imprime o diálogo com os servidores 
contatados (através do protocolo SMTP).

.ip -e
Se for dado um <arquivo> contendo o texto a ser enviado, executa o programa "vi" para editá-lo
(veja a opção "-f", abaixo).

.ip -f
O conteúdo da carta é dado no <arquivo>, ao invés de ser teclado. As opções
"-e" e "-f" são normalmente usadas pelos comandos internos "r" e "f"
(veja abaixo).

.ip -r
Não modifica a caixa postal. Isto significa que não é permitido remover cartas, e o estado das
cartas (lida/não lida) na caixa postal não é atualizado.

.ip -s
Substitui a data da carta no índice de cartas pelo seu tamanho.

.ip -u
Mostra apenas cartas não lidas.

.ip -d
Abre a <caixa postal> dada, ao invés da caixa postal do usuário
(normalmente "/usr/spool/mail/$USER").

.ep
.in -3

.in
.sp
.b "LEITURA DE CARTAS RECEBIDAS"
.in 5
Sem opções nem argumentos, "mail" imprime a primeira página do índice
das cartas presentes na "caixa postal" do usuário (veja a opção "-d", acima).

.sp
O formato do índice contém as seguintes colunas:

.in +3
.ip 1. 3
A letra "U" indica que a carta é nova (ou seja, ainda não lida), e
a letra "R" indica que a carta é antiga (ou seja, já lida).

.ip 2. 3
O número da carta.

.ip 3. 3
O remetente da carta (exceto quando o remetente é o próprio
usuário; neste caso dá o destinatário).

.ip 4. 3
A data/hora de recebimento da carta (veja a opção "-s", acima).

.ip 5. 3
O assunto da carta.
 
.ep
.in -3

.sp
Para ler uma carta, basta usar as teclas "-" e/ou "+" para mover o cursor, e
teclar <enter> quando o cursor estiver apontando para a carta desejada.

.sp
Os comandos internos para avançar/retroceder a página visível do
índice de cartas na tela e outras funções são dados abaixo
(na seção "comandos internos").

.in
.sp
.b "ENVIO DE CARTAS"
.in 5
Dando um mais <destinatário>s, "mail" envia a carta para os <destinatário>s dados
(outros <destinatário>s adicionais podem ser dados posteriormente através de "cc").

.sp
Após certificar-se de que a sintaxe dos vários <destinatário>s está correta, "mail"
imprime a linha "Assunto:", e espera que seja digitado o assunto da carta
(que pode ter apenas uma linha).

.sp
.(t
Em seguida, deve ser digitado o conteúdo da carta. Durante este processo, podem ser dados
os seguinte comandos (sempre no início da linha):

.in +3
.ip ~v 13
Invoca o editor de textos "vi" (cmd) para editar o texto já digitado.
Isto é especialmente útil para efetuar correções no texto
(o "assunto" no entanto, NÃO pode ser mais modificado).
A edição termina com "w" e "q".
.)t

.ip "~r <arquivo>" 13
Insere o conteúdo do <arquivo> no texto sendo digitado.

.ep
.in -3

.sp
Para encerrar o texto a ser enviado, deve-se teclar "." (ou <^D>) sozinho em uma linha.

.sp
A seguir, "mail" imprime a linha "Anexos: ", e espera que sejam digitados um (ou mais)
nomes de arquivos que devem ser anexados à carta
(separados por vírgulas, brancos ou <ht>, mas em apenas uma linha.

.sp
Em seguida, "mail" imprime a linha "Cc: ", e espera que sejam digitados um (ou mais)
destinatários adicionais (separados por vírgulas, brancos ou <ht>, mas em apenas uma linha.

.sp
Finalmente, "mail" tenta enviar a carta para os diversos destinatários.
Se houver algum insucesso, uma cópia da carta é adicionada ao final do arquivo "dead.letter"
do diretório "HOME" do usuário.

.sp
Na presente versão, "mail" tenta enviar a carta apenas UMA vez, isto é, NÃO tentando
enviar outras vezes posteriormente.

.in
.sp
.b "COMANDOS INTERNOS"
.in 5
Durante a leitura das cartas recebidas, temos 3 tipos de tela:

.in +3
.ip a. 3
O índice de cartas:
é a lista de todas as cartas recebidas, conforme o formato dado acima
em "leitura de cartas recebidas".

.ip b. 3
O índice de partes:
Se uma certa carta conter "partes" (conforme a sintaxe MIME),
é dada uma lista das partes da carta.

.ip c. 3
O texto de uma carta: 
Se uma carta consistir apenas de um texto simples, ou então uma das partes
for legível, ela é apresentada na tela.

.ep
.in -3

.sp
Os comandos internos para efetuar diversas funções durante a leitura de
cartas recebidas são as seguintes:

.in +3

.ip 'u' 14
.it 'o' 14
.it '<nl>' 14
.it '<cr>' 14
.it '<enter>' 14
.it '<page down>' 14
Desce de nível:
Se estiver no índice de cartas, mostra a carta apontada pelo cursor;
se estiver em um índice de partes, mostra a parte apontada pelo cursor (se for um texto ou
outro índice de partes),
ou o grava em um arquivo (em caso contrário).

.ip 'i' 14
.it '<page up>' 14
Sobe de nível:
Retorna ao índice de cartas ou ao índice de partes superior.

.ip '+' 14
.it '=' 14
.it 'j' 14
.it <^E> 14
.it '<arrow down>' 14
Avança uma linha.

.ip '-' 14
.it '_' 14
.it 'k' 14
.it <^Y> 14
.it '<arrow up>' 14
Retrocede uma linha.

.ip 'l' 14
.it '<arrow right>' 14
Avança para o irmão seguinte de mesmo nível:
Se estiver no texto de uma parte, avança para a parte seguinte da mesma carta;
Se estiver no texto de uma carta, avança para a carta seguinte.

.ip 'h' 14
.it '<arrow left>' 14
Retrocede para o irmão anterior de mesmo nível:
Se estiver no texto de uma parte, retrocede para a parte anterior da mesma carta;
Se estiver no texto de uma carta, retrocede para a carta anterior.

.(t
.ip <^D> 14
Avança meia página.
.)t

.ip <^U> 14
Retrocede meia página.

.ip "<^F>" 14
Avança uma página.

.ip "<^B>" 14
Retrocede uma página.

.ip "[<n>] g" 14
.it "[<n>] G" 14
Se estiver em uma carta, avança/retrocede para a linha <n>;
se estiver no índice de cartas, avança/retrocede para a carta de número <n>;
se estiver em um índice de partes, avança/retrocede para a parte de número <n>;
Se <n> for omitido, avança para a última parte/linha do índice/carta.

.ip <home> 14
Retrocede para o início do índice/carta.

.ip <end> 14
Avança para o final do índice/carta.

.ip <^R> 14
Reescreve a tela.

.ip '/<cadeia>' 14
Busca de cadeia: procura a primeira ocorrência da
<cadeia> na carta/índice de cartas.

.ip 'n' 14
Repete a busca de cadeia avançando:
procura a ocorrência seguinte da
<cadeia> na carta/índice de cartas.

.ip 'N' 14
Repete a busca de cadeia retrocedendo:
procura a ocorrência anterior da
<cadeia> na carta/índice de cartas.

.ip 's <arquivo>' 14
Se estiver no texto de uma carta/parte, copia-a para o final do <arquivo> dado.
A carta na caixa postal não é modificada.

.ip 'r' 14
Responde a carta.
Se estiver no texto de uma carta/parte,
o utilitário "mail" é invocado com o <destinatário> desta carta.
O texto é incluído na carta a enviar, e ele é dado para ser editado através
de "vi" (cmd).

.ip 'f <dest>' 14
Envia a carta para o outro destinatário <dest>.

.ip 'd' 14
Remove a carta presente.
Ela só é removida após uma confirmação do usuário.

.ip '! <comando>' 14
Executa o <comando> através do "sh" (cmd).

.ip <^C> 14
.it 'q' 14
.it 'Q' 14
Encerra a execução de "mail".

.ep
.in -3

.in
.sp
.b 'ARQUIVO ".mailrc"'
.in 5
O arquivo ".mailrc" do diretório "HOME" do usuário permite a definição
de nomes simbólicos para <destinatário>s.

.sp
Deste modo, se o arquivo ".mailrc" contiver a linha

.sp
.nf
	alias	paulo	paulosq@cateto.com.br paulo@infor.org.br
.fi

.sp
usar o comando

.sp
.nf
	mail paulo
.fi

.sp
é equivalente a usar

.sp
.nf
	mail paulosq@cateto.com.br paulo@infor.org.br
.fi

.sp
Os diversos campos devem vir separados por <sp> ou <ht>.

.sp
A linha de "alias" pode possuir linhas de continuação da seguinte forma:

.sp
.nf
	alias	paulo	paulosq@cateto.com.br	\
			paulo@infor.org.br
.fi

.sp
Além disto, através das linhas "set from_with_host" ou "unset from_with_host"
podemos definir se desejamos (ou não) incluir o nome do computador no endereço
do remetente. Normalmente o nome do computador NÃO é incluído.

.sp
A forma "unset from_with_host" é util caso o arquivo "/etc/mailrc" (veja abaixo)
contenha a linha "set from_with_host", mas não desejamos incluir o nome do computador.

.in
.sp
.b 'ARQUIVO "/etc/mailrc"'
.in 5
O arquivo "/etc/mailrc" permite as mesmas definições do arquivo ".mailrc" descrito
acima, mas estas definições serão globais e válidas para todos os usuários do sistema. 

.in
.sp
.b
VEJA TAMBÉM
.r
.in 5
.wo "(cmd): "
vi, show
.br

.in
.sp
.(t
.b
REFERÊNCIAS
.r
.in 5
Postel, J: "Simple Mail Transfer Protocol", RFC 821, Agosto de 1982.
.)t
.sp
Crooker, D: "Standard for the Format of ARPA Internet Text Messages", RFC 822, Agosto de 1982.

.in
.sp
.(t
.b ARQUIVOS
.in 5
/usr/spool/mail/$USER
 $HOME/.mailrc
 /etc/mailrc
 $HOME/.deadletter
.)t

.in
.sp
.b ESTADO
.in 5
Efetivo.

.in

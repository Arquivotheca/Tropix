.bp
.sp 5

.nf
     *********************************************************
     *							     *
     *			    CAPÍTULO 1			     *
     *							     *
     *			    INTRODUÇÃO			     *
     *							     *
     *********************************************************

.fi

.sp 4


.pp
A "shell" é um interpretador de comandos programável, que serve de
interface entre o usuário e o sistema operacional TROPIX.
Após o "login", quando o usuário ganha acesso ao sistema,
a "shell" é  invocada automaticamente. Neste caso, uma indicação ("prompt")
é escrita no terminal e a "shell" está pronta para aceitar comandos.
Ao receber um nome, a "shell" procura um comando com este nome numa
seqüência de diretórios previamente definida e o executa.
Normalmente, a partir deste ponto, o comando executado ganha o controle do
terminal.
Ao término da execução do comando, 
o controle do terminal retorna à "shell".

.pp
A "shell" oferece ao usuário uma série de facilidades para
indicar a forma de execução dos comandos, basicamente:
redirecionamento da entrada e saída, comunicação entre
processos através de "pipes", substituição de variáveis,
expansão de nomes de arquivos, modificação do ambiente
dos comandos,  manutenção e edição de um histórico de comandos
e disponobilidade de estruturas de controle para programação.

.pp
Os comandos que, em geral, são lidos do terminal poderão também
ser provenientes de um arquivo de comandos, previamente preparado.

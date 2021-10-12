.bp
.sp 4
.nf
     *********************************************************
     *                                                       *
     *                      CAPÍTULO 5                       *
     *                                                       *
     *                 EXPRESSÕES REGULARES                  *
     *                                                       *
     *********************************************************
.fi
.sp 4

.pp
O editor "vi" suporta uma forma limitada de expressões regulares.
Elas são usadas no modo Comandos,
em endereços para especificar linhas
e no comando "s" (substitute)
para especificar a parte da linha que será substituída,
e no modo Visual,
para localização no texto em edição.

.pp
Uma expressão regular especifica um conjunto de cadeias de caracteres.
Dizemos que um membro deste conjunto de cadeias
corresponde à expressão regular.
As regras a seguir definem
as expressões regulares aceitas pelo editor:

.in +3
.(t
.ip 1. 4
Qualquer caractere (*) exceto um caractere especial corresponde a si próprio.
Caracteres especiais são o delimitador da expressão regular
e mais os caracteres "\", "[", "." e em alguns casos "^", "*" e "$".
.)t

.(f
A palavra "caractere"
significa qualquer caractere do conjunto ISO à exceção do <nl>.
.)f

.(t
.ip 2. 4
Um "\" (barra invertida) seguido de qualquer caractere corresponde
ao próprio caractere.
O significado especial dos caracteres especiais
é anulado quando eles são precedidos por uma barra invertida.
.)t

.(t
.ip 3. 4
Um "." (ponto) corresponde a qualquer caractere.
.)t

.(t
.ip 4. 4
Uma cadeia não vazia "s" entre colchetes "[s]" (ou "[^s]")
corresponde a qualquer caractere presente (ou ausente) em "s".
Uma sub-cadeia na forma "a-b" dentro de "s",
com "a" e "b" em ordem crescente ISO,
significa o conjunto de caracteres ISO entre "a" e "b", inclusive.
Os caracteres "]", "-" e "\" podem ser especificados precedendo-os com "\".
.)t

.(t
.ip 5. 4
Uma expressão regular da forma 1 até 4 seguido de "*"
corresponde a zero ou mais ocorrências da expressão regular.
.)t

.(t
.ip 6. 4
Uma expressão regular "x" da forma 1 até 6, seguida
de uma expressão regular "y" da forma 1 até 5
corresponde a uma ocorrência de "x" seguida de uma ocorrência de "y"
com a correspondência para "x" sendo a mais longa
que permita a correspondência para "y".
.)t

.(t
.ip 7. 4
Uma expressão regular da forma 1 até 6 precedida por "^"
(ou seguida de "$"),
é restrita a correspondências no início (ou no fim)
de uma linha.
.)t

.(t
.ip 8. 4
Uma expressão regular da forma 1 até 7 corresponde
á mais longa das correspondências mais à esquerda da linha.
.)t

.(t
.ip 9. 4
Uma expressão regular vazia
equivale a cópia da última expressão regular procurada.
.)t
.ep
.in -3

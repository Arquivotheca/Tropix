echo -n "Entre com uma cadeia: ";  read x
case $x in
        [0-9]* ) echo Começa com dígito ;;
	a*     ) echo Começa com a ;;
	b*     ) echo Começa com b ;;
	*      ) echo Não começa nem com a nem com b ;;
esac

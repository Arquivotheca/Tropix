while true do
   #
	echo -n 'Continua? '
#
#	Corpo do WHILE.
#
	if askyesno -s then
#
		echo '   Início do FOR'
#
		for i in * do
			echo -n '   Próximo arquivo? '
			if askyesno -s then
				echo '      ' $i
			else
				break
			fi
		done
#
		echo '   Fim do FOR'
	else
		break
	fi
#
	echo 'Mais uma iteração do while completa'
done

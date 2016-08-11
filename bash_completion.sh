# bash completion for openvpn init.d script
# Written by Alberto Gonzalez Iniesta <agi@inittab.org>

_minecraft()
{

	local cur 

	COMPREPLY=()
	cur=${COMP_WORDS[COMP_CWORD]}
	if [ $COMP_CWORD -eq 1 ] ; then
		COMPREPLY=( $( compgen -W '$( /usr/local/bin/minecraft 2>&1 \
				| cut -d"{" -f2 | tr -d "}" | tr "|" " " | tr " " "\n" \
				| sed "s/\<SERVER\>/ /" | sed "s/\<COMMAND\>/ /" \
				| tr -d "<" | tr -d ">" \
				| sed "s/\[PLAYER\]/ /" | tr "\n" " ")' -- $cur ) )
	elif [ $COMP_CWORD -eq 2 ] ; then
		COMPREPLY=( $( compgen -W '$(sudo /usr/local/bin/minecraft list 2>&1)' -- $cur ) )
	fi
}


complete -F _minecraft /usr/local/bin/minecraft
complete -F _minecraft minecraft

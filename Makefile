build-minishell:
	gcc -o minishell minishell.c

clean-minishell:
	rm -f minishell *.o

build-strtok_example:
	gcc -o strtok_example strtok_example.c
	
clean-strtok_example:
	rm -f strtok_example *.o
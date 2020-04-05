#include "stdio.h"
#include "stdlib.h"

/**
    Henry Knoll
    3/3/20
    ATCS Year 4 Semester 2
    D Block

    In this program, I will be creating an interpreter for the Brazilian Fumarole language. The language
    consists of a series of runes that each execute different functions on a group of tribesmen and their pebbles.
**/

#define MAX_TRIBESMEN 50000 //Maximum number of tribesmen in the tribe
#define MAX_RUNES 10000	//Maximum number of runes in a ritual

/** I will simplify a group of adjacent runes into being one "token". This token will consist of the type of rune
	itself and the amount of times that it appears in a row.
**/
struct TokenStruct 
{
	char rune;
	int amount;
};
typedef struct TokenStruct token;

/** This function adds "amount" pebbles to the current bag, making sure to wrap around 255 pebbles. 
**/ 
void add(char* bag, int amount) 
{
	int bag_int = ((int) (*bag)) + amount;
	if (bag_int > 255) 
	{
		*bag = (char) (bag_int - 256);
	} 
	else 
	{
		*bag = (char) bag_int;
	}
}

/** This function subtracts "amount" pebbles to the current bag, making sure to wrap around 0 pebbles. 
**/ 
void subtract(char* bag, int amount) 
{
	int bag_int = ((int) (*bag)) - amount;
	if (bag_int < 0) 
	{
		*bag = (char) (256 + bag_int);
	} 
	else 
	{
		*bag = (char) bag_int;
	}
}

/** This function creates a pointer to a new token in space
**/
token* create_token(char rune, int amount)
{
	token* t = (token*) malloc(sizeof(token));
	t->rune = rune;
	t->amount = amount;
	return t;
}

/** This function parses through the bf code and simplifies the runes into being tokens. It will throw an
	error if there is any mistake in parentheses nesting.
**/ 
void parse(char* bf_code, token** tokens) 
{
	FILE* bf = fopen(bf_code, "r");
	int index = -1;
	int parens_count = 0;

	char rune;
	while ((rune = fgetc(bf)) != EOF) 
	{
		//Loop through each rune until the end of the file
		if (rune == '>' || rune == '<' || rune == '+' || rune == '-' || rune == '*') 
		{
			if (index >= 0) 
			{
				//See if rune is the same as the previous rune
				token* last_token = *(tokens + index);
				if (rune != (*last_token).rune) 
				{
					//Make a brand new token since it is a new rune
					index += 1;
		  			*(tokens + index) = create_token(rune, 1);
		  		} 
		  		else if (rune == (*last_token).rune) 
	  			{
	  				//Update the current token since it is the same rune
					(*(tokens + index))->amount = (*(*(tokens + index))).amount + 1;
		  		}
		  	}
		  	else 
	  		{
	  			//First token of the parse
				index += 1;
		  		*(tokens + index) = create_token(rune, 1);
		  	}
		} 
		//Parentheses are the only runes that aren't grouped together in one single token if there
		//are multiple adjacent ones
		else if (rune == '(') 
		{
			index += 1;
			parens_count += 1;
	  		*(tokens + index) = create_token(rune, 1);
		} 
		else if (rune == ')') 
		{
			index += 1;
			parens_count -= 1;
			if (parens_count < 0) 
			{
				printf("Invalid parentheses nesting. Quitting program.\n");
				exit(0);
			}
	  		*(tokens + index) = create_token(rune, 1);
		}	
	}

	//Add an end token so that we don't have to iterate through the entire array of MAX_RUNES size
	index += 1;
	token* end_token = (token*) malloc(sizeof(token));
	end_token->rune = 'E';
	end_token->amount = 1;
	*(tokens + index) = end_token;
	fclose(bf);
}

/** Need the header here since I access the parentheses() function in execute_token()
**/
void parentheses(token** tokens, int* token_position, char* tribesmen, int* tribesmen_index);

/** Executes a single token based on its amount and its rune. Special function for parentheses.
**/
void execute_token(token** tokens, int* token_position, char* tribesmen, int* tribesmen_index, char rune, int amount) 
{
	//Geaux Tigers
	switch (rune) 
	{
	case '>': 
		//Move to next tribesman
		*tribesmen_index += amount;
		if (*tribesmen_index >= MAX_TRIBESMEN) {
			printf("Overwrapped. Ended program.\n");
			exit(0);
		}
		break;
	case '<': 
		//Move to previous tribesman
		*tribesmen_index -= amount;
		if (*tribesmen_index < 0) 
		{
			printf("Underwrapped. Ended program.\n");
			exit(0);
		}
		break;
	case '+':
		//Add a pebble to current tribesman
		add((tribesmen + *tribesmen_index), amount);
		break;
	case '-':
		//Subtract a pebble from the current tribesman
		subtract((tribesmen + *tribesmen_index), amount);
		break;
	case '*':
		//Print out the ASCII value of the amount of the current tribesman's pebbles 
		for (int i = 0; i < amount; i++) 
		{
			printf("%c", *(tribesmen + *tribesmen_index));
		}
		break;
	case '(':
		//Parentheses more complicated, see next function
		parentheses(tokens, token_position, tribesmen, tribesmen_index);
		break;
	default:
		break;
	}

	//Increase token position since we can move to the next one
	*token_position = *token_position + 1;
}

/** This function deals with cases of parentheses and uses recursion for nested parentheses. The
	parentheses runes are basically the bounds of a while loop that will operate while a certain
	tribesman's pebbles aren't equal to zero.
**/
void parentheses(token** tokens, int* token_position, char* tribesmen, int* tribesmen_index) 
{
	int my_initial_token_position = *token_position;
	int current_tribesmen_int_value;
	if ((current_tribesmen_int_value = (int) (*(tribesmen + *tribesmen_index))) != 0) 
	{
		//Execute the tokens within the parentheses
		*token_position += 1;
		token current;
		while ((current = **(tokens + *token_position)).rune != ')') 
		{
			execute_token(tokens, token_position, tribesmen, tribesmen_index, (current).rune, (current).amount);
		}
		//Reached the end of the parentheses bound. Now check if the current tribesman has zero pebbles.
		//If he doesn't, then we need to run through the parentheses all over again through recursion.
		if (current_tribesmen_int_value != 0) 
		{
			token_position = &my_initial_token_position;
			parentheses(tokens, token_position, tribesmen, tribesmen_index);
		}
	} 
	else 
	{
		//Skip through the tokens in between the parentheses, even nested parentheses
		int parentheses_balance = 1;
		while (parentheses_balance != 0) 
		{
			*token_position += 1;
			char current_rune = (**(tokens + *token_position)).rune;
			if (current_rune == ')') 
			{
				parentheses_balance -= 1;
			} 
			else if (current_rune == '(') 
			{
				parentheses_balance += 1;
			}
		}
	}
}

/** The beginning function that starts running the Brazilian Fumarole code after it is parsed
**/
void run(token** tokens, int* token_position, char* tribesmen, int* tribesmen_index) 
{
	token current;
	while ((current = **(tokens + *token_position)).rune != 'E') 
	{
		//Run every token until the 'E' token, which is the ending marker
		execute_token(tokens, token_position, tribesmen, tribesmen_index, (current).rune, (current).amount);
	}
}

/** Destroy each of the tokens in the token array that we created to clear space when we are done
**/
void destroy(token** tokens) 
{
	int i = 0;
	while ((**(tokens + i)).rune != 'E') 
	{
		free(*(tokens + i));
		i += 1;
	}

	//Free the last token, the 'E' token
	free(*(tokens + i));
}

int main(int argc, char** argv) 
{
	//File is the first argument in the command line
	char* filename = argv[1];

	//The two arrays we will use, one for the tribesmen and one for the tokens
	char tribesmen[MAX_TRIBESMEN] = {0};
	token** tokens = (token**) malloc(MAX_RUNES * sizeof(token*));;
	
	int tokens_start = 0;
	int tribesmen_start = 0;

	//Parse the bf file, run the bf file, clear space created from the bf file
	parse(filename, tokens);
	run(tokens, &tokens_start, tribesmen, &tribesmen_start);
	destroy(tokens);
	//And we're done!
}
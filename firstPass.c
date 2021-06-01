#include "main.h"

enum status {PREFIRSTWORD,FIRSTWORD,POSTFIRSTWORD,POSTCOMMAND,POSTDIRECTIVE,POSTLABEL,POSTEXTERN,POSTENTRY,COMMANDORDIRECTIVE};

static char *directives[]={".db",".dw", ".dh", ".asciz"};

static char *commands[]={"add","sub", "and", "or", "nor", "move", "mvhi","mvlo", "addi", "subi", "andi", "ori","nori", "bne", "beq", "blt", "bgt","lb", "sb", "lw", "sw", "lh","sh", "jmp", "la", "call", "stop"};

int checkState(char *ptrInput);


void firstPass(NODE_T *ptrNode){
	int index,firstWordIndex,labelFlag,labelIndex,midLabel,CommandDirectiveIndex,DC,IC;
	NODE_T *current;
	char *ptrFirstWord;
	char *ptrLabel;
	char *ptrCommandDirective;

	enum status state;

	index = 0;
	current = ptrNode;
	labelFlag = FLAGOFF;
	midLabel = FLAGOFF;
	state = PREFIRSTWORD;

	IC = 100;
	DC = 0;
	while(1){

		if(current->inputChar[index] == '\0'){
			if(current->next == NULL){
				break;
			}
			else{
				current = current->next;
				state = PREFIRSTWORD;
				labelFlag = FLAGOFF;
				index = 0;

			}
		}
		if(current->inputChar[index] == '\t' || current->inputChar[index] == ' '){
			if(state == FIRSTWORD){
				state = POSTFIRSTWORD;
			}
			if(state == COMMANDORDIRECTIVE){
				ptrCommandDirective[CommandDirectiveIndex] = '\0';
				state = checkState(ptrCommandDirective);
				if(state == POSTDIRECTIVE){
					printf("%s \n",ptrCommandDirective);
				}
				else if(state == POSTCOMMAND){
					printf("%s \n",ptrCommandDirective);

				}
				else{
					printf("Error: Invalid directive or command \n");

				}
				exit(0);
			}
			else if((state == POSTEXTERN || state == POSTENTRY) && (midLabel == FLAGON)){
				ptrLabel[labelIndex] = '\0';
				midLabel = FLAGOFF;
				/*printf("%s \n",ptrFirstWord);
				printf("%s \n",ptrLabel);*/
				free(ptrFirstWord);
				free(ptrLabel);
				current = current->next;
				state = PREFIRSTWORD;
				labelFlag = FLAGOFF;
				index = 0;
				continue;
			} 
			else{
				index++;
				continue;
			}
		}
		if(current->inputChar[index] == ';' && state == PREFIRSTWORD){
			current = current->next;
			labelFlag = FLAGOFF;
			index = 0;
			continue;
		}
		if(current->inputChar[index] == '\n'){
			if((state == POSTEXTERN || state == POSTENTRY) && (midLabel == FLAGON)){
				ptrLabel[labelIndex] = '\0';
				midLabel = FLAGOFF;
				/*printf("%s \n",ptrFirstWord);
				printf("%s \n",ptrLabel);*/
				free(ptrFirstWord);
				free(ptrLabel);
			} 
			current = current->next;
			state = PREFIRSTWORD;
			labelFlag = FLAGOFF;
			index = 0;
			continue;
		}
		if(state == FIRSTWORD && current->inputChar[index] == ':'){
			state = POSTFIRSTWORD;
			labelFlag = FLAGON;
		}




		switch(state){
			case PREFIRSTWORD:
				/*printf("entered PREFIRSTWORD\n");*/
				ptrFirstWord = calloc(MAXLABELLEN,sizeof(char));
				firstWordIndex = 0;
				ptrFirstWord[firstWordIndex] = current->inputChar[index];
				firstWordIndex++;
				state = FIRSTWORD;
				index++;
				break;
			case FIRSTWORD:
				/*printf("entered FIRSTWORD\n");*/
				ptrFirstWord[firstWordIndex] = current->inputChar[index];
				firstWordIndex++;
				index++;
				break;
			case POSTFIRSTWORD:
				/*printf("entered POSTFIRSTWORD\n");*/
				ptrFirstWord[firstWordIndex] = '\0';
				/*printf("%s \n",ptrFirstWord);*/
				if(labelFlag == FLAGON){
					state = POSTLABEL;
				}
				else{
					state = checkState(ptrFirstWord);
					if(state == -1){
						printf("Error in Line %u: Invalid State \n",current->lineNumber);
						exit(0);
					}
				}
				switch(state){
					case PREFIRSTWORD:
						break;
					case FIRSTWORD:
						break;
					case POSTFIRSTWORD:
						break;
					case POSTDIRECTIVE:
						index++;
						break;
					case POSTEXTERN:
						ptrLabel = calloc(MAXLABELLEN,sizeof(char));
						labelIndex = 0;
						index++;
						break;
					case POSTENTRY:
						ptrLabel = calloc(MAXLABELLEN,sizeof(char));
						labelIndex = 0;
						index++;
						break;
					case POSTCOMMAND:
						index++;
						break;
					case POSTLABEL:
						index++;
						break;
					case COMMANDORDIRECTIVE:
						break;
				}
				break;
			case POSTCOMMAND:
				break;
			case POSTDIRECTIVE:
				break;
			case POSTLABEL:
				ptrCommandDirective = calloc(MAXDATACODELEN,sizeof(char));
				CommandDirectiveIndex = 0;
				ptrCommandDirective[CommandDirectiveIndex] = current->inputChar[index];
				CommandDirectiveIndex++;
				state = COMMANDORDIRECTIVE;
				index++;

				break;
			case POSTEXTERN:
				midLabel = FLAGON;
				ptrLabel[labelIndex] = current->inputChar[index];
				labelIndex++;
				index++;
				break;
			case POSTENTRY:
				midLabel = FLAGON;
				ptrLabel[labelIndex] = current->inputChar[index];
				labelIndex++;
				index++;
				break;
			case COMMANDORDIRECTIVE:
				ptrCommandDirective[CommandDirectiveIndex] = current->inputChar[index];
				CommandDirectiveIndex++;
				index++;
				break;
			
		}		

	}

}


int checkState(char *ptrInput){
	int i;
	enum status state;

	for(i=0;i<NUMDIRECTIVES;i++){
		if(!strcmp(directives[i], ptrInput)){
			state = POSTDIRECTIVE;
			return state;
		}
	}
	if(!strcmp(".extern", ptrInput)){
		state = POSTEXTERN;
		return state;
	}
	if(!strcmp(".entry", ptrInput)){
		state = POSTENTRY;
		return state;
	}
	for(i=0;i<NUMCOMMANDS;i++){
		if(!strcmp(commands[i], ptrInput)){
			state = POSTCOMMAND;
			return state;
		}
	}
	return -1;
}






#include "analizador.h"
#include <stdio.h>
#include <locale.h>


int cantidadError=0;	// Cantidad de errores
char conjuntosPrimero[11][7][15];//conjuntos primeros de la gramatica dada
char conjuntosSegundo[11][4][15];//Conjuntos segundos de la gramatica dada
typedef int bool;		// Tipo de Datos booleanos
enum { false, true };	// Tipos booleanos
char token[50];
FILE *tokens;
int linea = 1;
int vacio = 0;
int volver = 0;
void descendenteRecursivo()
{
	if (!(tokens = fopen("output.txt", "r")))
	{
		printf("Archivo no encontrado.\n");
		exit(1);
	}
    get_token();
    element();
	match("EOF");
	
	if (cantidadError == 0){
		printf("\nEl codigo fuente es sintacticamente correcto.\n");
	}else{
		printf("\nEl codigo fuente no es sintacticamente correcto.\n");
	}
	fclose(tokens);
	remove("output.txt");
}
void get_token()
{
    // contar linea
    char ch;
	int i = 0;
	vacio = 0;
	ch = fgetc(tokens);
	
	
	while (vacio == 0){
		if (ch == ' ' || ch == '\t'){
			ch = fgetc(tokens);
		}else if(ch == '\n')
        {
			linea += 1;
            ch = fgetc(tokens);
        }else{
			vacio = 1;
		}
	}

    do
    {
        if(ch == '\n')
        {
			linea += 1;
            break;
        }
		token[i] = ch;
		i++;
		ch = fgetc(tokens);
		
    }while(ch != ' ');
	token[i]='\0';
    ungetc(ch, tokens);
}


void attribute_value(){
	if(strcmp("LITERAL_CADENA",token)==0){
		match("LITERAL_CADENA");
	}
	else if(strcmp("LITERAL_NUM",token)==0){
		match("LITERAL_NUM");
	}
	else if(strcmp("PR_TRUE",token)==0){
		match("PR_TRUE");
	}
	else if(strcmp("PR_FALSE",token)==0){
		match("PR_FALSE");
	}
	else if(strcmp("PR_NULL",token)==0){
		match("PR_NULL");
	}
	else if (strcmp("L_CORCHETE",token)==0){
		element();
	}
	else if (strcmp("L_LLAVE",token)==0){
		element();
	}
	else {
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver =panicModeConSincronizacion(8,token);
		if (volver == 1){
			attribute_value();
		}
	}
		
}


void attribute_name(){
	if(strcmp("LITERAL_CADENA",token)==0){
		match("LITERAL_CADENA");
	}else {
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(7,token);
		if (volver == 1){
			attribute_name();
		}
	}
}

void attribute(){
	if (strcmp("LITERAL_CADENA",token)==0){
		attribute_name();
		match("DOS_PUNTOS");
		attribute_value();
	}
	else {
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(6,token);
		if (volver == 1){
			attribute();
		}
	}
}

void a(){
	if(strcmp("COMA",token)==0){
		match("COMA");
		attribute();
		a();
	}else{	
		bool existeSeg = existeTokenSegundo(5,token);
		if(!existeSeg){
			if (!strcmp(token,"EOF")==0){
				cantidadError = cantidadError + 1;
				printf("\nError Sintactico \n");
				scan();
				a();
			}
		}
	}
}

void array(){
	if(strcmp("L_CORCHETE",token)==0)
	{
		match("L_CORCHETE");
		if(!strcmp("R_CORCHETE",token)==0){
			element_list();
			match("R_CORCHETE");
		}
		else
		{
			match("R_CORCHETE");
		}
	}
	else
	{
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(3,token);
		if (volver == 1){
			array();
		}
	}
}

void object(){
	if (strcmp("L_LLAVE",token)==0){
		match("L_LLAVE");
		if (!strcmp("R_LLAVE",token)==0){
			attributes_list();
			match("R_LLAVE");
		}
		else{
			match("R_LLAVE");
		}
	} else{
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(2,token);
		if (volver == 1){
			object();
		}
	}
}

void element()
{		
		if (strcmp("L_LLAVE",token)==0)
		{
			object();
		}
		else if(strcmp("L_CORCHETE",token)==0)
		{
			array();
		}
		else
		{	
			cantidadError = cantidadError + 1;
			printf("\nError Sintactico \n");
			volver = panicModeConSincronizacion(1,token);
			if (volver == 1){
				element();
			}
		}
	
}

void attributes_list(){
	if(strcmp("LITERAL_CADENA",token)==0){
		attribute();
		a();
	}
	else{
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(4,token);
		if (volver == 1){
			attributes_list();
		}
	}
}

void element_list(){
	if(strcmp("L_CORCHETE",token)==0 || strcmp("L_LLAVE",token)==0){
		element();
		e();
	}
	else {
		cantidadError = cantidadError + 1;
		printf("\nError Sintactico \n");
		volver = panicModeConSincronizacion(9,token);
		if (volver == 1){
			element_list();
		}
	}
}	

void e(){
	if (strcmp("COMA",token)==0){
		match("COMA");
		element();
		e();
	}else{
		bool existeSeg = existeTokenSegundo(10,token);
		if(!existeSeg){
			if (!strcmp(token,"EOF")==0){
				cantidadError = cantidadError + 1;
				printf("\nError Sintactico \n");
				scan();
				e();
			}
		}
	}
}

void match(char* proximoToken){
	if (strcmp(token,proximoToken) == 0){
		if (!strcmp(token,"EOF") == 0){
			get_token();
		}
	}
	else{
		cantidadError = cantidadError + 1;
		printf("\nError en linea %d se esperaba: %s, en vez de %s \n",linea, proximoToken, token);
	}
	
}

void initConjuntosPrimero(){
	int i=0;
	//carga los conjuntos primeros en la matriz
	/*	0:json
		1:element
		2:object
		3:array
		4:attributes_list
		5:a
		6:attribute
		7:attribute_name
		8:attribute_value
		9:element_list
		10:e
	*/
		strcpy(conjuntosPrimero[1][0],"L_CORCHETE");
		strcpy(conjuntosPrimero[1][1],"L_LLAVE");
		strcpy(conjuntosPrimero[2][0],"L_LLAVE");
		strcpy(conjuntosPrimero[3][0],"L_CORCHETE");
		strcpy(conjuntosPrimero[4][0],"LITERAL_CADENA");
		strcpy(conjuntosPrimero[5][0],"COMA");
		strcpy(conjuntosPrimero[5][1],"EMPTY");
		strcpy(conjuntosPrimero[6][0],"LITERAL_CADENA");
		strcpy(conjuntosPrimero[7][0],"LITERAL_CADENA");
		strcpy(conjuntosPrimero[8][0],"LITERAL_CADENA");
		strcpy(conjuntosPrimero[8][1],"LITERAL_NUM");
		strcpy(conjuntosPrimero[8][2],"PR_TRUE");
		strcpy(conjuntosPrimero[8][3],"PR_FALSE");
		strcpy(conjuntosPrimero[8][4],"PR_NULL");
		strcpy(conjuntosPrimero[8][5],"L_CORCHETE");
		strcpy(conjuntosPrimero[8][6],"L_LLAVE");
		strcpy(conjuntosPrimero[9][0],"L_CORCHETE");
		strcpy(conjuntosPrimero[9][1],"L_LLAVE");
		strcpy(conjuntosPrimero[10][0],"COMA");
		strcpy(conjuntosPrimero[10][1],"EMPTY");
}

void initConjuntosSegundo(){
	int i=0;
	//carga los conjuntos segundos en la matriz
	/*	0:json
		1:element
		2:object
		3:array
		4:attributes_list
		5:a
		6:attribute
		7:attribute_name
		8:attribute_value
		9:element_list
		10:e
	*/
	
		
	strcpy(conjuntosSegundo[1][0],"EOF");
	strcpy(conjuntosSegundo[1][1],"COMA");
	strcpy(conjuntosSegundo[1][2],"R_LLAVE");
	strcpy(conjuntosSegundo[1][3],"R_CORCHETE");
	strcpy(conjuntosSegundo[2][0],"COMA");
	strcpy(conjuntosSegundo[2][1],"EOF");
	strcpy(conjuntosSegundo[2][2],"R_LLAVE");
	strcpy(conjuntosSegundo[2][3],"R_CORCHETE");
	strcpy(conjuntosSegundo[3][0],"COMA");
	strcpy(conjuntosSegundo[3][1],"EOF");
	strcpy(conjuntosSegundo[3][2],"R_LLAVE");
	strcpy(conjuntosSegundo[3][3],"R_CORCHETE");
	strcpy(conjuntosSegundo[4][0],"R_LLAVE");
	strcpy(conjuntosSegundo[5][0],"R_LLAVE");
	strcpy(conjuntosSegundo[6][0],"COMA");
	strcpy(conjuntosSegundo[6][1],"R_LLAVE");
	strcpy(conjuntosSegundo[7][0],"DOS_PUNTOS");
	strcpy(conjuntosSegundo[8][0],"COMA");
	strcpy(conjuntosSegundo[8][1],"R_LLAVE");
	strcpy(conjuntosSegundo[9][0],"R_CORCHETE");
	strcpy(conjuntosSegundo[10][0],"R_CORCHETE");

}


bool existeTokenPrimero(int produccion, char* tokenActual){
	
	int i=0;
	for(i=0; i<7; i++){
		//busca el token actual en la matriz de conjunto primero
		if(strcmp(tokenActual,conjuntosPrimero[produccion][i])==0){
			return true;
		}
	}
	return false;
}

bool existeTokenSegundo(int produccion, char* tokenActual){
	
	int i=0;
	for(i=0; i<4; i++){
		//busca el token actual en la matriz de conjunto segundo
		if(strcmp(tokenActual,conjuntosSegundo[produccion][i])==0){
			return true;
		}
	}
	return false;
}	

int panicModeConSincronizacion(int produccion, char *tokenActual){
	
	bool existeSegundo = existeTokenSegundo(produccion,tokenActual);
	if(!existeSegundo){
		bool existePrimero = existeTokenPrimero(produccion,tokenActual);
		if (!existePrimero){
			scan();
			return 1;
		}
	}else if(existeSegundo || strcmp(tokenActual,"EOF")==0){
		pop();
		return 0;
	}

}

void scan(){
	int k = 0;
	int ban = 0;
	for(k=0; k<12; k++){
		if(strcmp(token,comp_lex[k])==0){
			ban = 1;
		}
	}
	if (ban == 0){
		printf("En linea %d, %s \n", linea, token);
	}else{
		printf("SCAN. En linea %d, token %s \n", linea, token);
	}
	get_token();
}

void pop(){
	printf("POP. En linea %d, token %s \n", linea, token);
	//genera una produccion con EMPTY
}

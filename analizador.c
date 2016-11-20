/*---------- 	ANALIZADOR LEXICO ----------*/

/**---------- Inclusión de la cabecera ----------**/

#include "analizador.h"

#ifdef __linux__
#define AUX "/dev/tty"
#else 
#define AUX "CON"
#endif 

/**---------- Variables globales ----------**/

int consumir;

char cad[5*TAMLEX];
token1 t;

FILE *archivo; // Fuente JSON
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea

/**---------- Funciones ----------**/

void error(const char* mensaje)
{
	printf("Error_Lexico %s  ",mensaje);
}

void anaLex()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF)
	{

		if (c==' '){
			printf(" ");	//imprimir espacios en blanco
		}
        else if (c=='\t'){
            printf("\t");   // imprimir tabulaciones
		}
		else if(c=='\n')
		{
			//incrementar el numero de linea
            printf("\n");
			numLinea++;
			continue;
		}
		else if (isalpha(c))
		{
			//es un identificador (o palabra reservada)
			i=0;
			do{
				id[i]= tolower(c);
				i++;
				c=fgetc(archivo);
				if (i>=TAMLEX)
					error(msg);
			}while(isalpha(c));
			id[i]='\0';
            if (c!=EOF)
                ungetc(c,archivo);
            else
                c=0;
            if (strcmp(id,"true")==0 || strcmp(id,"false")==0 || strcmp(id,"null")==0){
                t.pe=buscar(id);
                t.compLex=t.pe->compLex;
            }else{
                error(msg);
                anaLex();
            }
			if (t.pe->compLex==opa)
			{
				strcpy(e.lexema,id);
				e.compLex=cadena;
				insertar(e);
				t.pe=buscar(id);
				t.compLex=cadena;
			}
			break;
		}
		else if (isdigit(c))
		{
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;

				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;

					case 1://un punto, debe seguir un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							estado=11;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=11;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=11;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						t.pe=buscar(id);
						if (t.pe->compLex==opa)
						{
							strcpy(e.lexema,id);
							e.compLex=numero;
							insertar(e);
							t.pe=buscar(id);
						}
						t.compLex=numero;
						break;
					case 11:
						if (c==EOF)
							error(msg);
						else
                            ungetc(c,archivo);
                            error(msg);
                            anaLex();
                        acepto=1;
						break;
					}
				}
			break;
		}
		else if (c==':')
		{
			t.compLex=dospuntos;
            t.pe=buscar(":");
            break;
		}
		else if (c==',')
		{
			t.compLex=coma;
			t.pe=buscar(",");
			break;
		}
		else if (c=='{')
		{
			if ((c=fgetc(archivo))=='*')
			{
				while(c!=EOF)
				{
					c=fgetc(archivo);
					if (c=='*')
					{
						if ((c=fgetc(archivo))=='}')
						{
							break;
						}
					}
					else if(c=='\n')
					{
						numLinea++;
					}
				}
				if (c==EOF)
					error(msg);
				continue;
			}
			else
			{
				ungetc(c,archivo);
				t.compLex=lllave;
				t.pe=buscar("{");
			}
			break;
		}
		else if (c=='}')
		{
			t.compLex=rllave;
			t.pe=buscar("}");
			break;
		}
		else if (c=='[')
		{
			t.compLex=lcorchete;
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			t.compLex=rcorchete;
			t.pe=buscar("]");
			break;
		}
		else if (c=='\"')
		{//un caracter o una cadena de caracteres
			i=0;
			id[i]=c;
			i++;
			do{
				c=fgetc(archivo);
				if (c=='\"')
				{
                    break;
				}
				else if((c==EOF) || (c=='\n'))
				{
					error(msg);
                    break;
				}else{
					id[i]=c;
					i++;
				}
			}while(!(c=='\"') || (c!='\n'));
			id[i]='\0';
			if ((c!=EOF) && (c!='\"')){
				ungetc(c,archivo);
                anaLex();
			}
			if (c=='\"'){
                t.pe=buscar(id);
                t.compLex=t.pe->compLex;
			}
			if (t.pe->compLex==opa)
			{
				strcpy(e.lexema,id);
				if (strlen(id)==3 || strcmp(id,"''''")==0)
					e.compLex=cadena;
				else
					e.compLex=cadena;
				insertar(e);
				t.pe=buscar(id);
				t.compLex=e.compLex;
			}
			break;
		}
		else if (c=='{')
		{
			while(c!=EOF)
			{
				c=fgetc(archivo);
				if (c=='}')
					break;
				else if(c=='\n')
				{
					numLinea++;
				}
			}
			if (c==EOF)
				error(msg);
		}
		else if (c!=EOF)
		{
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.compLex=opa;
		//sprintf(e.lexema,"EOF");
        //t.pe=&e;
	}
}

int main(int argc,char* args[])
{
	// inicializar analizador lexico
	iniciarTabla();
	iniciarTablaSimbolos();
	iniciarComlex();
	initConjuntosSegundo();
	initConjuntosPrimero();
    int g = 0;
	char d = 0;
	// vector que almacena los componentes lexicos

	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
			g = 1;
		}
		//remove("output.txt");
		freopen("output.txt", "w+", stdout); // crea el archivo de salida output.txt
		while (t.compLex!=opa){
			anaLex();
			printf("%s  ",comp_lex[t.compLex]);
		}
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
		g = 1;
		
	}
	fclose(archivo);
	fclose(stdout);
	freopen(AUX,"w",stdout);
	if (g == 0){
		descendenteRecursivo();
	}
	return 0;
	
}

#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include<stdbool.h>  
#include <pthread.h>

// Estructura especial para almacenar resultados de consultas
	//Creamos una conexion al servidor MYSQL 
MYSQL_RES *resultado;
int err;
MYSQL_ROW row; 
MYSQL *conn;

int i;
int sockets[100];

int contador; //Para el numero de servicios
//Estructura necesaria para acceso excluyente
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Estructura para almacenar 100 usuarios conectados
	typedef struct{ //Clase "Conectado"
	char nombre[20];
	int socket;
}Conectado;

typedef struct{ //Lista de Conectados
	Conectado conectados[100];
	int num;
}ListaConectados;
ListaConectados milista;


//funciones
int DameID(char consulta[100], char respuesta4[100])
{
	sprintf(consulta, "SELECT COUNT(jugadores.username)FROM jugadores");
	//recogemos el resultado de la consulta 
	err=mysql_query (conn, consulta);
	
	if(err!=0)
	{
		printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn),mysql_error(conn));
		exit(1);
	}
	resultado = mysql_store_result (conn); 
	row = mysql_fetch_row (resultado);
	if (row == NULL)
		printf ("No se han obtenido datos en la consulta\n");
	else
	{
		//El resultado debe ser una matriz con una sola fila
		//y una columna que contiene el numero de partidas
		printf ("Numero de jugadores registrados: %s\n", row[0]);
		sprintf(respuesta4, "%s", row[0]);
	}
}

//a�adir conectados
int AnadeConectado (ListaConectados *lista, char nombre[20], int socket) //Añade a la lista un nuevo usuario conectado
{
	if (lista->num == 100)
	{
		return -1; //Está la lista llena
	}
	else
	{
		strcpy(lista->conectados[lista->num].nombre, nombre);
		lista->conectados[lista->num].socket = socket;
		lista->num++;
		return 0; //Se ha añadido correctamente
	}
}

void Conectados(ListaConectados *lista, char conectados[300]) //Pone en el vector conectados el núm de usuarios en la lista y el usuario de cada jugador separado por barras
{
	printf("Voy a empezar \n");
	sprintf(conectados, "%d", lista->num);
	int i;
	//printf("Conectados: %s\n", conectados);
	for(i=0; i<lista->num; i++)
	{
		sprintf(conectados, "%s/%s",conectados, lista->conectados[i].nombre); //%s, conectados
		//printf("Conectados: %s\n", conectados);
	}
	printf("%s\n",conectados);
}

//Función para saber la posicion de ese usuario mediante el socket
int GetSocket (ListaConectados *lista, char nombre[20])
{
	int i = 0;
	int encontrado = 0;
	while((i < lista->num) && !encontrado)
	{
		if (strcmp(lista->conectados[i].nombre,nombre) == 0)
		{
			encontrado = 1;
		}
		if(!encontrado)
		{
			i = i+1;
		}
	}
	if (encontrado)
	{
		return lista->conectados[i].socket; //Devuelve el socket
	}
	else
	{
		return -1; //No está en la lista
	}
}

//int Registro (char usuario[20],char contrasena[20], char respuesta2[512], char consulta[80], int id) 
//{
	
/*	printf ("Nombre: %s, Contraseña: %s\n", usuario, contrasena);*/
/*	sprintf(consulta, "SELECT jugadores.usuario FROM jugadores WHERE jugadores.usuario='%s' ",usuario);*/
	// hacemos la consulta para saber si ese usuario ya está registrado
/*	int err =mysql_query (conn, consulta);*/
/*	if (err!=0)*/
/*	{*/
/*		printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));*/
/*		exit (1);*/
/*	}*/
/*	id = DameID(consulta, respuesta2)+1;*/
	//Recogemos el resultado
/*	resultado = mysql_store_result (conn);*/
/*	row=mysql_fetch_row(resultado);*/
/*	printf("row: %s\n", row);*/
	
/*	if (row==NULL)*/
/*	{*/
/*		printf("Voy a añadir este usuario: %s\n", usuario);*/
/*		printf("%s\n", row[0]);*/
		/*int idJugador = atoi(row[0]);*/
/*		char insert[150];*/
/*		sprintf(insert,"INSERT INTO jugadores VALUES(%d,'%s','%s');",id,usuario,contrasena); *///Para que el Id sea la siguiente posición libre
/*		err=mysql_query(conn, insert);*/
/*		{*/
/*			printf ("Error al insertar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));*/
/*			exit (1);*/
/*			sprintf(respuesta2,"2/No se ha podido insertar el usuario");          		 */
/*		}*/
/*	else*/
/*		{*/
/*			sprintf(respuesta2,"2/Registrado correctamente");*/
/*			printf("Se ha registrado correctamente el usuario: %s\n", usuario);*/
/*		}*/
/*	}*/
/*	else*/
/*	{*/
/*		sprintf(respuesta2,"2/No se ha podido acceder a la base de datos");*/
/*	} */
	
/*}*///

void InicioSesion(char usuario[20], char contrasena[20], char consulta[80], char respuesta1[512], int codigo, char conectado[300], int sock_conn)
{
	printf ("Codigo: %d, Nombre: %s, Contrase�a: %s\n", codigo, usuario, contrasena);
	sprintf(consulta, "SELECT jugadores.usuario, jugadores.pass FROM jugadores WHERE (jugadores.usuario='%s' AND jugadores.pass='%s')",usuario,contrasena);
	err=mysql_query(conn, consulta);
	if (err!=0){
		printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn),mysql_error(conn));
		exit(1);
	}
	//Recogemos el resultado de la consulta
	resultado = mysql_store_result(conn);
	row = mysql_fetch_row(resultado);
	if (row == NULL)
	{
		printf("No se ha obtenido la consulta \n");
		sprintf(respuesta1,"1/INCORRECTO");
	}
	else
	{
		printf("Inicio de sesion completado \n");
		sprintf(respuesta1,"1/INICIO DE SESION COMPLETADO");
		printf("%s,%s,%d \n",conectado,usuario,sock_conn);
		pthread_mutex_lock( &mutex );
		AnadeConectado(&milista,usuario,sock_conn);
		printf("Ya he añadido conectado \n");
		pthread_mutex_unlock( &mutex);
	}
}

int Desconectar(ListaConectados *lista, char nombre[20], char respuestaDesconectar[512])
{
	int posicion = GetSocket (lista,nombre);
	if (posicion == -1)
	{
		return -1; //No está conectado este usuario
	}
	else
	{
		int i;
		for (i = posicion; i < lista->num-1; i++)
		{
			strcpy(lista->conectados[i].nombre, lista->conectados[i+1].nombre);
			lista->conectados[i].socket = lista->conectados[i+1].socket;
		}
		lista->num--;
		sprintf(respuestaDesconectar,"0/Te has desconectado, hasta la próxima");
		return 0;
	}
}

void *Atencion(void*socket)
{
	int sock_conn;
	int *s;
	s=(int *)socket;
	sock_conn= *s;
	
	char conectado[300];
	int desconectar=0;
	
	//Conexion mediante sockets con el cliente
	int  sock_listen;
	int ret;
	char peticion[512];
	char respuesta[512];
	/*	char usuario[80];*/	
	int terminar = 0;
	
	while(terminar == 0) //Bucle de atencion al cliente
	{
		char usuario[80];
		char contrasena[80];
		int id;
		
		ret = read(sock_conn, peticion, sizeof(peticion));
		printf("Recibido \n");
		peticion[ret]='\0';
		
		printf("Peticion %s \n", peticion);
		
		char *p = strtok(peticion, "/");
		int codigo = atoi(p);
		int cont;
		char jugador1 [10];
		char jugador2 [10];
		char consulta [80];	
		
		if (codigo == 0) //Desconectarse y eliminar de la lista de conectados
		{
			terminar = 1;
			desconectar=1;
			pthread_mutex_lock( &mutex );
			Desconectar(&milista,usuario,respuesta);
			Conectados(&milista,conectado);
			pthread_mutex_unlock( &mutex);
			
			printf("Respuesta: %s \n", respuesta);
			write (sock_conn,respuesta, strlen(respuesta));
		}
		else if (codigo == 1) //Inicio de sesión
		{
			p=strtok(NULL,"/");
			strcpy(usuario,p);
			p=strtok(NULL,"/");
			strcpy(contrasena,p);
			
			if(desconectar==1)
			{
				AnadeConectado(&milista,usuario, sock_conn);
			}
			InicioSesion(usuario,contrasena, consulta, respuesta, codigo, conectado,sock_conn); //Lamamos a la funcion InicioSesion
			printf("Voy a sacar conectados \n");
			Conectados(&milista,conectado);
			printf("Ya tengo conectados: %s\n",conectado);
			
			/*	printf("Respuesta: %s \n", respuesta);*/
			
			desconectar=0;
			write(sock_conn, respuesta, strlen(respuesta));
		}
		else if(codigo==2) //Registro de nuevo usuario
		{
			p = strtok( NULL, "/");
			strcpy (usuario, p);
			p = strtok( NULL, "/");
			strcpy(contrasena,p);
			Registro(usuario,contrasena, respuesta, consulta, id);
			//Enviamos la respuesta
			printf("Respuesta: %s \n", respuesta);
			write (sock_conn,respuesta, strlen(respuesta));
		}
		else if(codigo ==3) //Consulta 1
		{
			
			
		}
			

int main(int argc, char **argv[]) {
	
	//conexion mediante sockets con cliente
	int sock_conn;
	int sock_listen;
	int ret;
	char peticion[512];
	char respuesta[512];
	char usuario[80];
	char contrasena[80];
	struct sockaddr_in serv_adr;
	
	 if ((sock_listen sock(AF_INET, SOCK_STREAM, 0))<0)
		printf("Error creando el socket\n");
	
	memset(&serv_adr,0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(9030);
	if(bind(sock_listen, (struct sockaddr *)&serv_adr, sizeof(serv_adr))<0)
		printf("Error al bind \n");
	if(listen(sock_listen, 3)<0)
		printf("Error en el listen \n");
	// Estructura especial para almacenar resultados de consultas 
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	//Creamos una conexion al servidor MYSQL 
	MYSQL *conn;
	conn = mysql_init(NULL);
	if (conn==NULL) {
		printf ("Error al crear la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	
	

int terminar = 0;
while (terminar == 0) //Bucle de atencion al cliente
{
	printf("Escuchando \n");
	
	sock_conn = accept(sock_listen, NULL, NULL);
	printf("He recibido conexion \n");
	
	ret = read(sock_conn, peticion, sizeof(peticion));
	printf("Recibido \n");
	peticion[ret]='\0';
	
	printf("Peticion %s \n", peticion);
	
	char *p = strtok(peticion, "/");
	int codigo = atoi(p);
	int err;
	int cont;
	char jugador [10];
	
	char consulta [80];
	//inicializar la conexion
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "BaseDatos",0, NULL, 0);
	if (conn==NULL) {
		printf ("Error al inicializar la conexion: %u %s\n", 
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	contador=0;
	pthread_t thread;
	i=0;
	char usuario[20];
	
	for(;;) //bucle para atender a los clientes
	{
		printf("Escuchando \n");
		
		sock_conn=accept(sock_listen, NULL, NULL);
		printf("He recibido conexión\n");
		
		sockets[i]=sock_conn; //Socket que usaremos para este cliente
		
		AnadeConectado(&milista,usuario,sock_conn);
		pthread_create(&thread, NULL, Atencion,&milista.conectados[milista.num-1].socket); //Crear Thread
		i=i+1;	
	}	
}
		}
		
	
	
	
	




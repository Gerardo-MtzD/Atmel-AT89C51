/*****************************************************************************/
/*****************************************************************************/
#include "at89c5131.h"
#include "lcd.h"
#include <math.h> //Librería para operaciones matemáticas
#include <stdio.h> //Librería para conversión de números a string

/*----------------------------------*/
/* declaración de funciones ocultas */
/*----------------------------------*/
void LCD_enviar_comando(char comando, char instr_dato, char cuatro_ocho);
void LCD_esperar_BF(void);
void LCD_retardo_ms(unsigned int ms);

/*-------------------------------------------------*/
/* Nombre: LCD_inicializar */
/* Descripcion: rutina para inicialización del LCD */
/* Parametros: */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_inicializar(void)
{
	//Secuencia de Inicialización
  LCD_retardo_ms(15);
  LCD_enviar_comando(0x30,0,8);
  LCD_retardo_ms(5);
  LCD_enviar_comando(0x30,0,8);
  LCD_retardo_ms(1);
  LCD_enviar_comando(0x30,0,8);
	
	//Se hacen instrucciones distintas según tamaño de interfaz
	if (TAM_INTERFAZ == 8) 
	{
		LCD_enviar_comando(0x38,0,8);
		LCD_esperar_BF();
		LCD_enviar_comando(0x08,0,8);
		LCD_esperar_BF();
		LCD_enviar_comando(0x01,0,8);
		LCD_esperar_BF();
		LCD_enviar_comando(0x06,0,8);
		LCD_esperar_BF();
	}
	else if (TAM_INTERFAZ == 4)
	{
		LCD_enviar_comando(0x20,0,8);
		LCD_esperar_BF();
		LCD_enviar_comando(0x28,0,4);
		LCD_esperar_BF();
		LCD_enviar_comando(0x08,0,4);
		LCD_esperar_BF();
		LCD_enviar_comando(0x01,0,4);
		LCD_esperar_BF();
		LCD_enviar_comando(0x06,0,4);
		LCD_esperar_BF();
	}
		
	LCD_enviar_comando(0x0C,0,TAM_INTERFAZ);
	LCD_esperar_BF();
	
} 

/*-------------------------------------------------*/
/* Nombre: LCD_borrar */
/* Descripcion: borra el contenido del LCD */
/* Parametros: */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_borrar(void)
{
	LCD_enviar_comando(0x01,0,TAM_INTERFAZ);
	LCD_esperar_BF();
}
/*-------------------------------------------------*/
/* Nombre: LCD_situar_cursor */
/* Descripcion: sitúa el cursor del LCD en una */
/* determinada posición */
/* Parametros: */
/* fila - fila seleccionada */
/* columna - columna */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_situar_cursor(unsigned char fila, unsigned char columna)
{
	//Se utiliza el DDRAM Address para establecer la posiciòn donde se pondrá el cursor
	unsigned int index;
	if (fila == 1) {
		index = 0x80+columna-1;
		LCD_enviar_comando(index,0,TAM_INTERFAZ);
		LCD_esperar_BF();
	}
	else if (fila == 2) {
		index = 0xC0+columna-1;
		LCD_enviar_comando(index,0,TAM_INTERFAZ);
		LCD_esperar_BF();		
	}
}
/*-------------------------------------------------*/
/* Nombre: LCD_escribir_caracter */
/* Descripcion: escribe un caracter en el LCD, en */
/* la posición actual del cursor */
/* Parametros: */
/* caracter - caracter visualizado */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_escribir_caracter(char caracter)
{
	LCD_enviar_comando(caracter,1,TAM_INTERFAZ);
}

void LCD_ecribir_string(unsigned char code linea[16]) {
	unsigned int i;
	for (i=0 ; i<16 ; i++) // escritura caracter a caracter
 {
	 if (linea[i] == '\0') // Se rellena de espacios en blanco del string para cumplir con 16 espacios
	 {
		 LCD_escribir_caracter(' ');
	 }
	 else 
	 {
		 LCD_escribir_caracter(linea[i]);
	 }
		 
 } 
}

/*-------------------------------------------------*/
/* Nombre: LCD_escribir_entero */
/* Descripcion: escribe un entero en el LCD, en */
/* la posición actual del cursor */
/* Parametros: */
/* valor - valor integer visualizado */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_escribir_entero(int valor)
{
	int n = log10(valor); // Comprobar número de cifras en valor 
	int i;
	char str[16];
	sprintf(str,"%d",valor); // Conversión de valor a string de caracteres con formato entero
	
	if (valor == 0){
		LCD_escribir_caracter('0');
	}
	//Se escribe caracter por caracter hasta la longitud del número
	
	for (i=0; i<16;i++){
		if (str[i] == '\0') // Se rellena de espacios en blanco del string para cumplir con 16 espacios
	 {
		 LCD_escribir_caracter(' ');
	 }
	 else
	 {
		 LCD_escribir_caracter(str[i]);
	 }
	}
}

/*-------------------------------------------------*/
/* Nombre: LCD_escribir_real */
/* Descripcion: escribe un entero flotante en el LCD, en */
/* la posición actual del cursor */
/* Parametros: */
/* valor - valor flotante visualizado */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_escribir_real(float valor, int decimales)
{
	char str[16];
	int n;
	unsigned int i;
	if (valor == 0){
		LCD_escribir_caracter('0');
	}
	sprintf(str,"%f",valor); //Conversión de número float a string de caracteres
	n=log10(valor)+1; //Comprobar número de cifras en parte entera
	//Escritura caracter por caracter hasta longitud deseada por el usuario
	for (i=0; i<n+decimales+1;i++){
		LCD_escribir_caracter(str[i]);
	}
	
}

void LCD_escribir_BCD(unsigned int valor)
{
	unsigned int valor_l, valor_h, entero;
	valor_l = valor & 0xF;
	valor_h = (valor>>4) & 0xF;
	entero = (valor_h)*10 + valor_l;
	LCD_escribir_entero(entero);
}

/*-------------------------------------------------*/
/* Nombre: LCD_enviar_comando */
/* Descripcion: envia un comando (instruccion o */
/* dato) al controlador del LCD */
/* Parametros: */
/* comando - comando enviado (DB7-DB0) */
/* instr_dato - 0 <-> instruccion, 1 <-> dato */
/* cuatro_ocho - 4 <-> 4 bits, 8 <-> 8 bits */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_enviar_comando(char comando, char instr_dato, char cuatro_ocho)
{
	//Secuencia para envío de datos o instrucción al display
	RS = instr_dato;
	RW = 0; //Activación de RW como escritura
	EN = 1;
	// Set de instrucciones para envío de información en paquetes de 4 u 8 bits
	if (cuatro_ocho == 8) 
	{
		LCD = comando;
		EN = 0;
	}
	else if (cuatro_ocho == 4) 
	{
		LCD = comando;
		EN = 0;
		EN = 1;
		//Para los cuatro bits, se envían los primeros cuatro 
		//y después se recorren 4 posiciones para el envío de los siguientes
		LCD = comando << 4; 
		EN = 0;
	}
	RW = 1; //Se desactiva RW como escritura
	
} 

/*-------------------------------------------------*/
/* Nombre: LCD_esperar_BF */
/* Descripcion: espera a que el BF sea 0 (fin op.) */
/* Parametros: */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_esperar_BF(void)
{ 
	LCD = 0xFF; //Seleccionamos el lcd del bus de datos
	RS = 0;
	RW = 1; //Activación de RW como lectura
	EN = 1;
	while ( BF == 1); //Se espera a que el BF indique que finalizó la tarea
	EN = 0;
	RW = 0; //Se desactiva RW como lectura
}
/*-------------------------------------------------*/
/* Nombre: LCD_retardo */
/* Descripcion: rutina de retardo */
/* Parametros: */
/* ms - milisegundos de retardo */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_retardo_ms(unsigned int ms)
{
	unsigned int i,j;
 for (i=0; i< ms; i++)
	{
		for (j=0; j<125; j++) //Retardo de aproximadamente 1 milisegundo
		{}
	}
} 
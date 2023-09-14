/*****************************************************************************/
/*****************************************************************************/
#include "at89c5131.h"
/*-------------------------------------*/
/* definicion de la interfaz del LCD */
/* debe adaptarse a la conexión entre */
/* LCD y micro */
/*-------------------------------------*/
// conexión micro-LCD en el kit I2Kit
#define TAM_INTERFAZ 4 // interfaz de 4 bits
#define LCD P2 // LCD conectado al puerto P2
sbit EN = P3^6; // ubicacion pin EN
sbit RW = P3^5; // ubicacion pin RW
sbit RS = P3^4; // ubicacion pin RS
sbit BF = P2^7; // ubicacion BF
/*-------------------------------------------------*/
/* Nombre: LCD_inicializar */
/* Descripcion: rutina para inicialización del LCD */
/* Parametros: */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_inicializar(void);

/*-------------------------------------------------*/
/* Nombre: LCD_borrar */
/* Descripcion: borra el contenido del LCD */
/* Parametros: */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_borrar(void); 

/*-------------------------------------------------*/
/* Nombre: LCD_situar_cursor */
/* Descripcion: sitúa el cursor del LCD en una */
/* determinada posición */
/* Parametros: */
/* fila - fila seleccionada */
/* columna - columna */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_situar_cursor(unsigned char fila, unsigned char columna);

/*-------------------------------------------------*/
/* Nombre: LCD_escribir_caracter */
/* Descripcion: escribe un caracter en el LCD, en */
/* la posición actual del cursor */
/* Parametros: */
/* caracter - caracter visualizado */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_escribir_caracter(char caracter);

/*-------------------------------------------------*/
/* Nombre: LCD_escribir_string */
/* Descripcion: escribe un string en el LCD, en */
/* la posición actual del cursor */
/* Parametros: */
/* linea - string de caracteres que van a ser visualizados */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_ecribir_string(unsigned char code linea[16]);
/*-------------------------------------------------*/
/* Nombre: LCD_escribir_entero */
/* Descripcion: escribe un entero en el LCD, en */
/* la posición actual del cursor */
/* Parametros: */
/* valor - valor entero visualizado */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_escribir_entero(int valor);

/*-------------------------------------------------*/
/* Nombre: LCD_escribir_real */
/* Descripcion: escribe un entero flotante en el LCD, en */
/* la posición actual del cursor */
/* Parametros: */
/* valor - valor flotante visualizado */
/* decimales - cantidad de decimales a desplegar */
/* Resultado: */
/*-------------------------------------------------*/
void LCD_escribir_real(float valor,int decimales);


//Se introduce el LCD_enviar_comando al fichero para poder utilizarlo en el ejemplo del archivo teclado.c
void LCD_enviar_comando(char comando, char instr_dato, char cuatro_ocho);

void LCD_escribir_BCD(unsigned int valor);
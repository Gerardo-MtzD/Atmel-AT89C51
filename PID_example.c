/*****************************************************************************/
/* Fichero - main.c */
/* Descripcion - programación de un controlador On/Off e implementación PWM en un LCD y con comunicación I2C */
/* Autor - Carlos & Gerardo */
/* Fecha - 24/05/2023 */
/*****************************************************************************/
//Se incluyen las librerías
#include <at89c5131.h>
#include "lcd.h"
#include "I2C.h"

//Declaración de variables
int cont = 0;
char seleccion;
char Accion_control;
char Valor_medida, dmux,to_dmux;
unsigned int estado;
unsigned int duty_cycle = 0;
sbit Control_V = P2^2;

//Declaración de funciones
char opcionAC (char seleccion); //elije la planta a tratar
char opcionVM (char seleccion); // elije si se lee los valores de planta o del potenciómetro 
void mandar_datos(void); //Envia los datos al puerto 9
void delay(unsigned int ms);
unsigned int medir(void); //Conversor ADC
unsigned int control_on_off(unsigned int consigna, unsigned int medicion); //Función de control
unsigned int PWM(unsigned int porcentaje); //Generador de señal PWM
void rsi(); //Rutina de interrupción

void main(){
	unsigned int consigna, medicion, ant, accion_c,error;
	accion_c = 0; medicion = 0; consigna = 0; error = 0;
	//Protocolo de Inicializacion de puertos
	P3 = 0x9F;
	P1 = 0xEF;
	LCD_inicializar();
	//Inicializacion de Timer0 para que desborde cada 10 ms
	TMOD = 0x01; 
	TH0 = 0xD8;
	TL0 = 0xEF;
	TR0 = 1;
	IEN0 = 0x82;
	while(1) {
		//Configuracion de interruptores
		P1 = 0xFE; //Activar interruptores
		P2 = 0xFF;
		ACC = P2;
		seleccion = ACC;
		P1 = 0xEF;
		
		//Lectura de interruptores y asignaciòn a variables
		Accion_control = opcionAC(seleccion);
		Valor_medida = opcionVM(seleccion);
		// Q8 = POTENCIOMETRO DE MEDIDA : Valor_medida = 1
		// Q7 = MEDIDA DE PLANTA: Valor_medida = 0
		// Q5 = PV: Accion_control = 1
		// Q6 = PT: Accion_control = 0
		
		if (Valor_medida == 1) //Potenciometro de medida
		{
			B = 0x80; // Valor asignado para el puerto 9
			mandar_datos();
			delay(10);
			medicion = medir();
			//Se envían los datos al LCD
			LCD_situar_cursor(1,1);
			LCD_escribir_caracter('M');
			LCD_situar_cursor(1,3);
			LCD_escribir_real(medicion,1);
			delay(10);
			
		}
		else if (Accion_control == 1 & Valor_medida == 0) { //Control de PV
			//Accion de control
			
			accion_c = control_on_off(consigna,medicion); //Te da la acción de salida segun el duty cycle del PWM
			B = 0x10 + 0x4*accion_c; //Se utiliza esta fórmula para siempre leer los datos de la planta independientemente del control
			mandar_datos();
			delay(10);
			medicion = medir();
			delay(10);
			// Se envían datos al LC
			LCD_situar_cursor(1,1);
			LCD_escribir_caracter('P');
			LCD_situar_cursor(1,2);
			LCD_escribir_caracter('V');
			LCD_situar_cursor(1,5);
			LCD_escribir_real(medicion,1);
			
			
		}
		else if (Accion_control == 0 & Valor_medida == 0) { //Control de PT	
			accion_c = control_on_off(consigna,medicion); //Te da la acción de salida segun el duty cycle del PWM
			B = 0x20 + 0x2*accion_c; //Se utiliza esta fórmula para siempre leer los datos de la planta independientemente del control
			mandar_datos();
			delay(10);
			medicion = medir();
			delay(10);
			//Se envían los datos al LCD
			LCD_situar_cursor(1,1);
			LCD_escribir_caracter('P');
			LCD_situar_cursor(1,2);
			LCD_escribir_caracter('T');
			LCD_situar_cursor(1,5);
			LCD_escribir_real(medicion,1);
		} 
		//Leer consigna
		if (Accion_control == 1 & Valor_medida == 0) //Lectura de consigna con control en planta Voltaje
			B = 0x40 + 0x4*accion_c;
		else if (Accion_control == 0 & Valor_medida == 0)//Lectura consigna con control en planta de temperatura
			B = 0x40 + 0x2*accion_c;
		mandar_datos();
		delay(15);
		consigna = medir();
		delay(10);
		//Enviar al LCD
		LCD_situar_cursor(2,1);
		LCD_escribir_caracter('C');
		LCD_situar_cursor(2,3);
		LCD_escribir_real(consigna,1);
		LCD_situar_cursor(2,12);
		LCD_escribir_caracter('P');
		LCD_situar_cursor(2,13);
		
		//Desplegar el porcentaje de PWM en relación al error
		error = ((consigna-medicion)/(1060.0-51.0))*100;
		delay(20);
		LCD_escribir_real(error,1);
	  
		//Se hace un clean al LCD para que no se guarden valores anteriores
		if (Valor_medida != ant)
			LCD_borrar();
		ant = Valor_medida;
		
	}
}

char opcionAC (char seleccion){
	if ((seleccion & 0x1) == 1) {
			//Seleccionar planta de voltaje PV
			Accion_control = 1;
		}
		else {
			//Seleccionar planta de temperatura PT
			Accion_control = 0;
		}
	return(Accion_control);
}

char opcionVM (char seleccion) {
	if ((seleccion>>1 & 0x1) == 1) {
			//Valores del potenciometro de medida
			Valor_medida = 1;
		}
		else
		{ //Valores de los datos reales de las plantas
			Valor_medida = 0;
		}
		return(Valor_medida);
}

void mandar_datos() { // Envío de datos al puerto 9
		// Seleccion de DMUX para leer con ADC
		P2 = B;
		P1 = 0xF7; //Puerto P1 de salidas en P9
		P1 = 0xEF;
}

void delay(unsigned int ms) { //Delay en ms
	unsigned int i,j;
  for (i=0; i< ms; i++)
	{
		for (j=0; j<125; j++) //Retardo de aproximadamente 1 milisegundo
		{}
	}
}
unsigned int medir(void) { //Configuración para la lectura del ADC mediante I2C
	unsigned int datos_h, datos_l;
	unsigned int resultado;
	I2C_start();
	I2C_write(0x91); //Declarando dispositivo y enviar orden de leer
	datos_h = I2C_read();
	I2C_ACK();
	datos_l = I2C_read();
	I2C_NACK();
	I2C_stop();
	resultado = (datos_h<<8) + datos_l;
	return(resultado);
}

unsigned int control_on_off(unsigned int consigna, unsigned int medicion) { //Acción de control para las plantas
	int accion, error;
	unsigned int por_error;
	error = consigna-medicion;
	por_error = (error/(1024.0-50.0))*100; //Cálculo del porcentaje respecto al error
	if (error > 2.0) //Valor de histeresis inferior
		accion = PWM(por_error); //Se activa PWM al 50%
	else if (error < -2.0) //valor de histeresis superior
		accion = PWM(0);
	return(accion);
}

unsigned int PWM(unsigned int porcentaje) { //Función para PWM
	duty_cycle = porcentaje;
	return(estado);
}

void rsi() interrupt 1
{ //Rutina de interrupcion cada 10ms
	    TH0 = 0xD8;
	    TL0 = 0xEF;
			cont = cont + 1;
			if (cont <= duty_cycle) //Flag segun el duty cycle establecido (1 s = 100)
			{
				// Estado on 
				estado = 1;
			}	
			else if (cont > duty_cycle & duty_cycle < 100) {
				//Estado off
				estado = 0;
			}
			if (cont > 100) {cont = 0;} //Hacer Reset al contador cada 100 ms
}
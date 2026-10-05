#include <stm32f7xx.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cmath>  // Necesario para roundf
#define BUFFER_SIZE 32

void SysTick_Init(void);
void SysTick_Wait(uint32_t n);
void SysTick_Wait1ms(uint32_t delay);
void GPIO_SETUP();
void RCC_SETUP();
int USART3_SendChar(int value);	
void Letra(char g);
void Estado(int b);
void add_to_buffer(char value, int is_data);
void process_lcd_buffer(void);
char arreglo[31];
int cont=0;
int cantidad=0;
char cant;
int s = 0;
char letra;
int grados1, pwm1,grados2, pwm2,grados3, pwm3,grados4, pwm4,grados5, pwm5,grados6, pwm6, grados31, grados61;
int posx1, posx2, posx3;
int	posy1, posy2, posy3; 
int posz1, posz2, posz3;
float sx, sy, sz;
int index = 0;
volatile uint32_t Reload,cuenta=0;
volatile int contando = -1;
volatile int h = 0;       // Bandera para iniciar el temporizador
volatile int cambio = 0;  // Bandera para señalar el cambio de estado
volatile int lcd_busy = 0; // Bandera para indicar si el LCD está ocupado
volatile struct {
    char value;   // Carácter o comando
    int is_data;  // 1 = dato (Letra), 0 = comando (Estado)
} lcd_buffer[BUFFER_SIZE];
volatile int buffer_index = 0; // Índice para agregar al buffer
volatile int buffer_pos = 0;   // Posición actual siendo procesada
bool ejecutando=false;
int main(void){
	SysTick_Init();
	RCC_SETUP();
	GPIO_SETUP();
	Estado(1);Estado(2);Estado(3);Estado(4);
	while(1){
		pwm1 = (((2600 - 350) / (180 - 0)) * grados1 ) + 350; 
		pwm2 = (((2600 - 350) / (180 - 0)) * grados2 ) + 350; 
		pwm3 = (((2600 - 350) / (180 - 0)) * grados3 ) + 350; 
		pwm4 = (((2600 - 350) / (180 - 0)) * grados4 ) + 350; 
		pwm5 = (((2600 - 350) / (180 - 0)) * grados5 ) + 350; 
		pwm6 = (((2600 - 350) / (180 - 0)) * grados61 ) + 350; 
		TIM5->CCR1=uint32_t(pwm1);//
		TIM5->CCR2=uint32_t(pwm2);//
		TIM5->CCR3=uint32_t(pwm3);//
		TIM5->CCR4=uint32_t(pwm4);//
		TIM3->CCR1=uint32_t(pwm5);//
		TIM3->CCR3=uint32_t(pwm6);//
		int a=0;
		process_lcd_buffer(); // Procesa el buffer en cada iteración

		Reload = TIM2->ARR;
	  cuenta = TIM2->CNT;
		
		if (((GPIOC->IDR)&(1<<13))==(1<<13) && a == 0){
			ejecutando=!ejecutando;
			Estado(1);
			a=1;
		}	
	 	if (ejecutando == false){
			Estado(5);
			Letra('G');Letra('R');Letra('A');Letra('D');Letra('O');Letra('S');Letra(':');Letra(' ');Letra(int(grados1/100) + 48);Letra(int((grados1/10)%10) + 48);Letra(int(grados1%10) + 48);Letra(':');
			Letra(int(grados2/100) + 48);Letra(int((grados2/10)%10) + 48);Letra(int(grados2%10) + 48);
			Estado(6);
			Letra(int(grados31/100) + 48);Letra(int((grados31/10)%10) + 48);Letra(int(grados31%10) + 48);Letra(':');
			Letra(int(grados4/100) + 48);Letra(int((grados4/10)%10) + 48);Letra(int(grados4%10) + 48);Letra(':');
			Letra(int(grados5/100) + 48);Letra(int((grados5/10)%10) + 48);Letra(int(grados5%10) + 48);Letra(':');
			Letra(int(grados61/100) + 48);Letra(int((grados61/10)%10) + 48);Letra(int(grados61%10) + 48);Letra(' ');Letra(' ');
			Estado(2);
	}
		else{
			Estado(5);
			Letra('T');Letra('C');Letra('P');Letra(':');Letra(' ');Letra('P');Letra('X');Letra(':');
			if(sx == 1.0){
				Letra(' ');
			}
			else{
				Letra('-');
			}
			Letra(int(posx1));  
			Letra('.');			
			Letra(int(posx2));   
			Letra(int(posx3));           
			Estado(6);
			Letra('P');Letra('Y');Letra(':');
      if(sy == 1.0){
				Letra(' ');
			}
			else{
				Letra('-');
			}
			Letra(int(posy1));  
			Letra('.');			
			Letra(int(posy2));   
			Letra(int(posy3));   
			
			Letra('P');Letra('Z');Letra(':');
      if(sz == 1.0){
				Letra(' ');
			}
			else{
				Letra('-');
			}
			Letra(int(posz1));  
			Letra('.');			
			Letra(int(posz2));   
			Letra(int(posz3));  
			Estado(2);
		}

		
		//TIM3->CCR3=1000;//
		//TIM3->CCR4=1000;//
			
//		TIM5->CCR1=2600;//
//		TIM5->CCR2=1000;//
//		TIM5->CCR3=1000;//
//		TIM5->CCR4=1000;//
//		TIM3->CCR1=1000;//
//		TIM3->CCR2=1000;//
//		TIM3->CCR3=1000;//
//		TIM3->CCR4=1000;//


	}
	}


void RCC_SETUP()
{
	RCC->AHB1ENR |= 0xA;
	RCC->APB1ENR |= (1<<18); 
	
	RCC->AHB1ENR|=0XFF;
	RCC->APB1ENR|=0XA; //Prender Timer 5 A0, A1, A2, A3, Prender Timer 3 C6, C7, C8, C9
	

}
 
void GPIO_SETUP()
{
		
	//To USART 3 by using PD8(TX) and PD9(RX)
	GPIOD->MODER |= 0XA0000;
	GPIOD->AFR[1] |=0X77; 	
	
	GPIOA->MODER|=(2<<0)|(2<<2)|(2<<4)|(2<<6); 
	GPIOA->AFR[0]|=0X2222; 
	GPIOC->MODER|=(2<<12)|(2<<14)|(2<<16)|(2<<18); 
	GPIOC->AFR[0]|=(2<<24)|(2<<28); 
	GPIOC->AFR[1]|=(2<<0)|(2<<4); 
	
	TIM5->ARR=20000-1; //ARR
	TIM5->PSC=16-1; //PSC Para tener 50 Hz
	TIM5->DIER|=0X1; //Habilitar interrupción
	TIM5->EGR|=0X1; //Reinicio del conteo
	TIM5->CCMR1|=0X6060; //Modo PWM Tim5  CH1 y 2 - Como salida 00 en CC1S
	TIM5->CCMR2|=0X6060; //Modo PWM Tim5  CH3 y 4 - Como salida 00 en CC1S
	TIM5->CCER|=(1<<0) | (1<<4)|(1<<8) | (1<<12);  //Habilitar salida de comparación y captura 
	TIM5->CR1|=0X1 | (0<<4); //Habilitar conteo
	TIM3->ARR=20000-1; //ARR
	TIM3->PSC=16-1; //PSC Para tener 50 Hz
	TIM3->DIER|=0X1; //Habilitar interrupción
	TIM3->EGR|=0X1; //Reinicio del conteo
	TIM3->CCMR1|=0X6060; //Modo PWM Tim5  CH1 y 2 - Como salida 00 en CC1S
	TIM3->CCMR2|=0X6060; //Modo PWM Tim5  CH3 y 4 - Como salida 00 en CC1S
	TIM3->CCER|=(1<<0) | (1<<4)|(1<<8) | (1<<12);  //Habilitar salida de comparación y captura 
	TIM3->CR1|=0X1 | (0<<4); //Habilitar conteo
	
//	USART3->BRR |= 0x683; //9600
	USART3->BRR |= 0x8B; //115200
	USART3->CR1 |=((0x2D) | (0<<15));
	//lcd
	RCC->AHB1ENR |=(1<<1)|(1<<3);//SE HABILITAN LOS PINES B, D,
	//LCD
	//D0,D1,D2,D3,D4,D5,D6,D7
	GPIOD ->MODER |= 0x5555;
	GPIOD ->PUPDR |=0xAAAA;
	//En PB8, RS PB9
	GPIOB ->MODER |= 0x50000;
	GPIOB ->PUPDR |=0xA0000;
	//timer
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	//TENER EN CUENTA QUE ESTAMOS CON HSI=16000000
	//I. Cargar valor en PSC
	TIM2->PSC = 350;
	//II. Cargar valor en ARR
	TIM2->ARR = 350;
	//III. Configurar DIER y CR1
	TIM2->DIER |= TIM_DIER_UIE;
	//TIM2->CR1 |=  TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM2_IRQn);
	
	//switch de usuario c13
	RCC->AHB1ENR |= (1<<2);
	GPIOC->PUPDR |= (1<<28);

	NVIC_EnableIRQ(USART3_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn); //Por si queremos la interrupción que igual trabaja con el ARR
	NVIC_EnableIRQ(TIM5_IRQn); //Por si queremos la interrupción que igual trabaja con el ARR

}
 
void SysTick_Init(void){
  SysTick->LOAD = 0x00FFFFFF;
	SysTick->CTRL = 0x00000005;
}

void SysTick_Wait(uint32_t n){
  
	SysTick->LOAD = n-1;
	SysTick->VAL = 0;
	while((SysTick->CTRL&0x00010000)==0);
}

void SysTick_Wait1ms(uint32_t delay){
 for(uint32_t i=0; i<delay; i++){
 SysTick_Wait(16000000/1000);
 }
}
 int USART3_SendChar(int value) { 
   USART3->TDR = value;
   while(!(USART3->ISR & USART_ISR_TXE));
   return 0;
 }
 

extern "C"{
void TIM5_IRQHandler(void){
TIM5->SR &=~(1UL<<0);
}
void TIM3_IRQHandler(void){
TIM3->SR &=~(1UL<<0);
}
void TIM2_IRQHandler(void){      // Interrupci?n Timer 
	TIM2->SR &= ~ (1<<0); //Atendemos la interrupci?n
	if (h == 1) {
					GPIOB->ODR &= ~(1<<8); // Apaga PB8 (ENABLE)
					h = 0;                 // Resetea la bandera
					lcd_busy = 0;          // Indica que el LCD está libre
			}
	
	}
}
// Agrega al buffer
void add_to_buffer(char value, int is_data) {
    if (buffer_index < BUFFER_SIZE) {
        lcd_buffer[buffer_index].value = value;
        lcd_buffer[buffer_index].is_data = is_data;
        buffer_index++;
    }
}

extern "C"{

void USART3_IRQHandler(void){

		while (USART3->ISR & USART_ISR_RXNE){
		char rx = (char)(USART3->RDR & 0xFF);
    if ((rx >= '0' && rx <= '9') || (rx == '#')|| (rx == '-')|| (rx == '+')) {
        if (index < 31) {  // Asegurar que no se salga del límite del arreglo
            arreglo[index] = rx;
            index++;  // Avanzar al siguiente espacio del arreglo
        }
				if(index == 31){
					grados1 = ((int(arreglo[1]) - 48) * 100) + ((int(arreglo[2] ) - 48) * 10) + (int(arreglo[3]) - 48);
					grados2 = ((int(arreglo[4]) - 48) * 100) + ((int(arreglo[5] ) - 48) * 10) + (int(arreglo[6]) - 48);
					grados3 = ((int(arreglo[7]) - 48) * 100) + ((int(arreglo[8] ) - 48) * 10) + (int(arreglo[9]) - 48);
					grados4 = ((int(arreglo[10]) - 48) * 100) + ((int(arreglo[11] ) - 48) * 10) + (int(arreglo[12]) - 48);
					grados5 = ((int(arreglo[13]) - 48) * 100) + ((int(arreglo[14] ) - 48) * 10) + (int(arreglo[15]) - 48);
					grados6 = ((int(arreglo[16]) - 48) * 100) + ((int(arreglo[17] ) - 48) * 10) + (int(arreglo[18]) - 48);
					if((arreglo[19]) == '+'){
						sx = 1.0;
					}
					else{
						sx = -1.0;
					}
					if((arreglo[23]) == '+'){
						sy = 1.0;
					}
					else{
						sy = -1.0;
					}
					if((arreglo[27]) == '+'){
						sz = 1.0;
					}
					else{
						sz = -1.0;
					}
					posx1 = (int(arreglo[20]));
					posx2 = (int(arreglo[21]));
					posx3 =	(int(arreglo[22]));
					posy1 = (int(arreglo[24]));
					posy2 = (int(arreglo[25]));
					posy3 =	(int(arreglo[26]));
					posz1 = (int(arreglo[28]));
					posz2 = (int(arreglo[29]));
					posz3 =	(int(arreglo[30]));
					grados61 = grados6;
					grados31 = grados3;
					grados3 = (180 + grados3 );
					if( grados6 > 20){
						grados6 = 0;
					}		
					index = 0;
				}
    }

  	
}

		}
}

// Procesa el buffer
void process_lcd_buffer(void) {
    if (buffer_pos < buffer_index && lcd_busy == 0) {
        lcd_busy = 1;
        if (lcd_buffer[buffer_pos].is_data) {
            GPIOB->ODR |= (1<<9);  // RS = 1 (dato)
            GPIOD->ODR &= ~(0xFF);
            switch (lcd_buffer[buffer_pos].value) {
							case '+':GPIOD->ODR |= (0x2B);
									break;
							case '-':GPIOD->ODR |= (0x2D);
									break;
							case '.':GPIOD->ODR |= (0x2E);
									break;
							case '0':GPIOD->ODR |= (0x30);
									break;
							case '1':GPIOD->ODR |= (0x31);
									break;
							case '2':GPIOD->ODR |= (0x32);
									break;
							case '3':GPIOD->ODR |= (0x33);
									break;
							case '4':GPIOD->ODR |= (0x34);
									break;
							case '5':GPIOD->ODR |= (0x35);
									break;
							case '6':GPIOD->ODR |= (0x36);
									break;
							case '7':GPIOD->ODR |= (0x37);
									break;
							case '8':GPIOD->ODR |= (0x38);
									break;
							case '9':GPIOD->ODR |= (0x39);
									break;
							case ' ':GPIOD->ODR |= (0x20);
									break;
							case 'A':GPIOD->ODR |= (0x41);
									break;
							case 'B':GPIOD->ODR |= (0x42);
									break;
							case 'C':GPIOD->ODR |= (0x43);
									break;
							case 'D':
									GPIOD->ODR |= (0x44);
									break;
							case 'E':
									GPIOD->ODR |= (0x45);
									break;
							case 'F':
									GPIOD->ODR |= (0x46);
									break;
							case 'G':
									GPIOD->ODR |= (0x47);
									break;
							case 'H':
									GPIOD->ODR |= (0x48);
									break;
							case 'I':
									GPIOD->ODR |= (0x49);
									break;
							case 'J':
									GPIOD->ODR |= (0x4A);
									break;
							case 'K':
									GPIOD->ODR |= (0x4B);
									break;
							case 'L':
									GPIOD->ODR |= (0x4C);
									break;
							case 'M':
									GPIOD->ODR |= (0x4D);
									break;
							case 'N':
									GPIOD->ODR |= (0x4E);
									break;
							case 'O':
									GPIOD->ODR |= (0x4F);
									break;
							case 'P':
									GPIOD->ODR |= (0x50);
									break;
							case 'Q':
									GPIOD->ODR |= (0x51);
									break;
							case 'R':
									GPIOD->ODR |= (0x52);
									break;
							case 'S':
									GPIOD->ODR |= (0x53);
									break;
							case 'T':
									GPIOD->ODR |= (0x54);
									break;
							case 'U':
									GPIOD->ODR |= (0x55);
									break;
							case 'V':
									GPIOD->ODR |= (0x56);
									break;
							case 'W':
									GPIOD->ODR |= (0x57);
									break;
							case 'X':
									GPIOD->ODR |= (0x58);
									break;
							case 'Y':
									GPIOD->ODR |= (0x59);
									break;
							case 'Z':
									GPIOD->ODR |= (0x5A);
									break;
							case '?': // Reemplacé '¿' por '?' asumiendo que era un error tipográfico, ajusta si necesitas el código correcto
									GPIOD->ODR |= (0x4E);
									break;
							case 'z':
									GPIOD->ODR |= (0x7A);
									break;
							case '*':
									GPIOD->ODR |= (0x2A);
									break;
							case '=':
									GPIOD->ODR |= (0x3D);
									break;
							case ':':
									GPIOD->ODR |= (0x3A);
									break;
							case ')':
									GPIOD->ODR |= (0x29);
									break;
							case '(':
									GPIOD->ODR |= (0x28);
									break;
							case '/':
									GPIOD->ODR |= (0x2F);
									break;
							default:
									GPIOD->ODR &= ~(0xFF);
									break;
            }
        } else {
            GPIOB->ODR &= ~(1<<9); // RS = 0 (comando)
            GPIOD->ODR &= ~(0xFF);
            switch (lcd_buffer[buffer_pos].value) {
                case 1: GPIOD->ODR |= 0x01; break;
                case 2: GPIOD->ODR |= 0x02; break;
                case 3: GPIOD->ODR |= 0x0F; break;
                case 4: GPIOD->ODR |= 0x38; break;
                case 5: GPIOD->ODR |= 0x80; break;
								case 6:GPIOD->ODR|=(0xC0); break;
	              case 7:GPIOD->ODR|=(0x10);break;
	              case 8:GPIOD->ODR|=(0x14);break;
                default: GPIOD->ODR &= ~(0xFF); break;
            }
        }
        GPIOB->ODR |= (1<<8);  // Enciende PB8 (ENABLE)
        h = 1;
        TIM2->CNT = 0;
        TIM2->CR1 |= (1<<0);
        buffer_pos++;
    }
    // Limpia el buffer cuando se procesa todo
    if (buffer_pos >= buffer_index) {
        buffer_index = 0;
        buffer_pos = 0;
    }
}
// Funciones para interactuar con el LCD
void Letra(char g) { add_to_buffer(g, 1); }
void Estado(int b) { add_to_buffer(b, 0); }
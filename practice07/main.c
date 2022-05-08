#include "device_config.h"

#define _XTAL_FREQ 8000000
#define SWEEP_FREQ 10

#define LCD_DATA_R          PORTD
#define LCD_DATA_W          LATD
#define LCD_DATA_DIR        TRISD
#define LCD_RS              LATCbits.LATC2
#define LCD_RS_DIR          TRISCbits.TRISC2
#define LCD_RW              LATCbits.LATC1
#define LCD_RW_DIR          TRISCbits.TRISC1
#define LCD_E               LATCbits.LATC0
#define LCD_E_DIR           TRISCbits.TRISC0

void send2LCD(char);
void LCD_rdy(void);
void LCD_init(void);
void LCD_cmd(char);
char key_scanner(void);
uint8_t char_to_seg(uint8_t);
    
void main(void) {
    OSCCON = 0x74;// Set the internal oscillator to 8MHz and stable
    ANSELC = 0;   // Set port C 
    TRISC  = 0x00;      // for Port C, set all pins as outputs
    ANSELD = 0;   // Set port D as Digital for 7 segment anodes
    TRISD  = 0x00;      // for Port D, set all pins as outputs for 7 segment anodes
    ANSELB = 0;         // Puerto B como digital 
    TRISB  = 0xF0;      // Primeros 4 pines de entrada y los siguientes 4 de salida
    
    LCD_DATA_DIR = 0x00;
    
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E  = 0;

    LCD_init(); // Inicializamos nuestro LCD
    char key = key_scanner();
    char simb = 0; // Variables que usaremos para los cálculos
    char res = 0;
    char num1 = 0;
    char num2 = 0;
    while (key == 'x')// El programa espera hasta que se presione una tecla del teclado matricial
    key = key_scanner();
    send2LCD(char_to_seg(key)); // El número es enviado y mostrado en la pantalla del LCD
    num1 = char_to_seg(key); // Guardamos el valor 1 para los calculos posteriores
    key = 'x';
    __delay_ms(2000); // Espera de 2s para no copiar el número anterior
    while (key == 'x')
    key = key_scanner();
    send2LCD(char_to_seg(key));
    simb = key; // Guardamos el valor del símbolo para los calculos posteriores
    key = 'x';
    __delay_ms(2000);
    while (key == 'x')
    key = key_scanner();
    send2LCD(char_to_seg(key));
    num2 = char_to_seg(key); // Guardamos el valor 2 para los calculos posteriores
    key = 'x';
    __delay_ms(2000);
    while (key != 14) // El programa espera a qué se presione la tecla "*" para mostrar el resultado
        key = key_scanner();
    send2LCD(0b00111101);
        if (simb == 10){ // Dependiendo del símbolo elegido, se realizará su operación correspondiente
            res = ((num1-48)+(num2-48));
        send2LCD(res +48);}
        else if (simb == 11){ //Resta
            res = ((num1-48)-(num2-48));
        send2LCD(res +48);
        }
        else if (simb == 12){ //Multiplicación
            res = ((num1-48)*(num2-48));
        send2LCD(res +48);
        }
         else if (simb == 13){ //División
            res = ((num1-48)/(num2-48));
        send2LCD(res +48);
        }
    key = 'x';
    __delay_ms(2000);
    while (key != 15) // Se espera a que se presione la tecla "#" para limpiar la pantalla del LCD.
    key = key_scanner();
    __delay_ms(2000);
    
    
}
void send2LCD(char xy){ 
    // Utilizando los valores correctos de RS, RW y E
    // el programa manda el número puesto a la pantalla del LCD
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_E  = 1;
    LCD_DATA_W = xy;
    Nop();
    Nop();
    LCD_E  = 0;
    __delay_ms(250);
}

void LCD_init(void){
    // Utilizando los valores correctos de RS, RW y E
    // el programa limpia la pantalla del LCD
    LATC = 0;               // Make sure LCD control port is low
    LCD_E_DIR = 0;          // Set Enable as output
    LCD_RS_DIR = 0;         // Set RS as output 
    LCD_RW_DIR = 0;         // Set R/W as output
    LCD_cmd(0x38);          // Display to 2x16
    __delay_ms(250);  
    LCD_cmd(0x0F);          // Display on, cursor on and blinking
    __delay_ms(250);  
    LCD_cmd(0x01);          // Clear display and move cursor home
    __delay_ms(250);  
}

void LCD_rdy(void){
    // Utilizando los valores correctos de RS, RW y E
    // el programa espera a que el LCD no esté realizando ninguna función
    char test;
    // configure LCD data bus for input
    LCD_DATA_DIR = 0xFF;
    test = 0x80;
    while(test){
        LCD_RS = 0;         // select IR register
        LCD_RW = 1;         // set READ mode
        LCD_E  = 1;         // setup to clock data
        test = LCD_DATA_R;
        Nop();
        LCD_E = 0;          // complete the READ cycle
        test &= 0x80;       // check BUSY FLAG 
    }
}

void LCD_cmd(char cx) {
    // Utilizando los valores correctos de RS, RW y E
    // el programa manda el comando al LCD
   // LCD_rdy();              // wait until LCD is ready
    LCD_RS = 0;             // select IR register
    LCD_RW = 0;             // set WRITE mode
    LCD_E  = 1;             // set to clock data
    Nop();
    LCD_DATA_W = cx;        // send out command
    Nop();                  // No operation (small delay to lengthen E pulse)
    LCD_E = 0;              // complete external write cycle
}

char key_scanner(void){
// Función para inicializar nuestro teclado matricial
    LATBbits.LB0 = 0;
    LATBbits.LB1 = 1;
    LATBbits.LB2 = 1;
    LATBbits.LB3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTBbits.RB4 == 0) {__delay_ms(SWEEP_FREQ); return 1;}
    else if (PORTBbits.RB5 == 0) {__delay_ms(SWEEP_FREQ); return 2;}
    else if (PORTBbits.RB6 == 0) {__delay_ms(SWEEP_FREQ); return 3;}
    else if (PORTBbits.RB7 == 0) {__delay_ms(SWEEP_FREQ); return 10;}
    LATBbits.LB0 = 1;
    LATBbits.LB1 = 0;
    LATBbits.LB2 = 1;
    LATBbits.LB3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTBbits.RB4 == 0) {__delay_ms(SWEEP_FREQ); return 4;}
    else if (PORTBbits.RB5 == 0) {__delay_ms(SWEEP_FREQ); return 5;}
    else if (PORTBbits.RB6 == 0) {__delay_ms(SWEEP_FREQ); return 6;}
    else if (PORTBbits.RB7 == 0) {__delay_ms(SWEEP_FREQ); return 11;}
    LATBbits.LB0 = 1;
    LATBbits.LB1 = 1;
    LATBbits.LB2 = 0;
    LATBbits.LB3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTBbits.RB4 == 0) {__delay_ms(SWEEP_FREQ); return 7;}
    else if (PORTBbits.RB5 == 0) {__delay_ms(SWEEP_FREQ); return 8;}
    else if (PORTBbits.RB6 == 0) {__delay_ms(SWEEP_FREQ); return 9;}
    else if (PORTBbits.RB7 == 0) {__delay_ms(SWEEP_FREQ); return 12;}
    LATBbits.LB0 = 1;
    LATBbits.LB1 = 1;
    LATBbits.LB2 = 1;
    LATBbits.LB3 = 0;
    __delay_ms(SWEEP_FREQ);
    if      (PORTBbits.RB4 == 0) {__delay_ms(SWEEP_FREQ); return 14;}
    else if (PORTBbits.RB5 == 0) {__delay_ms(SWEEP_FREQ); return 0;}
    else if (PORTBbits.RB6 == 0) {__delay_ms(SWEEP_FREQ); return 15;}
    else if (PORTBbits.RB7 == 0) {__delay_ms(SWEEP_FREQ); return 13;}   
    else return 'x';
}


uint8_t char_to_seg(uint8_t num){
    // Función para convertir el número del teclado matricial a su equivalente
    // para el LCD
    uint8_t segments;
    switch(num){
        case 0:  segments = 0b00110000; break;
        case 1:  segments = 0b00110001; break;
        case 2:  segments = 0b00110010; break;
        case 3:  segments = 0b00110011; break;
        case 4:  segments = 0b00110100; break;
        case 5:  segments = 0b00110101; break;
        case 6:  segments = 0b00110110; break;
        case 7:  segments = 0b00110111; break;
        case 8:  segments = 0b00111000; break;
        case 9:  segments = 0b00111001; break;
        case 10: segments = 0b00101011; break;
        case 11: segments = 0b00101101; break;
        case 12: segments = 0b00101010; break;
        case 13: segments = 0b00101111; break;
        

    }
    return segments;
}
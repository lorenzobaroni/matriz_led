#include <stdio.h>
#include "pico/stdlib.h"

// Número de LEDs
#define NUM_PIXELS 25

// Pino de saída
#define OUT_PIN 7

// Teclado Matricial
#define ROW1 10
#define ROW2 9
#define ROW3 8
#define ROW4 6

#define COL1 5
#define COL2 4
#define COL3 3
#define COL4 2

// Função para detectar tecla pressionada
char detect_key() {
    const char keys[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    int rows[4] = {ROW1, ROW2, ROW3, ROW4};
    int cols[4] = {COL1, COL2, COL3, COL4};

    for (int i = 0; i < 4; i++) {
        gpio_put(rows[i], 0);
        for (int j = 0; j < 4; j++) {
            if (gpio_get(cols[j]) == 0) {
                sleep_ms(200); // Debounce
                gpio_put(rows[i], 1);
                return keys[i][j];
            }
        }
        gpio_put(rows[i], 1);
    }

    return '\0';
}

// Função para configurar os GPIOs do teclado e LEDs
void setup_gpio() {
    int rows[] = {ROW1, ROW2, ROW3, ROW4};
    int cols[] = {COL1, COL2, COL3, COL4};

    // Configuração das linhas como saída
    for (int i = 0; i < 4; i++) {
        gpio_init(rows[i]);
        gpio_set_dir(rows[i], GPIO_OUT);
        gpio_put(rows[i], 1);
    }

    // Configuração das colunas como entrada com pull-up
    for (int i = 0; i < 4; i++) {
        gpio_init(cols[i]);
        gpio_set_dir(cols[i], GPIO_IN);
        gpio_pull_up(cols[i]);
    }
}

int main() {
    // Configurações iniciais
    stdio_init_all();
    setup_gpio();


    while (true) {
        char key = detect_key();
        if (key != '\0') {
            switch (key) {
                case 'A':
                    
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}

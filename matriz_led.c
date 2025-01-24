#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "pico/bootrom.h"

// Arquivo .pio
#include "matriz_led.pio.h"

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

// Pino do buzzer
#define BUZZER_PIN 21

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

    // Configuração do buzzer
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_put(BUZZER_PIN, 0);
}

// Função para gerar tons no buzzer
void buzzer_tone(uint frequency, uint duration_ms) {
    uint32_t period_us = 1000000 / frequency; 
    uint32_t half_period_us = period_us / 2;  
    uint32_t cycles = (frequency * duration_ms) / 1000;

    for (uint32_t i = 0; i < cycles; i++) {
        gpio_put(BUZZER_PIN, 1);
        sleep_us(half_period_us);
        gpio_put(BUZZER_PIN, 0);
        sleep_us(half_period_us);
    }
}

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

// Função para criar cor RGB
uint32_t matrix_rgb(double b, double r, double g) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

// Inicializa o PIO para a matriz de LEDs
void init_matriz_led(PIO pio, uint *offset, uint *sm) {
    *offset = pio_add_program(pio, &matriz_led_program);
    if (*offset < 0) {
        printf("Erro ao carregar programa PIO.\n");
        return;
    }

    *sm = pio_claim_unused_sm(pio, true);
    if (*sm < 0) {
        printf("Erro ao requisitar state machine.\n");
        return;
    }

    matriz_led_program_init(pio, *sm, *offset, OUT_PIN);
}

// Desenha um padrão na matriz de LEDs
void desenho_pio(PIO pio, uint sm, double b, double r, double g) {
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t valor_led = matrix_rgb(b, r, g);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}

// Função genérica para executar animações com cores diferentes
void executar_animacao(PIO pio, uint sm, double frames[][NUM_PIXELS], int num_frames, double r, double g, double b, int sleep_time) {
    for (int frame = 0; frame < num_frames; frame++) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            double intensidade = frames[frame][i];
            uint32_t valor_led = matrix_rgb(b * intensidade, r * intensidade, g * intensidade);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        sleep_ms(sleep_time);
    }
}

// Função de animação para o case '0'
void animacao_leds_0(PIO pio, uint sm) {
    double frames[5][NUM_PIXELS] = {
        {0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8},
        {0.8, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6},
        {0.6, 0.8, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4},
        {0.4, 0.6, 0.8, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2},
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0}
    };
    executar_animacao(pio, sm, frames, 5, 1.0, 0.0, 1.0, 400);
}

// Função de animação para o case '1'
void animacao_leds_1(PIO pio, uint sm) {
    double frames[5][NUM_PIXELS] = {
        {0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0},
        {0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2},
        {0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4},
        {0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6},
        {0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8}
    };
    executar_animacao(pio, sm, frames, 5, 1.0, 1.0, 0.0, 200);
}

// Função de animação para o case '2'
void animacao_leds_2(PIO pio, uint sm) {
    double frames[5][NUM_PIXELS] = {
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0},
        {1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2},
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0},
        {1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2},
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0}
    };
    executar_animacao(pio, sm, frames, 5, 0.0, 0.1, 1.0, 300);
}

// Sincroniza LEDs e buzzer
void animacao_leds_3(PIO pio, uint sm) {
    double frames[5][NUM_PIXELS] = {
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0},
        {1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2},
        {0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4},
        {0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8},
        {0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 1.0, 0.2}
    };

    for (int frame = 0; frame < 5; frame++) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            uint32_t valor_led = matrix_rgb(0.5, 0.0, frames[frame][i]); // LEDs azuis sincronizados
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        buzzer_tone(440, 100); // Sincroniza o som do buzzer
        sleep_ms(200); // Tempo entre os frames
    }
}

int main() {
    PIO pio = pio0;
    uint offset, sm;

    // Configurações iniciais
    stdio_init_all();
    setup_gpio();
    init_matriz_led(pio, &offset, &sm);

    while (true) {
        char key = detect_key();
        if (key != '\0') {
            switch (key) {
                case '0':
                    animacao_leds_0(pio, sm);
                    break;
                case '1':
                    animacao_leds_1(pio, sm);
                    break;
                case '2':
                    animacao_leds_2(pio, sm);
                    break;
                case '3':
                    animacao_leds_3(pio, sm);
                    break;
                case 'A':
                    desenho_pio(pio, sm, 0.0, 0.0, 0.0);
                    break;
                case 'B':
                    desenho_pio(pio, sm, 1.0, 0.0, 0.0);
                    break;
                case 'C':
                    desenho_pio(pio, sm, 0.0, 0.8, 0.0);
                    break;
                case 'D':
                    desenho_pio(pio, sm, 0.0, 0.0, 0.5);
                    break;
                case '#':
                    desenho_pio(pio, sm, 0.2, 0.2, 0.2);
                    break;
                case '*':
                    printf("HABILITANDO O MODO GRAVAÇÃO\n");
                    reset_usb_boot(0, 0);
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}

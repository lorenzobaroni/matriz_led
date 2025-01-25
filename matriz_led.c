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

// Estrutura para armazenar dados de uma animação
typedef struct {
    double frames[10][NUM_PIXELS];
    int num_frames;
    double r, g, b;
    int fps; // Frames por segundo, máximo de 30
} Animacao;

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
                sleep_ms(200);
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

void executar_animacao(PIO pio, uint sm, Animacao *anim, int buzzer_freq, int buzzer_duration) {
    int frame_delay = 1000 / anim->fps; // Calcula o tempo entre frames em milissegundos
    for (int frame = 0; frame < anim->num_frames; frame++) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            double intensidade = anim->frames[frame][i];
            uint32_t valor_led = matrix_rgb(anim->b * intensidade, anim->r * intensidade, anim->g * intensidade);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        if (buzzer_freq > 0 && buzzer_duration > 0) {
            buzzer_tone(buzzer_freq, buzzer_duration);
        }
        sleep_ms(frame_delay); // Usa o tempo calculado com base no FPS
    }
}

void executar_animacao_multicolor(PIO pio, uint sm, Animacao *anim, int buzzer_freq, int buzzer_duration, double r2, double g2, double b2) {
    int frame_delay = 1000 / anim->fps; // Calcula o tempo entre frames em milissegundos
    for (int frame = 0; frame < anim->num_frames; frame++) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            double intensidade = anim->frames[frame][i];
            uint32_t valor_led;
            if (i % 2 == 0) {
                valor_led = matrix_rgb(anim->b * intensidade, anim->r * intensidade, anim->g * intensidade);
            } else {
                valor_led = matrix_rgb(b2 * intensidade, r2 * intensidade, g2 * intensidade);
            }
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        if (buzzer_freq > 0 && buzzer_duration > 0) {
            buzzer_tone(buzzer_freq, buzzer_duration);
        }
        sleep_ms(frame_delay); // Usa o tempo calculado com base no FPS
    }
}

// Configuração de animações
Animacao animacao_0 = {
    .frames = {
        {0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8},
        {0.8, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6},
        {0.6, 0.8, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4},
        {0.4, 0.6, 0.8, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0, 0.2},
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0}
    },
    .num_frames = 5,
    .r = 1.0,
    .g = 0.0,
    .b = 1.0,
    .fps = 7
};

Animacao animacao_1 = {
    .frames = {
        {0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0, 0.2, 0.0},
        {0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2, 0.4, 0.2},
        {0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4, 0.6, 0.4},
        {0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6, 0.8, 0.6},
        {0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8, 1.0, 0.8}
    },
    .num_frames = 5,
    .r = 1.0,
    .g = 0.8,
    .b = 0.0,
    .fps = 5
};

Animacao animacao_2 = {
    .frames = {
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0},
        {1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2},
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0},
        {1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2},
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0}
    },
    .num_frames = 5,
    .r = 0.0,
    .g = 0.0,
    .b = 1.0,
    .fps = 5
};

Animacao animacao_3 = {
    .frames = {
        {0.8, 0.6, 0.4, 0.2, 0.0, 1.0, 0.8, 0.6, 0.4, 0.2, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 0.0},
        {0.2, 0.4, 0.6, 0.8, 1.0, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.0, 0.2, 0.4, 0.6},
        {1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2, 1.0, 0.8, 0.6, 0.4, 0.2},
        {0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4},
        {0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8, 1.0, 0.6, 0.8, 1.0, 0.2, 0.4, 0.6, 0.8}
    },
    .num_frames = 5,
    .r = 0.5,
    .g = 0.0,
    .b = 0.0,
    .fps = 5
};

Animacao animacao_4_lorenzo = {
    .frames = {
        {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0}, // L
        {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}, // O
        {1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}, // R
        {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0}, // E
        {1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0}, // N
        {1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0}, // Z
        {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0}  // O
    },
    .num_frames = 7,
    .r = 0.0,
    .g = 0.0,
    .b = 0.0, // As cores serão tratadas dinamicamente por letra
    .fps = 2
};

    const double lorenzo_colors[7][3] = {
        {1.0, 0.0, 0.0}, // L - Vermelho
        {0.0, 1.0, 0.0}, // O - Verde
        {0.0, 0.0, 1.0}, // R - Azul
        {1.0, 1.0, 0.0}, // E - Amarelo
        {1.0, 0.0, 1.0}, // N - Magenta
        {0.0, 1.0, 1.0}, // Z - Ciano
        {1.0, 0.5, 0.0}  // O - Laranja
    };

    Animacao animacao_5 = {
    .frames = {
        {1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
        {1.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
    },
    .num_frames = 10,
    .r = 0.0,
    .g = 1.0,
    .b = 1.0,
    .fps = 3
};

    void executar_animacao_lorenzo(PIO pio, uint sm) {
    for (int frame = 0; frame < animacao_4_lorenzo.num_frames; frame++) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            double intensidade = animacao_4_lorenzo.frames[frame][i];
            uint32_t valor_led = matrix_rgb(lorenzo_colors[frame][2] * intensidade, lorenzo_colors[frame][0] * intensidade, lorenzo_colors[frame][1] * intensidade);
            pio_sm_put_blocking(pio, sm, valor_led);
        }
        buzzer_tone(440 + (frame * 50), 200);
        sleep_ms(1000 / animacao_4_lorenzo.fps); // Calcula o tempo entre frames com base no FPS
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
                    executar_animacao(pio, sm, &animacao_0, 0, 0);
                    break;
                case '1':
                    executar_animacao(pio, sm, &animacao_1, 0, 0);
                    break;
                case '2':
                    executar_animacao(pio, sm, &animacao_2, 0, 0);
                    break;
                case '3':
                    executar_animacao_multicolor(pio, sm, &animacao_3, 440, 100, 0.0, 0.0, 1.0);
                    break;
                case '4':
                    executar_animacao_lorenzo(pio, sm);
                    break;
                case '5':
                    executar_animacao(pio, sm, &animacao_5, 0, 0);
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
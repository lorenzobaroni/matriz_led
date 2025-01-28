# 🚀 Projeto: Controle de Matriz de LEDs 5x5 com Buzzer e Teclado Matricial

Este projeto implementa um sistema interativo utilizando uma matriz de LEDs 5x5 controlada por um teclado matricial, um buzzer para efeitos sonoros e um Raspberry Pi Pico. Ele inclui diversas animações e funcionalidades que podem ser ativadas pelas teclas do teclado.

## Link do Vídeo Ensaio

[Demonstração de Funcionamento do Projeto](https://www.youtube.com/watch?v=_eAccCzs0Us)

## Funcionalidades Principais

### Animações

Cada tecla do teclado ativa uma animação ou funcionalidade específica na matriz de LEDs:

- **Tecla 0**: Gradiente rosa.
- **Tecla 1**: Animação de fade (0 > 1 > 0 > 1).
- **Tecla 2**: Pisca-pisca com buzzer.
- **Tecla 3**: Pisca-pisca multicolorido.
- **Tecla 4**: Animação de barras.
- **Tecla 5**: Exibe o nome "Lorenzo" com cores variadas.
- **Tecla 6**: Música (notas: dó, ré, mi, fá) com LEDs representando o ritmo.
- **Tecla 7**: Sirene de polícia com alternância de cores e som.
- **Tecla 8**: Contagem regressiva (5, 4, 3, 2, 1).
- **Tecla 9**: Animação personalizada.

### Outras Funções

- **Tecla A**: Desliga todos os LEDs.
- **Tecla B**: Liga todos os LEDs em azul (intensidade máxima).
- **Tecla C**: Liga todos os LEDs em vermelho (80% de intensidade).
- **Tecla D**: Liga todos os LEDs em verde (50% de intensidade).
- **Tecla #**: Liga todos os LEDs em branco (20% de intensidade).
- **Tecla \***: Ativa o modo de gravação e reinicia em modo USB.

## Componentes Utilizados

- **Raspberry Pi Pico**
- **Matriz de LEDs (5x5)**
- **Teclado matricial 4x4**
- **Buzzer**
- **Resistores para conexões**

## Requisitos do Projeto

1. **Animações com FPS e no mínimo 5 frames cada.**
2. **Controles de LEDs por intensidade de cor.**
3. **Sinalização sonora com buzzer.**
4. **Modo de gravação ativado pela tecla \***.
5. **Simulação Wokwi com VS Code.**

## Estrutura do Código

O projeto está estruturado da seguinte forma:

- **Setup GPIO**: Configuração inicial dos GPIOs para o teclado, LEDs e buzzer.
- **Detecção de Teclas**: Identificação da tecla pressionada no teclado matricial.
- **Animações**: Cada animação é implementada como uma estrutura com frames, FPS e cores.
- **Funções de Controle**: Funções para gerenciar LEDs e o buzzer.
- **Loop Principal**: Detecta a tecla pressionada e executa a funcionalidade correspondente.

## Como Executar

1. **Clone o repositório**:
   ```bash
   git clone https://github.com/lorenzobaroni/matriz_led.git
   ```

2. **Abra o projeto no VS Code.**

3. **Compile o código**

4. **Clique no arquivo diagram.json e teste o projeto no simulador do WOKWI Integrado no VS Code**

5. **Carregue o código no Raspberry Pi Pico.**

6. **Conecte os componentes (teclado, matriz de LEDs e buzzer) conforme descrito no código.**

7. **Pressione as teclas no teclado para ativar as animações e funcionalidades.**

## Diagrama de Conexões

- **Matriz de LEDs**: Pino de saída conectado ao GPIO 7.
- **Teclado Matricial**:
  - Linhas: GPIOs 10, 9, 8, 6.
  - Colunas: GPIOs 5, 4, 3, 2.
- **Buzzer**: Conectado ao GPIO 21.

## Pré-requisitos

- Ambiente de desenvolvimento configurado para o Raspberry Pi Pico.
- Bibliotecas do SDK do Pico.
- Simulador Wokwi integrado ao VS Code.

## Licença

Este programa foi desenvolvido como um exemplo educacional e pode ser usado livremente para fins de estudo e aprendizado.

## Autor

- LORENZO GIUSEPPE OLIVEIRA BARONI
- LUIZ VICTOR RIBEIRO SANTOS
- MARY NICOLE DE SOUSA MENDES
- ÍTALO ALVES TENÓRIO DE ALMEIDA
- JOÃO PEDRO FERREIRA DE JESUS
- JOÃO PAULO SANTOS LUZ PEREIRA
- FELIPE SILVA QUEIROZ

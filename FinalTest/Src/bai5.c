#include <stdint.h>
#include "stm32f401re_gpio.h"
#include "stm32f401re_rcc.h"

#define GPIO_PIN_SET        1
#define GPIO_PIN_RESET      0

// LED & Buzzer
#define LED_GREEN_PORT      GPIOA
#define LED_GREEN_PIN       GPIO_Pin_0

#define LED_BLUE_PORT       GPIOA
#define LED_BLUE_PIN        GPIO_Pin_10

#define LED3_BLUE_PORT      GPIOA
#define LED3_BLUE_PIN       GPIO_Pin_3

#define BUZZER_PORT         GPIOC
#define BUZZER_PIN          GPIO_Pin_9

// Nút
#define BUTTON_B2_PORT      GPIOB
#define BUTTON_B2_PIN       GPIO_Pin_3

#define BUTTON_B5_PORT      GPIOB
#define BUTTON_B5_PIN       GPIO_Pin_4

void delay(uint32_t time) {
    for (volatile uint32_t i = 0; i < time; i++);
}

void GPIO_Set(GPIO_TypeDef *port, uint16_t pin, uint8_t status) {
    if (status == GPIO_PIN_SET) {
        port->BSRRL = pin;
    } else {
        port->BSRRH = pin;
    }
}

uint8_t Button_Read(GPIO_TypeDef *port, uint16_t pin) {
    return (port->IDR & pin) ? 1 : 0;
}

void GPIO_Init_All(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    // Output: LED + Buzzer
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin = LED_GREEN_PIN;
    GPIO_Init(LED_GREEN_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LED_BLUE_PIN;
    GPIO_Init(LED_BLUE_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = LED3_BLUE_PIN;
    GPIO_Init(LED3_BLUE_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);

    // Input: Buttons
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_InitStructure.GPIO_Pin = BUTTON_B2_PIN;
    GPIO_Init(BUTTON_B2_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BUTTON_B5_PIN;
    GPIO_Init(BUTTON_B5_PORT, &GPIO_InitStructure);
}

void Blink(GPIO_TypeDef *port, uint16_t pin, int times) {
    for (int i = 0; i < times; i++) {
        GPIO_Set(port, pin, GPIO_PIN_SET);
        delay(300000);
        GPIO_Set(port, pin, GPIO_PIN_RESET);
        delay(300000);
    }
}

void Beep_Once(void) {
    GPIO_Set(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
    delay(300000);
    GPIO_Set(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
}

int main(void) {
    GPIO_Init_All();

    // Nháy LED BLUE một lần báo nguồn
    GPIO_Set(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_SET);
        GPIO_Set(LED3_BLUE_PORT, LED3_BLUE_PIN, GPIO_PIN_SET);

        // Nháy LED BLUE một lần báo nguồn
        Blink(LED_BLUE_PORT, LED_BLUE_PIN, 1);
        Blink(LED3_BLUE_PORT, LED3_BLUE_PIN, 1);
    // Thử nháy LED3_BLUE (PA3) để kiểm tra phần cứng
    Blink(LED3_BLUE_PORT, LED3_BLUE_PIN, 1);

    uint8_t B2_last = 1;
    uint8_t B5_last = 1;
    uint8_t blue_on = 0;

    while (1) {
        //Nút B2: nháy GREEN + BLUE, còi kêu 1 lần
        uint8_t B2_state = Button_Read(BUTTON_B2_PORT, BUTTON_B2_PIN);
        if (B2_state == 0 && B2_last == 1) {
            delay(100000); // debounce
            if (Button_Read(BUTTON_B2_PORT, BUTTON_B2_PIN) == 0) {
                GPIO_Set(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
                GPIO_Set(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_SET);
                GPIO_Set(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
                delay(300000);
                GPIO_Set(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
                GPIO_Set(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);
                GPIO_Set(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
            }
        }
        B2_last = B2_state;

        //Nút B5: Toggle LED_BLUE (LED2 - PA10)
        uint8_t B5_state = Button_Read(BUTTON_B5_PORT, BUTTON_B5_PIN);
        if (B5_last == 1 && B5_state == 0) {  // Phát hiện cạnh xuống
            delay(20000); // debounce
            if (Button_Read(BUTTON_B5_PORT, BUTTON_B5_PIN) == 0) {
                if (blue_on == 0) {
                    GPIO_Set(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_SET);
                    blue_on = 1;
                } else {
                    GPIO_Set(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_PIN_RESET);
                    blue_on = 0;
                }
            }
        }
        B5_last = B5_state;

        delay(10000);
    }

    return 0;
}

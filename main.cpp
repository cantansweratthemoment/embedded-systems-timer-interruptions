#include <iostream>
#include "hal.h"

using namespace std;

const int T = 150;

int sw_num[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};

int getDelay(int x) {
    return 500 + x * T;
}

int getX() {
    return (HAL_GPIO_ReadPin(GPIOE, sw_num[0]) * 1 + HAL_GPIO_ReadPin(GPIOE, sw_num[1]) * 2 +
            HAL_GPIO_ReadPin(GPIOE, sw_num[2]) * 4 + HAL_GPIO_ReadPin(GPIOE, sw_num[3]) * 8);
}

int pattern[8][8] = {{0, 0, 0, 0, 0, 0, 1, 1},
                     {0, 0, 0, 0, 0, 1, 1, 0},
                     {0, 0, 0, 0, 1, 1, 0, 0},
                     {0, 0, 0, 1, 1, 0, 0, 0},
                     {0, 0, 1, 1, 0, 0, 0, 0},
                     {0, 1, 1, 0, 0, 0, 0, 0},
                     {1, 1, 0, 0, 0, 0, 0, 0},
                     {1, 0, 0, 0, 0, 0, 0, 1}};

int leds_num[] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_11, GPIO_PIN_12};
int i = 0;

void TIM6_IRQ_Handler() {
    for (int j = 0; j < 8; j++) {
        if (pattern[i][j]) {
            HAL_GPIO_WritePin(GPIOD, leds_num[j], GPIO_PIN_SET);
        }
    }
}

void TIM7_IRQ_Handler() {
    for (int j = 0; j < 8; j++) {
        if (pattern[i][j]) {
            HAL_GPIO_WritePin(GPIOD, leds_num[j], GPIO_PIN_RESET);
        }
    }
    i++;
    if (i == 8) {
        i = 0;
    }
}

int umain() {
    i = 0;
    registerTIM6_IRQHandler(TIM6_IRQ_Handler);
    registerTIM7_IRQHandler(TIM7_IRQ_Handler);
    __enable_irq();
    WRITE_REG(TIM6_ARR, 500);
    WRITE_REG(TIM6_DIER, TIM_DIER_UIE);
    WRITE_REG(TIM6_PSC, 0);
    WRITE_REG(TIM7_ARR, 500);
    WRITE_REG(TIM7_DIER, TIM_DIER_UIE);
    WRITE_REG(TIM7_PSC, 1);
    WRITE_REG(TIM6_CR1, TIM_CR1_CEN);
    WRITE_REG(TIM7_CR1, TIM_CR1_CEN);
    while (true) {
        HAL_Delay(1);
        WRITE_REG(TIM6_ARR, getDelay(getX()));
        WRITE_REG(TIM7_ARR, getDelay(getX()));
    }
    return 0;
}

#include "stm32f4xx.h"

int main(void)
{
    RCC->AHB1ENR |= (1 << 0) | (1 << 2);    // clock GPIOA, GPIOC
    RCC->APB1ENR |= (1 << 0);               // clock TIM2

    // PC13 output (LED on-board, aktif low)
    GPIOC->MODER &= ~(3 << 26);
    GPIOC->MODER |=  (1 << 26);
    GPIOC->ODR |= (1 << 13);

    // PA0 input pull-up (tombol on-board)
    GPIOA->MODER &= ~(3 << 0);
    GPIOA->PUPDR &= ~(3 << 0);
    GPIOA->PUPDR |=  (1 << 0);

    // TIM2: 16 MHz / 16000 = 1 kHz, ARR 100 -> toggle tiap 100 ms (5 Hz)
    TIM2->PSC = 15999;
    TIM2->ARR = 99;
    TIM2->EGR |= (1 << 0);
    TIM2->SR = 0;
    TIM2->DIER |= (1 << 0);
    NVIC_EnableIRQ(TIM2_IRQn);

    while (1) {
        if ((GPIOA->IDR & (1 << 0)) == 0) { // tombol ditekan
            TIM2->CR1 |= (1 << 0);
        } else {                            // tombol dilepas
            TIM2->CR1 &= ~(1 << 0);
            TIM2->CNT = 0;
            GPIOC->ODR |= (1 << 13);        // LED mati
        }
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & (1 << 0)) {
        TIM2->SR &= ~(1 << 0);
        GPIOC->ODR ^= (1 << 13);
    }
}
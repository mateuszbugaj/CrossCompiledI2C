#include <cstddef>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

int main(void)
{
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
  rcc_periph_clock_enable(RCC_GPIOC);
  gpio_set_mode(GPIOC,
                GPIO_MODE_OUTPUT_2_MHZ,
                GPIO_CNF_OUTPUT_PUSHPULL,
                GPIO13);

  int *test_var = new (int);
  *test_var = 5;

  while (1)
  {
    gpio_set(GPIOC, GPIO13);
    for (std::size_t i(0); i < 6000000; ++i)
    {
      __asm__ volatile("nop");
    }

    gpio_clear(GPIOC, GPIO13);
    for (std::size_t i(0); i < 6000000; ++i)
    {
      __asm__ volatile("nop");
    }
  }

  return 0;
}
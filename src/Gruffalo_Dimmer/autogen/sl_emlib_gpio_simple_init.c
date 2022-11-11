#include "sl_emlib_gpio_simple_init.h"
#include "sl_emlib_gpio_init_btn1_Pin_config.h"
#include "sl_emlib_gpio_init_quad1_0_Pin_config.h"
#include "sl_emlib_gpio_init_quad1_1_Pin_config.h"
#include "em_gpio.h"
#include "em_cmu.h"

void sl_emlib_gpio_simple_init(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  GPIO_PinModeSet(SL_EMLIB_GPIO_INIT_BTN1_PIN_PORT,
                  SL_EMLIB_GPIO_INIT_BTN1_PIN_PIN,
                  SL_EMLIB_GPIO_INIT_BTN1_PIN_MODE,
                  SL_EMLIB_GPIO_INIT_BTN1_PIN_DOUT);

  GPIO_PinModeSet(SL_EMLIB_GPIO_INIT_QUAD1_0_PIN_PORT,
                  SL_EMLIB_GPIO_INIT_QUAD1_0_PIN_PIN,
                  SL_EMLIB_GPIO_INIT_QUAD1_0_PIN_MODE,
                  SL_EMLIB_GPIO_INIT_QUAD1_0_PIN_DOUT);

  GPIO_PinModeSet(SL_EMLIB_GPIO_INIT_QUAD1_1_PIN_PORT,
                  SL_EMLIB_GPIO_INIT_QUAD1_1_PIN_PIN,
                  SL_EMLIB_GPIO_INIT_QUAD1_1_PIN_MODE,
                  SL_EMLIB_GPIO_INIT_QUAD1_1_PIN_DOUT);
}

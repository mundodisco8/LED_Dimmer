#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// $[CMU]
// [CMU]$

// $[LFXO]
// [LFXO]$

// $[PRS.ASYNCH0]
// [PRS.ASYNCH0]$

// $[PRS.ASYNCH1]
// [PRS.ASYNCH1]$

// $[PRS.ASYNCH2]
// [PRS.ASYNCH2]$

// $[PRS.ASYNCH3]
// [PRS.ASYNCH3]$

// $[PRS.ASYNCH4]
// [PRS.ASYNCH4]$

// $[PRS.ASYNCH5]
// [PRS.ASYNCH5]$

// $[PRS.ASYNCH6]
// [PRS.ASYNCH6]$

// $[PRS.ASYNCH7]
// [PRS.ASYNCH7]$

// $[PRS.ASYNCH8]
// [PRS.ASYNCH8]$

// $[PRS.ASYNCH9]
// [PRS.ASYNCH9]$

// $[PRS.ASYNCH10]
// [PRS.ASYNCH10]$

// $[PRS.ASYNCH11]
// [PRS.ASYNCH11]$

// $[PRS.SYNCH0]
// [PRS.SYNCH0]$

// $[PRS.SYNCH1]
// [PRS.SYNCH1]$

// $[PRS.SYNCH2]
// [PRS.SYNCH2]$

// $[PRS.SYNCH3]
// [PRS.SYNCH3]$

// $[GPIO]
// [GPIO]$

// $[TIMER0]
// TIMER0 CC0 on PC00
#ifndef TIMER0_CC0_PORT                         
#define TIMER0_CC0_PORT                          SL_GPIO_PORT_C
#endif
#ifndef TIMER0_CC0_PIN                          
#define TIMER0_CC0_PIN                           0
#endif

// TIMER0 CC1 on PC01
#ifndef TIMER0_CC1_PORT                         
#define TIMER0_CC1_PORT                          SL_GPIO_PORT_C
#endif
#ifndef TIMER0_CC1_PIN                          
#define TIMER0_CC1_PIN                           1
#endif

// TIMER0 CC2 on PC02
#ifndef TIMER0_CC2_PORT                         
#define TIMER0_CC2_PORT                          SL_GPIO_PORT_C
#endif
#ifndef TIMER0_CC2_PIN                          
#define TIMER0_CC2_PIN                           2
#endif

// [TIMER0]$

// $[TIMER1]
// [TIMER1]$

// $[TIMER2]
// [TIMER2]$

// $[TIMER3]
// [TIMER3]$

// $[TIMER4]
// [TIMER4]$

// $[USART0]
// [USART0]$

// $[USART1]
// [USART1]$

// $[I2C1]
// [I2C1]$

// $[PDM]
// [PDM]$

// $[LETIMER0]
// [LETIMER0]$

// $[IADC0]
// [IADC0]$

// $[I2C0]
// [I2C0]$

// $[EUART0]
// EUART0 RX on PB00
#ifndef EUART0_RX_PORT                          
#define EUART0_RX_PORT                           SL_GPIO_PORT_B
#endif
#ifndef EUART0_RX_PIN                           
#define EUART0_RX_PIN                            0
#endif

// EUART0 TX on PA00
#ifndef EUART0_TX_PORT                          
#define EUART0_TX_PORT                           SL_GPIO_PORT_A
#endif
#ifndef EUART0_TX_PIN                           
#define EUART0_TX_PIN                            0
#endif

// [EUART0]$

// $[PTI]
// [PTI]$

// $[MODEM]
// [MODEM]$

// $[CUSTOM_PIN_NAME]
#ifndef _PORT                                   
#define _PORT                                    SL_GPIO_PORT_A
#endif
#ifndef _PIN                                    
#define _PIN                                     0
#endif




#ifndef quad1_0_PORT                            
#define quad1_0_PORT                             SL_GPIO_PORT_A
#endif
#ifndef quad1_0_PIN                             
#define quad1_0_PIN                              4
#endif

#ifndef quad1_1_PORT                            
#define quad1_1_PORT                             SL_GPIO_PORT_A
#endif
#ifndef quad1_1_PIN                             
#define quad1_1_PIN                              5
#endif

#ifndef btn0_PORT                               
#define btn0_PORT                                SL_GPIO_PORT_A
#endif
#ifndef btn0_PIN                                
#define btn0_PIN                                 6
#endif

#ifndef quad0_0_PORT                            
#define quad0_0_PORT                             SL_GPIO_PORT_A
#endif
#ifndef quad0_0_PIN                             
#define quad0_0_PIN                              7
#endif

#ifndef quad0_1_PORT                            
#define quad0_1_PORT                             SL_GPIO_PORT_A
#endif
#ifndef quad0_1_PIN                             
#define quad0_1_PIN                              8
#endif






#ifndef pwm0_PORT                               
#define pwm0_PORT                                SL_GPIO_PORT_C
#endif
#ifndef pwm0_PIN                                
#define pwm0_PIN                                 0
#endif

#ifndef pwm1_PORT                               
#define pwm1_PORT                                SL_GPIO_PORT_C
#endif
#ifndef pwm1_PIN                                
#define pwm1_PIN                                 1
#endif

#ifndef pwm2_PORT                               
#define pwm2_PORT                                SL_GPIO_PORT_C
#endif
#ifndef pwm2_PIN                                
#define pwm2_PIN                                 2
#endif


#ifndef btn1_PORT                               
#define btn1_PORT                                SL_GPIO_PORT_C
#endif
#ifndef btn1_PIN                                
#define btn1_PIN                                 4
#endif







// [CUSTOM_PIN_NAME]$

#endif // PIN_CONFIG_H


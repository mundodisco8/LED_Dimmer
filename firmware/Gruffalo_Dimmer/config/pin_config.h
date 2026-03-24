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
// GPIO EM4WU7 on PC05
#ifndef GPIO_EM4WU7_PORT                        
#define GPIO_EM4WU7_PORT                         SL_GPIO_PORT_C
#endif
#ifndef GPIO_EM4WU7_PIN                         
#define GPIO_EM4WU7_PIN                          5
#endif

// GPIO EM4WU8 on PC07
#ifndef GPIO_EM4WU8_PORT                        
#define GPIO_EM4WU8_PORT                         SL_GPIO_PORT_C
#endif
#ifndef GPIO_EM4WU8_PIN                         
#define GPIO_EM4WU8_PIN                          7
#endif

// [GPIO]$

// $[TIMER0]
// TIMER0 CC0 on PB01
#ifndef TIMER0_CC0_PORT                         
#define TIMER0_CC0_PORT                          SL_GPIO_PORT_B
#endif
#ifndef TIMER0_CC0_PIN                          
#define TIMER0_CC0_PIN                           1
#endif

// TIMER0 CC1 on PB02
#ifndef TIMER0_CC1_PORT                         
#define TIMER0_CC1_PORT                          SL_GPIO_PORT_B
#endif
#ifndef TIMER0_CC1_PIN                          
#define TIMER0_CC1_PIN                           2
#endif

// TIMER0 CC2 on PB03
#ifndef TIMER0_CC2_PORT                         
#define TIMER0_CC2_PORT                          SL_GPIO_PORT_B
#endif
#ifndef TIMER0_CC2_PIN                          
#define TIMER0_CC2_PIN                           3
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




#ifndef LED1_PORT                               
#define LED1_PORT                                SL_GPIO_PORT_A
#endif
#ifndef LED1_PIN                                
#define LED1_PIN                                 4
#endif

#ifndef LED2_PORT                               
#define LED2_PORT                                SL_GPIO_PORT_A
#endif
#ifndef LED2_PIN                                
#define LED2_PIN                                 5
#endif

#ifndef quad1_0_PORT                            
#define quad1_0_PORT                             SL_GPIO_PORT_A
#endif
#ifndef quad1_0_PIN                             
#define quad1_0_PIN                              6
#endif

#ifndef quad1_1_PORT                            
#define quad1_1_PORT                             SL_GPIO_PORT_A
#endif
#ifndef quad1_1_PIN                             
#define quad1_1_PIN                              7
#endif



#ifndef PWM2_PORT                               
#define PWM2_PORT                                SL_GPIO_PORT_B
#endif
#ifndef PWM2_PIN                                
#define PWM2_PIN                                 1
#endif

#ifndef PWM1_PORT                               
#define PWM1_PORT                                SL_GPIO_PORT_B
#endif
#ifndef PWM1_PIN                                
#define PWM1_PIN                                 2
#endif

#ifndef PWM0_PORT                               
#define PWM0_PORT                                SL_GPIO_PORT_B
#endif
#ifndef PWM0_PIN                                
#define PWM0_PIN                                 3
#endif





#ifndef quad0_0_PORT                            
#define quad0_0_PORT                             SL_GPIO_PORT_C
#endif
#ifndef quad0_0_PIN                             
#define quad0_0_PIN                              3
#endif

#ifndef LED0_PORT                               
#define LED0_PORT                                SL_GPIO_PORT_C
#endif
#ifndef LED0_PIN                                
#define LED0_PIN                                 4
#endif

#ifndef btn1_PORT                               
#define btn1_PORT                                SL_GPIO_PORT_C
#endif
#ifndef btn1_PIN                                
#define btn1_PIN                                 5
#endif

#ifndef quad0_1_PORT                            
#define quad0_1_PORT                             SL_GPIO_PORT_C
#endif
#ifndef quad0_1_PIN                             
#define quad0_1_PIN                              6
#endif

#ifndef btn0_PORT                               
#define btn0_PORT                                SL_GPIO_PORT_C
#endif
#ifndef btn0_PIN                                
#define btn0_PIN                                 7
#endif




// [CUSTOM_PIN_NAME]$

#endif // PIN_CONFIG_H


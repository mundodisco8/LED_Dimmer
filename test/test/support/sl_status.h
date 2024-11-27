#ifndef SL_STATUS_H
#define SL_STATUS_H

/*****************************************************************************************
 *
 * FAKE sl_status.H USED IN TESTS. CONTAINS THE MINIMUM AMOUNT OF IT TO RUN
 *
 ****************************************************************************************/
#include <inttypes.h>

typedef uint32_t sl_status_t;

#define SL_STATUS_OK    ((sl_status_t)0x0000)  ///< No error.
#define SL_STATUS_NOT_READY             ((sl_status_t)0x0003)  ///< Module is not ready for requested operation.
#define SL_STATUS_INVALID_STATE         ((sl_status_t)0x0002)  ///< Generic invalid state error.
#define SL_STATUS_NULL_POINTER          ((sl_status_t)0x0022)  ///< Invalid null pointer received as argument.


#endif // SL_STATUS_H
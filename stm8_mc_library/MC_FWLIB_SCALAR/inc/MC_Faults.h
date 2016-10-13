#define BUS_OVERVOLTAGE         1
#define BUS_UNDERVOLTAGE        2
#define OVER_CURRENT            4
#define HEATSINK_TEMPERATURE    8

#define HW_ERROR_MSG_1 " OVER VOLTAGE  "
#define HW_ERROR_MSG_2 " UNDER VOLTAGE "
#define HW_ERROR_MSG_3 " OVER CURRENT  "
#define HW_ERROR_MSG_4 "OVERTEMPERATURE"

#define HW_ERROR_NUM            4
#define HW_ERROR_NUM_MASK (((u16)1) << HW_ERROR_NUM)

#define NO_FAULT                0

#define STARTUP_FAILED          1
#define ERROR_ON_SPEED_FEEDBACK 2
#define TIME_OUT                3
#define MOTOR_IS_RUNNING        4

#define SW_ERROR_NUM            4

#define SW_ERROR_MSG_1 "STARTUP FAILED!"
#define SW_ERROR_MSG_2 "SPEEDFDBK ERROR"
#define SW_ERROR_MSG_3 "   TIME_OUT    "
#define SW_ERROR_MSG_4 " MOTOR RUNNING "
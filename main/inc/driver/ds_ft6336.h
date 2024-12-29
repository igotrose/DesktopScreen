#ifndef __DS_FT6236_H_
#define __DS_FT6236_H_

#define GPIO_RST_LOW 0
#define GPIO_RST_HIGH 1

#define TP_PRES_DOWN 0x80 // touchpad was pressed
#define TP_COORD_UD 0x40  // touchpad coordinate update

#define FT_DEVIDE_MODE 0x00    // ft6236 mode contral register
#define FT_REG_NUM_FINGER 0x02 // touchpad status register

#define FT_TP1_REG 0x03 // first touch point data address
#define FT_TP2_REG 0x09 // second touch point data address
#define FT_TP3_REG 0x0F // third touch point data address
#define FT_TP4_REG 0x15 // fourth touch point data address
#define FT_TP5_REG 0x1B // fifth touch point data address

#define FT_ID_G_LTB_VERSION 0xA1    // version
#define FT_ID_G_MODE   0xA4 // ft6236 interrupt mode contral register
#define FT_ID_G_THGROUP			0x80   		// touch the valid value set register
#define FT_ID_G_PERIODACTIVE    0x88       // activate the status cycle setting register
#define Chip_Vendor_ID          0xA3        //chip id 0x36
#define ID_G_FT6236ID			0xA8		//0x11

typedef struct 
{
    uint8_t status;
    uint16_t x;
    uint16_t y;
}TP_POSITION_T;

typedef struct 
{
    uint8_t touch_sta;
    uint8_t touch_count;
    uint16_t x[5];
    uint16_t y[5];
    bool update;
}TouchPoint_T ;

void get_ft6336_touch_sta(TP_POSITION_T *position);
void init_ft6336(void);

#endif

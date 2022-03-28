#define _ui_h
#include <stdint.h>
//#include <oled.h>

#include "headfile.h"

#ifndef FALSE
  #define  FALSE  0x00u                /* Boolean value FALSE. FALSE is defined always as a zero value. */
#endif
#ifndef TRUE
  #define  TRUE   0x01u                /* Boolean value TRUE. TRUE is defined always as a non zero value. */
#endif

#ifndef NULL
  #define  NULL   0x00u
#endif

typedef void (*const tIsrFunc)(void);



/* PE types definition */
  #ifndef bool 
typedef unsigned char           bool;




/******************************************
 ********UI命令与输入设备信号之间的对应关系**********
 ******************************************/
typedef enum{
	UI_None			= 'Z',
	UI_Value_Plus 		= 'A',
	UI_Value_Minus 		= 'B',
	UI_Item_Plus		= 'C',
	UI_Item_Minus		= 'D',
	UI_Page_Plus		= 'E',
	UI_Page_Minus		= 'F',
	UI_Accuracy_Plus	= 'G',
	UI_Accuracy_Minus	= 'H',
	UI_Save			= 'Q',
	UI_Read		    = 'Y',
	UI_Start        = 'M',
	UI_Break        = 'T'
}UI_CMD_e,*UI_CMD_Ptr_e;

/*************************
 **变量的参数，包括变量的类型和权限**
 *************************/
typedef struct {
	enum{
		Double,
		Float,
		Int32_t,
		Uint32_t,
		Int16_t,
		Uint16_t,
		Int8_t,
		Uint8_t,
		Function,	//表示显示图像的函数
	}Type;

	enum{
		RO,			//只读
		RW,			//读写
	}Authority;
}Variable_Attribute;

/********************************************
 ************一个Item所必备的元素*****************
 ********************************************/
typedef struct{
	signed char*		Name;				//Item名，字符串类型
	void*				DataPtr;			//这个Item指向的数据地址
	Variable_Attribute	Data_Attribute;		//这个Item对应数据的参数
}Item_Lib,*Item_LibPtr;

/********************************************
 ************一个Page所必备的元素*****************
 ********************************************/
typedef struct{
	signed char*		Name;				//Page名，字符串类型
	Item_LibPtr			ItemPtr;			//这个Page的第一个Item的首地址
	uint16_t			Item_Max;			//这个Page对应的最大的Item个数
}Page_Lib,*Page_LibPtr;

/********************************************
 ***************保存当前的活动状态*****************
 ********************************************/
typedef struct{
	float 				Accuracy;			//当前数据调整精度
	bool				ShowGraphic;		        //标记是否有图像显示
	int16_t				PageNum;			//当前的Page，类型：整形数
	Page_LibPtr			PagePtr;			//当前Page指向的Page信息首地址，类型：指针
	int16_t				ItemNum;			//当前的Item，类型：整形数
	Item_LibPtr			ItemPtr;			//当前Item指向的Item信息首地址，类型：指针
}Active_Type,*Active_TypePtr;

void UI_Init(void);
void UI_Main(void);


void UI_GetCmd(void);
void Read_Data(void);
void Save_Data(void);

extern UI_CMD_e UI_CMD;

#endif /* UI_H_ */

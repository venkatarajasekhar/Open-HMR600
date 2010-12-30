#ifndef _USB_VENDOR_REQUEST_H_
#define _USB_VENDOR_REQUEST_H_

//4	Set/Get Register related wIndex/Data
#define	RT_USB_RESET_MASK_OFF		0
#define	RT_USB_RESET_MASK_ON		1
#define	RT_USB_SLEEP_MASK_OFF		0
#define	RT_USB_SLEEP_MASK_ON		1
#define	RT_USB_LDO_ON				1
#define	RT_USB_LDO_OFF				0

//4	Set/Get SYSCLK related	wValue or Data
#define	RT_USB_SYSCLK_32KHZ		0
#define	RT_USB_SYSCLK_40MHZ		1
#define	RT_USB_SYSCLK_60MHZ		2


typedef enum _RT_USB_BREQUEST {
	RT_USB_SET_REGISTER		= 1,
	RT_USB_SET_SYSCLK		= 2,
	RT_USB_GET_SYSCLK		= 3,
	RT_USB_GET_REGISTER		= 4
} RT_USB_BREQUEST;


typedef enum _RT_USB_WVALUE {
	RT_USB_RESET_MASK	=	1,
	RT_USB_SLEEP_MASK	=	2,
	RT_USB_USB_HRCPWM	=	3,
	RT_USB_LDO			=	4,
	RT_USB_BOOT_TYPE	=	5
} RT_USB_WVALUE;


//BOOLEAN usbvendorrequest(PCE_USB_DEVICE	CEdevice, RT_USB_BREQUEST bRequest, RT_USB_WVALUE wValue, UCHAR wIndex, PVOID Data, UCHAR DataLength, BOOLEAN isDirectionIn);
//BOOLEAN CEusbGetStatusRequest(PCE_USB_DEVICE CEdevice, IN USHORT Op, IN USHORT Index, PVOID Data);
//BOOLEAN CEusbFeatureRequest(PCE_USB_DEVICE CEdevice, IN USHORT Op, IN USHORT FeatureSelector, IN USHORT Index);
//BOOLEAN CEusbGetDescriptorRequest(PCE_USB_DEVICE CEdevice, IN short urbLength, IN UCHAR DescriptorType, IN UCHAR Index, IN USHORT LanguageId, IN PVOID  TransferBuffer, IN ULONG TransferBufferLength);

#endif

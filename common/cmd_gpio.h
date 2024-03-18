
// Ralink: we define "DEBUG" in include/configs/rt2880.h
#ifdef DEBUG
#undef DEBUG
#endif 

enum ralink_gpio_reg {
	GPIO_REG_INT = 0,
	GPIO_REG_EDGE,
	GPIO_REG_RENA,
	GPIO_REG_FENA,
	GPIO_REG_DATA,
	GPIO_REG_DIR,
	GPIO_REG_POL,
	GPIO_REG_SET,
	GPIO_REG_RESET,
	GPIO_REG_TOGGLE,
	GPIO_REG_MAX
};

struct ralink_gpio_chip {
	// struct gpio_chip chip;
	u8 mem_offset;
	u8 gpio_base;
	u8 num_gpio;
	u8 regs[GPIO_REG_MAX];
	// spinlock_t lock;
	// void __iomem *membase;
	// struct irq_domain *domain;
	// int irq;

	// u32 rising;
	// u32 falling;
};

#define BIT(x)              ((1 << x))

#define	GPIO23_00_INT	0x0000	//	23 to 0 Interrupt Status 84  
#define	GPIO23_00_EDGE	0x0004	//	23 to 0 Edge Status 84  
#define	GPIO23_00_RMASK	0x0008	//	23 to 0 Rising Edge Interrupt Mask 85
#define	GPIO23_00_MASK	0x000C	//	23 to 0 Falling Edge Interrupt Mask 85
#define	GPIO23_00_DATA	0x0020	//	23 to 0 Data 85 
#define	GPIO23_00_DIR	0x0024	//	23 to 0 Data Direction 86  
#define	GPIO23_00_POL	0x0028	//	23 to 0 Data Polarity 86  
#define	GPIO23_00_SET	0x002C	//	23 to 0 Set Data Bit 86 
#define	GPIO23_00_RESET	0x0030	//	23 to 0 Clear Data Bit 86 
#define	GPIO23_00_TOG	0x0034	//	23 to 0 Toggle PIO Data Bit 86
#define	GPIO39_24_INT	0x0038	//	39 to 24 Pin Interrupt Status 87 
#define	GPIO39_24_EDGE	0x003C	//	39 to 24 Pin Edge Status 87 
#define	GPIO39_24_RMASK	0x0040	//	39 to 24 Rising Edge Interrupt Mask 88
#define	GPIO39_24_FMASK	0x0044	//	39 to 24 Falling Edge Interrupt Mask 88
#define	GPIO39_24_DATA	0x0048	//	39 to 24 Data 89 
#define	GPIO39_24_DIR	0x004C	//	39 to 24 Data Direction 89  
#define	GPIO39_24_POL	0x0050	//	39 to 24 Data Polarity 89  
#define	GPIO39_24_SET	0x0054	//	39 to 24 Set Data Bit 90 
#define	GPIO39_24_RESET	0x0058	//	39 to 24 Clear Data Bit 90 
#define	GPIO39_24_TOG	0x005C	//	39 to 24 Toggle Data Bit 90 
#define	GPIO71_40_INT	0x0060	//	71 to 40 Interrupt Status 90  
#define	GPIO71_40_EDGE	0x0064	//	71 to 40 Edge Status 91  
#define	GPIO71_40_RMASK	0x0068	//	71 to 40 Rising Edge Interrupt Mask 91
#define	GPIO71_40_FMASK	0x006C	//	71 to 40 Falling Edge Interrupt Mask 91
#define	GPIO71_40_DATA	0x0070	//	71 to 40 Data 92 
#define	GPIO71_40_DIR	0x0074	//	71 to 40 Data Direction 92  
#define	GPIO71_40_POL	0x0078	//	71 to 40 Data Polarity 92  
#define	GPIO71_40_SET	0x007C	//	71 to 40 Set Data Bit 93 
#define	GPIO71_40_RESET	0x0080	//	71 to 40 Clear Data Bit 93 
#define	GPIO71_40_TOG	0x0084	//	71 to 40 Toggle Data Bit 93 
#define	GPIO72_INT		0x0088	//	72 Interrupt Status 93  
#define	GPIO72_EDGE		0x008C	//	72 Edge Status 93  
#define	GPIO72_RMASK	0x0090	//	72 Rising Edge Interrupt Mask 94
#define	GPIO72_FMASK	0x0094	//	72 Falling Edge Interrupt Mask 94
#define	GPIO72_DATA		0x0098	//	72 Data 95 
#define	GPIO72_DIR		0x009C	//	72 Data Direction 95  
#define	GPIO72_POL		0x00A0	//	72 Data Polarity 95  
#define	GPIO72_SET		0x00A4	//	72 Set Data Bit 96 
#define	GPIO72_RESET	0x00A8	//	72 Clear Data Bit 96 
#define	GPIO72_TOG		0x00AC	//	72 Toggle Data Bit 96 

#define ra_inb(offset)		(*(volatile unsigned char *)(offset))
#define ra_inw(offset)		(*(volatile unsigned short *)(offset))
#define ra_inl(offset)		(*(volatile unsigned long *)(offset))

#define ra_outb(offset,val)	(*(volatile unsigned char *)(offset) = val)
#define ra_outw(offset,val)	(*(volatile unsigned short *)(offset) = val)
#define ra_outl(offset,val)	(*(volatile unsigned long *)(offset) = val)

#define ra_and(addr, value) ra_outl(addr, (ra_inl(addr) & (value)))
#define ra_or(addr, value) ra_outl(addr, (ra_inl(addr) | (value)))

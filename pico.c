#include <stdio.h>
#include "pico/stdlib.h"
#include <pico/binary_info.h>
#include <hardware/i2c.h>
#include <string.h>


#define SSD1315_SET_MEM_MODE _u(0x20)
#define SSD1315_SET_COL_ADDR _u(0x21)
#define SSD1315_SET_PAGE_ADDR _u(0x22)
#define SSD1315_SET_HORIZ_SCROLL _u(0x26)
#define SSD1315_SET_SCROLL _u(0x2E)
#define SSD1315_SET_DISP_START_LINE _u(0x40)
#define SSD1315_SET_CONTRAST _u(0x81)
#define SSD1315_SET_CHARGE_PUMP _u(0x8D)

#define SSD1315_SET_SEG_REMAP _u(0xA0)
#define SSD1315_SET_ENTIRE_ON _u(0xA4)
#define SSD1315_SET_ALL_ON _u(0xA5)
#define SSD1315_SET_NORM_DISP _u(0xA6)
#define SSD1315_SET_INV_DISP _u(0xA7)
#define SSD1315_SET_MUX_RATIO _u(0xA8)
#define SSD1315_SET_DISP _u(0xAE)
#define SSD1315_SET_COM_OUT_DIR _u(0xC0)
#define SSD1315_SET_COM_OUT_DIR_FLIP _u(0xC0)
 
#define SSD1315_SET_DISP_OFFSET _u(0xD3)
#define SSD1315_SET_DISP_CLK_DIV _u(0xD5)
#define SSD1315_SET_PRECHARGE _u(0xD9)
#define SSD1315_SET_COM_PIN_CFG _u(0xDA)
#define SSD1315_SET_VCOM_DESEL _u(0xDB)

#define SSD1315_PAGE_HEIGHT _u(8)
#define SSD1315_NUM_PAGES (SSD1315_HEIGHT / SSD1315_PAGE_HEIGHT)
#define SSD1315_BUF_LEN (SSD1315_NUM_PAGES * SSD1315_WIDTH)

#define SSD1315_WRITE_MODE _u(0xFE)
#define SSD1315_READ_MODE _u(0xFF)

#define SSD1315_WIDTH 128
#define SSD1315_HEIGHT 32
#define SSD1315_I2C_CLK 400 // can be increased

const uint BUTTON_PIN = 34; // port 34 is button

2 static int addr = 0x27;

struct render_area {
	uint8_t start_col;
	uint8_t end_col;
	uint8_t start_page;
	uint8_t end_page;
	int buflen;
};

enum =  types {CO2,TEMPERATURE,MOISTURE}
enum = state {NORMAL,ERROR}
enum types currentSensorType = CO2
enum state currentState = NORMAL


void main(){
	gpio_init(BUTTON_PIN); // gpio get to get the value of thge button
	i2c_init(i2c0, 100 * 1000); // isa slave used to communicate to d10
	i2c_init(i2c1, 100 * 1000) // master used to coommunicate to oled
	gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C); // port 4
	gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);// port 5
	gpio_set_function(BUTTON_PIN,GPIO_PIO0);
	gpio_set_function(6,GPIO_FUNC_I2C); 
	gpio_set_function(7,GPIO_FUNC_I2C); 
	gpio_set_dir(BUTTON_PIN,GPIO_IN);
	gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN); // keep signal high when  no data
	gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
	gpio_pull_up(6); 
	gpio_pull_up(7); 
	SSD1315_init();
	i2c_set_slave_mode(i2c0,true,0x55);
	bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN,GPIO_FUNC_I2C));
	bi_decl(bi_2pins_with_func(6,7,GPIO_FUNC_I2C));

	struct render_area frame_area = {
		start_col: 0,
		end_col : SSD1315_WIDTH - 1,
		start_page : 0,
		end_page : SSD1315_NUM_PAGES - 1
		};
		
		calc_render_area_buflen(&frame_area);
		// zero the entire display
		uint8_t buf[SSD1315_BUF_LEN];
		memset(buf, 0, SSD1315_BUF_LEN);
		render(buf, &frame_area);

	



	while(true){

		// we need to recieve the data from the de10
		// buttons change state
		switch (currentSensorType){
		case CO2:
			switch (currentState){
			case NORMAL:
				// we take the data from the co2 section of the message
				// in the normal case we we need to display the value we receive from the de10 on the lcd 


			case ERROR:
				// again display but its going to be an error message

			}
		case TEMPERATURE:
			switch (currentState){
			case NORMAL:


			case ERROR:
				
			}

		case MOISTURE:
			switch (currentState){
			case NORMAL:

			case ERROR:
				
			}
		default:
			// figure out
		}

		
		
		sleep_ms(500);
		
	}
}

void i2c_write_byte(uint8_t val) {
#ifdef i2c_default
	i2c_write_blocking(i2c_default, addr, &val, 1, false);
#endif
}

void calc_render_area_buflen(struct render_area *area) {
// calculate how long the flattened buffer will be for a render area
area->buflen = (area->end_col - area->start_col + 1) * (area->end_page - area-
 >start_page + 1);
 }

#ifdef i2c_default

void SSD1315_send_cmd(uint8_t cmd) {
 // I2C write process expects a control byte followed by data
// this "data" can be a command or data to follow up a command
 // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(i2c_default, SSD1315_I2C_ADDR, buf, 2, false);
}
void SSD1315_send_cmd_list(uint8_t *buf, int num) {
	for (int i=0;i<num;i++)
		SSD1315_send_cmd(buf[i]);
}
 
void SSD1315_send_buf(uint8_t buf[], int buflen) {
 // in horizontal addressing mode, the column address pointer auto-increments
 // and then wraps around to the next page, so we can send the entire frame
// buffer in one gooooooo!
 
// copy our frame buffer into a new buffer because we need to add the control byte
// to the beginning

uint8_t *temp_buf = malloc(buflen + 1);

temp_buf[0] = 0x40;
memcpy(temp_buf+1, buf, buflen);

i2c_write_blocking(i2c_default, SSD1315_I2C_ADDR, temp_buf, buflen + 1, false);

 free(temp_buf);
}

void SSD1315_init() {
   // Som
   // to demonstrate what the initialization sequence looks like
// Some configuration values are recommended by the board manufacturer

uint8_t cmds[] = {
SSD1315_SET_DISP, // set display off
/* memory mapping */
SSD1315_SET_MEM_MODE, // set memory address mode 0 = horizontal, 1 =vertical, 2 = page
0x00, // horizontal addressing mode
/* resolution and layout */
147 SSD1315_SET_DISP_START_LINE, // set display start line to 0
SSD1315_SET_SEG_REMAP | 0x01, // set segment re-map, column address 127 is mapped to SEG0
SSD1315_SET_MUX_RATIO, // set multiplex ratio
SSD1315_HEIGHT - 1, // Display height - 1
SSD1315_SET_COM_OUT_DIR | 0x08, // set COM (common) output scan direction. Scan frombottom up, COM[N-1] to COM0
SSD1315_SET_DISP_OFFSET, // set display offset
 0x00, // no offset
SSD1315_SET_COM_PIN_CFG, // set COM (common) pins hardware configuration.Board specific magic number.
// 0x02 Works for 128x32, 0x12 Possibly works for128x64. Other options 0x22, 0x32
 #if ((SSD1315_WIDTH == 128) && (SSD1315_HEIGHT == 32))
0x02,
#elif ((SSD1315_WIDTH == 128) && (SSD1315_HEIGHT == 64))
0x12,
#else
0x02,
#endif
/* timing and driving scheme */
SSD1315_SET_DISP_CLK_DIV, // set display clock divide ratio
0x80, // div ratio of 1, standard freq
SSD1315_SET_PRECHARGE, // set pre-charge period
0xF1, // Vcc internally generated on our board
SSD1315_SET_VCOM_DESEL, // set VCOMH deselect level
0x30, // 0.83xVcc
/* display */
SSD1315_SET_CONTRAST, // set contrast control
0xFF,
SSD1315_SET_ENTIRE_ON, // set entire display on to follow RAM content
SSD1315_SET_NORM_DISP, // set normal (not inverted) display
SSD1315_SET_CHARGE_PUMP, // set charge pump
0x14, // Vcc internally generated on our board
SSD1315_SET_SCROLL | 0x00, // deactivate horizontal scrolling if set. This is necessary as memory writes will corrupt if scrolling was enabled
SSD1315_SET_DISP | 0x01, // turn display on
};

SSD1315_send_cmd_list(cmds, count_of(cmds));
}

void SSD1315_scroll(bool on) {
	// configure horizontal scrolling
	uint8_t cmds[] = {
	SSD1315_SET_HORIZ_SCROLL | 0x00,
	0x00, // dummy byte
	0x00, // start page 0
	0x00, // time interval
	0x03, // end page 3 SSD1315_NUM_PAGES ??
	0x00, // dummy byte
	0xFF, // dummy byte
	SSD1315_SET_SCROLL | (on ? 0x01 : 0) // Start/stop scrolling
};

SSD1315_send_cmd_list(cmds, count_of(cmds));
}

void render(uint8_t *buf, struct render_area *area) {
	 // update a portion of the display with a render area
	 uint8_t cmds[] = {
	 SSD1315_SET_COL_ADDR,
	 area->start_col,
	 area->end_col,
	 SSD1315_SET_PAGE_ADDR,
	 area->start_page,
	 area->end_page
	};
	
	SSD1315_send_cmd_list(cmds, count_of(cmds));
	SSD1315_send_buf(buf, area->buflen);
	 }
	
	static void SetPixel(uint8_t *buf, int x,int y, bool on) {
	assert(x >= 0 && x < SSD1315_WIDTH && y >=0 && y < SSD1315_HEIGHT);
	
	// The calculation to determine the correct bit to set depends on which address
	// mode we are in. This code assumes horizontal
	
	 // The video ram on the SSD1315 is split up in to 8 rows, one bit per pixel.
	 // Each row is 128 long by 8 pixels high, each byte vertically arranged, so byte 0 is x=0,y=0->7,
	 // byte 1 is x = 1, y=0->7 etc
	
// This code could be optimised, but is like this for clarity. The compiler
	// should do a half decent job optimising it anyway.

	const int BytesPerRow = SSD1315_WIDTH ; // x pixels, 1bpp, but each row is 8 pixel high,so (x / 8) * 8
	
	int byte_idx = (y / 8) * BytesPerRow + x;
	uint8_t byte = buf[byte_idx];
	
	if (on)
	byte |= 1 << (y % 8);
	else
	byte &= ~(1 << (y % 8));
	
	buf[byte_idx] = byte;
}

static void DrawLine(uint8_t *buf, int x0, int y0, int x1, int y1, bool on) {
	int dx = abs(x1-x0);
	int sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0);
	int sy = y0<y1 ? 1 : -1;
	int err = dx+dy;
	int e2;
	
	while (true) {
		SetPixel(buf, x0, y0, on);
		if (x0 == x1 && y0 == y1)
			break;
		e2 = 2*err;
	
		if (e2 >= dy) {
			err += dy;
		x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
		y0 += sy;
		}
	}
}

static inline int GetFontIndex(uint8_t ch) {
	if (ch >= 'A' && ch <='Z') {
		return ch - 'A' + 1;
	}
	else if (ch >= '0' && ch <='9') {
		return ch - '0'
	}
	else return 0; // Not got that char so space.	
}

static void WriteChar(uint8_t *buf, int16_t x, int16_t y, uint8_t ch) {
	if (x > SSD1315_WIDTH - 8 || y > SSD1315_HEIGHT - 8)
		return;
		// For the moment, only write on Y row boundaries (every 8 vertical pixels)
	y = y/8;
	ch = toupper(ch);
	int idx = GetFontIndex(ch);
	int fb_idx = y * 1 x;
	for (int i=0;i<8;i++) {
		buf[fb_idx++] = font[idx * 8 + i];
	 }
}
	
static void WriteString(uint8_t *buf, int16_t x, int16_t y, char *str) {
	 // Cull out any string off the screen
	 if (x > SSD1315_WIDTH - 8 || y > SSD1315_HEIGHT - 8)
	 return;
	
	while (*str) {
		WriteChar(buf, x, y, *str++);
	x+=8;
	}
}

	
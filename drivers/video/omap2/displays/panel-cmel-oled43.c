/*
 * OLED panel driver for CMEL OLED43
 *
 * Author: Chris Ladden <christopher.ladden@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/gpio.h>

#include <plat/display.h>

#define CS_PIN			139
#define MOSI_PIN		144
#define CLK_PIN			138
#define RESET_PIN		137
#define PANEL_PWR_PIN	143

#define digitalWrite(PIN,VALUE) (gpio_direction_output(PIN,VALUE))

#define CS_LOW digitalWrite(CS_PIN, 0)
#define CS_HIGH digitalWrite(CS_PIN, 1)
#define MOSI_LOW digitalWrite(MOSI_PIN, 0)
#define MOSI_HIGH digitalWrite(MOSI_PIN, 1)
#define CLK_LOW digitalWrite(CLK_PIN, 0)
#define CLK_HIGH digitalWrite(CLK_PIN, 1)
#define RESET_LOW digitalWrite(RESET_PIN, 0)
#define RESET_HIGH digitalWrite(RESET_PIN, 1)
#define PANEL_PWR_LOW digitalWrite(PANEL_PWR_PIN, 0)
#define PANEL_PWR_HIGH digitalWrite(PANEL_PWR_PIN, 1)

static struct omap_video_timings oled43_timings = {
	.x_res = 480,
	.y_res = 272,

	.pixel_clock	= 12500,

	.hsw		= 30,
	.hfp		= 31,
	.hbp		= 102,

	.vsw		= 3,
	.vfp		= 11,
	.vbp		= 20,
// frame_rate = 1/((vbp + vfp + 272)*(hbp + hfp + 480)/pixel_clock)
// where hfp must be > 30
//       hbp must be = 102
//       vfp must be > 10
//       vbp must be = 20
};

static int oled43_panel_probe(struct omap_dss_device *dssdev)
{
	dssdev->panel.config = OMAP_DSS_LCD_TFT | OMAP_DSS_LCD_IVS |
		OMAP_DSS_LCD_IHS;
	dssdev->panel.timings = oled43_timings;

	return 0;
}

static void oled43_panel_remove(struct omap_dss_device *dssdev)
{
	//turn power supply off?
}

////////////////////////////////////////////////////////
// Wrapper to request a GPIO pin
static int digitalRequest(u16 pin) {
	int ret;
	char label[64];
	sprintf(label, "%s: GPIO_PIN_%d", "cmel_oled43_panel", pin);

	ret = gpio_request(pin, label);
	if (ret < 0) {
		pr_err("%s: Unable to get GPIO_PIN_%d, ret=%d\n", "cmel_oled43_panel", pin, ret);
	}

	return ret;
}

////////////////////////////////////////////////////////
// Initialize the software SPI interface
static void oled43_spi_init(void) {
    CS_HIGH;
    MOSI_HIGH;
    CLK_HIGH;
}

////////////////////////////////////////////////////////
// Write an 8-bit byte onto the SPI bus
static void oled43_spi_write8(uint8_t data) {
uint8_t ii;
uint8_t bit;

    for (ii=0; ii < 8; ii++) {
        bit = (data >> (7-ii)) & 1;
        if (bit) {
            MOSI_HIGH;
        } else {
            MOSI_LOW;
        }

        CLK_LOW;		//clock the data in
        CLK_HIGH;
    }
}

///////////////////////////////////////////////////////////
// Write a value to the OLED panel
static void oled43_writeReg(uint8_t index, uint8_t val) {

	pr_info("cmel_oled43_panel: Sending data %x,%x \n", index, val);
//    fprintf(stdout, "Sending data %x,%x \n", index, val);

    CS_LOW;
    oled43_spi_write8((index << 1));
    oled43_spi_write8(val);
    CS_HIGH;
}

static int oled43_panel_enable(struct omap_dss_device *dssdev)
{
	int r = 0;

	pr_info("cmel_oled43_panel: panel_enable begin\n");
	/* wait couple of vsyncs until enabling the LCD */
	msleep(50);

	if (dssdev->platform_enable)
		r = dssdev->platform_enable(dssdev);

	/* Panel init sequence from the cmel panel datasheet */

	PANEL_PWR_LOW;						// just to be sure, hold the oled power supply off
	RESET_LOW;							// panel in reset
	oled43_spi_init();					// init spi interface
	RESET_HIGH;							// panel out of reset

    oled43_writeReg(0x04, 0x23);
    oled43_writeReg(0x05, 0x82);

    oled43_writeReg(0x07, 0x0F);
    oled43_writeReg(0x34, 0x18);
    oled43_writeReg(0x35, 0x28);
    oled43_writeReg(0x36, 0x16);
    oled43_writeReg(0x37, 0x01);

    //set gamma
    oled43_writeReg(0x09, 0x24); //vgam
    oled43_writeReg(0x10, 0x24); //r stop
    oled43_writeReg(0x11, 0x24); //g stop
    oled43_writeReg(0x12, 0x24); //b stop

    oled43_writeReg(0x13, 0x00); //r
    oled43_writeReg(0x14, 0x05);
    oled43_writeReg(0x15, 0x05);
    oled43_writeReg(0x16, 0x03);
    oled43_writeReg(0x17, 0x02);
    oled43_writeReg(0x18, 0x03);
    oled43_writeReg(0x19, 0x04);
    oled43_writeReg(0x1A, 0x0A);

    oled43_writeReg(0x1B, 0x00); //G
    oled43_writeReg(0x1C, 0x07);
    oled43_writeReg(0x1D, 0x05);
    oled43_writeReg(0x1E, 0x04);
    oled43_writeReg(0x1F, 0x04);
    oled43_writeReg(0x20, 0x04);
    oled43_writeReg(0x21, 0x05);
    oled43_writeReg(0x22, 0x0B);

    oled43_writeReg(0x23, 0x00); //B
    oled43_writeReg(0x24, 0x05);
    oled43_writeReg(0x25, 0x07);
    oled43_writeReg(0x26, 0x05);
    oled43_writeReg(0x27, 0x04);
    oled43_writeReg(0x28, 0x04);
    oled43_writeReg(0x29, 0x04);
    oled43_writeReg(0x2A, 0x09);

    //brightness
    oled43_writeReg(0x3A, 0x11); //50 NITs birghtness

    //display on
    oled43_writeReg(0x06, 0x03);

	PANEL_PWR_HIGH;

	pr_info("cmel_oled43_panel: panel_enable end\n");

	return r;
}

static void oled43_panel_disable(struct omap_dss_device *dssdev)
{
	if (dssdev->platform_disable)
		dssdev->platform_disable(dssdev);

	PANEL_PWR_LOW;

	/* wait at least 5 vsyncs after disabling the LCD */
	msleep(100);
}

static int oled43_panel_suspend(struct omap_dss_device *dssdev)
{
	pr_info("cmel_oled43_panel: panel_suspend\n");
	oled43_panel_disable(dssdev);
	return 0;
}

static int oled43_panel_resume(struct omap_dss_device *dssdev)
{
	pr_info("cmel_oled43_panel: panel_resume\n");
	return oled43_panel_enable(dssdev);
}

static struct omap_dss_driver oled43_driver = {
	.probe		= oled43_panel_probe,
	.remove		= oled43_panel_remove,

	.enable		= oled43_panel_enable,
	.disable	= oled43_panel_disable,
	.suspend	= oled43_panel_suspend,
	.resume		= oled43_panel_resume,

	.driver         = {
		.name   = "cmel_oled43_panel",
		.owner  = THIS_MODULE,
	},
};

static int __init oled43_panel_drv_init(void)
{
	int ret = 0;

	printk(KERN_INFO "cmel_oled43_panel: init panel\n");

	//Get the GPIO pins used for the panel
	gpio_request(CS_PIN, "OLED43_CS_PIN");
	gpio_request(MOSI_PIN, "OLED43_MOSI_PIN");
	gpio_request(CLK_PIN, "OLED43_CLK_PIN");
	gpio_request(RESET_PIN, "OLED43_RESET_PIN");
	gpio_request(PANEL_PWR_PIN, "OLED43_PANEL_PWR_PIN");
	//digitalRequest(CS_PIN);
	//digitalRequest(MOSI_PIN);
	//digitalRequest(CLK_PIN);
	//digitalRequest(RESET_PIN);
	//digitalRequest(PANEL_PWR_PIN);

	PANEL_PWR_LOW;						// hold the oled power supply off

	ret = omap_dss_register_driver(&oled43_driver);
	if (ret != 0)
		pr_err("lgphilips_lb035q02: Unable to register panel driver: %d\n", ret);

	printk(KERN_INFO "cmel_oled43_panel: done\n");
	return ret;
}

static void __exit oled43_panel_drv_exit(void)
{
	omap_dss_unregister_driver(&oled43_driver);
}

module_init(oled43_panel_drv_init);
module_exit(oled43_panel_drv_exit);
MODULE_LICENSE("GPL");

#include "audio.h"
#include "pico.h"
#include "pico/audio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include <stdio.h>

#define I2C_ADDR 0x18
#define DEBUG_I2C (0)

static void writeRegister(uint8_t reg, uint8_t value) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = value;
    int res = i2c_write_timeout_us(i2c0, I2C_ADDR, buf, sizeof(buf),
                                   /* nostop */ false, 1000);
    if (res != 2) {
        panic("i2c_write_timeout failed: res=%d\n", res);
    }
    if (DEBUG_I2C)
        printf("Write Reg: %d = 0x%x\n", reg, value);
}

static uint8_t readRegister(uint8_t reg) {
    uint8_t buf[1];
    buf[0] = reg;
    int res = i2c_write_timeout_us(i2c0, I2C_ADDR, buf, sizeof(buf),
                                   /* nostop */ true, 1000);
    if (res != 1) {
        if (DEBUG_I2C)
            printf("res=%d\n", res);
        panic("i2c_write_timeout failed: res=%d\n", res);
    }
    res = i2c_read_timeout_us(i2c0, I2C_ADDR, buf, sizeof(buf),
                              /* nostop */ false, 1000);
    if (res != 1) {
        if (DEBUG_I2C)
            printf("res=%d\n", res);
        panic("i2c_read_timeout failed: res=%d\n", res);
    }
    uint8_t value = buf[0];
    if (DEBUG_I2C)
        printf("Read Reg: %d = 0x%x\n", reg, value);
    return value;
}

static void modifyRegister(uint8_t reg, uint8_t mask, uint8_t value) {
    uint8_t current = readRegister(reg);
    if (DEBUG_I2C)
        printf(
            "Modify Reg: %d = [Before: 0x%x] with mask 0x%x and value 0x%x\n",
            reg, current, mask, value);
    uint8_t new_value = (current & ~mask) | (value & mask);
    writeRegister(reg, new_value);
}

static void setPage(uint8_t page) {
    printf("Set page %d\n", page);
    writeRegister(0x00, page);
}

static void Wire_begin() {
    i2c_init(i2c0, 100000);
    gpio_set_function(20, GPIO_FUNC_I2C);
    gpio_set_function(21, GPIO_FUNC_I2C);
}

void audio_dac_init(void) {

    gpio_init(22);
    gpio_set_dir(22, true);
    gpio_put(22, true); // allow i2s to come out of reset

    Wire_begin();
    sleep_ms(1000);

    if (DEBUG_I2C)
        printf("initialize codec\n");

    // Reset codec
    writeRegister(0x01, 0x01);
    sleep_ms(10);

    // Interface Control
    modifyRegister(0x1B, 0xC0, 0x00);
    modifyRegister(0x1B, 0x30, 0x00);

    // Clock MUX and PLL settings
    modifyRegister(0x04, 0x03, 0x03);
    modifyRegister(0x04, 0x0C, 0x04);

    writeRegister(0x06, 0x20); // PLL J
    writeRegister(0x08, 0x00); // PLL D LSB
    writeRegister(0x07, 0x00); // PLL D MSB

    modifyRegister(0x05, 0x0F, 0x02); // PLL P/R
    modifyRegister(0x05, 0x70, 0x10);

    // DAC/ADC Config
    modifyRegister(0x0B, 0x7F, 0x08); // NDAC
    modifyRegister(0x0B, 0x80, 0x80);

    modifyRegister(0x0C, 0x7F, 0x02); // MDAC
    modifyRegister(0x0C, 0x80, 0x80);

    modifyRegister(0x12, 0x7F, 0x08); // NADC
    modifyRegister(0x12, 0x80, 0x80);

    modifyRegister(0x13, 0x7F, 0x02); // MADC
    modifyRegister(0x13, 0x80, 0x80);

    // PLL Power Up
    modifyRegister(0x05, 0x80, 0x80);

    // Headset and GPIO Config
    setPage(1);
    modifyRegister(0x2e, 0xFF, 0x0b);
    setPage(0);
    modifyRegister(0x43, 0x80, 0x80); // Headset Detect
    modifyRegister(0x30, 0x80, 0x80); // INT1 Control
    modifyRegister(0x33, 0x3C, 0x14); // GPIO1

    // DAC Setup
    modifyRegister(0x3F, 0xC0, 0xC0);

    // DAC Routing
    setPage(1);
    modifyRegister(0x23, 0xC0, 0x40);
    modifyRegister(0x23, 0x0C, 0x04);

    // DAC Volume Control
    setPage(0);
    modifyRegister(0x40, 0x0C, 0x00);
    writeRegister(0x41, 0x28); // Left DAC Vol
    writeRegister(0x42, 0x28); // Right DAC Vol

    // ADC Setup
    modifyRegister(0x51, 0x80, 0x80);
    modifyRegister(0x52, 0x80, 0x00);
    writeRegister(0x53, 0x68); // ADC Volume

    // Headphone and Speaker Setup
    setPage(1);
    modifyRegister(0x1F, 0xC0, 0xC0); // HP Driver
    modifyRegister(0x28, 0x04, 0x04); // HP Left Gain
    modifyRegister(0x29, 0x04, 0x04); // HP Right Gain
    writeRegister(0x24, 0x0A);        // Left Analog HP
    writeRegister(0x25, 0x0A);        // Right Analog HP

    modifyRegister(0x28, 0x78, 0x40); // HP Left Gain
    modifyRegister(0x29, 0x78, 0x40); // HP Right Gain

    // Speaker Amp
    modifyRegister(0x20, 0x80, 0x80);
    modifyRegister(0x2A, 0x04, 0x04);
    modifyRegister(0x2A, 0x18, 0x08);
    writeRegister(0x26, 0x0A);

    // Return to page 0
    setPage(0);

    if (DEBUG_I2C)
        printf("Initialization complete!\n");

    // Read all registers for verification
    if (DEBUG_I2C) {
        printf("Reading all registers for verification:\n");

        setPage(0);
        readRegister(0x00); // AIC31XX_PAGECTL
        readRegister(0x01); // AIC31XX_RESET
        readRegister(0x03); // AIC31XX_OT_FLAG
        readRegister(0x04); // AIC31XX_CLKMUX
        readRegister(0x05); // AIC31XX_PLLPR
        readRegister(0x06); // AIC31XX_PLLJ
        readRegister(0x07); // AIC31XX_PLLDMSB
        readRegister(0x08); // AIC31XX_PLLDLSB
        readRegister(0x0B); // AIC31XX_NDAC
        readRegister(0x0C); // AIC31XX_MDAC
        readRegister(0x0D); // AIC31XX_DOSRMSB
        readRegister(0x0E); // AIC31XX_DOSRLSB
        readRegister(0x10); // AIC31XX_MINI_DSP_INPOL
        readRegister(0x12); // AIC31XX_NADC
        readRegister(0x13); // AIC31XX_MADC
        readRegister(0x14); // AIC31XX_AOSR
        readRegister(0x19); // AIC31XX_CLKOUTMUX
        readRegister(0x1A); // AIC31XX_CLKOUTMVAL
        readRegister(0x1B); // AIC31XX_IFACE1
        readRegister(0x1C); // AIC31XX_DATA_OFFSET
        readRegister(0x1D); // AIC31XX_IFACE2
        readRegister(0x1E); // AIC31XX_BCLKN
        readRegister(0x1F); // AIC31XX_IFACESEC1
        readRegister(0x20); // AIC31XX_IFACESEC2
        readRegister(0x21); // AIC31XX_IFACESEC3
        readRegister(0x22); // AIC31XX_I2C
        readRegister(0x24); // AIC31XX_ADCFLAG
        readRegister(0x25); // AIC31XX_DACFLAG1
        readRegister(0x26); // AIC31XX_DACFLAG2
        readRegister(0x27); // AIC31XX_OFFLAG
        readRegister(0x2C); // AIC31XX_INTRDACFLAG
        readRegister(0x2D); // AIC31XX_INTRADCFLAG
        readRegister(0x2E); // AIC31XX_INTRDACFLAG2
        readRegister(0x2F); // AIC31XX_INTRADCFLAG2
        readRegister(0x30); // AIC31XX_INT1CTRL
        readRegister(0x31); // AIC31XX_INT2CTRL
        readRegister(0x33); // AIC31XX_GPIO1
        readRegister(0x3C); // AIC31XX_DACPRB
        readRegister(0x3D); // AIC31XX_ADCPRB
        readRegister(0x3F); // AIC31XX_DACSETUP
        readRegister(0x40); // AIC31XX_DACMUTE
        readRegister(0x41); // AIC31XX_LDACVOL
        readRegister(0x42); // AIC31XX_RDACVOL
        readRegister(0x43); // AIC31XX_HSDETECT
        readRegister(0x51); // AIC31XX_ADCSETUP
        readRegister(0x52); // AIC31XX_ADCFGA
        readRegister(0x53); // AIC31XX_ADCVOL

        setPage(1);
        readRegister(0x1F); // AIC31XX_HPDRIVER
        readRegister(0x20); // AIC31XX_SPKAMP
        readRegister(0x21); // AIC31XX_HPPOP
        readRegister(0x22); // AIC31XX_SPPGARAMP
        readRegister(0x23); // AIC31XX_DACMIXERROUTE
        readRegister(0x24); // AIC31XX_LANALOGHPL
        readRegister(0x25); // AIC31XX_RANALOGHPR
        readRegister(0x26); // AIC31XX_LANALOGSPL
        readRegister(0x27); // AIC31XX_RANALOGSPR
        readRegister(0x28); // AIC31XX_HPLGAIN
        readRegister(0x29); // AIC31XX_HPRGAIN
        readRegister(0x2A); // AIC31XX_SPLGAIN
        readRegister(0x2B); // AIC31XX_SPRGAIN
        readRegister(0x2C); // AIC31XX_HPCONTROL
        readRegister(0x2E); // AIC31XX_MICBIAS
        readRegister(0x2F); // AIC31XX_MICPGA
        readRegister(0x30); // AIC31XX_MICPGAPI
        readRegister(0x31); // AIC31XX_MICPGAMI
        readRegister(0x32); // AIC31XX_MICPGACM

        setPage(3);
        readRegister(0x10); // AIC31XX_TIMERDIVIDER
    }
}

/** Dependencies **************************************************************/
#include <cc2511_map.h>
#include <board.h>
#include <time.h>

#include <gpio.h>
#include <adc.h>
#include <usb.h>
#include <usb_com.h>
#include <random.h>
#include <radio_queue.h>

#define MAX_TX_INTERVAL 10 // maximum time between transmissions (ms)

void updateLeds()
{
    usbShowStatusWithGreenLed();
    LED_YELLOW(vinPowerPresent());
}

void main()
{
    uint8 XDATA * txBuf;

    uint8 lastTx = 0;

    systemInit();
    usbInit();

    radioQueueInit();
    randomSeedFromSerialNumber();

    setDigitalInput(1, HIGH_IMPEDANCE);
    setDigitalInput(2, HIGH_IMPEDANCE);

    while(1)
    {
        updateLeds();
        boardService();
        usbComService();

        if ((uint8)(getMs() - lastTx) > MAX_TX_INTERVAL && (txBuf = radioQueueTxCurrentPacket()))
        {
            txBuf[1] = -((int16)adcRead(2 | ADC_BITS_7) - 1024) / 64;
            txBuf[2] = ((int16)adcRead(1 | ADC_BITS_7) - 1024) / 64;
            txBuf[3] = !isPinHigh(0);
            *txBuf = 3; // set packet length byte
            radioQueueTxSendPacket();

            lastTx = getMs();
        }

    }
}

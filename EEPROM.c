#include<16f818.h>
#fuses HS, NOWDT, INTRC_IO, NOMCLR
#use delay(clock = 8M)
#use i2c(master, sda = PIN_B1, scl = PIN_B4)
#define isPressed FALSE
#define isUnPressed TRUE
#define WRITE PIN_B7
#define CHECK PIN_B6
#define limit 8
#define WRITE_ACK 0
#define WRITE_NO_ACK 1
#define READ_ACK 1
#define READ_NO_ACK 0
void Init_eeprom()
{
	output_float(PIN_B1);
	output_float(PIN_B4);
}
void Write_byte(int16 address, int8 data)
{
	int8 r, high, low;
	do {
		i2c_start();
		r = i2c_write(0xA0);
	} while (r != WRITE_ACK);
	high = (address >> 8);
	low = address;
	//ack between data writes is not important
	i2c_write(high);
	i2c_write(low);
	i2c_write(data);
	i2c_stop();
}
int8 Read_byte(int16 address)
{
	int8 r, high, low, data;
	do {
		i2c_start();
		r = i2c_write(0xA0);
	} while (r != WRITE_ACK);
	high = (address >> 8);
	low = address;
	i2c_write(high);
	i2c_write(low);
	//to change the direction of communication use i2c_start()
	i2c_start();
	i2c_write(0xA1);
	data = i2c_read(READ_NO_ACK);
	return data;
}
void Write_Page(int16 address, int8 data, int8 count)
{
	int8 r, high, low, i;
	do {
		i2c_start();
		r = i2c_write(0xA0);
	} while (r != WRITE_ACK);
	high = (address >> 8);
	low = address;
	i2c_write(high);
	i2c_write(low);
	if (count > 64)
		count = 64;
	for (i = 0; i<count; i++)
	{
		i2c_write(data + i);
	}
	i2c_stop();
}
void Read_Sequential(int16 address, int8 data[], int count)
{
	int8 r, high, low, i;
	do {
		i2c_start();
		r = i2c_write(0xA0);
	} while (r != WRITE_ACK);
	high = (address >> 8);
	low = address;
	i2c_write(high);
	i2c_write(low);
	i2c_start();
	i2c_write(0xA1);
	for (i = 0; i<count - 1; i++)
	{
		data[i] = i2c_read(READ_ACK); //READ_ACK means there is another read to be done
		data[count -1] = i2c_read(READ_NO_ACK); //READ_NO_ACK means this is the last read
		i2c_stop();
	}
	void main()
	{
		int8 c1 = 0;
		int8 c2 = 0;
		int8 i = 0;
		int8 FLAGE = 1;
		Port_b_pullups(TRUE);
		setup_oscillator(OSC_8MHZ);
		Init_eeprom();
		while (TRUE)
		{
			if (input(WRITE) == isUnPressed)
			{
				if (c1 < limit) c1++;
			}
			else
			{
				if (c1 == limit)
				{
					//ACTION
					for (i = 0; i<70; i++)
					{
						Write_byte(i, i + 10);
					}
				} c1 = 0;
			}
			if (input(CHECK) == isUnPressed)
			{
				if (c2 < limit) c2++;
			}
			else
			{
				if (c2 == limit)
				{
					//ACTION
					FLAGE = 1;
					for (i = 0; i<70; i++)
					{
						if (i + 10 != Read_byte(i))
							FLAGE = 0;
					}
				}
				c2 = 0;
			}
			if (FLAGE == 1)
			{
				output_high(PIN_A6);
				output_low(PIN_A7);
			}
			else
			{
				output_high(PIN_A7);
				output_low(PIN_A6);
			} delay_ms(1);
		}
	}
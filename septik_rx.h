/*
контроллер сепника, приемная часть
Септик с помощью передатчика OPEN-SMARt RF433 периодически передает пакеты, обозначающие, что он работает в штатном режиме
В случае переполнения их передача прекращается. При этом приемная часть должна выполнить следующие действия:  
1. выключить питание на септике
2. отключить насос, поддерживающий давление в системе
3. запретить использовать скважный насос
4. Снять питание с электромагнитного клапана, открывающего прокачку через септик.
*/

#ifndef SEPTIK_RX_LIB
#define SEPTIK_RX_LIB

#include <VirtualWire.h>

//скорость обмена по rf433
#define RF433_RATE  2000
//период отправки пакета подтверждения нормального состояния
#define PING_PERIOD 3000
enum SEPTIK_STATE {SEPTIK_OK, SEPTIK_FAIL};

//паттерн нормального состояния септика
const uint8_t msg[5] = {0x10, 0xA5, 0x5A, 0xF0, 0x0F};

class SeptikRX{
	//буфер для приема пакетов
	uint8_t buf[VW_MAX_MESSAGE_LEN];
	uint8_t buflen = VW_MAX_MESSAGE_LEN;
	//пин для VirtualWire
	uint8_t	rx_pin;
	//время последне   успешной операции чтения
	unsigned long last_read;

	//текущее количество пропущенных пакетов	
	uint8_t fail_cnt;
	//количество пропущенных пакетов, приводящее к выключеню септика
	int fail_limit;
	//состояние септика
	SEPTIK_STATE state;
public:

	SeptikRX(uint8_t _pin, int _fail_limit);
	void init(int speed);
	bool check();
	bool is_state_normal(){return SEPTIK_OK == state;};
};
#endif

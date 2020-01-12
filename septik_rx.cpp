
#include "septik_rx.h"
/*
@param in _pin		вход к которому подключен приемник
@param in speed		скорость передачи, должна соответствовать скорости передатчика
@param in fail_limit	количество пропущенных пакетов, приводящее к выключеню септика

*/
SeptikRX::SeptikRX(uint8_t _pin, int _fail_limit){
	fail_cnt = 0;
	last_read = 0;
	fail_limit = _fail_limit;
	state = SEPTIK_OK;
	rx_pin = _pin;

}
void SeptikRX::init(int speed){
	vw_set_rx_pin(rx_pin);
    vw_set_ptt_pin(15);//отключаем ptt, так как он по умолчанию на 10м пине, который яркость LCD-shield'a
  	vw_setup(speed);
  	vw_rx_start();
}
/*
проверка состояния септика
функция не блокирующая
возвращает true в том случае, если состояние септика изменилось
*/
bool SeptikRX::check(){
  bool r = false;
  unsigned long time_diff;
  
  buflen = sizeof(msg);
  if ( vw_get_message(buf, &buflen)) // Non-blocking
  {
      last_read = millis();
      int i;
      // Message with a good checksum received, dump it.
      Serial.print("Got: ");       
      for (i = 0; i < buflen; i++)
      {
          Serial.print(buf[i], HEX);
          Serial.print(" ");
      }
      Serial.println("");
      if (!memcmp(buf, msg, sizeof(msg)))
      {
        Serial.println("Normal state");
        fail_cnt = 0;
    	if (SEPTIK_OK != state){
	    	state = SEPTIK_OK;
		    r = true;
	    }
      }
      else
      {
        fail_cnt++;
        Serial.print(millis());
        Serial.print(" counter up ");
        Serial.println(fail_cnt);
        
      }
  }
  else{
    time_diff = millis() - last_read;
    Serial.print(time_diff);
    Serial.println(" no input"); 
    if ( time_diff > PING_PERIOD){ 
      fail_cnt++;
      Serial.print("Counter up ");
      Serial.println(fail_cnt);
    }
  }
  
  if (fail_cnt > fail_limit){
	if (SEPTIK_FAIL != state){
		state = SEPTIK_FAIL;
		r = true;
    		Serial.println("Septik fail");
  	}
  }
  return r;
}

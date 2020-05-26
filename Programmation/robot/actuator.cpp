#include "actuator.hpp"

Actuator::Actuator() : Robot() {}

Actuator::Actuator(const Actuator&) {}

Actuator::Actuator(std::string nom, SPI *pSpi, uint8_t id, int nb_servo, int nb_moteur4Q, int nb_moteur, int nb_capt_cur, int nb_capt_couleur, int nb_rupteur, int nb_ax12, int nb_capt_dist) 
: Robot(nom, pSpi, id) {
  int i;
	m_nb_servo = nb_servo;
	m_nb_moteur4Q = nb_moteur4Q;
	m_nb_moteur = nb_moteur;
	m_nb_capt_cur = nb_capt_cur;
	m_nb_capt_couleur = nb_capt_couleur;
	m_nb_rupteur = nb_rupteur;
	m_nb_ax12 = nb_ax12;
	m_nb_capt_dist = nb_capt_dist;
	std::cout << m_nom << " comporte : " << std::endl;
	if(m_nb_servo) {std::cout << m_nb_servo << " servo moteurs" << std::endl;}
  if(m_nb_moteur4Q) {
		for(i=0;i<m_nb_moteur4Q;i++) {
			SetMot4QPos(i,100,0,400);
			delay(1000);
		}
		std::cout << m_nb_moteur4Q << " pont en H" << std::endl;
	}
	if(m_nb_moteur) {std::cout << m_nb_moteur << " moteurs" << std::endl;}
	if(m_nb_capt_cur) {std::cout << m_nb_capt_cur << " capteurs de courant" << std::endl;}
	if(m_nb_capt_couleur) {std::cout << m_nb_capt_couleur << " capteurs de couleur" << std::endl;}
	if(m_nb_rupteur) {std::cout << m_nb_rupteur << " rupteurs" << std::endl;}
	if(m_nb_ax12) {std::cout << m_nb_ax12 << " ax12" << std::endl;}
	if(m_nb_capt_dist) {std::cout << m_nb_capt_dist << " capteurs de distance" << std::endl;}
	std::cout << std::endl;
}

Actuator::Actuator(std::string nom, SPI *pSpi, uint8_t id, int nb_servo, int nb_moteur4Q, int nb_moteur, int nb_capt_cur, int nb_capt_couleur, int nb_rupteur, int nb_ax12, int nb_capt_dist, int nb_uart, std::string uart_name[], int uart_addr[]) 
: Robot(nom, pSpi, id) {
	int i;
	m_nb_servo = nb_servo;
	m_nb_moteur4Q = nb_moteur4Q;
	m_nb_moteur = nb_moteur;
	m_nb_capt_cur = nb_capt_cur;
	m_nb_capt_couleur = nb_capt_couleur;
	m_nb_uart = nb_uart;
	m_nb_rupteur = nb_rupteur;
	m_nb_ax12 = nb_ax12;
	m_nb_capt_dist = nb_capt_dist;
  for(int j=0;j<nb_uart;j++) {
    m_uart_name[j] = uart_name[j];
    m_uart_addr[j] = uart_addr[j];
  }
	std::cout << m_nom << " comporte : " << std::endl;
	if(m_nb_servo) {std::cout << m_nb_servo << " servo moteurs" << std::endl;}
	if(m_nb_moteur4Q) {
		for(i=0;i<m_nb_moteur4Q;i++) {
			SetMot4QPos(i,100,0,400);
			delay(1000);
		}
		std::cout << m_nb_moteur4Q << " pont en H" << std::endl;
	}
	if(m_nb_moteur) {std::cout << m_nb_moteur << " moteurs" << std::endl;}
	if(m_nb_capt_cur) {std::cout << m_nb_capt_cur << " capteurs de courant" << std::endl;}
	if(m_nb_capt_couleur) {std::cout << m_nb_capt_couleur << " capteurs de couleur" << std::endl;}
	if(m_nb_rupteur) {std::cout << m_nb_rupteur << " rupteurs" << std::endl;}
	if(m_nb_ax12) {std::cout << m_nb_ax12 << " ax12" << std::endl;}
	if(m_nb_capt_dist) {std::cout << m_nb_capt_dist << " capteurs de distance" << std::endl;}
	if(m_nb_uart) {
		int cpt = 0;
		std::cout << m_nb_uart << " uart : " << std::endl << " ";
		for(i=0;i<m_nb_uart;i++) {
		  resetXbee(uart_addr[i]);
			delay(200);
			PingXbee(uart_addr[i]);
			while(cpt < 4000 && !m_pingXbee) {
				delay(1);
				checkMessages();
				cpt++;
			}
			if(m_pingXbee) {
				DEBUG_ROBOT_PRINTLN(uart_name[i] << " connecté");
				m_connected_uart[i] = true;
			} else {
				resetXbee(uart_addr[i]);
				delay(200);
				PingXbee(uart_addr[i]);
				cpt = 0;
				while(cpt < 4000 && !m_pingXbee) {
					delay(1);
					checkMessages();
					cpt++;
				}
				if(m_pingXbee) {
					DEBUG_ROBOT_PRINTLN(uart_name[i] << " connecté 2")
					m_connected_uart[i] = true;
				} else {
					DEBUG_ROBOT_PRINTLN(uart_name[i] << " non connecté ERROR")
					m_connected_uart[i] = false;
				}
			}
		}
	}
	std::cout << std::endl;
}

Actuator::~Actuator() {}

void Actuator::DecodMsg(uint8_t buf[]) {
	switch(buf[1]){	//type of msg
		case ACT_CMD_CUR:
			m_cur[buf[2]] = buf[3]*256+buf[4];
			break;
		case ACT_CMD_COLOR:
			m_color[buf[2]] = buf[3]*256+buf[4];
			break;
		case ACT_CMD_RUPT:
			m_rupt[buf[2]] = buf[3];
			break;
		case ACT_CMD_DIST:
			m_dist[buf[2]] = buf[3]*256+buf[4];
			break;
		case CMD_PING_UART:
			m_pingXbee = true;
			break;
		case MSG_NON_PRIS_EN_CHARGE_UART:
			DEBUG_ROBOT_PRINTLN("UART receive msg with not concern it !!! ERROR" << buf[1]);
			break;
		default:
    		DEBUG_ROBOT_PRINTLN("message non pris en charge")
			break;
	}
}

void Actuator::PingXbee(uint8_t id) {
  if(m_nb_uart) {
    UartSend(CMD_PING_UART, id, '0');
  } else {  
    DEBUG_ROBOT_PRINTLN("erreur pas d'uart sur cette carte")
  }
}

bool Actuator::GetPingXbee() {
	m_mutex.lock();
	bool b = m_pingXbee;
	m_pingXbee = false;
	m_mutex.unlock();
	return b;
}

void Actuator::resetXbee(uint8_t id) {
 if(m_nb_uart) {
	    UartSend(CMD_RST, id, '0');
  } else {  
    	DEBUG_ROBOT_PRINTLN("erreur pas d'uart sur cette carte")
  }
}

bool Actuator::startThreadDetection(){
	m_mutex.lock();
	m_continueThread = true;
	m_mutex.unlock();
	int rc = pthread_create(&m_thread, NULL, thread_act, this);
	if (rc) {
    	DEBUG_ROBOT_PRINTLN("Error:unable to create thread," << rc)
		return false;
    }
	return true;
}

void* thread_act(void *threadid){
	Actuator *actuator = (Actuator*)threadid;
	while(actuator->isContinueThread()){
		actuator->checkMessages();
	}
	pthread_exit(NULL);
}

void Actuator::MoveServo(int nb_bras, int pos) {
    uint8_t buffer[4];
	if(nb_bras < 0 || nb_bras > m_nb_servo-1) {
    	DEBUG_ROBOT_PRINTLN("erreur bras = " << nb_bras)
	} else {
		if(pos < 600 || pos > 1800) {
    	DEBUG_ROBOT_PRINTLN("erreur pos = " << pos)
		} else {
			buffer[0] = ACT_CMD_SERVO;
			buffer[1] = nb_bras;
			buffer[2] = (uint8_t)(pos/256);
			buffer[3] = pos%256;
			sendSPI(buffer,4);
		}
	}
}

void Actuator::SetMot(int nb_bras, int state) {
	uint8_t buffer[3];
	if(nb_bras < 0 || nb_bras > m_nb_moteur-1) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		if(state < 0 || state > 1) {
    	DEBUG_ROBOT_PRINTLN("erreur state = " << state)
		} else {
			buffer[0] = ACT_CMD_SET_MOT;
			buffer[1] = nb_bras;
			buffer[2] = state;
			sendSPI(buffer,3);
		}
	}
}

void Actuator::SetMot4QVit(int nb_bras, int vit, int sens) {
	uint8_t buffer[4];
	if(nb_bras < 0 || nb_bras > m_nb_moteur4Q-1) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		if(sens < 0 || sens > 1) {
    		DEBUG_ROBOT_PRINTLN("erreur sens = " << sens)
		} else {
			if(vit < 0 || vit > 255) {
    			DEBUG_ROBOT_PRINTLN("erreur vit = " << vit)
			} else {
				buffer[0] = ACT_CMD_SET_MOT4QVit;
				buffer[1] = nb_bras;
				buffer[2] = sens;
				buffer[3] = vit;
				sendSPI(buffer,4);
			}
		}
	}
}

void Actuator::SetMot4QPos(int nb_bras, int vit, int sens, int temps) {
	uint8_t buffer[6];
	if(nb_bras < 0 || nb_bras > m_nb_moteur4Q-1) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		if(sens < 0 || sens > 1) {
    		DEBUG_ROBOT_PRINTLN("erreur sens = " << sens)
		} else {
			if(vit < 0 || vit > 255) {
    			DEBUG_ROBOT_PRINTLN("erreur vit = " << vit)
			} else {
				if(temps < 0 || temps > 10000) {
					DEBUG_ROBOT_PRINTLN("erreur temps = " << temps)
				} else {
					buffer[0] = ACT_CMD_SET_MOT4QPos;
					buffer[1] = nb_bras;
					buffer[2] = sens;
					buffer[3] = vit;
					buffer[4] = (uint8_t)(temps/256);
					buffer[5] = temps%256;
					sendSPI(buffer,6);
				}
			}
		}
	}
}

void Actuator::SetAx12(int nb_bras, int pos) {
	uint8_t buffer[4];
	if(nb_bras < 1 || nb_bras > m_nb_ax12) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		if(pos < 0 || pos > 1000) {
    		DEBUG_ROBOT_PRINTLN("erreur pos = " << pos)
		} else {
			buffer[0] = ACT_CMD_AX12;
			buffer[1] = nb_bras;
			buffer[2] = (uint8_t)(pos/256);
			buffer[3] = pos%256;
			sendSPI(buffer,4);
		}
	}
}

void Actuator::GetColor(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_capt_couleur-1) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		buffer[0] = ACT_CMD_COLOR;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(NB_FLUSH_MIN);
}

void Actuator::GetCurrent(int nb_bras){
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_capt_cur-1) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		buffer[0] = ACT_CMD_CUR;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(NB_FLUSH_MIN);
}

void Actuator::GetRupt(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_rupteur-1) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		buffer[0] = ACT_CMD_RUPT;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(NB_FLUSH_MIN);
}

void Actuator::GetDist(int nb_bras) {
	uint8_t buffer[2];
	if(nb_bras < 0 || nb_bras > m_nb_capt_dist-1) {
    	DEBUG_ROBOT_PRINTLN("erreur nb_bras = " << nb_bras)
	} else {
		buffer[0] = ACT_CMD_DIST;
		buffer[1] = nb_bras;
		sendSPI(buffer,2);
	}
	flush(NB_FLUSH_MIN);
}

int Actuator::Cur(int nb_bras) {
	m_mutex.lock();
	int cpt = 0;
	do{
		GetCurrent(nb_bras);
		delay(20);
		cpt++;
	}while(m_cur[nb_bras] == ERROR_VALUE && cpt < 10);
	if(cpt > 1) {DEBUG_ROBOT_PRINTLN("plusieur essais " << cpt)}
	if(m_cur[nb_bras] == ERROR_VALUE) {DEBUG_ROBOT_PRINTLN("ERROR courant")}
	int b = m_cur[nb_bras];
	m_cur[nb_bras] = ERROR_VALUE;
	m_mutex.unlock();
	return b;
}

int Actuator::Color(int nb_bras) {
	m_mutex.lock();
	int cpt = 0;
	do{
		GetColor(nb_bras);
		delay(20);
		cpt++;
	}while(m_color[nb_bras] == ERROR_VALUE && cpt < 10);
	if(cpt > 1) {DEBUG_ROBOT_PRINTLN("plusieur essais " << cpt)}
	if(m_color[nb_bras] == ERROR_VALUE) {DEBUG_ROBOT_PRINTLN("ERROR couleur")}
	int b = m_color[nb_bras];
	m_color[nb_bras] = ERROR_VALUE;
	m_mutex.unlock();
	return b;
}

int Actuator::Dist(int nb_bras) {
	m_mutex.lock();
	int cpt = 0;
	do{
		GetDist(nb_bras);
		delay(20);
		cpt++;
	}while(m_dist[nb_bras] == ERROR_VALUE && cpt < 10);
	if(cpt > 1) {DEBUG_ROBOT_PRINTLN("plusieur essais " << cpt)}
	if(m_dist[nb_bras] == ERROR_VALUE) {DEBUG_ROBOT_PRINTLN("ERROR couleur")}
	int b = m_dist[nb_bras];
	m_dist[nb_bras] = ERROR_VALUE;
	m_mutex.unlock();
	return b;
}

int Actuator::Rupt(int nb_bras) {
	m_mutex.lock();
	int cpt = 0;
	GetRupt(nb_bras);
	while(m_rupt[nb_bras] == ERROR_VALUE && cpt < 20) {
		delay(1);
		cpt++;
	}
	if(cpt > 1) {DEBUG_ROBOT_PRINTLN("plusieur essais " << cpt)}
	if(m_rupt[nb_bras] == ERROR_VALUE) {DEBUG_ROBOT_PRINTLN("ERROR rupteur")}
	int b = m_rupt[nb_bras];
	m_rupt[nb_bras] = ERROR_VALUE;
	m_mutex.unlock();
	return b;
}

int Actuator::RuptOne(int nb_bras) {
	m_mutex.lock();
	GetRupt(nb_bras);
	delay(20);
	int b = m_rupt[nb_bras];
	m_rupt[nb_bras] = ERROR_VALUE;
	m_mutex.unlock();
	return b;
}

int Actuator::ColorOne(int nb_bras) {
	m_mutex.lock();
	GetColor(nb_bras);
	delay(20);
	int b = m_color[nb_bras];
	m_color[nb_bras] = ERROR_VALUE;
	m_mutex.unlock();
	return b;
}

void Actuator::UartSend(unsigned char Send, unsigned char id_uart, unsigned char msg) {
	uint8_t buffer[5];
	buffer[0] = ACT_CMD_UART_SEND;
	buffer[1] = Send;
	buffer[2] = id_uart;
	buffer[3] = msg;
	buffer[4] = (ACT_CMD_UART_SEND+id_uart+Send+msg)%256;
	sendSPI(buffer,5);
}

void Actuator::allumerPhare(void) {
  if(m_nb_uart) {
    UartSend(PHARE_STATE,UART_ID_PHARE, '1');
  } else {
    DEBUG_ROBOT_PRINTLN("erreur pas d'uart sur cette carte")
  }
}

void Actuator::eteindrePhare(void) {
    if(m_nb_uart) {
    UartSend(PHARE_STATE,UART_ID_PHARE, '0');
  } else {
    DEBUG_ROBOT_PRINTLN("erreur pas d'uart sur cette carte")
  }
}

void Actuator::setSeuilColor(int seuil, int valeur) {
	uint8_t buffer[4];
	if(seuil > 7 || seuil < 0) {
    	DEBUG_ROBOT_PRINTLN("erreur seuil " << seuil)
	} else {
		buffer[0] = ACT_CMD_SEUIL_COLOR;
		buffer[1] = seuil;
		buffer[2] = (uint8_t)(valeur/256);
		buffer[3] = valeur%256;
		sendSPI(buffer,4);
	}
}

void Actuator::resetCptColor(void) {
	uint8_t buffer[1];
	buffer[0] = ACT_CMD_RESET_CPT_COLOR;
	sendSPI(buffer,1);
}
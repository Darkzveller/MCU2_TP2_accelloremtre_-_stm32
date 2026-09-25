#include "stm32f10x.h"
#include <stdint.h>
#include "ADXL345.h"
#include "stdbool.h"
#include <stdio.h>
#include <stdarg.h>

#define ADXL345_G_TO_THRESH(g) ((uint8_t)((g) / 0.0625f)) // Valeur_registre = Seuil_voulu_en_g / 0,0625
#define ADXL345_TIME_LSB_S 1.0f
#define ADXL345_SEC_TO_TIME(s) ((uint8_t)(((s) / ADXL345_TIME_LSB_S) + 0.5f))

#define SIZE_FIFO 32

typedef enum
{
	GPIO_INPUT_ANALOG = 0x0,
	GPIO_OUTPUT_PP_10MHZ = 0x1,
	GPIO_OUTPUT_PP_2MHZ = 0x2,
	GPIO_OUTPUT_PP_50MHZ = 0x3,

	GPIO_INPUT_FLOATING = 0x4,
	GPIO_OUTPUT_OD_10MHZ = 0x5,
	GPIO_OUTPUT_OD_2MHZ = 0x6,
	GPIO_OUTPUT_OD_50MHZ = 0x7,

	GPIO_INPUT_PULL = 0x8,

	GPIO_AF_PP_10MHZ = 0x9,
	GPIO_AF_PP_2MHZ = 0xA,
	GPIO_AF_PP_50MHZ = 0xB,

	GPIO_AF_OD_10MHZ = 0xD,
	GPIO_AF_OD_2MHZ = 0xE,
	GPIO_AF_OD_50MHZ = 0xF

} GPIO_Config_t;

typedef enum
{
	GPIO_LOW = 0,
	GPIO_HIGH = 1

} GPIO_State_t;

void GPIO_InitPin(GPIO_TypeDef *GPIOx, uint8_t pin, GPIO_Config_t config, GPIO_State_t defaultState)
{
	uint32_t shift;

	if (pin > 15)
		return;

	/* ==========================
	   Activation horloge GPIO
	   ========================== */

	if (GPIOx == GPIOA)
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	}
	else if (GPIOx == GPIOB)
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	}
	else if (GPIOx == GPIOC)
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	}
	else if (GPIOx == GPIOD)
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	}
	else
	{
		return;
	}

	/* ==========================
	   Etat par d�faut
	   ========================== */

	if (defaultState == GPIO_HIGH)
	{
		GPIOx->BSRR = (1U << pin);
	}
	else
	{
		GPIOx->BRR = (1U << pin);
	}

	/* ==========================
	   Configuration
	   ========================== */

	if (pin < 8)
	{
		shift = pin * 4;

		GPIOx->CRL &= ~(0xFU << shift);
		GPIOx->CRL |= ((uint32_t)config << shift);
	}
	else
	{
		shift = (pin - 8) * 4;

		GPIOx->CRH &= ~(0xFU << shift);
		GPIOx->CRH |= ((uint32_t)config << shift);
	}
}
GPIO_State_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t pin)
{
	if (pin > 15)
		return GPIO_LOW;

	// IDR = Input Data Register
	if (GPIOx->IDR & (1U << pin))
	{
		return GPIO_HIGH;
	}
	else
	{
		return GPIO_LOW;
	}
}
void delay_ms(uint32_t ms)
{
	uint32_t ticks;

	/*
	 * Nombre de cycles nécessaires pour 1 ms.
	 * SystemCoreClock contient la fréquence du processeur.
	 */
	ticks = SystemCoreClock / 1000U;

	/*
	 * SysTick est un compteur 24 bits.
	 * LOAD contient la valeur de départ.
	 */
	SysTick->LOAD = ticks - 1U;

	/*
	 * Remise à zéro du compteur.
	 */
	SysTick->VAL = 0U;

	/*
	 * CLKSOURCE = 1 : horloge processeur
	 * ENABLE    = 1 : active SysTick
	 * TICKINT   = 0 : aucune interruption
	 */
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
					SysTick_CTRL_ENABLE_Msk;

	while (ms > 0U)
	{
		/*
		 * COUNTFLAG passe à 1 lorsque SysTick
		 * atteint zéro.
		 */
		if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
		{
			ms--;
		}
	}

	/*
	 * Arrêt de SysTick
	 */
	SysTick->CTRL = 0U;
}



void config_usart2(uint32_t cpu_freq_hz, uint32_t baudrate)
{
    uint32_t pclk1_freq;
    uint32_t ppre1;
    uint32_t apb1_div;
    uint32_t usartdiv_x16;
    uint32_t mantissa;
    uint32_t fraction;

    /* =========================================================
     * 1. CONFIGURATION DES ENTREES / SORTIES DE L'USART2
     * ========================================================= */

    /* Activation horloge GPIOA */
    RCC->APB2ENR |= (1U << 2);

    /*
     * USART2 :
     * PA2 = TX
     * PA3 = RX
     *
     * PA2 :
     * MODE2 = 01 -> sortie 10 MHz
     * CNF2  = 10 -> Alternate Function Push-Pull
     *
     * PA3 :
     * MODE3 = 00 -> entrée
     * CNF3  = 01 -> entrée flottante
     */

    GPIOA->CRL &= ~(0xFFU << 8);
    GPIOA->CRL |=  (0x49U << 8);


    /* =========================================================
     * 2. ACTIVATION HORLOGE USART2
     * ========================================================= */

    RCC->APB1ENR |= (1U << 17);


    /* =========================================================
     * 3. DETERMINATION DE LA FREQUENCE APB1
     * ========================================================= */

    /*
     * USART2 est connecté au bus APB1.
     *
     * On récupère PPRE1 = bits [10:8] de RCC_CFGR.
     */

    ppre1 = (RCC->CFGR >> 8) & 0x7U;

    /*
     * PPRE1 :
     *
     * 0xx = HCLK / 1
     * 100 = HCLK / 2
     * 101 = HCLK / 4
     * 110 = HCLK / 8
     * 111 = HCLK / 16
     */

    if (ppre1 < 4U)
    {
        apb1_div = 1U;
    }
    else
    {
        apb1_div = 1U << (ppre1 - 3U);
    }

    /*
     * Fréquence réellement reçue par USART2
     */
    pclk1_freq = cpu_freq_hz / apb1_div;


    /* =========================================================
     * 4. CALCUL AUTOMATIQUE DU BAUDRATE
     * ========================================================= */

    /*
     * USARTDIV = PCLK1 / (16 * Baudrate)
     *
     * Mais comme BRR contient directement :
     *
     * Mantisse * 16 + Fraction
     *
     * on peut calculer :
     *
     * USARTDIV_x16 = PCLK1 / Baudrate
     *
     * Le + baudrate/2 permet d'arrondir au plus proche.
     */

    usartdiv_x16 = (pclk1_freq + (baudrate / 2U)) / baudrate;

    /*
     * Partie entière
     */
    mantissa = usartdiv_x16 / 16U;

    /*
     * Partie fractionnaire
     */
    fraction = usartdiv_x16 % 16U;

    /*
     * BRR :
     *
     * [15:4] = mantisse
     * [3:0]  = fraction
     */
    USART2->BRR = (mantissa << 4) | fraction;


    /* ---------------------------------------------------------
     * USART_CR2
     * 1 bit de stop
     * ---------------------------------------------------------
     *
     * STOP[1:0] = bits [13:12]
     * 00 = 1 bit de stop
     */

    USART2->CR2 &= ~(0x3U << 12);


    /* ---------------------------------------------------------
     * USART_CR3
     * Pas de contrôle de flux matériel RTS / CTS
     * ---------------------------------------------------------
     *
     * CTSE = bit 9 = 0
     * RTSE = bit 8 = 0
     */

    USART2->CR3 &= ~((1U << 9) |
                     (1U << 8));


    /* ---------------------------------------------------------
     * USART_CR1
     * ---------------------------------------------------------
     *
     * UE  = 1 -> bit 13 : USART activé
     * M   = 0 -> bit 12 : mot de 8 bits
     * PCE = 0 -> bit 10 : pas de parité
     * TE  = 1 -> bit 3  : transmission activée
     * RE  = 1 -> bit 2  : réception activée
     */

    /* On efface d'abord tous les champs que l'on veut configurer */
    USART2->CR1 &= ~((1U << 13) |
                     (1U << 12) |
                     (1U << 10) |
                     (1U << 3)  |
                     (1U << 2));

    USART2->CR1 |= ((1U << 13) |
                    (1U << 3)  |
                    (1U << 2));
}
/*
 * Envoie un caractère sur USART2
 */
void Serial_WriteChar(char c)
{
	/* Attendre que le registre de transmission soit vide */
	while (!(USART2->SR & USART_SR_TXE))
	{
	}

	/* Envoyer le caractère */
	USART2->DR = (uint8_t)c;
}

/*
 * Envoie une chaîne de caractères
 */
void Serial_WriteString(const char *str)
{
	while (*str != '\0')
	{
		Serial_WriteChar(*str);
		str++;
	}
}

/*
 * Fonction équivalente à printf()
 *
 * Exemple :
 *
 * Serial_Print("Bonjour\r\n");
 * Serial_Print("Valeur = %d\r\n", valeur);
 * Serial_Print("X=%d Y=%d Z=%d\r\n", x, y, z);
 */
void Serial_Print(const char *format, ...)
{
	char buffer[128];

	va_list args;

	/* Récupération des arguments après format */
	va_start(args, format);

	/* Transformation en chaîne de caractères */
	vsnprintf(buffer, sizeof(buffer), format, args);

	va_end(args);

	/* Envoi sur USART2 */
	Serial_WriteString(buffer);
}
/*
 * Equivalent de Serial.println()
 *
 * Ajoute automatiquement :
 *
 * \r = retour au début de la ligne
 * \n = passage à la ligne suivante
 */
void Serial_Println(const char *format, ...)
{
    char buffer[128];

    va_list args;

    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    Serial_WriteString(buffer);

    /*
     * Retour au début de la ligne
     */
    Serial_WriteChar('\r');

    /*
     * Passage à la ligne suivante
     */
    Serial_WriteChar('\n');
}

void config_SPI_1(void)
{
	RCC->APB2ENR |= (1 << 12); // horloge SPI1
	RCC->APB2ENR |= (1 << 2);  // horologe GPIOA
	// configuration commune à tous les SPI
	SPI1->CR1 = (0b001 << 3)   // BR : Fpclk/4
				| (0 << 7)	   // LSBFIRST = 0 (MSB first)
				| (1 << 9)	   // SSM = 1
				| (1 << 8)	   // SSI = 1
				| (0 << 11)	   // DFF = 0 (8 bits)
				| (1 << 2)	   // MSTR = 1
				| (0b11 << 0); // CPOL=1, CPHA=1 (Mode 3)

	SPI1->CR1 |= (1 << 6);									// SPE = 1, activé en dernier
	GPIO_InitPin(GPIOA, 4, GPIO_OUTPUT_PP_10MHZ, GPIO_LOW); // CS  = OUT, piloté à la main (SSM=1)
	GPIO_InitPin(GPIOA, 5, GPIO_AF_PP_10MHZ, GPIO_LOW);		// SCK = OUT (AF, piloté par le SPI)
	GPIO_InitPin(GPIOA, 6, GPIO_INPUT_FLOATING, GPIO_LOW);	// MISO = IN
	GPIO_InitPin(GPIOA, 7, GPIO_AF_PP_10MHZ, GPIO_LOW);		// MOSI = OUT (AF, piloté par le SPI)
}
void gere_cs(bool cs)
{
	if (cs == true)
	{
		GPIOA->BSRR = (1UL << 4);
	}
	else
		GPIOA->BRR = (1UL << 4);
	{
	}
}
void config_regADXL(uint8_t ADXL345_REG, uint8_t val)
{
	uint8_t poubelle;
	gere_cs(0);
	SPI1->DR = 0x00 | (ADXL345_REG & 0x3F); // R=0, MB=0, adresse sur 6 bits
	while (!(SPI1->SR & SPI_SR_RXNE))
		;
	poubelle = SPI1->DR;

	SPI1->DR = val;
	while (!(SPI1->SR & SPI_SR_RXNE))
		;
	poubelle = SPI1->DR;
	gere_cs(1);
}

void lire_regADXL(uint8_t ADXL345_REG, uint8_t *val_recu)
{
	uint8_t poubelle;

	gere_cs(0);
	SPI1->DR = 0x80 | (ADXL345_REG & 0x3F); // R=1, MB=0, adresse sur 6 bits
	while (!(SPI1->SR & SPI_SR_RXNE))
		;
	poubelle = SPI1->DR;

	SPI1->DR = 0x00;
	while (!(SPI1->SR & SPI_SR_RXNE))
		;

	*val_recu = SPI1->DR;

	gere_cs(1);
}

void lire_multiple_regADXL(uint8_t ADXL345_REG, uint8_t nbr_registre_a_lire, uint8_t *val_recu)
{
	uint8_t poubelle;
	uint8_t cpt_lecture = 0;

	gere_cs(0);
	SPI1->DR = 0xC0 | (ADXL345_REG & 0x3F); // R=1, MB=1, adresse sur 6 bits
	while (!(SPI1->SR & SPI_SR_RXNE))
		;
	poubelle = SPI1->DR;

	while (cpt_lecture < nbr_registre_a_lire)
	{
		SPI1->DR = 0x00;
		while (!(SPI1->SR & SPI_SR_RXNE))
			;

		val_recu[cpt_lecture] = SPI1->DR;
		cpt_lecture++;
	}

	gere_cs(1);
}

void config_ADXL(void)
{
	// appeler la config du SPI et initialiser la patte /CS en controle manuel
	config_SPI_1();
	gere_cs(1);

	// configurer les GPIO pour surveiller les signaux int0 et int1 (la scruptation suffira)
	GPIO_InitPin(GPIOA, 10, GPIO_INPUT_FLOATING, GPIO_LOW); // INT 1
	GPIO_InitPin(GPIOA, 11, GPIO_INPUT_FLOATING, GPIO_LOW); // INT2

	// initialiser power : sequence en plusieurs �tapes conseill�e
	config_regADXL(ADXL345_POWER_CTL, 0b00000000); // Wakeup = 00 = 8Hz
	config_regADXL(ADXL345_POWER_CTL, 0b00001000); // Measure = 1
												   //*******************************************DATA FORMAT ***********************
	// FORMAT DES DONNES  :  passer en 16G justifi� droit ,selectionner le mode 4 FILS, it actif niveau bas, full resolution

	config_regADXL(ADXL345_DATA_FORMAT, 0b00001011); // SELF_TEST = 0 | SPI 4 fils | INT active HIGH | FULL_RES = 1 | Justify à droite (DATAX0 (0x32) = octet faible  (LSB) DATAX1 (0x33) = octet fort (MSB)| Range = ±16 g
	//***************************************inactivit�/ choc niveau1**********************

	config_regADXL(ADXL345_ACT_INACT_CTL, 0b01110111); // ACT : mode DC, axes X/Y/Z activés |  INACT : mode DC, axes X/Y/Z activés
	// la valeur est par pas de 62.5mG  ainsi  256 correspondrait � 16G : choisir 1G comme seuil  activit� et 0,5G en inactivity , pour une dur�e 1s
	config_regADXL(ADXL345_THRESH_ACT, ADXL345_G_TO_THRESH(1.0f));
	config_regADXL(ADXL345_THRESH_INACT, ADXL345_G_TO_THRESH(0.5f));
	config_regADXL(ADXL345_TIME_INACT, ADXL345_SEC_TO_TIME(1)); // 1 seconde

	//*******************************************choc**********************************************************

	config_regADXL(ADXL345_TAP_AXES, 0b00000111);	// detection choc de tous les cot�s
	config_regADXL(ADXL345_THRESH_TAP, 0b01010000); // detection choc r�gl�e � 5G
	config_regADXL(ADXL345_DUR, 0b00010000);		// duree minimale du choc 10ms (pas de 625us)
	config_regADXL(ADXL345_LATENT, 0x00);			// ecart minimum entre tap pas 1.25ms : 0 desactive
	config_regADXL(ADXL345_TAP_AXES, 0x00);			// fenetre seconde frappe pas 1.25ms : 0 desactive
													//*****************************************************************************************************************
	config_regADXL(ADXL345_THRESH_FF, 0x09);		// seuil detection FREE FALL  pas 62.5mG 0.6G
	config_regADXL(ADXL345_TIME_FF, 10);			// duree minimale de chute pas 5ms  : 100 ms =20

	//***************************************************************************************************************
	// interruptions  activer les ITs : ADXL345_INT_ENABLE  bit � 0 = INT1 , bit  1 = INT2

	config_regADXL(ADXL345_INT_ENABLE, 0b00000000);
	config_regADXL(ADXL345_INT_MAP, 0b00000001);	// D1 WATERMARK = 0 -> INT1 | D0 OVERRUN   = 1 -> INT2
	config_regADXL(ADXL345_INT_ENABLE, 0b00000011); // D1 = 1 -> WATERMARK activée |s D0 = 1 -> OVERRUN activée

	//***********************************************************************************************************
	// gestion par FIFO pour stocker sans danger

	config_regADXL(ADXL345_BW_RATE, 0b00001010);  // Fonctionnement � 100 HZ
	config_regADXL(ADXL345_FIFO_CTL, 0b10010000); // stream, trig int1, avertissement sur mi remplissage (16)
}

typedef struct
{
	uint8_t data[SIZE_FIFO]; // bah la data
	uint8_t index_write;	 // Index d'écriture
	uint8_t index_read;		 // Index de lecture
	uint8_t available_space; // Nombre de places libres

} t_fifo;
t_fifo fifo_tx = {0, 0, 0, SIZE_FIFO};

int main(void)
{
	// config_usart2();
	config_usart2(72000000U, 9600);
	config_SPI_1();
	config_ADXL();
	// GPIO_InitPin(GPIOA, 5, GPIO_OUTPUT_PP_50MHZ, GPIO_LOW);
	Serial_Print("Demarrage du STM32");
	Serial_Println("");

	int valeur = 42;

	Serial_Print("Valeur = %d", valeur);
	Serial_Println("");


	while (1)
	{
		if (GPIO_ReadPin(GPIOA, 12))
		{
			uint8_t fifo_status;
			uint8_t nb_mesures;

			lire_regADXL(ADXL345_FIFO_STATUS, &fifo_status);
			nb_mesures = fifo_status & 0x3F;
			for (uint8_t i = 0; i < nb_mesures; i++)
			{
				// lire_multiple_regADXL(ADXL345_DATAX0, 6, data);

				// uint16_t axe_x = (int16_t)((data[1] << 8) | data[0]);
				// uint16_t axe_y = (int16_t)((data[3] << 8) | data[2]);
				// uint16_t axe_z = (int16_t)((data[5] << 8) | data[4]);
			}
		}
		else
		{
		}
		// static int time = 1000;
		// GPIOA->BSRR = (1U << 5);
		// delay_ms(time);

		// GPIOA->BRR = (1U << 5);
		// delay_ms(time);
	}
}
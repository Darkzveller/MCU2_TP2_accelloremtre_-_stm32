
 
#define ADXL345_DEVID			 0x00		// Device ID
#define ADXL345_RESERVED1		 0x01		// Reserved. Do Not Access. 
#define ADXL345_THRESH_TAP		 0x1D		// Tap Threshold. 
#define ADXL345_OFSX			 0x1E		// X-Axis Offset. 
#define ADXL345_OFSY			 0x1F		// Y-Axis Offset.
#define ADXL345_OFSZ			 0x20		// Z- Axis Offset.
#define ADXL345_DUR				 0x21		// Tap Duration.
#define ADXL345_LATENT			 0x22		// Tap Latency.
#define ADXL345_WINDOW			 0x23		// Tap Window.
#define ADXL345_THRESH_ACT		 0x24		// Activity Threshold
#define ADXL345_THRESH_INACT	 0x25		// Inactivity Threshold
#define ADXL345_TIME_INACT		 0x26		// Inactivity Time
#define ADXL345_ACT_INACT_CTL	 0x27		// Axis Enable Control for Activity and Inactivity Detection
#define ADXL345_THRESH_FF		 0x28		// Free-Fall Threshold.
#define ADXL345_TIME_FF			 0x29		// Free-Fall Time.
#define ADXL345_TAP_AXES		 0x2A		// Axis Control for Tap/Double Tap.
#define ADXL345_ACT_TAP_STATUS	 0x2B		// Source of Tap/Double Tap
#define ADXL345_BW_RATE			 0x2C		// Data Rate and Power mode Control
#define ADXL345_POWER_CTL		 0x2D		// Power-Saving Features Control
#define ADXL345_INT_ENABLE		 0x2E		// Interrupt Enable Control
#define ADXL345_INT_MAP			 0x2F		// Interrupt Mapping Control
#define ADXL345_INT_SOURCE		 0x30		// Source of Interrupts
#define ADXL345_DATA_FORMAT		 0x31		// Data Format Control
#define ADXL345_DATAX0			 0x32		// X-Axis Data 0
#define ADXL345_DATAX1			 0x33		// X-Axis Data 1
#define ADXL345_DATAY0			 0x34		// Y-Axis Data 0
#define ADXL345_DATAY1			 0x35		// Y-Axis Data 1
#define ADXL345_DATAZ0			 0x36		// Z-Axis Data 0
#define ADXL345_DATAZ1			 0x37		// Z-Axis Data 1
#define ADXL345_FIFO_CTL		 0x38		// FIFO Control
#define ADXL345_FIFO_STATUS		 0x39		// FIFO Status

#define ADXL345_BW_1600			 0xF			// 1111		IDD = 40uA
#define ADXL345_BW_800			 0xE			// 1110		IDD = 90uA
#define ADXL345_BW_400			 0xD			// 1101		IDD = 140uA
#define ADXL345_BW_200			 0xC			// 1100		IDD = 140uA
#define ADXL345_BW_100			 0xB			// 1011		IDD = 140uA 
#define ADXL345_BW_50			 0xA			// 1010		IDD = 140uA
#define ADXL345_BW_25			 0x9			// 1001		IDD = 90uA
#define ADXL345_BW_12_5		     0x8			// 1000		IDD = 60uA 
#define ADXL345_BW_6_25			 0x7			// 0111		IDD = 50uA
#define ADXL345_BW_3_13			 0x6			// 0110		IDD = 45uA
#define ADXL345_BW_1_56			 0x5			// 0101		IDD = 40uA
#define ADXL345_BW_0_78			 0x4			// 0100		IDD = 34uA
#define ADXL345_BW_0_39			 0x3			// 0011		IDD = 23uA
#define ADXL345_BW_0_20			 0x2			// 0010		IDD = 23uA
#define ADXL345_BW_0_10			 0x1			// 0001		IDD = 23uA
#define ADXL345_BW_0_05			 0x0			// 0000		IDD = 23uA


 /************************** INTERRUPT PINS **************************/
#define ADXL345_INT1_PIN		 0x00		//INT1: 0
#define ADXL345_INT2_PIN		 0x01		//INT2: 1
 

 /********************** INTERRUPT BIT POSITION **********************/
#define ADXL345_INT_DATA_READY_BIT		 0x07
#define ADXL345_INT_SINGLE_TAP_BIT		 0x06
#define ADXL345_INT_DOUBLE_TAP_BIT		 0x05
#define ADXL345_INT_ACTIVITY_BIT		 0x04
#define ADXL345_INT_INACTIVITY_BIT		 0x03
#define ADXL345_INT_FREE_FALL_BIT		 0x02
#define ADXL345_INT_WATERMARK_BIT		 0x01
#define ADXL345_INT_OVERRUNY_BIT		 0x00

#define ADXL345_DATA_READY				 0x07
#define ADXL345_SINGLE_TAP				 0x06
#define ADXL345_DOUBLE_TAP				 0x05
#define ADXL345_ACTIVITY				 0x04
#define ADXL345_INACTIVITY				 0x03
#define ADXL345_FREE_FALL				 0x02
#define ADXL345_WATERMARK				 0x01
#define ADXL345_OVERRUNY				 0x00


 /****************************** ERRORS ******************************/
#define ADXL345_OK			 1		// No Error
#define ADXL345_ERROR		 0		// Error Exists

#define ADXL345_NO_ERROR	 0		// Initial State
#define ADXL345_READ_ERROR	 1		// Accelerometer Reading Error
#define ADXL345_BAD_ARG		 2		// Bad Argument



/*
void config_ADXL(void)
{// initialiser la patte /CS en controle manuel

// appeler la config du SPI


// configurer les GPIO pour surveiller les signaux int0 et int1 (la scruptation suffira)

//initialiser power : sequence en plusieurs étapes conseillée
    config_regADXL(ADXL345_POWER_CTL,  );// Wakeup  
    config_regADXL(ADXL345_POWER_CTL, );// Measure
 *******************************************DATA FORMAT ***********************  
    // FORMAT DES DONNES  :  passer en 16G justifié droit ,selectionner le mode 4 FILS, it actif niveau bas, full resolution
 
    config_regADXL(ADXL345_DATA_FORMAT, ________);
 ***************************************inactivité/ choc niveau1**********************
 
    config_regADXL(ADXL345_ACT_INACT_CTL, _________ );
    // la valeur est par pas de 62.5mG  ainsi  256 correspondrait à 16G : choisir 1G comme seuil  activité et 0,5G en inactivity , pour une durée 1s
    config_regADXL(ADXL345_THRESH_ACT, ______      );
    config_regADXL(ADXL345_THRESH_INACT, ______        );
    config_regADXL(ADXL345_TIME_INACT, ________        ) ; 
 *******************************************choc**********************************************************   
 
   config_regADXL(ADXL345_TAP_AXES, _____); // detection choc de tous les cotés
   config_regADXL(ADXL345_THRESH_TAP, _____); // detection choc réglée à 5G
   config_regADXL(ADXL345_DUR, _______); // duree minimale du choc 10ms (pas de 625us)
   config_regADXL(ADXL345_LATENT, 0x00); // ecart minimum entre tap pas 1.25ms : 0 desactive
   config_regADXL(ADXL345_TAP_AXES, 0x00); // fenetre seconde frappe pas 1.25ms : 0 desactive
 *****************************************************************************************************************
   config_regADXL(ADXL345_THRESH_FF, 0x09); // seuil detection FREE FALL  pas 62.5mG 0.6G
   config_regADXL(ADXL345_TIME_FF, 10); // duree minimale de chute pas 5ms  : 100 ms =20
  
 ****************************************************************************************************************
 //interruptions  activer les ITs : ADXL345_INT_ENABLE  bit à 0 = INT1 , bit à 1 = INT2
 
    config_regADXL(ADXL345_INT_ENABLE, );
    config_regADXL(ADXL345_INT_MAP, ); 
    config_regADXL(ADXL345_INT_ENABLE, ); 
   
  ***********************************************************************************************************
  //gestion par FIFO pour stocker sans danger  

    config_regADXL(ADXL345_BW_RATE, );  // Fonctionnement à 100 HZ
    config_regADXL(ADXL345_FIFO_CTL, ); // stream, trig int1, avertissement sur mi remplissage (16)
}
*/
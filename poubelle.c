
void config_usart2(void)
{
    /* =========================================================
     * 1. CONFIGURATION DES ENTREES / SORTIES DE L'USART2
     * ========================================================= */

    /*
     * Activation de l'horloge du GPIOA.
     * IOPAEN = bit 2 de RCC_APB2ENR
     *
     * Cette ligne est nécessaire pour pouvoir configurer PA2 et PA3.
     */
    RCC->APB2ENR |= (1U << 2);

    /*
     * USART2 :
     * PA2 = TX
     * PA3 = RX
     *
     * PA2 :
     * MODE2 = 01  -> sortie 10 MHz
     * CNF2  = 10  -> Alternate Function Push-Pull
     *
     * PA3 :
     * MODE3 = 00  -> entrée
     * CNF3  = 01  -> entrée flottante
     *
     * Les bits [15:8] de GPIOA_CRL doivent donc valoir 0x49.
     */

    /* Effacement de la zone correspondant à PA2 et PA3 */
    GPIOA->CRL &= ~(0xFFU << 8);

    /* Ecriture de la configuration : bits [15:8] = 0x49 */
    GPIOA->CRL |= (0x49U << 8);

    /* =========================================================
     * 2. MISE SOUS TENSION / ACTIVATION DE L'HORLOGE USART2
     * ========================================================= */
    /*
     * USART2EN = bit 17 de RCC_APB1ENR
     */
    RCC->APB1ENR |= (1U << 17);
    /* =========================================================
     * 3. CONFIGURATION DE L'USART2
     * ========================================================= */

    /* ---------------------------------------------------------
     * Configuration du débit : 9600 bauds
     * ---------------------------------------------------------
     *
     * USARTDIV = 8 MHz / (16 * 9600)
     *          = 52.0833
     *
     * Mantisse = 52 = 0x34
     * Fraction = 0.0833 * 16
     *          = 1.33 ≈ 1 = 0x1
     *
     * BRR :
     * DIV_Mantissa [15:4] = 0x34
     * DIV_Fraction [3:0]  = 0x1
     */

    // USART2->BRR = (0x34U << 4) | 0x1U;
    // USART2->BRR = (0x341 << 4) | 0x1U;
    USART2->BRR = 0xEA6;

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
                     (1U << 3) |
                     (1U << 2));

    /*
     * Puis on met uniquement à 1 :
     * UE, TE et RE.
     *
     * M et PCE restent à 0.
     */
    USART2->CR1 |= ((1U << 13) |
                    (1U << 3) |
                    (1U << 2));
}

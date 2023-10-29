#include "USART.h"

// Index used for tracking received data
uint8_t usart_index = 0;
bool command_processed = true;

void usart_init(void) {
    UBRR0H = UBRRH_VALUE;                    // Set USART baud rate
    UBRR0L = UBRRL_VALUE;
    // UCSR0A |= (1 << U2X0);                   // Use double speed mode
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);   // Enable USART transmitter and receiver
    UCSR0B |= (1 << RXCIE0);                 // Receive complete interrut enable
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // Set the size of data in frame to 8 bits

    sei();
}

void transmit_byte(unsigned char data) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = data;
}

void usart_print(char content[]) {
    while(*content != '\0') {
        transmit_byte(*content);
        content++;
    }
}

void usart_print_char(char c){
    transmit_byte(c);
}

void usart_print_num(uint16_t number){
char buffer[6]; // enough to hold all digits of int16_t
    uint8_t index = 0;
    int16_t original_number = number;  // store original number

    if(number == 0){ 
        usart_print("0");
        return;
    }

    if(number < 0){
        number *= -1;
    }

    while(number){
        buffer[index++] = '0' + (number%10);
        number /= 10;
    }

    // reverse the string in the buffer
    for (uint8_t i = 0; i < index / 2; ++i) {
        char temp = buffer[i];
        buffer[i] = buffer[index - i - 1];
        buffer[index - i - 1] = temp;
    }

    // check if the original number was negative
    if(original_number < 0) {
        // shift characters in buffer to right by one position
        for(uint8_t i = index; i > 0; i--) {
            buffer[i] = buffer[i - 1];
        }

        // insert '-' sign at the start
        buffer[0] = '-';
        index++;
    }

    buffer[index] = '\0'; // null-terminate the string

    usart_print(buffer);
}

void usart_print_float(float number, int precision){
    int16_t int_part = (int16_t)number;
    usart_print_num(int_part);
    usart_print(".");

    float fractional_part = number - int_part;
    if(fractional_part < 0){
        fractional_part *= -1;
    }

    while(precision--){
        fractional_part *= 10;
        usart_print_num((int16_t)(fractional_part + 0.5f)); // add 0.5 for rounding
        fractional_part -= (int16_t)fractional_part;
    }
}


void clear(char* arr, uint8_t size){
    while(size--){
        arr[size] = 0;
    }
}

uint8_t str_len(char str[], uint8_t max_length){
    uint8_t len = 0;
    while(len < max_length && str[len] != '\0') len++;
    return len;
}

bool usart_compare_str(char* s1, char* s2, uint8_t s1_size, uint8_t s2_size){
    uint8_t index = 0;
    if(s1_size != s2_size) return false;
    while(index < s1_size){
        if(s1[index] != s2[index]) return false;
        index++;
    }
    return true;
}

bool usart_contains_str(char* s1, char* s2, uint8_t s1_size, uint8_t s2_size){
    if(s2_size > s1_size) return false;

    uint8_t s1_index = 0;
    uint8_t s2_index = 0;
    while(s1_index < s1_size) {
        if(s1[s1_index] == s2[s2_index]) {
            if(s2_index == s2_size - 1) return true;
            s2_index++;
        } else {
            s2_index = 0;
        }
        s1_index++;
    }
    return false;
}

bool is_digit(char c){
    return (c >= '0' && c <= '9') ? true : false;
}


bool contains(char str[], char c, uint8_t str_size){
    uint8_t index = 0;
    while(index < str_size){
        if(str[index] == c) return true;
        index++;
    } 
    return false;
}

float get_number(char str[], uint8_t str_size){
    uint8_t index = 0;
    int16_t wholeNumberMultiplier = str[0] == '-' ? -1 : 1;
    int16_t decimalNumberMultiplier = 1;
    int16_t wholeNumber = 0;
    float decimalNumber = 0;
    float resultingNumber = 0;
    bool decimalNumberPortion = contains(str, '.', str_size);

    while(index < str_size) {
        uint8_t byte = str[index];
        if(byte == '.'){
            decimalNumberPortion = false;
            index++;
            continue;
        }

        if(decimalNumberPortion == false){
            if(!is_digit(byte)) byte = '0';
            wholeNumber += (byte - '0') * wholeNumberMultiplier;
            wholeNumberMultiplier *= 10;
        } else {
            if(!is_digit(byte)) byte = '0';
            decimalNumber += (byte - '0') * decimalNumberMultiplier;
            decimalNumberMultiplier *= 10;
        }
        index++;
    }

    if(decimalNumber != 0){
        if(wholeNumberMultiplier > 0){
            resultingNumber = wholeNumber + decimalNumber/decimalNumberMultiplier;
        } else {
            resultingNumber = wholeNumber - decimalNumber/decimalNumberMultiplier;
        }
    } else {
        resultingNumber = wholeNumber;
    }

    return resultingNumber;
}

float* usart_get_numbers(char str[], uint8_t str_size, float* buffer){
    uint8_t str_index = 0;
    uint8_t number_index = 0;
    uint8_t digit_index = 0;
    bool number_reading = false;
    char temp_buffer[6] = {0};

    while(str_index < str_size){
        if(is_digit(str[str_index]) || str[str_index] == '-' || str[str_index] == '.'){
            number_reading = true;
            temp_buffer[digit_index] = str[str_index];
            digit_index++;
        } else if(number_reading){
            buffer[number_index] = get_number(temp_buffer, digit_index);
            clear(temp_buffer, 6);
            digit_index = 0;
            number_reading = false;
            number_index++;
        }
        str_index++;
    }

    if(number_reading){
        buffer[number_index] = get_number(temp_buffer, digit_index);
        clear(temp_buffer, 6);
    }

    return buffer;
}


/* Interrupt routine activated by the incoming data */
ISR(USART_RX_vect){
    uint8_t byte = UDR0;

    if(byte == 127){ // backspace 
        usart_print("\b \b");
        usart_last_command[usart_index] = '\0';
        if(usart_index > 0) usart_index--;
        return;
    }

    if(byte!='\r')
        transmit_byte(byte);

    if(byte == '\r'){
        usart_print("\r\n");

        usart_last_command[usart_index] = '\0';

        usart_index = 0;
        command_processed = false;
    } else {
        if(usart_index < USART_BUFFER_SIZE - 1) {
            usart_last_command[usart_index] = byte;
            usart_index++;
        }
    }
}

bool usart_command_processed(){
    if(!command_processed){
        command_processed = true;
        return false;
    }

    return true;
}

void usart_waitForCommand(){
    while(command_processed == true){
        _delay_ms(1);
    };
    command_processed = true;
}
// TO ADD
// 43 '+'
// 45 '-'
// Deg sym (not standard! 42 * might work)

void digitZero(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 3*LED_SEGMENT);
    stripClock.fill(colour, (4*LED_SEGMENT + offset), 3*LED_SEGMENT);
}

void digitOne(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (4*LED_SEGMENT + offset), 1*LED_SEGMENT);
}

void digitTwo(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 2*LED_SEGMENT);
    stripClock.fill(colour, (3*LED_SEGMENT + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (5*LED_SEGMENT + offset), 2*LED_SEGMENT);
}

void digitThree(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 2*LED_SEGMENT);
    stripClock.fill(colour, (3*LED_SEGMENT + offset), 3*LED_SEGMENT);
}

void digitFour(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 3*LED_SEGMENT);
}

void digitFive(int offset, uint32_t colour){
    stripClock.fill(colour, (1*LED_SEGMENT + offset), 5*LED_SEGMENT);
}

void digitSix(int offset, uint32_t colour){
    stripClock.fill(colour, (1*LED_SEGMENT + offset), 6*LED_SEGMENT);
}

void digitSeven(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 2*LED_SEGMENT);
    stripClock.fill(colour, (4*LED_SEGMENT + offset), 1*LED_SEGMENT);
}

void digitEight(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 7*LED_SEGMENT);
}

void digitNine(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 6*LED_SEGMENT);
}


void digit_char_a(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 5*LED_SEGMENT);
    stripClock.fill(colour, (6*LED_SEGMENT + offset), 1*LED_SEGMENT);    
}

void digit_char_b(int offset, uint32_t colour){
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 5*LED_SEGMENT);    
}

void digit_char_c(int offset, uint32_t colour){
    stripClock.fill(colour, (1*LED_SEGMENT + offset), 2*LED_SEGMENT); 
    stripClock.fill(colour, (5*LED_SEGMENT + offset), 2*LED_SEGMENT);    
}

void digit_char_d(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (3*LED_SEGMENT + offset), 4*LED_SEGMENT);    
}

// FIX
void digit_char_e(int offset, uint32_t colour){
    stripClock.fill(colour, (1*LED_SEGMENT + offset), 3*LED_SEGMENT);
    stripClock.fill(colour, (5*LED_SEGMENT + offset), 2*LED_SEGMENT);    
}

void digit_char_f(int offset, uint32_t colour){
    stripClock.fill(colour, (1*LED_SEGMENT + offset), 3*LED_SEGMENT);
    stripClock.fill(colour, (6*LED_SEGMENT + offset), 1*LED_SEGMENT);    
}

void digit_char_g(int offset, uint32_t colour){
    stripClock.fill(colour, (1*LED_SEGMENT + offset), 2*LED_SEGMENT);
    stripClock.fill(colour, (4*LED_SEGMENT + offset), 3*LED_SEGMENT);    
}

void digit_char_h(int offset, uint32_t colour){
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 3*LED_SEGMENT);
    stripClock.fill(colour, (6*LED_SEGMENT + offset), 1*LED_SEGMENT);    
}

void digit_char_i(int offset, uint32_t colour){
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (6*LED_SEGMENT + offset), 1*LED_SEGMENT);    
}

void digit_char_j(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (4*LED_SEGMENT + offset), 3*LED_SEGMENT);    
}

void digit_char_k(int offset, uint32_t colour){
    stripClock.fill(colour, (1*LED_SEGMENT + offset), 4*LED_SEGMENT);
    stripClock.fill(colour, (6*LED_SEGMENT + offset), 1*LED_SEGMENT);    
}

void digit_char_l(int offset, uint32_t colour){
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (5*LED_SEGMENT + offset), 2*LED_SEGMENT);    
}

void digit_char_m(int offset, uint32_t colour){
    stripClock.fill(colour, (1*LED_SEGMENT + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (4*LED_SEGMENT + offset), 1*LED_SEGMENT);  
    stripClock.fill(colour, (6*LED_SEGMENT + offset), 1*LED_SEGMENT);	
}

void digit_char_n(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 3*LED_SEGMENT);
    stripClock.fill(colour, (4*LED_SEGMENT + offset), 1*LED_SEGMENT);    
    stripClock.fill(colour, (6*LED_SEGMENT + offset), 1*LED_SEGMENT);
}

void digit_char_o(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 3*LED_SEGMENT);
    stripClock.fill(colour, (4*LED_SEGMENT + offset), 3*LED_SEGMENT);    
}

void digit_char_p(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 4*LED_SEGMENT);
    stripClock.fill(colour, (6*LED_SEGMENT + offset), 1*LED_SEGMENT);    
}

void digit_char_q(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 5*LED_SEGMENT);
}

void digit_char_r(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 3*LED_SEGMENT);
    stripClock.fill(colour, (6*LED_SEGMENT + offset), 1*LED_SEGMENT);    
}

void digit_char_s(int offset, uint32_t colour){
    stripClock.fill(colour, (1*LED_SEGMENT + offset), 5*LED_SEGMENT);    
}

void digit_char_t(int offset, uint32_t colour){
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 2*LED_SEGMENT);
    stripClock.fill(colour, (5*LED_SEGMENT + offset), 2*LED_SEGMENT);    
}

void digit_char_u(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 1*LED_SEGMENT);    
    stripClock.fill(colour, (4*LED_SEGMENT + offset), 3*LED_SEGMENT);    
}

void digit_char_v(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 1*LED_SEGMENT);    
    stripClock.fill(colour, (4*LED_SEGMENT + offset), 2*LED_SEGMENT);     
}

void digit_char_w(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 1*LED_SEGMENT);   
    stripClock.fill(colour, (5*LED_SEGMENT + offset), 1*LED_SEGMENT);	
}

void digit_char_x(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 3*LED_SEGMENT);
    stripClock.fill(colour, (6*LED_SEGMENT + offset), 1*LED_SEGMENT);    
}

void digit_char_y(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 1*LED_SEGMENT);
    stripClock.fill(colour, (2*LED_SEGMENT + offset), 4*LED_SEGMENT);    
}

void digit_char_z(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 2*LED_SEGMENT);
    stripClock.fill(colour, (3*LED_SEGMENT + offset), 1*LED_SEGMENT); 
    stripClock.fill(colour, (5*LED_SEGMENT + offset), 1*LED_SEGMENT); 	
}
void digit_char_deg(int offset, uint32_t colour){
    stripClock.fill(colour, (0 + offset), 4*LED_SEGMENT);
}

/* 
 * 
 */

#include "math.h"
#include "lstm.h"

#include "weights/lstm1_wx.h"
#include "weights/lstm1_wh.h"
#include "weights/lstm1_wb.h"

#include "weights/dense1_w.h"
#include "weights/dense1_b.h"



#pragma hls_design top
void lstm(
		input_t lstm_in[N_TS*N1_LX],
		result_t lstm_out[MODEL_OUT]
){

	#pragma HLS ARRAY_RESHAPE variable=lstm_out complete dim=0

	result_t lstm1_out [N1_LH];
	result_t lstm4_out [DENSE1_OUT];

	#pragma HLS DATAFLOW

	const int input_factor = N1_LX;
    #pragma HLS ARRAY_PARTITION variable=lstm_in cyclic factor=input_factor

    nnet::lstm<input_t, result_t, config1, config_a, config_x, config_h>(lstm_in, lstm1_wx, lstm1_wh, lstm1_wb, lstm1_out);

#ifndef __SYNTHESIS__
    if (DEBUG==1) {
    	std::cout <<"\n LSTM layer 1 output ";
    	for(int ff = 0; ff < N1_LH; ff++) {
    		std::cout <<", "<< lstm1_out[ff];
    	}
    }
#endif

    result_t layer2_out[DENSE1_OUT];
    #pragma HLS ARRAY_PARTITION variable=layer2_out complete dim=0
    nnet::dense_simple<input_t, result_t, config2>(lstm1_out, layer2_out, dense1_w, dense1_b);

#ifndef __SYNTHESIS__
    if (DEBUG==1) {
    	std::cout <<"\n Dense output ";
    	for(int ff = 0; ff < DENSE1_OUT; ff++) {
    		std::cout <<", "<< layer2_out[ff];
    	}
    }
#endif

    nnet::softmax<input_t, result_t, softmax_config>(layer2_out, lstm_out);


#ifndef __SYNTHESIS__
    if (DEBUG==1) {
    	std::cout <<"\n Softmax output ";
    	for(int ff = 0; ff < DENSE1_OUT; ff++) {
    		std::cout <<", "<< lstm_out[ff];
    	}
    }
#endif



}

